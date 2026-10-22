#include "EcsWorld.h"
#include "ActorFactory.h"
#include "System/PlayerControlSystem.h"
#include "System/MovementSystem.h"
#include "System/CollisionSystem.h"
#include "System/Collision2DSystem.h"
#include "System/TransformSystem.h"
#include "System/CameraSystem.h"
#include "System/ModelSystem.h"
#include "System/ColliderDebugDrawSystem.h"
#include "System/DebugGuiSystem.h"

void EcsWorld::Initialize(void)
{
	updateSystems_.clear();
	drawSystems_.clear();

	AddUpdateSystem<PlayerControlSystem>();
	AddUpdateSystem<MovementSystem>();
	AddUpdateSystem<CollisionSystem>();
	AddUpdateSystem<Collision2DSystem>();
	AddUpdateSystem<TransformSystem>();
	AddUpdateSystem<CameraSystem>();

#ifdef _DEBUG
	AddUpdateSystem<DebugGuiSystem>();
	colliderDebugDrawSystem_ = std::make_shared<ColliderDebugDrawSystem>();
#endif // _DEBUG

	AddDrawSystem<ModelSystem>();

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

#ifdef _DEBUG
	if (colliderDebugDrawSystem_ != nullptr)
	{
		colliderDebugDrawSystem_->Draw(ecsRegistry_);
	}
#endif // _DEBUG
}

void EcsWorld::Release(void)
{
	updateSystems_.clear();
	drawSystems_.clear();
	colliderDebugDrawSystem_.reset();
}

Entity EcsWorld::SpawnActor(const std::wstring& actorName)
{
	return ActorFactory::CreateActor(ecsRegistry_, actorName);
}

Entity EcsWorld::SpawnCamera(Entity targetEntity)
{
	return ActorFactory::CreateCamera(ecsRegistry_, targetEntity);
}