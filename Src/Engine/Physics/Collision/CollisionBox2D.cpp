#include "CollisionBox2D.h"

#include "../Collider2D/Collider2DBox.h"
#include "../../../Common/Vector2.h"

bool CollisionBox2D::CheckBoxVsBox(const Collider2DBase* boxA, const Collider2DBase* boxB)
{
	if (!boxA || !boxB)
	{
		return false;
	}

	const auto* BOX_A = dynamic_cast<const Collider2DBox*>(boxA);
	const auto* BOX_B = dynamic_cast<const Collider2DBox*>(boxB);

	if (BOX_A == nullptr || BOX_B == nullptr)
	{
		return false;
	}

	const Vector2F LEFT_TOP_A = BOX_A->GetLeftTop();         // 矩形Aの左上座標
	const Vector2F RIGHT_BOTTOM_A = BOX_A->GetRightBottom(); // 矩形Aの右下座標

	const Vector2F LEFT_TOP_B = BOX_B->GetLeftTop();         // 矩形Bの左上座標
	const Vector2F RIGHT_BOTTOM_B = BOX_B->GetRightBottom(); // 矩形Bの右下座標

	// X軸の判定
	if (RIGHT_BOTTOM_A.x_ < LEFT_TOP_B.x_)
	{
		return false;
	}
	if (LEFT_TOP_A.x_ > RIGHT_BOTTOM_B.x_)
	{
		return false;
	}

	// Y軸の判定
	if (RIGHT_BOTTOM_A.y_ < LEFT_TOP_B.y_)
	{
		return false;
	}
	if (LEFT_TOP_A.y_ > RIGHT_BOTTOM_B.y_)
	{
		return false;
	}

	return true;
}