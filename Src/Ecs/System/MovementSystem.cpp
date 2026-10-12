#include "MovementSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/VelocityComponent.h"
#include "../../System/Time/TimeManager.h"

void MovementSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<TransformComponent, VelocityComponent>();

	for (Entity entity : entities)
	{
		UpdatePosition(registry, entity);
	}
}
void MovementSystem::UpdatePosition(EcsRegistry& registry, Entity entity)
{
	if (!registry.HasComponent<TransformComponent>(entity) ||
		!registry.HasComponent<VelocityComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	VelocityComponent& velocityData = registry.GetComponent<VelocityComponent>(entity);

	float deltaTime = TimeManager::GetInstance().GetDeltaTime();
	VECTOR deltaAcceleration = VScale(velocityData.acceleration_, deltaTime);
	constexpr float MIN_LIMIT_SPEED = 0.0f;

	velocityData.velocity_ = VAdd(velocityData.velocity_, deltaAcceleration);

	if (velocityData.maxSpeed_ >= MIN_LIMIT_SPEED)
	{
		float currentSpeed = VSize(velocityData.velocity_);
		
		if (currentSpeed > velocityData.maxSpeed_)
		{
			VECTOR normalizedVelocity = UtilityMath::VNormalize(velocityData.velocity_);
			
			velocityData.velocity_ = VScale(normalizedVelocity, velocityData.maxSpeed_);
		}
	}

	VECTOR moveDistance = VScale(velocityData.velocity_, deltaTime);
	
	transform.position_ = VAdd(transform.position_, moveDistance);
}

void MovementSystem::AddForce(EcsRegistry& registry, Entity entity, const VECTOR& force)
{
	if (!registry.HasComponent<VelocityComponent>(entity))
	{
		return;
	}

	VelocityComponent& velocityData = registry.GetComponent<VelocityComponent>(entity);

	velocityData.acceleration_ = VAdd(velocityData.acceleration_, force);
}

void MovementSystem::SetVelocity(EcsRegistry& registry, Entity entity, const VECTOR& velocity)
{
	if (!registry.HasComponent<VelocityComponent>(entity))
	{
		return;
	}

	VelocityComponent& velocityData = registry.GetComponent<VelocityComponent>(entity);

	velocityData.velocity_ = velocity;
}
