#pragma once

#include "IPlayerState.h"
#include "PlayerActionFacade.h"

/// @brief プレイヤーの待機状態を表すステートクラス
class PlayerIdleState : public IPlayerState
{
public:

	/// @brief コンストラクタ
	PlayerIdleState(void) = default;

	/// @brief デストラクタ
	~PlayerIdleState(void) override = default;

	/// @brief ステート開始時の処理を行う
	/// @param action プレイヤーアクション窓口Facade
	void EnterState(PlayerActionFacade& action) override;

	/// @brief ステート更新処理を行う
	/// @param action プレイヤーアクション窓口Facade
	/// @param deltaTime フレーム経過時間
	/// @return 遷移先のステート
	std::shared_ptr<IPlayerState> UpdateState(PlayerActionFacade& action,
		float detaTime) override;

	/// @brief ステート終了時の処理を行う
	/// @param action プレイヤーアクション窓口Facade
	void ExitState(PlayerActionFacade& action) override;

	/// @brief ステートIDを取得する
	/// @return STATE_ID::IDLE
	STATE_ID GetStateId(void) const override;
};

