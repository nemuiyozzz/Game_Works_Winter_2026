#pragma once

#include "IPlayerState.h"

/// @brief プレイヤーの被弾（ダメージ）状態を管理するステート
class PlayerDamageState : public IPlayerState
{
public:
	PlayerDamageState(void) = default;
	~PlayerDamageState(void) override = default;

	/// @brief ステート開始時：ダメージアニメーション再生、ノックバック設定、無敵付与
	void EnterState(PlayerActionFacade& action) override;

	/// @brief ステート更新時：アニメーション終了監視
	std::shared_ptr<IPlayerState> UpdateState(
		PlayerActionFacade& action, float deltaTime) override;

	/// @brief ステート終了時
	void ExitState(PlayerActionFacade& action) override;

	/// @brief ステートIDの取得
	STATE_ID GetStateId(void) const override { return STATE_ID::DAMAGE; }
};