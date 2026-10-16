#include "ActorFactory.h"
#include "EcsRegistry.h"
#include "../Application.h"
#include "../Engine/System/Resource/ResourceManager.h"
#include "../Engine/System/ExternalLoader/ActorDataLoader.h"
#include "Component/TransformComponent.h"
#include "Component/VelocityComponent.h"
#include "Component/PlayerInputComponent.h"
#include "Component/ModelComponent.h"
#include "Component/CameraComponent.h"
#include "Component/DebugGuiComponent.h"

namespace
{
	// ファイル名関連
	constexpr const wchar_t* PLAYER_CSV_NAME = L"PlayerParam.csv"; // プレイヤーCSV名
	constexpr const wchar_t* PLAYER_BIN_NAME = L"PlayerParam.bin"; // プレイヤーバイナリ名
	constexpr const wchar_t* CAMERA_CSV_NAME = L"CameraParam.csv"; // カメラCSV名
	constexpr const wchar_t* CAMERA_BIN_NAME = L"CameraParam.bin"; // カメラバイナリ名

	// リソースキー名
	constexpr const wchar_t* KEY_MODEL_PLAYER = L"MODEL_SAMPLE";
}

Entity ActorFactory::CreatePlayer(EcsRegistry& registry)
{
	PlayerBinaryData playerParam{};
	std::wstring csvPath = std::wstring(Application::PATH_CSV) + PLAYER_CSV_NAME;
	std::wstring binPath = std::wstring(Application::PATH_BINARY) + PLAYER_BIN_NAME;

	ActorDataLoader::LoadPlayerData(csvPath, binPath, playerParam);

	Entity playerEntity = registry.CreateEntity();

	TransformComponent playerTransform{};
	playerTransform.scale = playerParam.scale;
	registry.AddComponent<TransformComponent>(playerEntity, playerTransform);

	VelocityComponent playerVelocity{};
	playerVelocity.maxSpeed = playerParam.maxSpeed;
	registry.AddComponent<VelocityComponent>(playerEntity, playerVelocity);

	PlayerInputComponent playerInput{};
	playerInput.moveSpeed = playerParam.moveSpeed;
	playerInput.jumpPower = playerParam.jumpPower;
	registry.AddComponent<PlayerInputComponent>(playerEntity, playerInput);

	ModelComponent model{};
	model.modelHandle = ResourceManager::GetInstance().GetHandleId(KEY_MODEL_PLAYER);
	registry.AddComponent<ModelComponent>(playerEntity, model);

#ifdef _DEBUG
	DebugGuiComponent playerGui{};
	playerGui.windowTitle = "Player";
	playerGui.saveType = DEBUG_SAVE_TYPE::PLAYER;
	playerGui.csvFilePath = csvPath;
	playerGui.binaryFilePath = binPath;
	registry.AddComponent<DebugGuiComponent>(playerEntity, playerGui);
#endif // _DEBUG

	return playerEntity;
}

Entity ActorFactory::CreateCamera(EcsRegistry& registry, Entity targetEntity)
{
	CameraBinaryData cameraParam{};
	std::wstring csvPath = std::wstring(Application::PATH_CSV) + CAMERA_CSV_NAME;
	std::wstring binPath = std::wstring(Application::PATH_BINARY) + CAMERA_BIN_NAME;

	ActorDataLoader::LoadCameraData(csvPath, binPath, cameraParam);

	Entity cameraEntity = registry.CreateEntity();
	registry.AddComponent<TransformComponent>(cameraEntity, {});

	CameraComponent cameraData{};
	cameraData.targetEntity = targetEntity;
	cameraData.mode = CAMERA_MODE::TRACKING;
	cameraData.targetOffset = cameraParam.targetOffset;
	cameraData.distance = cameraParam.distance;
	cameraData.angleH = cameraParam.angleHorizontal;
	cameraData.angleV = cameraParam.angleVertical;
	cameraData.rotationSpeed = cameraParam.rotationSpeed;
	cameraData.freeMoveSpeed = cameraParam.freeMoveSpeed;
	cameraData.fov = cameraParam.fov;
	cameraData.nearZ = cameraParam.nearZ;
	cameraData.farZ = cameraParam.farZ;
	registry.AddComponent<CameraComponent>(cameraEntity, cameraData);

#ifdef _DEBUG
	DebugGuiComponent cameraGui{};
	cameraGui.windowTitle = "Camera";
	cameraGui.saveType = DEBUG_SAVE_TYPE::CAMERA;
	cameraGui.csvFilePath = csvPath;
	cameraGui.binaryFilePath = binPath;
	registry.AddComponent<DebugGuiComponent>(cameraEntity, cameraGui);
#endif // _DEBUG

	return cameraEntity;
}