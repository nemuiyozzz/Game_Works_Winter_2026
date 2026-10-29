#include "PlayerEvadeState.h"
#include "PlayerIdleState.h"

namespace
{
	// 回避アクションの持続時間・減速関連
	constexpr float EVADE_DURATION = 0.45f;        // 回避の全体持続時間
	constexpr float EVADE_INVINCIBLE_TIME = 0.30f; // 回避開始からの無敵持続時間
	constexpr float EVADE_DECELERATION = 0.80f;    // 回避後半の減速率
}

void PlayerEvadeState::EnterState(PlayerActionFacade& action)
{
	action.SetInvincibleTimer(EVADE_INVINCIBLE_TIME);
	action.ExecuteEvadeStep();
	action.PlayEvadeAnimation();
}

std::shared_ptr<IPlayerState> PlayerEvadeState::UpdateState(
	PlayerActionFacade& action, float deltaTime)
{
	float currentTimer = action.GetStateTimer();

	if (currentTimer >= EVADE_INVINCIBLE_TIME)
	{
		action.DecelerateHorizontalMovement(EVADE_DECELERATION);
	}

	if (currentTimer >= EVADE_DURATION)
	{
		return std::make_shared<PlayerIdleState>();
	}

	return nullptr;
}

void PlayerEvadeState::ExitState(PlayerActionFacade& action)
{
}

IPlayerState::STATE_ID PlayerEvadeState::GetStateId(void) const
{
	return STATE_ID::EVADE;
}