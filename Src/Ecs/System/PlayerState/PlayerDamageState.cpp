#include "PlayerDamageState.h"
#include "PlayerIdleState.h"
#include "PlayerActionFacade.h"
#include "../../System/PlayerControlSystem.h"

void PlayerDamageState::EnterState(PlayerActionFacade& action)
{
	action.SetWeaponColliderActive(PlayerActionFacade::ATTACK_HAND::RIGHT, false);
	action.SetWeaponColliderActive(PlayerActionFacade::ATTACK_HAND::LEFT, false);

	action.PlayDamageAnimation();

	action.ExecuteForwardStep(-1.5f);

	action.SetInvincibleTimer(1.5f);
}

std::shared_ptr<IPlayerState> PlayerDamageState::UpdateState(
	PlayerActionFacade& action, float deltaTime)
{
	action.DecelerateHorizontalMovement(0.9f);


	if (action.CheckAnimationEnded())
	{
		return std::make_shared<PlayerIdleState>();
	}

	return nullptr;
}

void PlayerDamageState::ExitState(PlayerActionFacade& action)
{
	action.StopHorizontalMovement();
}