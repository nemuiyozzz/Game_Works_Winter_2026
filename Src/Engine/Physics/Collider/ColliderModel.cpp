#include "../../../Pch.h"
#include "ColliderModel.h"
#include "../../../Ecs/Component/TransformComponent.h"

namespace
{
    // 空間分割・座標計算関連
    constexpr int ALL_FRAME_INDEX = -1;       // 全フレーム指定インデックス
    constexpr int DEFAULT_DIV_NUM = 8;        // 通常モデルの空間分割数
    constexpr int SUB_FRAME_DIV_NUM = 4;      // 細分化フレームの空間分割数
    constexpr float HALF_SCALE = 0.5f;        // 中心算出用の係数
    constexpr int BOX_CORNER_COUNT = 8;       // AABBの頂点数

    // デバッグ描画関連
    constexpr int ACTIVE_ALPHA_PARAM = 160;   // 稼働中フレームの不透明度
    constexpr int INACTIVE_ALPHA_PARAM = 40;  // 非稼働フレームの不透明度
    constexpr int NO_BLEND_PARAM = 0;         // ブレンド解除パラメータ
}

ColliderModel::ColliderModel(TAG collisionTag, const TransformComponent* followTarget)
    : ColliderBase(SHAPE::MODEL, collisionTag, followTarget)
    , modelHandle_(-1)
{
}

void ColliderModel::SetModelHandle(int modelHandle)
{
    modelHandle_ = modelHandle;
    subFrames_.clear();
    debugActiveFrames_.clear();

    if (modelHandle_ == -1)
    {
        return;
    }

    int totalFrameNum = MV1GetFrameNum(modelHandle_); 

    // ポリゴンが存在する有効なフレームを収集
    std::vector<int> validFrameIndices;
    for (int frameIndex = 0; frameIndex < totalFrameNum; ++frameIndex)
    {
        if (MV1GetFrameTriangleNum(modelHandle_, frameIndex) > 0)
        {
            validFrameIndices.push_back(frameIndex);
        }
    }

    // 複数フレームに分割されている場合
    if (validFrameIndices.size() > 1)
    {
        subFrames_.reserve(validFrameIndices.size());

        for (int frameIndex : validFrameIndices)
        {
            MV1SetupCollInfo(
                modelHandle_,
                frameIndex,
                SUB_FRAME_DIV_NUM,
                SUB_FRAME_DIV_NUM,
                SUB_FRAME_DIV_NUM);

            VECTOR minLocal = MV1GetFrameMinVertexLocalPosition(modelHandle_, frameIndex);
            VECTOR maxLocal = MV1GetFrameMaxVertexLocalPosition(modelHandle_, frameIndex);

            // フレーム固有のローカル→ワールド変換行列を取得する
            MATRIX frameWorldMat = MV1GetFrameLocalWorldMatrix(modelHandle_, frameIndex);

            // ローカルAABBの8頂点をワールド変換して、正確なワールドAABBと中心を求める
            VECTOR corners[BOX_CORNER_COUNT] =
            {
                VGet(minLocal.x, minLocal.y, minLocal.z),
                VGet(maxLocal.x, minLocal.y, minLocal.z),
                VGet(minLocal.x, maxLocal.y, minLocal.z),
                VGet(maxLocal.x, maxLocal.y, minLocal.z),
                VGet(minLocal.x, minLocal.y, maxLocal.z),
                VGet(maxLocal.x, minLocal.y, maxLocal.z),
                VGet(minLocal.x, maxLocal.y, maxLocal.z),
                VGet(maxLocal.x, maxLocal.y, maxLocal.z)
            };

            VECTOR worldMin = VGet(FLT_MAX, FLT_MAX, FLT_MAX);
            VECTOR worldMax = VGet(-FLT_MAX, -FLT_MAX, -FLT_MAX);

            for (int cornerIdx = 0; cornerIdx < BOX_CORNER_COUNT; ++cornerIdx)
            {
                VECTOR worldPt = VTransform(corners[cornerIdx], frameWorldMat);
                if (worldPt.x < worldMin.x) { worldMin.x = worldPt.x; }
                if (worldPt.y < worldMin.y) { worldMin.y = worldPt.y; }
                if (worldPt.z < worldMin.z) { worldMin.z = worldPt.z; }

                if (worldPt.x > worldMax.x) { worldMax.x = worldPt.x; }
                if (worldPt.y > worldMax.y) { worldMax.y = worldPt.y; }
                if (worldPt.z > worldMax.z) { worldMax.z = worldPt.z; }
            }

            SubFrameInfo info{};
            info.frameIndex = frameIndex;
            info.worldMinPos = worldMin;
            info.worldMaxPos = worldMax;
            info.worldCenterPos = VScale(VAdd(worldMin, worldMax), HALF_SCALE);
            subFrames_.push_back(info);
        }
    }
    else
    {
        MV1SetupCollInfo(
            modelHandle_,
            ALL_FRAME_INDEX,
            DEFAULT_DIV_NUM,
            DEFAULT_DIV_NUM,
            DEFAULT_DIV_NUM);
    }
}

