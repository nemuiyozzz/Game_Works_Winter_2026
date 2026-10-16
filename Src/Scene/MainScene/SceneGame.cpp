#include "../../Pch.h"
#include "SceneGame.h"
#include "../../Application.h"
#include "../../Engine/System/Resource/ResourceManager.h"
#include "../../Ecs/Component/TransformComponent.h"
#include "../../Ecs/Component/VelocityComponent.h"
#include "../../Ecs/Component/PlayerInputComponent.h"
#include "../../Ecs/Component/ModelComponent.h"
#include "../../Ecs/Component/CameraComponent.h"
#include "../../Ecs/Component/DebugGuiComponent.h"
#include "../../Ecs/System/PlayerControlSystem.h"
#include "../../Ecs/System/MovementSystem.h"
#include "../../Ecs/System/TransformSystem.h"
#include "../../Ecs/System/CameraSystem.h"
#include "../../Ecs/System/DebugGuiSystem.h"
#include "../Stage/StageLobby.h"
#include "../Stage/StageBattle.h"
#include "../Stage/StageTraining.h"
#include "../../Engine/System/ExternalLoader/ActorDataLoader.h"

namespace
{
	// ファイル名関連
	constexpr const wchar_t* PLAYER_CSV_NAME = L"PlayerParam.csv"; // プレイヤーCSV名
	constexpr const wchar_t* PLAYER_BIN_NAME = L"PlayerParam.bin"; // プレイヤーバイナリ名
	constexpr const wchar_t* CAMERA_CSV_NAME = L"CameraParam.csv"; // カメラCSV名
	constexpr const wchar_t* CAMERA_BIN_NAME = L"CameraParam.bin"; // カメラバイナリ名
}

SceneGame::SceneGame(void)
	: playerEntity_(NULL_ENTITY)
	, cameraEntity_(NULL_ENTITY)
{
}

SceneGame::~SceneGame(void)
{
}

void SceneGame::Load(void)
{
	SceneBase::Load();

	ResourceManager::GetInstance().Load(ResourceManager::RESOURCE_ID::MODEL_SAMPLE);
}

void SceneGame::Initialize(void)
{
	SceneBase::Initialize();

	updateSystems_.push_back(std::make_shared<PlayerControlSystem>());
	updateSystems_.push_back(std::make_shared<MovementSystem>());
	updateSystems_.push_back(std::make_shared<TransformSystem>());
	updateSystems_.push_back(std::make_shared<CameraSystem>());
#ifdef _DEBUG
	updateSystems_.push_back(std::make_shared<DebugGuiSystem>());
#endif // _DEBUG

	modelSystem_ = std::make_shared<ModelSystem>();

	PlayerBinaryData playerParam{};
	ActorDataLoader::LoadPlayerData(
		std::wstring(Application::PATH_CSV) + PLAYER_CSV_NAME,
		std::wstring(Application::PATH_BINARY) + PLAYER_BIN_NAME,
		playerParam);

	playerEntity_ = ecsRegistry_.CreateEntity();

	TransformComponent playerTransform{};
	playerTransform.scale = playerParam.scale;
	ecsRegistry_.AddComponent<TransformComponent>(playerEntity_, playerTransform);

	VelocityComponent playerVelocity{};
	playerVelocity.maxSpeed = playerParam.maxSpeed;
	ecsRegistry_.AddComponent<VelocityComponent>(playerEntity_, playerVelocity);

	PlayerInputComponent playerInput{};
	playerInput.moveSpeed = playerParam.moveSpeed;
	playerInput.jumpPower = playerParam.jumpPower;
	ecsRegistry_.AddComponent<PlayerInputComponent>(playerEntity_, playerInput);

	int playerModelHandle = ResourceManager::GetInstance().GetHandleId(
		ResourceManager::RESOURCE_ID::MODEL_SAMPLE);
	ModelComponent model{};
	model.modelHandle = playerModelHandle;
	ecsRegistry_.AddComponent<ModelComponent>(playerEntity_, model);

#ifdef _DEBUG
	DebugGuiComponent playerGui{};
	playerGui.windowTitle = "Player";
	playerGui.saveType = DEBUG_SAVE_TYPE::PLAYER;
	playerGui.csvFilePath = std::wstring(Application::PATH_CSV) + PLAYER_CSV_NAME;
	playerGui.binaryFilePath = std::wstring(Application::PATH_BINARY) + PLAYER_BIN_NAME;
	ecsRegistry_.AddComponent<DebugGuiComponent>(playerEntity_, playerGui);
#endif // _DEBUG

	CameraBinaryData cameraParam{};
	ActorDataLoader::LoadCameraData(
		std::wstring(Application::PATH_CSV) + CAMERA_CSV_NAME,
		std::wstring(Application::PATH_BINARY) + CAMERA_BIN_NAME,
		cameraParam);

	cameraEntity_ = ecsRegistry_.CreateEntity();
	ecsRegistry_.AddComponent<TransformComponent>(cameraEntity_, {});

	CameraComponent cameraData{};
	cameraData.targetEntity = playerEntity_;
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
	ecsRegistry_.AddComponent<CameraComponent>(cameraEntity_, cameraData);

#ifdef _DEBUG
	DebugGuiComponent cameraGui{};
	cameraGui.windowTitle = "Camera";
	cameraGui.saveType = DEBUG_SAVE_TYPE::CAMERA;
	cameraGui.csvFilePath = std::wstring(Application::PATH_CSV) + CAMERA_CSV_NAME;
	cameraGui.binaryFilePath = std::wstring(Application::PATH_BINARY) + CAMERA_BIN_NAME;
	ecsRegistry_.AddComponent<DebugGuiComponent>(cameraEntity_, cameraGui);
#endif // _DEBUG

	stageManager_ = std::make_unique<StageManager>(ecsRegistry_, playerEntity_);
	stageManager_->Initialize();
	stageManager_->SetupFirstStage(std::make_shared<StageLobby>(ecsRegistry_));
}

void SceneGame::Update(void)
{
	stageManager_->Update();

	if (stageManager_->GetStageChanging())
	{
		return;
	}

#ifdef _DEBUG

	if (CheckHitKey(KEY_INPUT_F5) != 0)
	{
		stageManager_->ChangeStage(std::make_shared<StageLobby>(ecsRegistry_));
	}
	else if (CheckHitKey(KEY_INPUT_F6) != 0)
	{
		stageManager_->ChangeStage(std::make_shared<StageBattle>(ecsRegistry_));
	}
	else if (CheckHitKey(KEY_INPUT_F7) != 0)
	{
		stageManager_->ChangeStage(std::make_shared<StageTraining>(ecsRegistry_));
	}

#endif // _DEBUG

	for (auto& system : updateSystems_)
	{
		system->Update(ecsRegistry_);
	}
}

void SceneGame::Draw(void)
{
	modelSystem_->Update(ecsRegistry_);

	stageManager_->DrawStage();

	stageManager_->DrawOverlay();
}

void SceneGame::Release(void)
{
	if (stageManager_ != nullptr)
	{
		stageManager_->Release();
	}

	updateSystems_.clear();
}