#include "../../Pch.h"
#include "CameraSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/CameraComponent.h"
#include "../../Engine/System/Input/KeyConfInputManager.h"

namespace
{
	// カメラ関連
	constexpr float MOUSE_SENSITIVITY_SCALE = 0.01f;
	constexpr float MAX_ANGLE_VERTICAL = 80.0f * (DX_PI_F / 180.0f);
	constexpr float MIN_ANGLE_VERTICAL = -20.0f * (DX_PI_F / 180.0f);

	// フリーカメラ時は真上や真下を向けるように制限を緩和
	constexpr float FREE_MAX_ANGLE_VERTICAL = 3.14159265f / 2.0f - 0.01f;
	constexpr float FREE_MIN_ANGLE_VERTICAL = -3.14159265f / 2.0f + 0.01f;

	constexpr float DASH_SPEED_MULTIPLIER = 5.0f;
}

void CameraSystem::Update(EcsRegistry& registry)
{
	auto cameras = registry.GetEntitiesWith<TransformComponent, CameraComponent>();

	for (Entity cameraEntity : cameras)
	{
		TransformComponent& cameraTransform = registry.GetComponent<TransformComponent>(cameraEntity);
		CameraComponent& camera = registry.GetComponent<CameraComponent>(cameraEntity);

		if (camera.mode == CAMERA_MODE::TRACKING)
		{
			UpdateTrackingCamera(registry, cameraTransform, camera);
		}
		else if (camera.mode == CAMERA_MODE::FREE)
		{
			UpdateFreeCamera(cameraTransform, camera);
		}
	}
}

void CameraSystem::UpdateTrackingCamera(EcsRegistry& registry, TransformComponent& cameraTransform,
	CameraComponent& camera)
{
	VECTOR targetPosition = VGet(0.0f, 0.0f, 0.0f);

	if (camera.targetEntity != NULL_ENTITY &&
		registry.HasComponent<TransformComponent>(camera.targetEntity))
	{
		TransformComponent& targetTransformComponent = registry.GetComponent<TransformComponent>(camera.targetEntity);
		targetPosition = targetTransformComponent.position;
	}

	// マウスと右スティックの入力を取得
	Vector2F mouseMovement = KeyConfInputManager::GetInstance().GetMouseVelocityAndFixCenter();
	Vector2F rightStickInput = KeyConfInputManager::GetInstance().GetRIghtStick();

	// 入力量を統合
	float horizontalAngleAmount = (mouseMovement.x_ * MOUSE_SENSITIVITY_SCALE) + rightStickInput.x_;
	float verticalAngleAmount = (mouseMovement.y_ * MOUSE_SENSITIVITY_SCALE) + (-rightStickInput.y_);

	// 回転速度をかけて角度を更新
	camera.angleH += horizontalAngleAmount * camera.rotationSpeed;
	camera.angleV += verticalAngleAmount * camera.rotationSpeed;

	// 垂直角度の制限
	if (camera.angleV > MAX_ANGLE_VERTICAL)
	{
		camera.angleV = MAX_ANGLE_VERTICAL;
	}
	if (camera.angleV < MIN_ANGLE_VERTICAL)
	{
		camera.angleV = MIN_ANGLE_VERTICAL;
	}

	// 球面座標からXYZのオフセット座標へ変換
	float cosineVertical = std::cos(camera.angleV);
	float sineVertical = std::sin(camera.angleV);
	float cosineHorizontal = std::cos(camera.angleH);
	float sineHorizontal = std::sin(camera.angleH);

	VECTOR offsetPosition;
	offsetPosition.x = camera.distance * cosineVertical * sineHorizontal;
	offsetPosition.y = camera.distance * sineVertical;
	offsetPosition.z = camera.distance * cosineVertical * -cosineHorizontal;

	// カメラ座標と注視点の計算
	VECTOR lookAtPosition = VAdd(targetPosition, camera.targetOffset);
	cameraTransform.position = VAdd(lookAtPosition, offsetPosition);

	// DxLibのカメラ設定を更新
	SetupCamera_Perspective(camera.fov);
	SetCameraNearFar(camera.nearZ, camera.farZ);
	SetCameraPositionAndTarget_UpVecY(cameraTransform.position, lookAtPosition);
}

void CameraSystem::UpdateFreeCamera(TransformComponent& cameraTransform, CameraComponent& camera)
{
	// 視点操作（マウスと右スティック）
	Vector2F mouseMovement = KeyConfInputManager::GetInstance().GetMouseVelocityAndFixCenter();
	Vector2F rightStickInput = KeyConfInputManager::GetInstance().GetRIghtStick();

	float horizontalAngleAmount = (mouseMovement.x_ * MOUSE_SENSITIVITY_SCALE) + rightStickInput.x_;
	float verticalAngleAmount = (mouseMovement.y_ * MOUSE_SENSITIVITY_SCALE) + (-rightStickInput.y_);

	camera.angleH += horizontalAngleAmount * camera.rotationSpeed;
	camera.angleV += verticalAngleAmount * camera.rotationSpeed;

	if (camera.angleV > FREE_MAX_ANGLE_VERTICAL)
	{
		camera.angleV = FREE_MAX_ANGLE_VERTICAL;
	}
	if (camera.angleV < FREE_MIN_ANGLE_VERTICAL)
	{
		camera.angleV = FREE_MIN_ANGLE_VERTICAL;
	}

	float cosineVertical = std::cos(camera.angleV);
	float sineVertical = std::sin(camera.angleV);
	float cosineHorizontal = std::cos(camera.angleH);
	float sineHorizontal = std::sin(camera.angleH);

	VECTOR forwardVector = VGet(cosineVertical * sineHorizontal, sineVertical,
		cosineVertical * cosineHorizontal);
	VECTOR rightVector = VGet(cosineHorizontal, 0.0f, -sineHorizontal);
	VECTOR upVector = VGet(0.0f, 1.0f, 0.0f);

	VECTOR moveVector = VGet(0.0f, 0.0f, 0.0f);

	if (KeyConfInputManager::GetInstance().isPressed(L"CAMERA_UP"))
	{
		moveVector = VAdd(moveVector, forwardVector);
	}
	if (KeyConfInputManager::GetInstance().isPressed(L"CAMERA_DOWN"))
	{
		moveVector = VSub(moveVector, forwardVector);
	}
	if (KeyConfInputManager::GetInstance().isPressed(L"CAMERA_LEFT"))
	{
		moveVector = VAdd(moveVector, rightVector);
	}
	if (KeyConfInputManager::GetInstance().isPressed(L"CAMERA_RIGHT"))
	{
		moveVector = VSub(moveVector, rightVector);
	}
	if (CheckHitKey(KEY_INPUT_E) != 0)
	{
		moveVector = VAdd(moveVector, upVector);
	}
	if (CheckHitKey(KEY_INPUT_Q) != 0)
	{
		moveVector = VSub(moveVector, upVector);
	}

	float currentSpeed = camera.freeMoveSpeed;

	if (CheckHitKey(KEY_INPUT_LSHIFT) != 0)
	{
		currentSpeed *= DASH_SPEED_MULTIPLIER;
	}

	if (VSquareSize(moveVector) > 0.0f)
	{
		moveVector = VNorm(moveVector);
		moveVector = VScale(moveVector, currentSpeed);
		cameraTransform.position = VAdd(cameraTransform.position, moveVector);
	}

	SetupCamera_Perspective(camera.fov);
	SetCameraNearFar(camera.nearZ, camera.farZ);
	SetCameraPositionAndAngle(cameraTransform.position, camera.angleV, camera.angleH, 0.0f);
}