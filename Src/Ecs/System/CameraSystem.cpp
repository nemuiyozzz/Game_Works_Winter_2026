#include "../../Pch.h"
#include "CameraSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/CameraComponent.h"

void CameraSystem::Update(EcsRegistry& registry)
{
	auto cameras = registry.GetEntitiesWith<TransformComponent, CameraComponent>();

	for (Entity cameraEntity : cameras)
	{
		TransformComponent& camraTrans = registry.GetComponent<TransformComponent>(cameraEntity);
		CameraComponent& camera = registry.GetComponent<CameraComponent>(cameraEntity);
		VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

		if (camera.targetEntity_ != NULL_ENTITY &&
			registry.HasComponent<TransformComponent>(camera.targetEntity_))
		{
			TransformComponent& targetTransform = registry.GetComponent<TransformComponent>(camera.targetEntity_);

			targetPos = targetTransform.position_;
		}

		VECTOR lookAtPos = VAdd(targetPos, camera.targetOffset_);

		camraTrans.position_ = VAdd(targetPos, camera.offset_);

		SetupCamera_Perspective(camera.fov_);
		SetCameraNearFar(camera.nearZ_, camera.farZ_);
		SetCameraPositionAndTarget_UpVecY(camraTrans.position_, lookAtPos);
	}
}
