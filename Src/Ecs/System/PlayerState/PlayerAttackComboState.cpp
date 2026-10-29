#include "PlayerAttackComboState.h"
#include "PlayerIdleState.h"
#include "PlayerEvadeState.h"

namespace
{
	// コンボ攻撃のフレーム時間・速度倍率関連
	constexpr float ATTACK_DURATION = 0.55f;        // 攻撃1段あたりの全体持続秒数
	constexpr float ATTACK_HIT_START_TIME = 0.12f;  // 剣の当たり判定が発生する秒数
	constexpr float ATTACK_HIT_END_TIME = 0.40f;    // 剣の当たり判定が消える秒数
	constexpr float COMBO_BUFFER_START_TIME = 0.20f;// 次段の先行入力を受け付ける開始秒数
	constexpr float ATTACK_STEP_SPEED_RATIO = 0.45f;// 通常移動速度に対する前方踏み込み倍率
	constexpr float STEP_DECELERATION_RATE = 0.85f; // 踏み込み速度の減衰率
}

PlayerAttackComboState::PlayerAttackComboState(int comboStep)
	: currentStep_(comboStep)
{
}

void PlayerAttackComboState::EnterState(PlayerActionFacade& action)
{
	action.SetupComboStep(currentStep_);
	action.ExecuteForwardStep(ATTACK_STEP_SPEED_RATIO);
	action.PlayAttackAnimation();
}

std::shared_ptr<IPlayerState> PlayerAttackComboState::UpdateState(
	PlayerActionFacade& action, float deltaTime)
{
	float currentTimer = action.GetStateTimer(); // 現在のステート経過時間

	bool isHitActiveWindow = (currentTimer >= ATTACK_HIT_START_TIME &&
		currentTimer <= ATTACK_HIT_END_TIME); // 攻撃判定有効期間か
	action.SetSwordColliderActive(isHitActiveWindow);

	if (currentTimer > ATTACK_HIT_START_TIME)
	{
		action.DecelerateHorizontalMovement(STEP_DECELERATION_RATE);
	}

	if (currentTimer >= COMBO_BUFFER_START_TIME && currentStep_ < MAX_COMBO_STEP)
	{
		if (action.CheckAttackTriggered())
		{
			action.SetNextComboBuffered(true);
		}
	}

	if (currentTimer >= ATTACK_HIT_END_TIME && action.CheckEvadeTriggered())
	{
		if (action.TryConsumeStamina(PlayerActionFacade::COST_EVADE))
		{
			return std::make_shared<PlayerEvadeState>();
		}
	}

	if (currentTimer >= ATTACK_DURATION || action.CheckAnimationEnded())
	{
		if (action.CheckNextComboBuffered())
		{
			if (action.TryConsumeStamina(PlayerActionFacade::COST_ATTACK))
			{
				return std::make_shared<PlayerAttackComboState>(currentStep_ + 1);
			}
		}
		return std::make_shared<PlayerIdleState>();
	}

	return nullptr;
}

void PlayerAttackComboState::ExitState(PlayerActionFacade& action)
{
	action.SetSwordColliderActive(false);
}

IPlayerState::STATE_ID PlayerAttackComboState::GetStateId(void) const
{
	return STATE_ID::ATTACK_COMBO;
}