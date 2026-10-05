#include "../../Pch.h"
#include "PlayerControlSystem.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/VelocityComponent.h"
#include "../../System/Input/KeyConfInputManager.h"

void PlayerControlSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<PlayerInputComponent, VelocityComponent>();

	for (Entity entity : entities)
	{
		UpdatePlayerControl(registry, entity);
	}
}

void PlayerControlSystem::UpdatePlayerControl(EcsRegistry& reigstry, Entity entity)
{
	if (!reigstry.HasComponent<PlayerInputComponent>(entity) ||
		!reigstry.HasComponent<VelocityComponent>(entity))
	{
		return;
	}

	PlayerInputComponent& inputInfo = reigstry.GetComponent<PlayerInputComponent>(entity);
	VelocityComponent& velocityData = reigstry.GetComponent<VelocityComponent>(entity);

	VECTOR moveDirection = KeyConfInputManager::GetInstance().GetLeftStickDirection();
	constexpr float MIN_DIRECTION = 0.0f;

	if (VSize(moveDirection) > MIN_DIRECTION)
	{
		VECTOR moveVelocity = VScale(moveDirection, inputInfo.moveSpeed_);

		velocityData.velocity_.x = moveDirection.x;
		velocityData.velocity_.z = moveDirection.z;
	}
	else
	{
		constexpr float INITIALIZE_SPEED = 0.0f;

		velocityData.velocity_.x = INITIALIZE_SPEED;
		velocityData.velocity_.z = INITIALIZE_SPEED;
	}

	if (VSize(moveDirection) == 0.0f)
	{
		float x = 0.0f;
		float z = 0.0f;

		if (KeyConfInputManager::GetInstance().isPressed(L"UP"))    z += 1.0f;
		if (KeyConfInputManager::GetInstance().isPressed(L"DOWN"))  z -= 1.0f;
		if (KeyConfInputManager::GetInstance().isPressed(L"RIGHT")) x += 1.0f;
		if (KeyConfInputManager::GetInstance().isPressed(L"LEFT"))  x -= 1.0f;

		VECTOR wasdDir = VGet(x, 0.0f, z);

		if (VSize(wasdDir) > 0.0f)
		{
			moveDirection = UtilityMath::VNormalize(wasdDir);
		}
	}

	if (VSize(moveDirection) > MIN_DIRECTION)
	{
		VECTOR moveVelocity = VScale(moveDirection, inputInfo.moveSpeed_);

		velocityData.velocity_.x = moveVelocity.x;
		velocityData.velocity_.z = moveVelocity.z;
	}
	else
	{
		constexpr float INITIALIZE_SPEED = 0.0f;

		velocityData.velocity_.x = INITIALIZE_SPEED;
		velocityData.velocity_.z = INITIALIZE_SPEED;
	}

	if (KeyConfInputManager::GetInstance().isTrigerDown(L"JUMP"))
	{
		velocityData.velocity_.y = inputInfo.jumpPower_;
	}
}
