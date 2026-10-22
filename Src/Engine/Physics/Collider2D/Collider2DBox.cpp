#include "../../../Pch.h"
#include "Collider2DBox.h"

Collider2DBox::Collider2DBox(const Vector2F& centerPos, float width, 
	float height, TAG_2D collisionTag)
	: Collider2DBase(Collider2DBase::SHAPE_2D::BOX, collisionTag)
	, width_(width)
	, height_(height)
	, centerPos_(centerPos)
{
}

Vector2F Collider2DBox::GetCenterPos(void) const
{
	return GetWorldCenterPos();
}

void Collider2DBox::SetCenterPos(const Vector2F& centerPos)
{
	centerPos_ = centerPos;
}

void Collider2DBox::SetWidth(float width)
{
	width_ = width;
}

void Collider2DBox::SetHeight(float height)
{
	height_ = height;
}

Vector2F Collider2DBox::GetWorldCenterPos(void) const
{
	return centerPos_;
}

float Collider2DBox::GetWidth(void) const
{
	return width_;
}

float Collider2DBox::GetHeight(void) const
{
	return height_;
}

Vector2F Collider2DBox::GetLeftTop(void) const
{
	Vector2F center = GetWorldCenterPos();
	float halfWidth = width_ / 2.0f;
	float halfHeight = height_ / 2.0f;

	return Vector2F(center.x_ - halfWidth, center.y_ - halfHeight);
}

Vector2F Collider2DBox::GetRightBottom(void) const
{
	Vector2F center = GetWorldCenterPos();
	float halfWidth = width_ / 2.0f;
	float halfHeight = height_ / 2.0f;

	return Vector2F(center.x_ + halfWidth, center.y_ + halfHeight);
}

void Collider2DBox::DrawDebug(int color) const
{
	Vector2F leftTop = GetLeftTop();
	Vector2F rightBottom = GetRightBottom();

	DrawBox(
		static_cast<int>(leftTop.x_), static_cast<int>(leftTop.y_),
		static_cast<int>(rightBottom.x_), static_cast<int>(rightBottom.y_),
		color, false
	);
}