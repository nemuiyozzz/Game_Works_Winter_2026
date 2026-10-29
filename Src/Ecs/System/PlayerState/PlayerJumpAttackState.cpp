#include "PlayerJumpAttackState.h"
#include "PlayerIdleState.h"

void PlayerJumpAttackState::EnterState(PlayerActionFacade& action)
{
	// ジャンプ力に対する急降下速度の倍率
	constexpr float JUMP_ATTACK_DIVE_RATIO = 1.5f;

	action.ExecuteDownwardDive(JUMP_ATTACK_DIVE_RATIO);
	action.SetSwordColliderActive(true);
	action.PlayAttackAnimation();
}

std::shared_ptr<IPlayerState> PlayerJumpAttackState::UpdateState(
	PlayerActionFacade& action, float deltaTime)
{
	// 着地後の硬直を含む最低持続秒数
	constexpr float LANDING_RECOVERY_TIME = 0.35f;

	if (action.CheckGrounded())
	{
		action.StopHorizontalMovement();

		if (action.GetStateTimer() >= LANDING_RECOVERY_TIME ||
			action.CheckAnimationEnded())
		{
			return std::make_shared<PlayerIdleState>();
		}
	}

	return nullptr;
}

void PlayerJumpAttackState::ExitState(PlayerActionFacade& action)
{
	action.SetSwordColliderActive(false);
}

IPlayerState::STATE_ID PlayerJumpAttackState::GetStateId(void) const
{
	return STATE_ID::JUMP_ATTACK;
}