#include "CollisionSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/VelocityComponent.h"
#include "../Component/CollisionComponent.h"
#include "../../Engine/Physics/Collision/CollisionCapsule.h"
#include "../../Engine/Physics/Collision/CollisionLine.h"
#include "../../Engine/Physics/Collision/CollisionSphere.h"
#include "../../Engine/Physics/Collider/ColliderBase.h"
#include "../../Engine/Physics/Collision/CollisionBox3D.h"

namespace
{
	// 押し戻し関連
	constexpr float PUSH_BACK_STATIC_RATIO = 1.0f;  // 静的オブジェクトに対する押し戻し率
	constexpr float PUSH_BACK_DYNAMIC_RATIO = 0.5f; // 動的オブジェクト同士の押し戻し率
	constexpr float GROUND_NORMAL_Y = 0.5f;         // 地面と判定する法線Yのしきい値
}

void CollisionSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<TransformComponent, CollisionComponent>();

	// ポインタ更新と履歴の初期化
	UpdateCollidersTarget(registry, entities);

	// 総当たり判定と押し戻しの実行
	CheckAllCollisions(registry, entities);
}

void CollisionSystem::UpdateCollidersTarget(EcsRegistry& registry, 
	const std::vector<Entity>& entities)
{
	for (Entity entity : entities)
	{
		TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
		CollisionComponent& collision = registry.GetComponent<CollisionComponent>(entity);

		collision.hitEntities.clear();

		for (auto& collider : collision.colliders)
		{
			collider->SetFollowTarget(&transform);
		}
	}
}

