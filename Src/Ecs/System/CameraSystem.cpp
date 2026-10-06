#include "../../Pch.h"
#include "CameraSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/CameraComponent.h"
#include "../../System/Input/KeyConfInputManager.h"

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

		// マウスと右スティックの入力を取得
		Vector2F mouseDelta = KeyConfInputManager::GetInstance().GetMouseVelocityAndFixCenter();
		Vector2F rightStick = KeyConfInputManager::GetInstance().GetRIghtStick();

		// 入力量を統合
		constexpr float MOUSE_SENSITIVITY_SCALE = 0.01f;
		float deltaH = (mouseDelta.x * MOUSE_SENSITIVITY_SCALE) + rightStick.x;
		float deltaV = (mouseDelta.y * MOUSE_SENSITIVITY_SCALE) + (-rightStick.y);

		// 回転速度をかけて角度を更新
		camera.angleH_ += deltaH * camera.rotationSpeed_;
		camera.angleV_ += deltaV * camera.rotationSpeed_;

		// 垂直角度の制限
		constexpr float MAX_ANGLE_V = 80.0f * (DX_PI_F / 180.0f);
		constexpr float MIN_ANGLE_V = -20.0f * (DX_PI_F / 180.0f);
		if (camera.angleV_ > MAX_ANGLE_V) camera.angleV_ = MAX_ANGLE_V;
		if (camera.angleV_ < MIN_ANGLE_V) camera.angleV_ = MIN_ANGLE_V;

		// 球面座標からXYZのオフセット座標へ変換
		float cosV = cosf(camera.angleV_);
		float sinV = sinf(camera.angleV_);
		float cosH = cosf(camera.angleH_);
		float sinH = sinf(camera.angleH_);

		VECTOR offset;
		offset.x = camera.distance_ * cosV * sinH;
		offset.y = camera.distance_ * sinV;
		offset.z = camera.distance_ * cosV * -cosH;

		// カメラ座標と注視点の計算
		VECTOR lookAtPos = VAdd(targetPos, camera.targetOffset_);
		camraTrans.position_ = VAdd(lookAtPos, offset);

		// DxLibのカメラ設定を更新
		SetupCamera_Perspective(camera.fov_);
		SetCameraNearFar(camera.nearZ_, camera.farZ_);
		SetCameraPositionAndTarget_UpVecY(camraTrans.position_, lookAtPos);
	}
}