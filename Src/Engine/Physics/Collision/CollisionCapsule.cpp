#include "CollisionCapsule.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "CollisionInfo.h"
#include "CollisionSphere.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// ポリゴン判定・比率計算関連
	constexpr float HALF_SCALE = 0.5f;             // 中間点算出用の係数
	constexpr float WALL_NORMAL_Y_THRESHOLD = 0.5f; // 壁専用タグ時に床とみなす法線Yの閾値
	constexpr float TRIANGLE_VERTEX_INV = 1.0f / 3.0f; // 三角形重心算出用の逆数
}

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

	const VECTOR CENTER_A = VScale(VAdd(START_POS_A, END_POS_A), HALF_SCALE);

	VECTOR nearestPosB = UtilityMath::GetNearestPointOnSegment(
		START_POS_B, END_POS_B, CENTER_A);
	VECTOR nearestPosA = UtilityMath::GetNearestPointOnSegment(
		START_POS_A, END_POS_A, nearestPosB);

	nearestPosB = UtilityMath::GetNearestPointOnSegment(
		START_POS_B, END_POS_B, nearestPosA);

	const VECTOR DISTANCE_VEC = VSub(nearestPosA, nearestPosB);
	const float DIST_SQUARE = static_cast<float>(UtilityMath::SqrMagnitude(DISTANCE_VEC));

	const float RADIUS_SUM = CAPSULE_A->GetRadius() + CAPSULE_B->GetRadius();

	if (DIST_SQUARE < (RADIUS_SUM * RADIUS_SUM))
	{
		const float DISTANCE = sqrtf(DIST_SQUARE);

		outInfo.myCollider = colliderA;
		outInfo.hitCollider = colliderB;
		outInfo.isActive = true;

		outInfo.hitPosition = VAdd(
			nearestPosA, VScale(VSub(nearestPosB, nearestPosA), HALF_SCALE));

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

	// カプセルの中心座標を基準に、最も近い最大4フレームを抽出する
	const VECTOR CAPSULE_CENTER = VScale(VAdd(START_POS, END_POS), HALF_SCALE);
	int targetFrames[ColliderModel::NEAREST_FRAME_COUNT]{}; // 判定対象フレーム配列
	int frameCount = 0;                                     // 抽出されたフレーム数
	MODEL->FindNearestFrames(CAPSULE_CENTER, targetFrames, frameCount);

	float maxPenetration = -1.0f;            // 最大めり込み量
	bool hasValidHit = false;                // 有効な衝突があったか
	MV1_COLL_RESULT_POLY bestPolygon{};      // 最も深く当たったポリゴン情報
	const ColliderBase::TAG MODEL_TAG = modelCol->GetCollisionTag(); // モデルのタグ

	for (int frameIdx = 0; frameIdx < frameCount; ++frameIdx)
	{
		MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(
			MODEL_HANDLE, targetFrames[frameIdx], START_POS, END_POS, RADIUS);

		if (hitResult.HitNum > 0)
		{
			for (int polyIdx = 0; polyIdx < hitResult.HitNum; ++polyIdx)
			{
				const auto& POLY = hitResult.Dim[polyIdx];

				if (MODEL->IsExcludedFrame(POLY.FrameIndex))
				{
					continue;
				}

				if (MODEL_TAG == ColliderBase::TAG::WALL &&
					POLY.Normal.y > WALL_NORMAL_Y_THRESHOLD)
				{
					continue;
				}

				const VECTOR POLY_POINT = POLY.Position[0];
				const VECTOR NEAREST_ON_AXIS = UtilityMath::GetNearestPointOnSegment(
					START_POS, END_POS, POLY_POINT);
				const VECTOR TO_HIT = VSub(POLY_POINT, NEAREST_ON_AXIS);
				const float DIST_ALONG_NORMAL = VDot(TO_HIT, POLY.Normal);
				const float POLY_PENETRATION = RADIUS - DIST_ALONG_NORMAL;

				if (POLY_PENETRATION > maxPenetration)
				{
					maxPenetration = POLY_PENETRATION;
					bestPolygon = POLY;
					hasValidHit = true;
				}
			}
		}

		MV1CollResultPolyDimTerminate(hitResult);
	}

	if (!hasValidHit)
	{
		return false;
	}

	outInfo.myCollider = capsuleCol;
	outInfo.hitCollider = modelCol;

	const VECTOR BEST_POLY_POINT = VScale(
		VAdd(VAdd(bestPolygon.Position[0], bestPolygon.Position[1]),
			bestPolygon.Position[2]),
		TRIANGLE_VERTEX_INV);

	outInfo.hitPosition = BEST_POLY_POINT;
	outInfo.hitNormal = bestPolygon.Normal;
	outInfo.isActive = true;

	outInfo.penetration = maxPenetration;
	if (outInfo.penetration < 0.0f)
	{
		outInfo.penetration = 0.0f;
	}

	return true;
}