#pragma once

#include <DxLib.h>

class ColliderBase;
struct CollisionInfo;

/// @brief 球体の衝突判定を管理するクラス
class CollisionSphere
{
public:
	/// @brief 球体 vs 球体の衝突判定
	/// @param colliderA 判定対象A（球体）
	/// @param colliderB 判定対象B（球体）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckSphereVsSphere(const ColliderBase* colliderA, const ColliderBase* colliderB,
		CollisionInfo& outInfo);

	/// @brief 球体 vs カプセルの衝突判定
	/// @param sphereCol 判定対象（球体）
	/// @param capsuleCol 判定対象（カプセル）
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckSphereVsCapsule(const ColliderBase* sphereCol, const ColliderBase* capsuleCol,
		CollisionInfo& outInfo);

	/// @brief 球体 vs モデルの衝突判定
	/// @param sphereCol 判定対象 (球体)
	/// @param modelCol 判定対象 (3Dモデル/メッシュ)
	/// @param outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckSphereVsModel(const ColliderBase* sphereCol, const ColliderBase* modelCol,
		CollisionInfo& outInfo);

	/// @brief 衝撃波の当たり判定を行う
	/// @param hitCapsuleCol 当たる側のコライダー（カプセル）
	/// @param waveCol 衝撃波のコライダー（球体）
	/// @return 当たっていれば true
	static bool CheckHitWave(const ColliderBase* hitCapsuleCol, ColliderBase* waveCol);

	/// @brief 球とモデルの当たり判定の実処理。中心座標・半径を直接指定
	/// @param centerPos 判定したい球の中心座標
	/// @param radius 判定したい球の半径
	/// @param modelCol 判定対象のモデルコライダー
	/// @param outInfo 衝突情報の出力先
	/// @return 衝突していればtrue
	static bool CheckSphereVsModelCore(const VECTOR& centerPos, float radius,
		const ColliderBase* modelCol, CollisionInfo* outInfo);

private:

	// 衝撃波の定数関連
	static constexpr float HIT_WAVE_THICKNESS = 2.0f; // 衝撃波の厚み
	static constexpr float HIT_WAVE_HEIGHT = 75.0f;   // 衝撃波の高さ
};