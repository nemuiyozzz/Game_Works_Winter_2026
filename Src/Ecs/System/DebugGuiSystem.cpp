#include "../../Pch.h"
#include "DebugGuiSystem.h"
#include "../EcsRegistry.h"
#include "../Component/TransformComponent.h"
#include "../Component/VelocityComponent.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/CameraComponent.h"
#include "../../Engine/System/ExternalLoader/ActorDataLoader.h"
#include "../../Common/Quaternion.h"
#include "../../../Libs/ImGui/imgui.h"
#include "../../Engine/Utility/UtilityMath.h"

namespace
{
	// ウィンドウ初期配置関連
	constexpr float HIERARCHY_POS_X = 10.0f;   // Hierarchyの初期X座標
	constexpr float HIERARCHY_POS_Y = 10.0f;   // Hierarchyの初期Y座標
	constexpr float HIERARCHY_WIDTH = 220.0f;  // Hierarchyの初期横幅
	constexpr float HIERARCHY_HEIGHT = 300.0f; // Hierarchyの初期縦幅

	constexpr float INSPECTOR_POS_X = 240.0f;  // Inspectorの初期X座標
	constexpr float INSPECTOR_POS_Y = 10.0f;   // Inspectorの初期Y座標
	constexpr float INSPECTOR_WIDTH = 380.0f;  // Inspectorの初期横幅
	constexpr float INSPECTOR_HEIGHT = 450.0f; // Inspectorの初期縦幅
}

DebugGuiSystem::DebugGuiSystem(void)
	: selectedEntity_(NULL_ENTITY)
{
}

void DebugGuiSystem::Update(EcsRegistry& registry)
{
#ifdef _DEBUG
	auto entities = registry.GetEntitiesWith<DebugGuiComponent>();

	if (entities.empty())
	{
		return;
	}

	// まだ何も選択されていない、または選択中のエンティティが破棄された場合は先頭を選択する
	if (selectedEntity_ == NULL_ENTITY ||
		!registry.HasComponent<DebugGuiComponent>(selectedEntity_))
	{
		selectedEntity_ = entities.front();
	}

	// 1. Hierarchyウィンドウ（エンティティ一覧）を描画
	DrawHierarchyGui(registry, entities);

	// 2. Inspectorウィンドウ（選択中エンティティのパラメータ）を描画
	if (selectedEntity_ != NULL_ENTITY &&
		registry.HasComponent<DebugGuiComponent>(selectedEntity_))
	{
		DrawInspectorGui(registry, selectedEntity_);
	}
#endif // _DEBUG
}

