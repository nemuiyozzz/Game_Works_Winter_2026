#include "Collision2DSystem.h"
#include "../Component/Collision2DComponent.h"
#include "../../Engine/Physics/Collision/CollisionBox2D.h"
#include "../../Engine/Physics/Collision/CollisionCircle.h"
#include "../../Engine/Physics/Collider2D/Collider2DBase.h"

void Collision2DSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<Collision2DComponent>();

	ClearHitHistories(registry, entities);

	CheckAllCollisions(registry, entities);
}

void Collision2DSystem::ClearHitHistories(EcsRegistry& registry, 
	const std::vector<Entity>& entities)
{
	for (Entity entity : entities)
	{
		Collision2DComponent& collision = registry.GetComponent<Collision2DComponent>(entity);

		collision.hitEntities.clear();
	}
}

void Collision2DSystem::CheckAllCollisions(EcsRegistry& registry,
	const std::vector<Entity>& entities)
{
	for (size_t i = 0; i < entities.size(); ++i)
	{
		Entity entityA = entities[i];
		Collision2DComponent& collisionA = registry.GetComponent<Collision2DComponent>(entityA);

		for (size_t j = i + 1; j < entities.size(); ++j)
		{
			Entity entityB = entities[j];
			Collision2DComponent& collisionB = registry.GetComponent<Collision2DComponent>(entityB);

			for (auto& colliderA : collisionA.colliders)
			{
				for (auto& colliderB : collisionB.colliders)
				{
					if (!colliderA->IsTargetTag(colliderB->GetCollisionTag()) &&
						!colliderB->IsTargetTag(colliderA->GetCollisionTag()))
					{
						continue;
					}

					ProcessCollisionPair(registry, entityA, entityB, colliderA, colliderB);
				}
			}
		}
	}
}

void Collision2DSystem::ProcessCollisionPair(EcsRegistry& registry, Entity entityA, Entity entityB,
	std::shared_ptr<Collider2DBase>& colliderA, std::shared_ptr<Collider2DBase>& colliderB)
{
	bool isHit = false;

	const auto SHAPE_A = colliderA->GetShapeType();
	const auto SHAPE_B = colliderB->GetShapeType();

	if (SHAPE_A == Collider2DBase::SHAPE_2D::BOX &&
		SHAPE_B == Collider2DBase::SHAPE_2D::BOX)
	{
		isHit = CollisionBox2D::CheckBoxVsBox(colliderA.get(), colliderB.get());
	}
	else if (SHAPE_A == Collider2DBase::SHAPE_2D::CIRCLE &&
		SHAPE_B == Collider2DBase::SHAPE_2D::CIRCLE)
	{
		isHit = CollisionCircle::CheckCircleVsCircle(colliderA.get(), colliderB.get());
	}
	else if (SHAPE_A == Collider2DBase::SHAPE_2D::CIRCLE &&
		SHAPE_B == Collider2DBase::SHAPE_2D::BOX)
	{
		isHit = CollisionCircle::CheckCircleVsBox(colliderA.get(), colliderB.get());
	}
	else if (SHAPE_A == Collider2DBase::SHAPE_2D::BOX &&
		SHAPE_B == Collider2DBase::SHAPE_2D::CIRCLE)
	{
		isHit = CollisionCircle::CheckCircleVsBox(colliderB.get(), colliderA.get());
	}

	if (isHit)
	{
		Collision2DComponent& collisionA = registry.GetComponent<Collision2DComponent>(entityA);
		Collision2DComponent& collisionB = registry.GetComponent<Collision2DComponent>(entityB);

		// 当たった相手のエンティティIDをお互いの履歴に登録する
		collisionA.hitEntities.push_back(entityB);
		collisionB.hitEntities.push_back(entityA);
	}
}
