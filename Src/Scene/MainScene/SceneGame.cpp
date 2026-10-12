#include "../../Pch.h"
#include "SceneGame.h"
#include "../../Engine/System/Resource/ResourceManager.h"
#include "../../Ecs/Component/TransformComponent.h"
#include "../../Ecs/Component/VelocityComponent.h"
#include "../../Ecs/Component/PlayerInputComponent.h"
#include "../../Ecs/Component/ModelComponent.h"
#include "../../Ecs/Component/CameraComponent.h"
#include "../../Ecs/System/PlayerControlSystem.h"
#include "../../Ecs/System/MovementSystem.h"
#include "../../Ecs/System/TransformSystem.h"
#include "../../Ecs/System/CameraSystem.h"

SceneGame::SceneGame(void)
	: playerEntity_(NULL_ENTITY)
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

	modelSystem_ = std::make_shared<ModelSystem>();

	playerEntity_ = ecsRegistry_.CreateEntity();
	
	TransformComponent playerTransform{};

	playerTransform.scale = VGet(50.0f, 50.0f, 50.0f);

	ecsRegistry_.AddComponent<TransformComponent>(playerEntity_, playerTransform);
	ecsRegistry_.AddComponent<VelocityComponent>(playerEntity_, {});
	ecsRegistry_.AddComponent<PlayerInputComponent>(playerEntity_, { 100.0f, 150.0f });

	int playerModelHandle = ResourceManager::GetInstance().GetHandleId(ResourceManager::RESOURCE_ID::MODEL_SAMPLE);
	ModelComponent model{};
	model.modelHandle = playerModelHandle;
	ecsRegistry_.AddComponent<ModelComponent>(playerEntity_, model);

	cameraEntity_ = ecsRegistry_.CreateEntity();
	ecsRegistry_.AddComponent<TransformComponent>(cameraEntity_, {});

	CameraComponent cameraData{};
	cameraData.targetEntity = playerEntity_;
	cameraData.mode = CAMERA_MODE::TRACKING;
	ecsRegistry_.AddComponent<CameraComponent>(cameraEntity_, cameraData);
}

void SceneGame::Update(void)
{
	for (auto& system : updateSystems_)
	{
		system->Update(ecsRegistry_);
	}
}

void SceneGame::Draw(void)
{
	modelSystem_->Update(ecsRegistry_);

	int textDrawY = 20;
	const int TEXT_COLOR = 0xffffff;

	const TransformComponent& transform = ecsRegistry_.GetComponent<TransformComponent>(playerEntity_);

	DrawFormatString(20, textDrawY, TEXT_COLOR, L"PLAYER POS : [%.2f, %.2f, %.2f]",
		transform.position.x, transform.position.y, transform.position.z);

	const TransformComponent& cametransform = ecsRegistry_.GetComponent<TransformComponent>(cameraEntity_);

	DrawFormatString(20, textDrawY + 100, TEXT_COLOR, L"CAMERA POS : [%.2f, %.2f, %.2f]",
		cametransform.position.x, cametransform.position.y, cametransform.position.z);
}

void SceneGame::Release(void)
{
	updateSystems_.clear();
}