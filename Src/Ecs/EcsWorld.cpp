#include "EcsWorld.h"
#include "ActorFactory.h"
#include "System/PlayerControlSystem.h"
#include "System/MovementSystem.h"
#include "System/CollisionSystem.h"
#include "System/Collision2DSystem.h"
#include "System/TransformSystem.h"
#include "System/CameraSystem.h"
#include "System/ModelSystem.h"
#include "System/ImageSystem.h"
#include "System/ShaderSystem.h"
#include "System/ColliderDebugDrawSystem.h"
#include "System/DebugGuiSystem.h"
#include "System/AnimationSystem.h"
#include "System/EnemyAiSystem.h"
#include "System/HudDrawSystem.h"
#include "System/SoundSystem.h"

void EcsWorld::Initialize(void)
{
	updateSystems_.clear();
	drawSystems_.clear();

	AddUpdateSystem<PlayerControlSystem>();
	AddUpdateSystem<EnemyAiSystem>();
	AddUpdateSystem<MovementSystem>();
	AddUpdateSystem<CollisionSystem>();
	AddUpdateSystem<Collision2DSystem>();
	AddUpdateSystem<TransformSystem>();
	AddUpdateSystem<CameraSystem>();
	AddUpdateSystem<AnimationSystem>();
	AddUpdateSystem<SoundSystem>();

#ifdef _DEBUG
	AddUpdateSystem<DebugGuiSystem>();
	colliderDebugDrawSystem_ = std::make_shared<ColliderDebugDrawSystem>();
#endif // _DEBUG

	//AddDrawSystem<ModelSystem>();
	shaderSystem_ = std::make_shared<ShaderSystem>();
	shaderSystem_->Initialize();           
	drawSystems_.push_back(shaderSystem_);

	AddDrawSystem<HudDrawSystem>();

	AddDrawSystem<ImageSystem>();

	ActorFactory::RegisterDefaultRecipes();
}

void EcsWorld::Update(void)
{
	for (auto& system : updateSystems_)
	{
		system->Update(ecsRegistry_);
	}

#ifdef _DEBUG
	if (colliderDebugDrawSystem_ != nullptr)
	{
		colliderDebugDrawSystem_->Update();
	}
#endif // _DEBUG
}

void EcsWorld::Draw(void)
{
	for (auto& system : drawSystems_)
	{
		system->Update(ecsRegistry_);
	}

	if (shaderSystem_ != nullptr)
	{
		shaderSystem_->ExecuteDrawCommands();
	}

#ifdef _DEBUG
	if (colliderDebugDrawSystem_ != nullptr)
	{
		colliderDebugDrawSystem_->Draw(ecsRegistry_);
	}

	EnemyAiSystem::DrawDebugVision(ecsRegistry_);
#endif
}

void EcsWorld::Release(void)
{
	updateSystems_.clear();
	drawSystems_.clear();
	colliderDebugDrawSystem_.reset();

	if (shaderSystem_ != nullptr)
	{
		shaderSystem_->Release();
		shaderSystem_.reset();
	}
}

Entity EcsWorld::SpawnActor(const std::wstring& actorName)
{
	return ActorFactory::CreateActor(ecsRegistry_, actorName);
}

Entity EcsWorld::SpawnCamera(Entity targetEntity)
{
	return ActorFactory::CreateCamera(ecsRegistry_, targetEntity);
}