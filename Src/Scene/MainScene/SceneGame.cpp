#include "../../Pch.h"
#include "SceneGame.h"
#include "../../System/Resource/ResourceManager.h"
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

	ecsRegistry_.AddComponent<TransformComponent>(playerEntity_, {});
	ecsRegistry_.AddComponent<VelocityComponent>(playerEntity_, {});
	ecsRegistry_.AddComponent<PlayerInputComponent>(playerEntity_, { 100.0f, 150.0f });

	int playerModelHandle = ResourceManager::GetInstance().GetHandleId(ResourceManager::RESOURCE_ID::MODEL_SAMPLE);
	ModelComponent model{};
	model.modelHandle_ = playerModelHandle;
	ecsRegistry_.AddComponent<ModelComponent>(playerEntity_, model);

	cameraEntity_ = ecsRegistry_.CreateEntity();
	ecsRegistry_.AddComponent<TransformComponent>(cameraEntity_, {});

	CameraComponent cameraData{};
	cameraData.targetEntity_ = playerEntity_;
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
}

void SceneGame::Release(void)
{
	updateSystems_.clear();
}