#include "../../Pch.h"
#include "PlayerControlSystem.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/VelocityComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/TransformComponent.h"
#include "../../Engine/System/Input/KeyConfInputManager.h"
#include "../../Engine/Utility/UtilityMath.h"

void PlayerControlSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<PlayerInputComponent, VelocityComponent>();

	for (Entity entity : entities)
	{
		UpdatePlayerControl(registry, entity);
	}
}

void PlayerControlSystem::UpdatePlayerControl(EcsRegistry& registry, Entity entity)
{
	if (!registry.HasComponent<PlayerInputComponent>(entity) ||
		!registry.HasComponent<VelocityComponent>(entity) ||
		!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	PlayerInputComponent& inputInfo = registry.GetComponent<PlayerInputComponent>(entity);
	VelocityComponent& velocityData = registry.GetComponent<VelocityComponent>(entity);
	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

	VECTOR moveDirection = KeyConfInputManager::GetInstance().GetLeftStickDirection();
	constexpr float MIN_DIRECTION = 0.0f;

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
		float cameraAngleH = 0.0f;
		auto cameras = registry.GetEntitiesWith<CameraComponent>();

		if (!cameras.empty())
		{
			CameraComponent& camera = registry.GetComponent<CameraComponent>(cameras[0]);
			
			cameraAngleH = camera.angleH;
		}

		float cosH = cosf(cameraAngleH);
		float sinH = sinf(cameraAngleH);
		VECTOR rotatedDir;

		rotatedDir.x = moveDirection.x * cosH + moveDirection.z * sinH;
		rotatedDir.y = MIN_DIRECTION;
		rotatedDir.z = -moveDirection.x * sinH + moveDirection.z * cosH;
		rotatedDir = UtilityMath::VNormalize(rotatedDir);

		VECTOR moveVelocity = VScale(moveDirection, inputInfo.moveSpeed);

		velocityData.velocity.x = moveVelocity.x;
		velocityData.velocity.z = moveVelocity.z;

		constexpr float ROTATION_SMOOTHNESS = 0.2f;
		float targetAngle = atan2f(rotatedDir.x, rotatedDir.z);

		targetAngle += DX_PI_F;

		float currentAngle = transform.rotationEuler.y;
		float diffAngle = targetAngle - currentAngle;
		float shortestAngle = atan2f(sinf(diffAngle), cosf(diffAngle));
		float newAngle = currentAngle + (shortestAngle * ROTATION_SMOOTHNESS);

		transform.rotationEuler = VGet(MIN_DIRECTION, newAngle, MIN_DIRECTION);
		transform.rotation_ = Quaternion::Euler(MIN_DIRECTION, newAngle, MIN_DIRECTION);
	}
	else
	{
		constexpr float INITIALIZE_SPEED = 0.0f;

		velocityData.velocity.x = INITIALIZE_SPEED;
		velocityData.velocity.z = INITIALIZE_SPEED;
	}

	if (KeyConfInputManager::GetInstance().isTrigerDown(L"JUMP"))
	{
		velocityData.velocity.y = inputInfo.jumpPower;
	}
}
