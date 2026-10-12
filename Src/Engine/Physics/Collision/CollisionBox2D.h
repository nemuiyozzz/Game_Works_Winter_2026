#pragma once

class Collider2DBase;

/// @brief 矩形同士の衝突判定を管理するクラス
class CollisionBox2D
{
public:
	/// @brief 矩形と矩形の衝突判定
	/// @param boxA 1つ目の矩形コライダー
	/// @param boxB 2つ目の矩形コライダー
	/// @return 衝突しているか
	static bool CheckBoxVsBox(const Collider2DBase* boxA, const Collider2DBase* boxB);
};

