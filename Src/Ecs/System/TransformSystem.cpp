#include "../../Pch.h"
#include "TransformSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/HierarchyComponent.h"

void TransformSystem::Update(EcsRegistry& registry)
{
	// Transformコンポーネントを持つすべてのエンティティを取得
	auto entities = registry.GetEntitiesWith<TransformComponent>();

	for (Entity entity : entities)
	{
		UpdateTransform(registry, entity);
	}

	auto hierarchyEntities = registry.GetEntitiesWith<TransformComponent, HierarchyComponent>();

	for (Entity entity : hierarchyEntities)
	{
		const HierarchyComponent& hierarchy = registry.GetComponent<HierarchyComponent>(entity);

		if (hierarchy.parent != NULL_ENTITY &&
			registry.HasComponent<TransformComponent>(hierarchy.parent))
		{
			TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

			const TransformComponent& parentTransform = 
				registry.GetComponent<TransformComponent>(hierarchy.parent);

			transform.combinedMatrix = MMult(transform.combinedMatrix,
				parentTransform.combinedMatrix);

			transform.position = VTransform(VGet(0.0f, 0.0f, 0.0f), transform.combinedMatrix);
		}
	}
}

void TransformSystem::UpdateTransform(EcsRegistry& registry, Entity entity)
{
	if (!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	transform.previousPosition = transform.position;
	transform.scaleMatrix = MGetScale(transform.scale);
	transform.rotationEuler = transform.rotation_.ToEuler();
	transform.rotationMatrix = transform.rotation_.ToMatrix();

	bool hasParent = false;
	if (registry.HasComponent<HierarchyComponent>(entity))
	{
		if (registry.GetComponent<HierarchyComponent>(entity).parent != NULL_ENTITY)
		{
			hasParent = true;
		}
	}

	if (hasParent)
	{
		transform.positionMatrix = MGetTranslate(transform.localPosition);
	}
	else
	{
		transform.position = transform.localPosition;
		transform.positionMatrix = MGetTranslate(transform.position);
	}

	MATRIX tempMatrix = MGetIdent();
	tempMatrix = MMult(tempMatrix, transform.scaleMatrix);
	Quaternion combinedQuaternion = transform.rotation_.Mult(transform.localRotation);
	tempMatrix = MMult(tempMatrix, combinedQuaternion.ToMatrix());

	transform.combinedMatrix = MMult(tempMatrix, transform.positionMatrix);
}

void TransformSystem::TranslatePosition(
	EcsRegistry& registry, Entity entity, const VECTOR& moveVector)
{
	if (!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	transform.position = VAdd(transform.position, moveVector);

	UpdateTransform(registry, entity);
}

void TransformSystem::RotateQuaternion(
	EcsRegistry& registry, Entity entity, const Quaternion& rotation)
{
	if (!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	transform.rotation_ = Quaternion::Mult(transform.rotation_, rotation);

	UpdateTransform(registry, entity);
}

void TransformSystem::SetUniformScale(EcsRegistry& registry, Entity entity, float targetScale)
{
	if (!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	transform.scale = VGet(targetScale, targetScale, targetScale);

	UpdateTransform(registry, entity);
}

VECTOR TransformSystem::GetForwardDirection(EcsRegistry& registry, Entity entity) const
{
	return GetTargetDirection(registry, entity, UtilityMath::DIR_FORWARD);
}

VECTOR TransformSystem::GetTargetDirection(
	EcsRegistry& registry, Entity entity, const VECTOR& baseDirection) const
{
	if (!registry.HasComponent<TransformComponent>(entity))
	{
		return UtilityMath::VECTOR_ZERO;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	return transform.rotation_.PosAxis(baseDirection);
}