#include "TransformSystem.h"
#include "../Component/TransformComponent.h"

void TransformSystem::UpdateTransform(EcsRegistry& registry, Entity entity)
{
	if (!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

	transform.previousPosition_ = transform.position_;

	transform.scaleMatrix_ = MGetScale(transform.scale_);

	transform.rotationEuler_ = transform.rotation_.ToEuler();
	transform.rotationMatrix_ = transform.rotation_.ToMatrix();

	VECTOR finalPosition = VAdd(transform.position_, transform.localPosition_);
	transform.positionMatrix_ = MGetTranslate(finalPosition);

	MATRIX tempMatrix = MGetIdent();
	tempMatrix = MMult(tempMatrix, transform.scaleMatrix_);

	Quaternion combinedQuaternion = transform.rotation_.Mult(transform.localRotation_);
	tempMatrix = MMult(tempMatrix, combinedQuaternion.ToMatrix());

	transform.combinedMatrix_ = MMult(tempMatrix, transform.positionMatrix_);
}

void TransformSystem::TranslatePosition(
	EcsRegistry& registry, Entity entity, const VECTOR& moveVector)
{
	if (!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	transform.position_ = VAdd(transform.position_, moveVector);

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
	transform.scale_ = VGet(targetScale, targetScale, targetScale);

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
