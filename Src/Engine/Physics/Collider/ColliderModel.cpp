#include "../../../Pch.h"
#include "ColliderModel.h"
#include "../../../Ecs/Component/TransformComponent.h"

ColliderModel::ColliderModel(TAG collisionTag, const TransformComponent* followTarget)
    : ColliderBase(SHAPE::MODEL, collisionTag, followTarget)
    , modelHandle_(-1)
{
}

void ColliderModel::SetModelHandle(int _modelHandle_)
{
    modelHandle_ = _modelHandle_;

    if (modelHandle_ != -1)
    {
        // 空間分割関連
        constexpr int ALL_FRAME_INDEX = -1; 
        constexpr int SPATIAL_DIV_NUM = 8;  

        MV1SetupCollInfo(
            modelHandle_,
            ALL_FRAME_INDEX,
            SPATIAL_DIV_NUM,
            SPATIAL_DIV_NUM,
            SPATIAL_DIV_NUM);
    }
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
    int modelHandle = GetModelHandle();
    if (modelHandle == -1)
    {
        return;
    }

    const int ALPHA_BLEND_PARAM = 1; // 半透明のブレンドパラメータ
    const int NO_BLEND_PARAM = 0;    // ブレンドなしのパラメータ

    // 半透明にして描画する
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALPHA_BLEND_PARAM);

    MV1DrawModel(modelHandle);

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NO_BLEND_PARAM);
}

