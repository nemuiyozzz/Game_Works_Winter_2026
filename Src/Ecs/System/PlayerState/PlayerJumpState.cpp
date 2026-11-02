#include "PlayerJumpState.h"
#include "PlayerIdleState.h"
#include "PlayerJumpAttackState.h"

void PlayerJumpState::EnterState(PlayerActionFacade& action)
{
	action.ExecuteJumpVelocity();
	action.PlayJumpAnimation();
}

std::shared_ptr<IPlayerState> PlayerJumpState::UpdateState(
	PlayerActionFacade& action, float deltaTime)
{
	// 離陸直後の着地誤判定を防ぐ最低滞空秒数
	constexpr float MIN_AIR_TIME = 0.1f;

	action.MoveInAir();

	if (action.CheckAttackTriggered())
	{
		if (action.TryConsumeStamina(PlayerActionFacade::COST_JUMP_ATTACK))
		{
			return std::make_shared<PlayerJumpAttackState>();
		}
	}

	if (action.GetStateTimer() > MIN_AIR_TIME && action.CheckGrounded())
	{
		return std::make_shared<PlayerIdleState>();
	}

	return nullptr;
}

void PlayerJumpState::ExitState(PlayerActionFacade& action)
{
}

IPlayerState::STATE_ID PlayerJumpState::GetStateId(void) const
{
	return STATE_ID::JUMP;
}