#include "CollisionLine.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "CollisionInfo.h"
#include "../../Utility/UtilityMath.h"

bool CollisionLine::CheckLineVsModel(const ColliderBase* lineCol,
	const ColliderBase* modelCol, CollisionInfo& outInfo)
{
	if (!lineCol || !modelCol)
	{
		return false;
	}

	const auto* LINE = static_cast<const ColliderLine*>(lineCol);
	const auto* MODEL = static_cast<const ColliderModel*>(modelCol);

	if (!LINE || !MODEL)
	{
		return false;
	}

	const int MODEL_HANDLE = MODEL->GetModelHandle();
	if (MODEL_HANDLE == -1)
	{
		return false;
	}

	const VECTOR START_POS = LINE->GetWorldStartPos();
	const VECTOR END_POS = LINE->GetWorldEndPos();

	// 線分とモデルの衝突判定
	const MV1_COLL_RESULT_POLY HIT_RESULT = MV1CollCheck_Line(MODEL_HANDLE, -1, START_POS, END_POS);

	if (HIT_RESULT.HitFlag == 1)
	{
		// 判定から除外するフレームに当たった場合は無視する
		if (MODEL->IsExcludedFrame(HIT_RESULT.FrameIndex))
		{
			return false;
		}

		outInfo.myCollider = lineCol;
		outInfo.hitCollider = modelCol;
		outInfo.isActive = true;

		outInfo.hitPosition = HIT_RESULT.HitPosition;
		outInfo.hitNormal = HIT_RESULT.Normal;
		outInfo.penetration = UtilityMath::MagnitudeF(VSub(END_POS, HIT_RESULT.HitPosition));

		return true;
	}

	return false;
}