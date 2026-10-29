#include "PlayerMoveState.h"
#include "PlayerIdleState.h"
#include "PlayerSprintState.h"
#include "PlayerJumpState.h"
#include "PlayerAttackComboState.h"
#include "PlayerEvadeState.h"

void PlayerMoveState::EnterState(PlayerActionFacade& action)
{
	action.PlayMoveAnimation();
}

std::shared_ptr<IPlayerState> PlayerMoveState::UpdateState(
	PlayerActionFacade& action, float deltaTime)
{
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

	if (!action.MoveNormal())
	{
		return std::make_shared<PlayerIdleState>();
	}

	if (action.CheckSprintHeld())
	{
		return std::make_shared<PlayerSprintState>();
	}

	return nullptr;
}

void PlayerMoveState::ExitState(PlayerActionFacade& action)
{
}

IPlayerState::STATE_ID PlayerMoveState::GetStateId(void) const
{
	return STATE_ID::MOVE;
}