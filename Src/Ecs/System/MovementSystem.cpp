#include "MovementSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/VelocityComponent.h"
#include "../../Engine/System/Time/TimeManager.h"

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
	VECTOR deltaAcceleration = VScale(velocityData.acceleration, deltaTime);
	constexpr float MIN_LIMIT_SPEED = 0.0f;

	velocityData.velocity = VAdd(velocityData.velocity, deltaAcceleration);

	if (velocityData.maxSpeed >= MIN_LIMIT_SPEED)
	{
		float currentSpeed = VSize(velocityData.velocity);
		
		if (currentSpeed > velocityData.maxSpeed)
		{
			VECTOR normalizedVelocity = UtilityMath::VNormalize(velocityData.velocity);
			
			velocityData.velocity = VScale(normalizedVelocity, velocityData.maxSpeed);
		}
	}

	VECTOR moveDistance = VScale(velocityData.velocity, deltaTime);
	
	transform.position = VAdd(transform.position, moveDistance);
}

void MovementSystem::AddForce(EcsRegistry& registry, Entity entity, const VECTOR& force)
{
	if (!registry.HasComponent<VelocityComponent>(entity))
	{
		return;
	}

	VelocityComponent& velocityData = registry.GetComponent<VelocityComponent>(entity);

	velocityData.acceleration = VAdd(velocityData.acceleration, force);
}

void MovementSystem::SetVelocity(EcsRegistry& registry, Entity entity, const VECTOR& velocity)
{
	if (!registry.HasComponent<VelocityComponent>(entity))
	{
		return;
	}

	VelocityComponent& velocityData = registry.GetComponent<VelocityComponent>(entity);

	velocityData.velocity = velocity;
}
