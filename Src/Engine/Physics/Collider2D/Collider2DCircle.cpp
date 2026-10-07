#include "Collider2DCircle.h"

Collider2DCircle::Collider2DCircle(const Vector2F& centerPos, float radius, 
	Collider2DBase::TAG_2D collisionTag)
	: Collider2DBase(Collider2DBase::SHAPE_2D::CIRCLE, collisionTag)
	, radius_(radius)
	, centerPos_(centerPos)
{
}

void Collider2DCircle::SetCenterPos(const Vector2F& centerPos)
{
	centerPos_ = centerPos;
}

void Collider2DCircle::SetRadius(float radius)
{
	radius_ = radius;
}

Vector2F Collider2DCircle::GetWorldCenterPos(void) const
{
	return centerPos_;
}

float Collider2DCircle::GetRadius(void) const
{
	return radius_;
}

void Collider2DCircle::DrawDebug(int color) const
{
	Vector2F currentPos = GetWorldCenterPos();
	DrawCircle(static_cast<int>(currentPos.x), static_cast<int>(currentPos.y), static_cast<int>(radius_), color, false);
}