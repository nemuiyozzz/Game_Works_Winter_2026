#include "../../Pch.h"
#include "PlayerControlSystem.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/VelocityComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/TransformComponent.h"
#include "../../Engine/System/Input/KeyConfInputManager.h"
#include "../../Engine/Utility/UtilityMath.h"
#include "AnimationSystem.h"

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

	// 移動・回転処理
	if (VSize(moveDirection) > 0.0f)
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

		rotatedDir.x = moveDirection.x * cosH - moveDirection.z * sinH;
		rotatedDir.y = 0.0f;
		rotatedDir.z = moveDirection.x * sinH + moveDirection.z * cosH;
		rotatedDir = UtilityMath::VNormalize(rotatedDir);

		VECTOR moveVelocity = VScale(rotatedDir, inputInfo.moveSpeed);
		velocityData.velocity.x = moveVelocity.x;
		velocityData.velocity.z = moveVelocity.z;

		constexpr float ROTATION_SMOOTHNESS = 0.2f;
		float targetAngle = atan2f(rotatedDir.x, rotatedDir.z) + DX_PI_F;

		float currentAngle = transform.rotationEuler.y;
		float diffAngle = targetAngle - currentAngle;
		float shortestAngle = atan2f(sinf(diffAngle), cosf(diffAngle));
		float newAngle = currentAngle + (shortestAngle * ROTATION_SMOOTHNESS);

		transform.rotationEuler = VGet(0.0f, newAngle, 0.0f);
		transform.rotation_ = Quaternion::Euler(0.0f, newAngle, 0.0f);
	}
	else
	{
		velocityData.velocity.x = 0.0f;
		velocityData.velocity.z = 0.0f;
	}

	// アニメーションとアクション処理
	if (KeyConfInputManager::GetInstance().isTrigerDown(L"ATTACK_NORMAL"))
	{
		AnimationSystem::Play(registry, entity, static_cast<int>(PLAYER_ANIM::ATTACK), false, 0.1f);
	}
	else if (KeyConfInputManager::GetInstance().isTrigerDown(L"JUMP"))
	{
		velocityData.velocity.y = inputInfo.jumpPower;
		AnimationSystem::Play(registry, entity, static_cast<int>(PLAYER_ANIM::JUMP), false, 0.2f);
	}
	else
	{
		int currentAnim = AnimationSystem::GetPlayType(registry, entity);
		bool isActionFinished = AnimationSystem::IsEnd(registry, entity);

		if (currentAnim == -1 ||
			currentAnim == static_cast<int>(PLAYER_ANIM::IDLE) ||
			currentAnim == static_cast<int>(PLAYER_ANIM::MOVE) ||
			isActionFinished)
		{
			if (VSize(moveDirection) > 0.0f)
			{
				AnimationSystem::Play(registry, entity, static_cast<int>(PLAYER_ANIM::MOVE), true, 0.15f);
			}
			else
			{
				AnimationSystem::Play(registry, entity, static_cast<int>(PLAYER_ANIM::IDLE), true, 0.15f);
			}
		}
	}
}