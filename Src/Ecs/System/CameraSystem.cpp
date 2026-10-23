#include "../../Pch.h"
#include "CameraSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/CameraComponent.h"
#include "../../Engine/System/Input/KeyConfInputManager.h"
#include "../../../Libs/ImGui/imgui.h"

namespace
{
	// カメラ角度・感度関連
	constexpr float MOUSE_SENSITIVITY_SCALE = 0.01f;                // マウス感度の基本倍率
	constexpr float MAX_ANGLE_VERTICAL = 80.0f * (DX_PI_F / 180.0f); // 追従時の最大仰角
	constexpr float MIN_ANGLE_VERTICAL = -20.0f * (DX_PI_F / 180.0f);// 追従時の最小俯角

	// フリーカメラ角度制限関連
	constexpr float FREE_MAX_ANGLE_VERTICAL = DX_PI_F / 2.0f - 0.01f;  // フリー時の最大角度
	constexpr float FREE_MIN_ANGLE_VERTICAL = -DX_PI_F / 2.0f + 0.01f; // フリー時の最小角度

	// フリーカメラ移動・ズーム倍率関連
	constexpr float DASH_SPEED_MULTIPLIER = 5.0f;   // Shift押下時の加速倍率
	constexpr float WHEEL_ZOOM_MULTIPLIER = 25.0f;  // ホイール回転時のズーム速度倍率
	constexpr float MIDDLE_PAN_MULTIPLIER = 0.5f;   // 中ボタンドラッグ時の平行移動倍率
}

