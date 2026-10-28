#include "../../../Pch.h"
#include "ColliderBase.h"
#include <algorithm>
#include "../../../Ecs/Component/TransformComponent.h"

ColliderBase::ColliderBase(
	SHAPE shapeType, TAG collisionTag, const TransformComponent* followTarget)
	: shapeType_(shapeType)
	, collisionTag_(collisionTag)
	, followTarget_(followTarget)
	, isActive_(true)
	, isTrigger_(true)
{
}

void ColliderBase::Draw(void) const
{
	int color = DEBUG_COLOR_INACTIVE;

	if (isActive_)
	{
		color = DEBUG_COLOR_ACTIVE;
	}

	DrawDebug(color);
}

VECTOR ColliderBase::TransformLocalToWorld(const VECTOR& localPosition) const
{
	if (followTarget_ == nullptr)
	{
		return localPosition;
	}

	VECTOR localRotationPosition = followTarget_->rotation_.PosAxis(localPosition);

	return VAdd(followTarget_->position, localRotationPosition);
}

const TransformComponent* ColliderBase::GetFollowTarget(void) const
{
	return followTarget_;
}

void ColliderBase::SetFollowTarget(const TransformComponent* followTarget)
{
	followTarget_ = followTarget;
}

ColliderBase::SHAPE ColliderBase::GetShapeType(void) const
{
	return shapeType_;
}

ColliderBase::TAG ColliderBase::GetCollisionTag(void) const
{
	return collisionTag_;
}

void ColliderBase::SetCollisionTag(TAG collisionTag)
{
	collisionTag_ = collisionTag;
}

bool ColliderBase::IsActive(void) const
{
	return isActive_;
}

void ColliderBase::SetActive(bool isActive)
{
	isActive_ = isActive;
}

bool ColliderBase::IsTrigger(void) const
{
	return isTrigger_;
}

void ColliderBase::SetTrigger(bool isTrigger)
{
	isTrigger_ = isTrigger;
}

void ColliderBase::AddTargetTag(TAG targetTag)
{
	for (auto tag : targetTags_)
	{
		if (tag == targetTag)
		{
			return;
		}
	}

	targetTags_.push_back(targetTag);
}

void ColliderBase::RemoveTargetTag(TAG targetTag)
{
	targetTags_.erase(
		std::remove(targetTags_.begin(), targetTags_.end(), targetTag),
		targetTags_.end());
}

void ColliderBase::ClearTargetTags(void)
{
	targetTags_.clear();
}

bool ColliderBase::IsTargetTag(TAG targetTag) const
{
	for (auto tag : targetTags_)
	{
		if (tag == targetTag)
		{
			return true;
		}
	}

	return false;
}