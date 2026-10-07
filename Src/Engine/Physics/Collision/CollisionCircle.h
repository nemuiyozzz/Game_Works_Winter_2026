#pragma once

class Collider2DBase;

/// @brief 円と他の2D形状の衝突判定を行うクラス
class CollisionCircle
{
public:
	/// @brief 円と円の衝突判定
	/// @param circleA 1つ目の円コライダー
	/// @param circleB 2つ目の円コライダー
	/// @return 衝突しているか
	static bool CheckCircleVsCircle(const Collider2DBase* circleA,
		const Collider2DBase* circleB);

	/// @brief 円と矩形の衝突判定
	/// @param circle 円コライダー
	/// @param box 矩形コライダー
	/// @return 衝突しているか
	static bool CheckCircleVsBox(const Collider2DBase* circle, const Collider2DBase* box);
};