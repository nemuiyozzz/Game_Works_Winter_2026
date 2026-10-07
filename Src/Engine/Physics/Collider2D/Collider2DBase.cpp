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