void DebugGuiSystem::DrawHierarchyGui(
	EcsRegistry& registry, const std::vector<Entity>& entities)
{
	ImGui::SetNextWindowPos(
		ImVec2(HIERARCHY_POS_X, HIERARCHY_POS_Y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(
		ImVec2(HIERARCHY_WIDTH, HIERARCHY_HEIGHT), ImGuiCond_FirstUseEver);

	ImGui::Begin("Hierarchy");

	for (Entity entity : entities)
	{
		const DebugGuiComponent& debugGui =
			registry.GetComponent<DebugGuiComponent>(entity);

		bool isSelected = (selectedEntity_ == entity);

		ImGui::PushID(static_cast<int>(entity));
		if (ImGui::Selectable(debugGui.windowTitle.c_str(), isSelected))
		{
			selectedEntity_ = entity;
		}
		ImGui::PopID();
	}

	ImGui::End();
}

void DebugGuiSystem::DrawInspectorGui(EcsRegistry& registry, Entity entity)
{
	ImGui::SetNextWindowPos(
		ImVec2(INSPECTOR_POS_X, INSPECTOR_POS_Y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(
		ImVec2(INSPECTOR_WIDTH, INSPECTOR_HEIGHT), ImGuiCond_FirstUseEver);

	ImGui::Begin("Inspector");

	const DebugGuiComponent& debugGuiData =
		registry.GetComponent<DebugGuiComponent>(entity);

	ImGui::Text("Target : %s (Entity ID: %d)",
		debugGuiData.windowTitle.c_str(), static_cast<int>(entity));
	ImGui::Separator();

	if (registry.HasComponent<CameraComponent>(entity))
	{
		DrawCameraGui(registry, entity);
	}
	else if (registry.HasComponent<TransformComponent>(entity))
	{
		DrawTransformGui(registry, entity);
	}

	if (registry.HasComponent<PlayerInputComponent>(entity))
	{
		DrawPlayerMovementGui(registry, entity);
	}

	if (debugGuiData.saveType != DEBUG_SAVE_TYPE::NONE)
	{
		DrawSaveButtonGui(registry, entity, debugGuiData);
	}

	ImGui::End();
}

void DebugGuiSystem::DrawTransformGui(EcsRegistry& registry, Entity entity)
{
	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const float SLIDER_MIN_ROTATION = 0.0f;   // 回転スライダーの最小値
		const float SLIDER_MAX_ROTATION = 360.0f; // 回転スライダーの最大値

		ImGui::InputFloat3("Position", &transform.position.x);
		ImGui::InputFloat3("Scale", &transform.scale.x);

		float rotationDegree[3] = 
		{
			UtilityMath::Rad2DegF(transform.rotationEuler.x),
			UtilityMath::Rad2DegF(transform.rotationEuler.y),
			UtilityMath::Rad2DegF(transform.rotationEuler.z)
		};

		if (ImGui::SliderFloat3("Rotation(deg)", rotationDegree,
			SLIDER_MIN_ROTATION, SLIDER_MAX_ROTATION))
		{
			transform.rotationEuler.x = UtilityMath::Deg2RadF(rotationDegree[0]);
			transform.rotationEuler.y = UtilityMath::Deg2RadF(rotationDegree[1]);
			transform.rotationEuler.z = UtilityMath::Deg2RadF(rotationDegree[2]);
			transform.rotation_ = Quaternion::Euler(
				transform.rotationEuler.x,
				transform.rotationEuler.y,
				transform.rotationEuler.z);
		}
	}
}

void DebugGuiSystem::DrawPlayerMovementGui(EcsRegistry& registry, Entity entity)
{
	PlayerInputComponent& playerInput =
		registry.GetComponent<PlayerInputComponent>(entity);

	if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat("Move Speed", &playerInput.moveSpeed);
		ImGui::InputFloat("Jump Power", &playerInput.jumpPower);

		if (registry.HasComponent<VelocityComponent>(entity))
		{
			VelocityComponent& velocity = registry.GetComponent<VelocityComponent>(entity);
			ImGui::InputFloat("Max Speed", &velocity.maxSpeed);
		}
	}
}

void DebugGuiSystem::DrawCameraGui(EcsRegistry& registry, Entity entity)
{
	CameraComponent& cameraData = registry.GetComponent<CameraComponent>(entity);

	if (ImGui::CollapsingHeader("Camera Mode", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int currentMode = (cameraData.mode == CAMERA_MODE::TRACKING) ? 0 : 1;
		if (ImGui::RadioButton("Tracking", &currentMode, 0))
		{
			cameraData.mode = CAMERA_MODE::TRACKING;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Free", &currentMode, 1))
		{
			cameraData.mode = CAMERA_MODE::FREE;
		}
	}

	if (ImGui::CollapsingHeader("Tracking & Rotation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const float SLIDER_MIN_DISTANCE = 50.0f;    // 距離の最小値
		const float SLIDER_MAX_DISTANCE = 3000.0f;  // 距離の最大値
		const float SLIDER_MIN_ROT_SPEED = 0.001f;  // 回転速度の最小値
		const float SLIDER_MAX_ROT_SPEED = 0.2f;    // 回転速度の最大値
		const float SLIDER_MIN_FREE_SPEED = 0.5f;   // フリー移動速度の最小値
		const float SLIDER_MAX_FREE_SPEED = 50.0f;  // フリー移動速度の最大値
		const float SLIDER_MIN_ANGLE_H = -180.0f;   // 水平角度の最小値
		const float SLIDER_MAX_ANGLE_H = 180.0f;    // 水平角度の最大値
		const float SLIDER_MIN_ANGLE_V = -80.0f;    // 垂直角度の最小値
		const float SLIDER_MAX_ANGLE_V = 80.0f;     // 垂直角度の最大値

		if (registry.HasComponent<TransformComponent>(entity))
		{
			TransformComponent& cameraTransform =
				registry.GetComponent<TransformComponent>(entity);
			ImGui::InputFloat3("Camera Pos", &cameraTransform.position.x);
		}

		ImGui::InputFloat3("Target Offset", &cameraData.targetOffset.x);
		ImGui::SliderFloat("Distance", &cameraData.distance,
			SLIDER_MIN_DISTANCE, SLIDER_MAX_DISTANCE);
		ImGui::SliderFloat("Rotation Speed", &cameraData.rotationSpeed,
			SLIDER_MIN_ROT_SPEED, SLIDER_MAX_ROT_SPEED);
		ImGui::SliderFloat("Free Speed", &cameraData.freeMoveSpeed,
			SLIDER_MIN_FREE_SPEED, SLIDER_MAX_FREE_SPEED);

		float angleHDegree = UtilityMath::Deg2RadF(cameraData.angleH);
		if (ImGui::SliderFloat("Angle H(deg)", &angleHDegree,
			SLIDER_MIN_ANGLE_H, SLIDER_MAX_ANGLE_H))
		{
			cameraData.angleH = UtilityMath::Deg2RadF(angleHDegree);
		}

		float angleVDegree = UtilityMath::Deg2RadF(cameraData.angleV);
		if (ImGui::SliderFloat("Angle V(deg)", &angleVDegree,
			SLIDER_MIN_ANGLE_V, SLIDER_MAX_ANGLE_V))
		{
			cameraData.angleV = UtilityMath::Deg2RadF(angleVDegree);
		}
	}

	if (ImGui::CollapsingHeader("Lens & Clip"))
	{
		const float SLIDER_MIN_FOV = 30.0f;  // 視野角の最小値
		const float SLIDER_MAX_FOV = 120.0f; // 視野角の最大値

		float fovDegree = UtilityMath::Deg2RadF(cameraData.fov);
		if (ImGui::SliderFloat("FOV(deg)", &fovDegree, SLIDER_MIN_FOV, SLIDER_MAX_FOV))
		{
			cameraData.fov = UtilityMath::Deg2RadF(fovDegree);
		}

		ImGui::InputFloat("Near Z", &cameraData.nearZ);
		ImGui::InputFloat("Far Z", &cameraData.farZ);
	}
}

void DebugGuiSystem::DrawSaveButtonGui(
	EcsRegistry& registry, Entity entity, const DebugGuiComponent& debugGui)
{
	ImGui::Separator();

	if (debugGui.saveType == DEBUG_SAVE_TYPE::PLAYER)
	{
		if (ImGui::Button("Save Player Data"))
		{
			PlayerBinaryData saveData{};
			if (registry.HasComponent<TransformComponent>(entity))
			{
				saveData.scale = registry.GetComponent<TransformComponent>(entity).scale;
			}
			if (registry.HasComponent<PlayerInputComponent>(entity))
			{
				const auto& input = registry.GetComponent<PlayerInputComponent>(entity);
				saveData.moveSpeed = input.moveSpeed;
				saveData.jumpPower = input.jumpPower;
			}
			if (registry.HasComponent<VelocityComponent>(entity))
			{
				saveData.maxSpeed =
					registry.GetComponent<VelocityComponent>(entity).maxSpeed;
			}

			ActorDataLoader::SavePlayerData(
				debugGui.csvFilePath, debugGui.binaryFilePath, saveData);
		}
	}
	else if (debugGui.saveType == DEBUG_SAVE_TYPE::CAMERA)
	{
		if (ImGui::Button("Save Camera Data"))
		{
			if (registry.HasComponent<CameraComponent>(entity))
			{
				const auto& cameraData = registry.GetComponent<CameraComponent>(entity);
				CameraBinaryData saveData{};
				saveData.targetOffset = cameraData.targetOffset;
				saveData.distance = cameraData.distance;
				saveData.angleHorizontal = cameraData.angleH;
				saveData.angleVertical = cameraData.angleV;
				saveData.rotationSpeed = cameraData.rotationSpeed;
				saveData.freeMoveSpeed = cameraData.freeMoveSpeed;
				saveData.fov = cameraData.fov;
				saveData.nearZ = cameraData.nearZ;
				saveData.farZ = cameraData.farZ;

				ActorDataLoader::SaveCameraData(
					debugGui.csvFilePath, debugGui.binaryFilePath, saveData);
			}
		}
	}
}