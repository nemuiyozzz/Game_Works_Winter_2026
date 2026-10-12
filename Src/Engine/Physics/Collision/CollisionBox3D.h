#pragma once

#include <DxLib.h>

class ColliderBase;
struct CollisionInfo;

/// @brief 3Dボックスの衝突判定を管理するクラス
class CollisionBox3D
{
public:

	/// @brief ボックス vs ボックスの衝突判定
	/// @param boxA 判定対象A（ボックス）
	/// @param boxB 判定対象B（ボックス）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckBoxVsBox(const ColliderBase* boxA, const ColliderBase* boxB,
		CollisionInfo& outInfo);

	/// @brief ボックス vs 球体の衝突判定
	/// @param boxCol 判定対象（ボックス）
	/// @param sphereCol 判定対象（球体）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckBoxVsSphere(const ColliderBase* boxCol, const ColliderBase* sphereCol,
		CollisionInfo& outInfo);

	/// @brief ボックス vs カプセルの衝突判定
	/// @param boxCol 判定対象（ボックス）
	/// @param capsuleCol 判定対象（カプセル）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckBoxVsCapsule(const ColliderBase* boxCol, const ColliderBase* capsuleCol,
		CollisionInfo& outInfo);
};