void ColliderModel::FindNearestFrames(const VECTOR& targetWorldPosition, int* outFrameIndices, 
    int& outFrameCount) const
{
    outFrameCount = 0;

    if (modelHandle_ == -1)
    {
        return;
    }

    if (subFrames_.empty())
    {
        outFrameIndices[0] = -1;
        outFrameCount = 1;
        return;
    }

    struct FrameDistancePair
    {
        float distanceSq = 0.0f;
        int frameIndex = -1;
    };

    std::vector<FrameDistancePair> candidates;

    candidates.reserve(subFrames_.size());

    for (const SubFrameInfo& info : subFrames_)
    {
        if (IsExcludedFrame(info.frameIndex))
        {
            continue;
        }

        float clampedX = targetWorldPosition.x;

        if (clampedX < info.worldMinPos.x) { clampedX = info.worldMinPos.x; }
        if (clampedX > info.worldMaxPos.x) { clampedX = info.worldMaxPos.x; }

        float clampedZ = targetWorldPosition.z;

        if (clampedZ < info.worldMinPos.z) { clampedZ = info.worldMinPos.z; }
        if (clampedZ > info.worldMaxPos.z) { clampedZ = info.worldMaxPos.z; }

        float diffX = targetWorldPosition.x - clampedX;
        float diffZ = targetWorldPosition.z - clampedZ;
        float distSq = (diffX * diffX) + (diffZ * diffZ);
        
        if (distSq == 0.0f)
        {
            float centerDiffX = targetWorldPosition.x - info.worldCenterPos.x;
            float centerDiffZ = targetWorldPosition.z - info.worldCenterPos.z;
            distSq = ((centerDiffX * centerDiffX) + (centerDiffZ * centerDiffZ)) * 0.0001f;
        }

        candidates.push_back({ distSq, info.frameIndex });
    }

    if (candidates.empty())
    {
        return;
    }

    int selectCount = (static_cast<int>(candidates.size()) < NEAREST_FRAME_COUNT)
        ? static_cast<int>(candidates.size()) : NEAREST_FRAME_COUNT;
   
    std::partial_sort(candidates.begin(), candidates.begin() + selectCount, candidates.end(),
        [](const FrameDistancePair& left, const FrameDistancePair& right)
        {
            return left.distanceSq < right.distanceSq;
        });

    for (int i = 0; i < selectCount; ++i)
    {
        outFrameIndices[i] = candidates[i].frameIndex;

        if (std::find(debugActiveFrames_.begin(), debugActiveFrames_.end(),
            candidates[i].frameIndex) == debugActiveFrames_.end())
        {
            debugActiveFrames_.push_back(candidates[i].frameIndex);
        }
    }

    outFrameCount = selectCount;
}

void ColliderModel::AddExcludeFrameIds(const std::wstring& exclusionName)
{
    // モデル未設定なら処理しない
    if (modelHandle_ == -1) { return; }

    // モデルのフレーム総数を取得
    int num = MV1GetFrameNum(modelHandle_);

    for (int i = 0; i < num; i++)
    {
        const TCHAR* frameName = MV1GetFrameName(modelHandle_, i);
        
        std::wstring frameNameStr = frameName;

        // 指定文字列を含むフレームか判定
        if (frameNameStr.find(exclusionName) != std::wstring::npos)
        {
            // 該当フレームを除外リストに登録
            excludedFrameIndices_.insert(i);
        }
    }
}

void ColliderModel::ClearExcludedFrames(void)
{
    // 除外リストを全削除
    excludedFrameIndices_.clear();
}

bool ColliderModel::IsExcludedFrame(int frameIdx) const
{
    // セットに存在するかで判定
    return excludedFrameIndices_.count(frameIdx) > 0;
}

void ColliderModel::DrawDebug(int debugColor) const
{
    if (modelHandle_ == -1)
    {
        return;
    }

    if (subFrames_.empty())
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, INACTIVE_ALPHA_PARAM);
        MV1DrawModel(modelHandle_);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NO_BLEND_PARAM);
        return;
    }

    unsigned int activeColor = GetColor(255, 30, 30);   // 稼働中のフレーム：赤色
    unsigned int inactiveColor = GetColor(0, 180, 255); // 待機中のフレーム：水色

    for (const SubFrameInfo& info : subFrames_)
    {
        bool isActiveFrame = (std::find(
            debugActiveFrames_.begin(),
            debugActiveFrames_.end(),
            info.frameIndex) != debugActiveFrames_.end());

        if (!isActiveFrame)
        {
            DrawCube3D(info.worldMinPos, info.worldMaxPos, inactiveColor, inactiveColor, FALSE);
        }
    }

    for (const SubFrameInfo& info : subFrames_)
    {
        bool isActiveFrame = (std::find(
            debugActiveFrames_.begin(),
            debugActiveFrames_.end(),
            info.frameIndex) != debugActiveFrames_.end());

        if (isActiveFrame)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, ACTIVE_ALPHA_PARAM);
            MV1DrawFrame(modelHandle_, info.frameIndex);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NO_BLEND_PARAM);

            DrawCube3D(info.worldMinPos, info.worldMaxPos, activeColor, activeColor, FALSE);
        }
    }

    debugActiveFrames_.clear();
}