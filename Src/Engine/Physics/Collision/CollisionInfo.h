#pragma once

#include <DxLib.h>

class ColliderBase;

/// @brief 衝突結果を格納する構造体
struct CollisionInfo
{
	// 衝突したコライダー関連
	const ColliderBase* myCollider = nullptr;  // 自身のコライダーポインタ
	const ColliderBase* hitCollider = nullptr; // 相手のコライダーポインタ

	// 衝突座標関連
	VECTOR hitPosition = VGet(0.0f, 0.0f, 0.0f); // 衝突位置
	VECTOR hitNormal = VGet(0.0f, 1.0f, 0.0f);   // 衝突面における法線ベクトル
	float penetration = 0.0f;                    // 衝突の深さ（めり込み量）

	// 衝突情報の有効フラグ
	bool isActive = false;
};