void CameraSystem::Update(EcsRegistry& registry)
{
	auto cameras = registry.GetEntitiesWith<TransformComponent, CameraComponent>();

	for (Entity cameraEntity : cameras)
	{
		TransformComponent& cameraTransform =
			registry.GetComponent<TransformComponent>(cameraEntity);
		CameraComponent& camera =
			registry.GetComponent<CameraComponent>(cameraEntity);

		if (KeyConfInputManager::GetInstance().isTrigerDown(L"DEBUG_GUI"))
		{
			camera.isCursorLocked = false;
			SetMouseDispFlag(true);
		}

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

void CameraSystem::UpdateTrackingCamera(EcsRegistry& registry,
	TransformComponent& cameraTransform, CameraComponent& camera)
{
	Vector2F mouseMovement = { 0.0f, 0.0f };

	if (camera.isCursorLocked)
	{
		mouseMovement =
			KeyConfInputManager::GetInstance().GetMouseVelocityAndFixCenter();
	}

	Vector2F rightStickInput = KeyConfInputManager::GetInstance().GetRIghtStick();
	VECTOR targetPosition = VGet(0.0f, 0.0f, 0.0f);

	if (camera.targetEntity != NULL_ENTITY &&
		registry.HasComponent<TransformComponent>(camera.targetEntity))
	{
		TransformComponent& targetTransformComponent =
			registry.GetComponent<TransformComponent>(camera.targetEntity);
		targetPosition = targetTransformComponent.position;
	}

	// 入力量を統合
	float horizontalAngleAmount =
		-(mouseMovement.x_ * MOUSE_SENSITIVITY_SCALE) + rightStickInput.x_;
	float verticalAngleAmount =
		(mouseMovement.y_ * MOUSE_SENSITIVITY_SCALE) + (-rightStickInput.y_);

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

	VECTOR offsetPosition{};
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

void CameraSystem::UpdateFreeCamera(TransformComponent& cameraTransform,
	CameraComponent& camera)
{
	auto& inputManager = KeyConfInputManager::GetInstance();

	// カーソル非固定時でもドラッグ操作ができるよう、前フレームとのマウス座標差分を計算
	static Vector2 previousMousePos = inputManager.GetMousePosition();
	Vector2 currentMousePos = inputManager.GetMousePosition();
	Vector2F mouseDragDelta =
	{
		static_cast<float>(currentMousePos.x_ - previousMousePos.x_),
		static_cast<float>(currentMousePos.y_ - previousMousePos.y_)
	};
	previousMousePos = currentMousePos;

	// ImGuiのウィンドウ上にマウスがあるかどうかを判定
	bool isImGuiCapturingMouse = false;
	if (ImGui::GetCurrentContext() != nullptr)
	{
		isImGuiCapturingMouse = ImGui::GetIO().WantCaptureMouse;
	}

	const int mouseButtonState = GetMouseInput();
	const bool isRightMouseDown = (mouseButtonState & MOUSE_INPUT_RIGHT) != 0;
	const bool isMiddleMouseDown = (mouseButtonState & MOUSE_INPUT_MIDDLE) != 0;

	// 視点回転：カーソルロック中、または右クリックドラッグ中に視点を回す
	Vector2F lookMouseMovement = { 0.0f, 0.0f };
	if (camera.isCursorLocked)
	{
		lookMouseMovement = inputManager.GetMouseVelocityAndFixCenter();
		previousMousePos = inputManager.GetMousePosition();
	}
	else if (!isImGuiCapturingMouse && isRightMouseDown)
	{
		lookMouseMovement = mouseDragDelta;
	}

	Vector2F rightStickInput = inputManager.GetRIghtStick();

	float horizontalAngleAmount =
		(lookMouseMovement.x_ * MOUSE_SENSITIVITY_SCALE) + rightStickInput.x_;
	float verticalAngleAmount =
		(lookMouseMovement.y_ * MOUSE_SENSITIVITY_SCALE) + (-rightStickInput.y_);

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

	// SetCameraPositionAndAngleの回転仕様に一致する前方・右方・上方ベクトルを算出
	float cosineVertical = std::cos(camera.angleV);
	float sineVertical = std::sin(camera.angleV);
	float cosineHorizontal = std::cos(camera.angleH);
	float sineHorizontal = std::sin(camera.angleH);

	VECTOR forwardVector = VGet(
		cosineVertical * sineHorizontal,
		-sineVertical,
		cosineVertical * cosineHorizontal);
	VECTOR rightVector = VGet(cosineHorizontal, 0.0f, -sineHorizontal);
	VECTOR cameraUpVector = VCross(forwardVector, rightVector);
	VECTOR worldUpVector = VGet(0.0f, 1.0f, 0.0f);

	// Shiftキー押下時は移動スピードを加速
	float currentSpeed = camera.freeMoveSpeed;
	if (CheckHitKey(KEY_INPUT_LSHIFT) != 0)
	{
		currentSpeed *= DASH_SPEED_MULTIPLIER;
	}

	if (!isImGuiCapturingMouse)
	{
		// マウスホイール回転：向いている方向へ拡大・縮小
		int wheelRotation = inputManager.GetMouseWheel();
		if (wheelRotation != 0)
		{
			float zoomDistance =
				static_cast<float>(wheelRotation) * currentSpeed * WHEEL_ZOOM_MULTIPLIER;
			cameraTransform.position = VAdd(
				cameraTransform.position, VScale(forwardVector, zoomDistance));
		}

		// 中ボタンドラッグ：カメラ平面に沿った平行移動
		if (isMiddleMouseDown)
		{
			float panSpeed = currentSpeed * MIDDLE_PAN_MULTIPLIER;
			VECTOR panRight = VScale(rightVector, -mouseDragDelta.x_ * panSpeed);
			VECTOR panUp = VScale(cameraUpVector, mouseDragDelta.y_ * panSpeed);
			cameraTransform.position = VAdd(
				cameraTransform.position, VAdd(panRight, panUp));
		}
	}

	// キーボード移動
	VECTOR moveVector = VGet(0.0f, 0.0f, 0.0f);

	if (inputManager.isPressed(L"CAMERA_UP") ||
		(isRightMouseDown && inputManager.isPressed(L"UP")))
	{
		moveVector = VAdd(moveVector, forwardVector);
	}
	if (inputManager.isPressed(L"CAMERA_DOWN") ||
		(isRightMouseDown && inputManager.isPressed(L"DOWN")))
	{
		moveVector = VSub(moveVector, forwardVector);
	}
	if (inputManager.isPressed(L"CAMERA_RIGHT") ||
		(isRightMouseDown && inputManager.isPressed(L"RIGHT")))
	{
		moveVector = VAdd(moveVector, rightVector);
	}
	if (inputManager.isPressed(L"CAMERA_LEFT") ||
		(isRightMouseDown && inputManager.isPressed(L"LEFT")))
	{
		moveVector = VSub(moveVector, rightVector);
	}
	if (CheckHitKey(KEY_INPUT_E) != 0)
	{
		moveVector = VAdd(moveVector, worldUpVector);
	}
	if (CheckHitKey(KEY_INPUT_Q) != 0)
	{
		moveVector = VSub(moveVector, worldUpVector);
	}

	if (VSquareSize(moveVector) > 0.0f)
	{
		moveVector = VNorm(moveVector);
		moveVector = VScale(moveVector, currentSpeed);
		cameraTransform.position = VAdd(cameraTransform.position, moveVector);
	}

	SetupCamera_Perspective(camera.fov);
	SetCameraNearFar(camera.nearZ, camera.farZ);
	SetCameraPositionAndAngle(
		cameraTransform.position, camera.angleV, camera.angleH, 0.0f);
}