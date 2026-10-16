#include "EcsWorld.h"
#include "System/PlayerControlSystem.h"
#include "System/MovementSystem.h"
#include "System/TransformSystem.h"
#include "System/CameraSystem.h"
#include "System/ModelSystem.h"
#include "System/DebugGuiSystem.h"

void EcsWorld::Initialize(void)
{
	updateSystems_.clear();

	updateSystems_.push_back(std::make_shared<PlayerControlSystem>());
	updateSystems_.push_back(std::make_shared<MovementSystem>());
	updateSystems_.push_back(std::make_shared<TransformSystem>());
	updateSystems_.push_back(std::make_shared<CameraSystem>());

#ifdef _DEBUG
	updateSystems_.push_back(std::make_shared<DebugGuiSystem>());
#endif // _DEBUG

	modelSystem_ = std::make_shared<ModelSystem>();
}

void EcsWorld::Update(void)
{
	for (auto& system : updateSystems_)
	{
		system->Update(ecsRegistry_);
	}
}

void EcsWorld::Draw(void)
{
	if (modelSystem_ != nullptr)
	{
		modelSystem_->Update(ecsRegistry_);
	}
}

void EcsWorld::Release(void)
{
	updateSystems_.clear();
	modelSystem_.reset();
}

EcsRegistry& EcsWorld::GetRegistry(void)
{
	return ecsRegistry_;
}