#pragma once

#include <DxLib.h>
#include "../../EcsRegistry.h"
#include "../../Entity.h"
#include "../../Component/EnemyComponent.h"

/// @brief エネミー関連のサブシステムを隠蔽し、AIシステムへ単一の窓口を提供するFacadeクラス
class EnemyActionFacade
{
public:

	/// @brief コンストラクタ
	/// @param registry ECSレジストリ参照
	/// @param enemyEntity 操作対象のエネミーエンティティ
	EnemyActionFacade(EcsRegistry& registry, Entity enemyEntity);

	/// @brief デストラクタ
	~EnemyActionFacade(void) = default;

	/// @brief スポーン座標の初期記録とタイマー更新を行う
	/// @param deltaTime フレーム経過時間
	void UpdateTimersAndInitializeHome(float deltaTime);

	/// @brief エネミーが死亡しているか確認する
	/// @return HPが0以下ならtrue
	bool CheckDead(void) const;

	/// @brief 現在のAIステートを取得する
	/// @return 現在のAI_STATE
	EnemyComponent::AI_STATE GetCurrentState(void) const;

	/// @brief AIステートを変更し、経過タイマーをリセットする
	/// @param nextState 遷移先のAI_STATE
	void ChangeState(EnemyComponent::AI_STATE nextState);

	/// @brief 現在のステート経過時間を取得する
	/// @return 経過秒数
	float GetStateTimer(void) const;

	/// @brief プレイヤー発見フラグを設定する
	/// @param isDetected 発見状態ならtrue
	void SetPlayerDetected(bool isDetected);

	/// @brief 攻撃クールダウンタイマーを設定する
	/// @param cooldownTime クールダウン秒数
	void SetAttackCooldown(float cooldownTime);

	/// @brief 未発見時の前方扇形視野内にプレイヤーがいるか判定する
	/// @param playerPosition プレイヤーのワールド座標
	/// @return 扇形視野内にいればtrue
	bool CheckPlayerInConeVision(const VECTOR& playerPosition) const;

	/// @brief 発見後の球状（360度）アグロ範囲内にプレイヤーがいるか判定する
	/// @param playerPosition プレイヤーのワールド座標
	/// @return 球状範囲内にいればtrue
	bool CheckPlayerInSphericalAggro(const VECTOR& playerPosition) const;

	/// @brief 攻撃可能距離内にプレイヤーがおり、クールダウンが完了しているか判定する
	/// @param playerPosition プレイヤーのワールド座標
	/// @return 攻撃開始可能ならtrue
	bool CheckCanStartAttack(const VECTOR& playerPosition) const;

	/// @brief 初期スポーン位置へ帰還完了したか判定する
	/// @return 初期位置付近に到着していればtrue
	bool CheckArrivedAtHome(void) const;

	/// @brief プレイヤーの座標へ向かって移動・回転する
	/// @param playerPosition プレイヤーのワールド座標
	void MoveTowardsPlayer(const VECTOR& playerPosition);

	/// @brief 初期スポーン位置へ向かって移動・回転する
	void MoveTowardsHome(void);

	/// @brief 水平方向（X, Z）の移動速度をゼロに停止させる
	void StopHorizontalMovement(void);

	/// @brief デバッグ用の索敵範囲（未発見時は扇形、発見時は球状）を描画する
	void DrawDebugVisionRange(void) const;

private:

	// サブシステム参照関連
	EcsRegistry& registry_; // ECSレジストリ参照
	Entity enemyEntity_;    // エネミーエンティティID

	/// @brief 指定した目標座標へ向かって水平移動とY軸回転を行う内部処理
	/// @param targetPosition 目標ワールド座標
	void MoveTowardsPosition(const VECTOR& targetPosition);
};