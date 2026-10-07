#include "CollisionCapsule.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "CollisionInfo.h"
#include "CollisionSphere.h"
#include "../../Utility/UtilityMath.h"

bool CollisionCapsule::CheckCapsuleVsCapsule(const ColliderBase* colliderA,
	const ColliderBase* colliderB, CollisionInfo& outInfo)
{
	const auto* CAPSULE_A = static_cast<const ColliderCapsule*>(colliderA);
	const auto* CAPSULE_B = static_cast<const ColliderCapsule*>(colliderB);

	if (!CAPSULE_A || !CAPSULE_B)
	{
		return false;
	}

	const VECTOR START_POS_A = CAPSULE_A->GetWorldStartPos();
	const VECTOR END_POS_A = CAPSULE_A->GetWorldEndPos();

	const VECTOR START_POS_B = CAPSULE_B->GetWorldStartPos();
	const VECTOR END_POS_B = CAPSULE_B->GetWorldEndPos();

	const VECTOR CENTER_A = VScale(VAdd(START_POS_A, END_POS_A), 0.5f);

	VECTOR nearestPosB = UtilityMath::GetNearestPointOnSegment(START_POS_B, END_POS_B, CENTER_A);
	VECTOR nearestPosA = UtilityMath::GetNearestPointOnSegment(START_POS_A, END_POS_A, nearestPosB);

	nearestPosB = UtilityMath::GetNearestPointOnSegment(START_POS_B, END_POS_B, nearestPosA);

	const VECTOR DISTANCE_VEC = VSub(nearestPosA, nearestPosB);
	const float DIST_SQUARE = static_cast<float>(UtilityMath::SqrMagnitude(DISTANCE_VEC));

	const float RADIUS_SUM = CAPSULE_A->GetRadius() + CAPSULE_B->GetRadius();

	if (DIST_SQUARE < (RADIUS_SUM * RADIUS_SUM))
	{
		const float DISTANCE = sqrtf(DIST_SQUARE);

		outInfo.myCollider = colliderA;
		outInfo.hitCollider = colliderB;
		outInfo.isActive = true;

		outInfo.hitPosition = VAdd(nearestPosA, VScale(VSub(nearestPosB, nearestPosA), 0.5f));

		if (DISTANCE > 0.0f)
		{
			outInfo.hitNormal = VScale(VSub(nearestPosA, nearestPosB), 1.0f / DISTANCE);
		}
		else
		{
			outInfo.hitNormal = VGet(0.0f, 1.0f, 0.0f);
		}

		outInfo.penetration = RADIUS_SUM - DISTANCE;
		return true;
	}

	return false;
}

bool CollisionCapsule::CheckCapsuleVsSphere(const ColliderBase* capsuleCol,
	const ColliderBase* sphereCol, CollisionInfo& outInfo)
{
	return CollisionSphere::CheckSphereVsCapsule(sphereCol, capsuleCol, outInfo);
}

bool CollisionCapsule::CheckCapsuleVsModel(const ColliderBase* capsuleCol,
	const ColliderBase* modelCol, CollisionInfo& outInfo)
{
	if (!capsuleCol || !modelCol)
	{
		return false;
	}

	const auto* CAPSULE = static_cast<const ColliderCapsule*>(capsuleCol);
	const auto* MODEL = static_cast<const ColliderModel*>(modelCol);

	if (CAPSULE == nullptr || MODEL == nullptr)
	{
		return false;
	}

	const int MODEL_HANDLE = MODEL->GetModelHandle();

	if (MODEL_HANDLE == -1)
	{
		return false;
	}

	const VECTOR START_POS = CAPSULE->GetWorldStartPos();
	const VECTOR END_POS = CAPSULE->GetWorldEndPos();
	const float RADIUS = CAPSULE->GetRadius();

	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(MODEL_HANDLE, -1, START_POS, END_POS, RADIUS);

	if (hitResult.HitNum > 0)
	{
		float maxPenetration = -1.0f;
		int bestIndex = -1;

		const ColliderBase::TAG MODEL_TAG = modelCol->GetCollisionTag();

		for (int i = 0; i < hitResult.HitNum; ++i)
		{
			if (MODEL->IsExcludedFrame(hitResult.Dim[i].FrameIndex)) continue;

			const auto& POLY = hitResult.Dim[i];

			if (MODEL_TAG == ColliderBase::TAG::STAGE && POLY.Normal.y <= 0.5f) continue;
			if (MODEL_TAG == ColliderBase::TAG::WALL && POLY.Normal.y > 0.5f) continue;

			const VECTOR POLY_POINT = POLY.Position[0];
			const VECTOR NEAREST_ON_AXIS = UtilityMath::GetNearestPointOnSegment(START_POS, END_POS, POLY_POINT);
			const VECTOR TO_HIT = VSub(POLY_POINT, NEAREST_ON_AXIS);
			const float DIST_ALONG_NORMAL = VDot(TO_HIT, POLY.Normal);
			const float POLY_PENETRATION = RADIUS - DIST_ALONG_NORMAL;

			if (POLY_PENETRATION > maxPenetration)
			{
				maxPenetration = POLY_PENETRATION;
				bestIndex = i;
			}
		}

		if (bestIndex == -1)
		{
			MV1CollResultPolyDimTerminate(hitResult);
			return false;
		}

		const auto& BEST_HIT = hitResult.Dim[bestIndex];

		outInfo.myCollider = capsuleCol;
		outInfo.hitCollider = modelCol;

		const VECTOR BEST_POLY_POINT = VScale(
			VAdd(VAdd(BEST_HIT.Position[0], BEST_HIT.Position[1]), BEST_HIT.Position[2]),
			1.0f / 3.0f);

		outInfo.hitPosition = BEST_POLY_POINT;
		outInfo.hitNormal = BEST_HIT.Normal;
		outInfo.isActive = true;

		const VECTOR NEAREST_ON_AXIS = UtilityMath::GetNearestPointOnSegment(START_POS, END_POS, BEST_POLY_POINT);
		const VECTOR TO_HIT = VSub(BEST_POLY_POINT, NEAREST_ON_AXIS);
		const float DIST_ALONG_NORMAL = VDot(TO_HIT, BEST_HIT.Normal);

		outInfo.penetration = RADIUS - DIST_ALONG_NORMAL;
		if (outInfo.penetration < 0.0f) outInfo.penetration = 0.0f;

		MV1CollResultPolyDimTerminate(hitResult);
		return true;
	}

	MV1CollResultPolyDimTerminate(hitResult);
	return false;
}