void CollisionSystem::CheckAllCollisions(EcsRegistry& registry,
	const std::vector<Entity>& entities)
{
	for (size_t i = 0; i < entities.size(); ++i)
	{
		Entity entityA = entities[i];
		CollisionComponent& collisionA = registry.GetComponent<CollisionComponent>(entityA);

		for (size_t j = i + 1; j < entities.size(); ++j)
		{
			Entity entityB = entities[j];
			CollisionComponent& collisionB = registry.GetComponent<CollisionComponent>(entityB);

			for (auto& colliderA : collisionA.colliders)
			{
				if (!colliderA->IsActive())
				{
					continue;
				}

				for (auto& colliderB : collisionB.colliders)
				{
					if (!colliderB->IsActive())
					{
						continue;
					}

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

void CollisionSystem::ProcessCollisionPair(EcsRegistry& registry, Entity entityA, Entity entityB,
	std::shared_ptr<ColliderBase>& colliderA, std::shared_ptr<ColliderBase>& colliderB)
{
	CollisionInfo info;
	bool isHit = false;

	const auto SHAPE_A = colliderA->GetShapeType();
	const auto SHAPE_B = colliderB->GetShapeType();

	if (SHAPE_A == ColliderBase::SHAPE::CAPSULE &&
		SHAPE_B == ColliderBase::SHAPE::MODEL)
	{
		isHit = CollisionCapsule::CheckCapsuleVsModel(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::MODEL &&
		SHAPE_B == ColliderBase::SHAPE::CAPSULE)
	{
		isHit = CollisionCapsule::CheckCapsuleVsModel(
			colliderB.get(), colliderA.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::CAPSULE &&
		SHAPE_B == ColliderBase::SHAPE::CAPSULE)
	{
		isHit = CollisionCapsule::CheckCapsuleVsCapsule(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::CAPSULE &&
		SHAPE_B == ColliderBase::SHAPE::CAPSULE)
	{
		isHit = CollisionCapsule::CheckCapsuleVsCapsule(
			colliderB.get(), colliderA.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::SPHERE &&
		SHAPE_B == ColliderBase::SHAPE::SPHERE)
	{
		isHit = CollisionSphere::CheckSphereVsSphere(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::CAPSULE &&
		SHAPE_B == ColliderBase::SHAPE::SPHERE)
	{
		isHit = CollisionCapsule::CheckCapsuleVsSphere(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::SPHERE &&
		SHAPE_B == ColliderBase::SHAPE::CAPSULE)
	{
		isHit = CollisionCapsule::CheckCapsuleVsSphere(
			colliderB.get(), colliderA.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::LINE &&
		SHAPE_B == ColliderBase::SHAPE::MODEL)
	{
		isHit = CollisionLine::CheckLineVsModel(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::MODEL &&
		SHAPE_B == ColliderBase::SHAPE::LINE)
	{
		isHit = CollisionLine::CheckLineVsModel(
			colliderB.get(), colliderA.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::BOX &&
		SHAPE_B == ColliderBase::SHAPE::BOX)
	{
		isHit = CollisionBox3D::CheckBoxVsBox(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::BOX &&
		SHAPE_B == ColliderBase::SHAPE::SPHERE)
	{
		isHit = CollisionBox3D::CheckBoxVsSphere(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::SPHERE &&
		SHAPE_B == ColliderBase::SHAPE::BOX)
	{
		isHit = CollisionBox3D::CheckBoxVsSphere(
			colliderB.get(), colliderA.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::BOX &&
		SHAPE_B == ColliderBase::SHAPE::CAPSULE)
	{
		isHit = CollisionBox3D::CheckBoxVsCapsule(
			colliderA.get(), colliderB.get(), info);
	}
	else if (SHAPE_A == ColliderBase::SHAPE::CAPSULE &&
		SHAPE_B == ColliderBase::SHAPE::BOX)
	{
		isHit = CollisionBox3D::CheckBoxVsCapsule(
			colliderB.get(), colliderA.get(), info);
	}

	if (isHit)
	{
		CollisionComponent& collisionA = registry.GetComponent<CollisionComponent>(entityA);
		CollisionComponent& collisionB = registry.GetComponent<CollisionComponent>(entityB);

		collisionA.hitEntities.push_back(entityB);
		collisionB.hitEntities.push_back(entityA);

		if (colliderA->IsTrigger() && colliderB->IsTrigger())
		{
			ResolvePenetration(registry, entityA, entityB, info);
		}
	}
}

void CollisionSystem::ResolvePenetration(EcsRegistry& registry, Entity entityA,
	Entity entityB, const CollisionInfo& info)
{
	TransformComponent& transformA = registry.GetComponent<TransformComponent>(entityA);
	TransformComponent& transformB = registry.GetComponent<TransformComponent>(entityB);

	const bool IS_STATIC_A = !registry.HasComponent<VelocityComponent>(entityA);
	const bool IS_STATIC_B = !registry.HasComponent<VelocityComponent>(entityB);

	if (IS_STATIC_A && IS_STATIC_B)
	{
		return;
	}

	VECTOR pushVector = VScale(info.hitNormal, info.penetration);

	if (!IS_STATIC_A && IS_STATIC_B)
	{
		if (info.hitNormal.y > GROUND_NORMAL_Y)
		{
			transformA.position = VAdd(transformA.position, pushVector);
			transformA.localPosition = transformA.position;
			registry.GetComponent<VelocityComponent>(entityA).velocity.y = 0.0f;
		}
		else
		{
			pushVector.y = 0.0f;
			transformA.position = VAdd(transformA.position, pushVector);
			transformA.localPosition = transformA.position;
		}
	}
	else if (IS_STATIC_A && !IS_STATIC_B)
	{
		if (info.hitNormal.y > GROUND_NORMAL_Y)
		{
			transformB.position = VAdd(transformB.position, pushVector);
			transformB.localPosition = transformB.position;
			registry.GetComponent<VelocityComponent>(entityB).velocity.y = 0.0f;
		}
		else
		{
			pushVector.y = 0.0f;
			transformB.position = VAdd(transformB.position, pushVector);
			transformB.localPosition = transformB.position;
		}
	}
	else
	{
		VECTOR halfPush = VScale(pushVector, PUSH_BACK_DYNAMIC_RATIO);
		transformA.position = VAdd(transformA.position, halfPush);
		transformA.localPosition = transformA.position;

		transformB.position = VSub(transformB.position, halfPush);
		transformB.localPosition = transformB.position;
	}
}