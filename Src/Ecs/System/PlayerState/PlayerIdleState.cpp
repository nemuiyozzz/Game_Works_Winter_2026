#include "PlayerIdleState.h"
#include "PlayerMoveState.h"
#include "PlayerSprintState.h"
#include "PlayerJumpState.h"
#include "PlayerAttackComboState.h"
#include "PlayerEvadeState.h"

void PlayerIdleState::EnterState(PlayerActionFacade& action)
{
	action.StopHorizontalMovement();
	action.PlayIdleAnimation();
}

std::shared_ptr<IPlayerState> PlayerIdleState::UpdateState(PlayerActionFacade& action,
	float detaTime)
{
	action.StopHorizontalMovement();

	if (action.CheckEvadeTriggered())
	{
		if (action.TryConsumeStamina(PlayerActionFacade::COST_EVADE))
		{
			return std::make_shared<PlayerEvadeState>();
		}
	}

	if (action.CheckAttackTriggered())
	{
		if (action.TryConsumeStamina(PlayerActionFacade::COST_ATTACK))
		{
			return std::make_shared<PlayerAttackComboState>(0);
		}
	}

	if (action.CheckGrounded() && action.CheckJumpTriggered())
	{
		if (action.TryConsumeStamina(PlayerActionFacade::COST_JUMP))
		{
			return std::make_shared<PlayerJumpState>();
		}
	}

	if (action.CheckMoveInput())
	{
		if (action.CheckSprintHeld())
		{
			return std::make_shared<PlayerSprintState>();
		}
		return std::make_shared<PlayerMoveState>();
	}

	return nullptr;
}

void PlayerIdleState::ExitState(PlayerActionFacade& action)
{
}

IPlayerState::STATE_ID PlayerIdleState::GetStateId(void) const
{
	return STATE_ID::IDLE;
}
