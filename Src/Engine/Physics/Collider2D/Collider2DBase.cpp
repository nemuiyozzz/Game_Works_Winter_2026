#include "../../../Pch.h"
#include "Collider2DBase.h"

Collider2DBase::Collider2DBase(SHAPE_2D shapeType, TAG_2D collisionTag)
	: shapeType_(shapeType)
	, collisionTag_(collisionTag)
{
}

Collider2DBase::SHAPE_2D Collider2DBase::GetShapeType(void) const
{
	return shapeType_;
}

Collider2DBase::TAG_2D Collider2DBase::GetCollisionTag(void) const
{
	return collisionTag_;
}

void Collider2DBase::AddTargetTag(TAG_2D targetTag)
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

bool Collider2DBase::IsTargetTag(TAG_2D targetTag) const
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