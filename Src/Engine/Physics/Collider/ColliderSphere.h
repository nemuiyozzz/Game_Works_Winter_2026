#pragma once

#include <DxLib.h>

#include "ColliderBase.h"

struct TransformComponent;

// @brief 球体コライダクラス
class ColliderSphere : public ColliderBase
{
public:

    /// @brief コンストラクタ
    /// @param collisionTag 衝突種別
    /// @param followTarget 追従対象のTransform
    /// @param localPos ローカル空間での中心位置
    /// @param radius 球体の半径
    ColliderSphere(TAG collisionTag, const TransformComponent* followTarget,
        const VECTOR& localPosition, float radius);

    /// @brief デストラクタ
    ~ColliderSphere(void) override = default;

    /// @brief ローカル座標の設定
    /// @param position ローカル空間での中心位置
    void SetLocalPosition(const VECTOR& position);

    /// @brief 半径の設定
    /// @param radius 球体の半径
    void SetRadius(float radius) override;

    /// @brief ローカル座標の取得
    /// @return ローカル空間での中心位置
    const VECTOR& GetLocalPosition(void) const;

    /// @brief ワールド座標の取得
    /// @return ワールド空間での中心位置
    VECTOR GetWorldPosition(void) const;

    /// @brief 半径の取得
    /// @return 球体の半径
    float GetRadius(void) const;

protected:

    /// @brief デバッグ描画
    /// @param color 描画色
    void DrawDebug(int debugColor) const override;

private:

    // デバッグ表示の球体ポリゴン分割数
    static constexpr int DEBUG_SEGMENT_COUNT = 16;

    // コライダーの形状パラメータ関連
    VECTOR localPos_; // 球体の中心座標
    float radius_;    // 球体の半径
};