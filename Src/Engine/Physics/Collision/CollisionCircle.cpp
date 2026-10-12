#include "../../../Pch.h"
#include "CollisionCircle.h"
#include "../Collider2D/Collider2DCircle.h"
#include "../Collider2D/Collider2DBox.h"
#include "../../../Common/Vector2.h"

bool CollisionCircle::CheckCircleVsCircle(const Collider2DBase* circleA, const Collider2DBase* circleB)
{
	if (!circleA || !circleB)
	{
		return false;
	}

	const auto* CIRCLE_A = static_cast<const Collider2DCircle*>(circleA);
	const auto* CIRCLE_B = static_cast<const Collider2DCircle*>(circleB);

	if (CIRCLE_A == nullptr || CIRCLE_B == nullptr)
	{
		return false;
	}

	const Vector2F POS_A = CIRCLE_A->GetWorldCenterPos();
	const Vector2F POS_B = CIRCLE_B->GetWorldCenterPos();

	const float DIST_X = POS_B.x_ - POS_A.x_;
	const float DIST_Y = POS_B.y_ - POS_A.y_;
	const float DIST_SQUARE = (DIST_X * DIST_X) + (DIST_Y * DIST_Y);

	const float RADIUS_SUM = CIRCLE_A->GetRadius() + CIRCLE_B->GetRadius();
	const float RADIUS_SUM_SQUARE = RADIUS_SUM * RADIUS_SUM;

	if (DIST_SQUARE < RADIUS_SUM_SQUARE)
	{
		return true;
	}

	return false;
}

bool CollisionCircle::CheckCircleVsBox(const Collider2DBase* circle, const Collider2DBase* box)
{
	if (!circle || !box)
	{
		return false;
	}

	const auto* CIRCLE = static_cast<const Collider2DCircle*>(circle);
	if (CIRCLE == nullptr)
	{
		CIRCLE = static_cast<const Collider2DCircle*>(box);
	}

	const auto* BOX = static_cast<const Collider2DBox*>(box);
	if (BOX == nullptr)
	{
		BOX = static_cast<const Collider2DBox*>(circle);
	}

	if (CIRCLE == nullptr || BOX == nullptr)
	{
		return false;
	}

	const Vector2F CIRCLE_POS = CIRCLE->GetWorldCenterPos();
	const Vector2F BOX_LEFT_TOP = BOX->GetLeftTop();
	const Vector2F BOX_RIGHT_BOTTOM = BOX->GetRightBottom();

	// 矩形内で円の中心に最も近い点を算出
	const float CLOSEST_X = std::max(BOX_LEFT_TOP.x_, std::min(CIRCLE_POS.x_, BOX_RIGHT_BOTTOM.x_));
	const float CLOSEST_Y = std::max(BOX_LEFT_TOP.y_, std::min(CIRCLE_POS.y_, BOX_RIGHT_BOTTOM.y_));

	const float DIST_X = CIRCLE_POS.x_ - CLOSEST_X;
	const float DIST_Y = CIRCLE_POS.y_ - CLOSEST_Y;
	const float DIST_SQUARE = (DIST_X * DIST_X) + (DIST_Y * DIST_Y);

	const float RADIUS_SQUARE = CIRCLE->GetRadius() * CIRCLE->GetRadius();

	if (DIST_SQUARE < RADIUS_SQUARE)
	{
		return true;
	}

	return false;
}