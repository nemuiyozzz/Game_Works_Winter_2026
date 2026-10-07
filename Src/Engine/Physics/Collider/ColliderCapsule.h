#pragma once

#include <DxLib.h>

#include "ColliderBase.h"

struct TransformComponent;

/// @brief カプセル形状のコライダクラス
class ColliderCapsule : public ColliderBase
{
public:

    /// @brief コンストラクタ
    /// @param collisionTag 衝突種別
    /// @param followTarget 追従対象のTransform
    /// @param localStartPos ローカル空間での開始位置
    /// @param localEndPos ローカル空間での終了位置
    /// @param radius カプセルの半径
    ColliderCapsule(TAG collisionTag, const TransformComponent* followTarget, 
        const VECTOR& localStartPos, const VECTOR& localEndPos, float radius);

    /// @brief デストラクタ
    ~ColliderCapsule(void) override = default;

    /// @brief ローカル開始位置の設定
    /// @param pos ローカル空間での開始位置
    void SetLocalStartPos(const VECTOR& position);

    /// @brief ローカル終了位置の設定
    /// @param pos ローカル空間での終了位置
    void SetLocalEndPos(const VECTOR& position);

    /// @brief 半径の設定
    /// @param radius カプセルの半径
    void SetRadius(float radius) override;

    /// @brief ローカル開始位置の取得
    /// @return ローカル空間での開始位置
    const VECTOR& GetLocalStartPos(void) const;

    /// @brief ローカル終了位置の取得
    /// @return ローカル空間での終了位置
    const VECTOR& GetLocalEndPos(void) const;

    /// @brief ワールド開始位置の取得
    /// @return ワールド空間での開始位置
    VECTOR GetWorldStartPos(void) const;

    /// @brief ワールド終了位置の取得
    /// @return ワールド空間での開始位置
    VECTOR GetWorldEndPos(void) const;

    /// @brief 半径の取得
   /// @return カプセルの半径
    float GetRadius(void) const;

protected:
    /// @brief デバッグ描画
    /// @param debugColor 描画色
    void DrawDebug(int debugColor) const override;

private:
    // デバッグ表示の球体・カプセルポリゴン分割数
    static constexpr int DEBUG_SEGMENT_COUNT = 8;

    // コライダーの形状パラメータ関連
    VECTOR localStartPos_; // カプセルの開始座標
    VECTOR localEndPos_;   // カプセルの終了座標
    float radius_;         // カプセルの半径
};