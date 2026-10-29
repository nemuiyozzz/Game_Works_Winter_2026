#include "PlayerSprintState.h"
#include "PlayerIdleState.h"
#include "PlayerMoveState.h"
#include "PlayerJumpState.h"
#include "PlayerAttackComboState.h"
#include "PlayerEvadeState.h"

void PlayerSprintState::EnterState(PlayerActionFacade& action)
{
	action.PlaySprintAnimation();
}

std::shared_ptr<IPlayerState> PlayerSprintState::UpdateState(
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

	if (!action.CheckSprintHeld())
	{
		return std::make_shared<PlayerMoveState>();
	}

	// 交戦中のみスタミナを消費する
	if (action.CheckInCombat())
	{
		float sprintStaminaCost =
			PlayerActionFacade::COST_SPRINT_PER_SECOND * deltaTime; 
		if (!action.TryConsumeStamina(sprintStaminaCost))
		{
			return std::make_shared<PlayerMoveState>();
		}
	}

	if (!action.MoveSprint())
	{
		return std::make_shared<PlayerIdleState>();
	}

	return nullptr;
}

void PlayerSprintState::ExitState(PlayerActionFacade& action)
{
}

IPlayerState::STATE_ID PlayerSprintState::GetStateId(void) const
{
	return STATE_ID::SPRINT;
}