#pragma once

#include <unordered_set>
#include <vector>
#include <string>
#include <Dxlib.h>

#include "ColliderBase.h"

struct TransformComponent;

/// @brief モデルベースのコライダクラス
class ColliderModel : public ColliderBase
{
public:

    // 近傍フレーム検索の最大数
    static constexpr int NEAREST_FRAME_COUNT = 4;

    /// @brief 分割フレームのキャッシュ情報
    struct SubFrameInfo
    {
        int frameIndex = -1;                         // DxLibのフレーム番号
        VECTOR worldCenterPos = { 0.0f, 0.0f, 0.0f }; // ワールド中心座標
        VECTOR worldMinPos = { 0.0f, 0.0f, 0.0f };    // ワールド最小座標（デバッグ表示・AABB用）
        VECTOR worldMaxPos = { 0.0f, 0.0f, 0.0f };    // ワールド最大座標（デバッグ表示・AABB用）
    };


    /// @brief コンストラクタ
    /// @param collisionTag 衝突種別
    /// @param followTarget 追従対象のTransform
    ColliderModel(TAG collisionTag, const TransformComponent* followTarget);

    /// @brief デストラクタ
    ~ColliderModel(void) override = default;

    /// @brief モデルハンドルの取得
    /// @return モデルハンドル
    int GetModelHandle(void) const { return modelHandle_; }

    /// @brief モデルハンドルの設定
    void SetModelHandle(int _modelHandle__);

    /// @brief 対象座標のXZ平面上で最も近い上位4フレームを取得する
    /// @param targetWorldPosition 判定対象のワールド座標
    /// @param outFrameIndices 抽出されたフレーム番号の格納先配列
    /// @param outFrameCount 実際に抽出されたフレーム数
    void FindNearestFrames(
        const VECTOR& targetWorldPosition,
        int* outFrameIndices,
        int& outFrameCount) const;

    /// @brief 指定文字列を含むフレームを当たり判定から除外
    /// @param exclusionName 除外対象フレーム名に含まれる文字列
    void AddExcludeFrameIds(const std::wstring& exclusionName);

    /// @brief 除外フレームのクリア
    void ClearExcludedFrames(void);

    /// @brief フレームが除外対象か判定
    /// @param frameIndex フレーム番号
    /// @return 除外対象ならtrue
    bool IsExcludedFrame(int frameIndex) const;

protected:
    /// @brief デバッグ描画
    /// @param debugColor 描画色
    void DrawDebug(int debugColor) const override;

private:
    // 衝突判定から除外するフレーム番号
    std::unordered_set<int> excludedFrameIndices_;

    // 細分化された有効なメッシュフレーム一覧
    std::vector<SubFrameInfo> subFrames_;

    // 直近の判定で選ばれたフレーム番号リスト（デバッグ描画確認用）
    mutable std::vector<int> debugActiveFrames_;

    // 自身の判定に使用するモデルハンドル
    int modelHandle_;
};