#pragma once

#include "IPlayerState.h"
#include "PlayerActionFacade.h"

/// @brief プレイヤーの地上コンボ攻撃状態を表すステートクラス
class PlayerAttackComboState : public IPlayerState
{
public:

	// 最大コンボ段数インデックス
	static constexpr int MAX_COMBO_STEP = 2;

	/// @brief コンストラクタ
	/// @param comboStep コンボ段数
	explicit PlayerAttackComboState(int comboStep = 0);

	/// @brief デストラクタ
	~PlayerAttackComboState(void) override = default;

	/// @brief ステート開始時の処理を行う
	/// @param action プレイヤーアクション窓口Facade
	void EnterState(PlayerActionFacade& action) override;

	/// @brief ステート更新処理を行う
	/// @param action プレイヤーアクション窓口Facade
	/// @param deltaTime フレーム経過時間
	/// @return 遷移先のステート
	std::shared_ptr<IPlayerState> UpdateState(
		PlayerActionFacade& action, float deltaTime) override;

	/// @brief ステート終了時の処理を行う
	/// @param action プレイヤーアクション窓口Facade
	void ExitState(PlayerActionFacade& action) override;

	/// @brief ステートIDを取得する
	/// @return STATE_ID::ATTACK_COMBO
	STATE_ID GetStateId(void) const override;

private:

	// 現在のコンボ段数
	int currentStep_;
};