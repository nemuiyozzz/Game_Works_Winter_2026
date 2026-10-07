#include "../../../Pch.h"
#include "CollisionSphere.h"
#include "../Collider/ColliderSphere.h"
#include "../Collider/ColliderCapsule.h"
#include "../Collider/ColliderModel.h"
#include "CollisionInfo.h"
#include "../../Utility/UtilityMath.h"

bool CollisionSphere::CheckSphereVsSphere(const ColliderBase* colliderA,
	const ColliderBase* colliderB, CollisionInfo& outInfo)
{
	if (!colliderA || !colliderB)
	{
		return false;
	}

	const auto* SPHERE_A = static_cast<const ColliderSphere*>(colliderA);
	const auto* SPHERE_B = static_cast<const ColliderSphere*>(colliderB);

	if (SPHERE_A == nullptr || SPHERE_B == nullptr)
	{
		return false;
	}

	// 距離計算
	const VECTOR POS_A = SPHERE_A->GetWorldPosition();
	const VECTOR POS_B = SPHERE_B->GetWorldPosition();

	const float DIST_X = POS_A.x - POS_B.x;
	const float DIST_Y = POS_A.y - POS_B.y;
	const float DIST_Z = POS_A.z - POS_B.z;

	// 距離の二乗を計算
	const float DIST_SQUARE = (DIST_X * DIST_X) + (DIST_Y * DIST_Y) + (DIST_Z * DIST_Z);

	// 半径の合計値
	const float RADIUS_SUM = SPHERE_A->GetRadius() + SPHERE_B->GetRadius();

	// 衝突判定
	if (DIST_SQUARE < (RADIUS_SUM * RADIUS_SUM))
	{
		const float DISTANCE = sqrtf(DIST_SQUARE);

		// 衝突情報の設定
		outInfo.myCollider = colliderA;
		outInfo.hitCollider = colliderB;
		outInfo.isActive = true;

		// 衝突位置の計算
		outInfo.hitPosition = VAdd(POS_B, VScale(VSub(POS_A, POS_B), 0.5f));

		// 法線ベクトルと押し出し量の計算
		if (DISTANCE > 0.0f)
		{
			outInfo.hitNormal = VScale(VSub(POS_A, POS_B), 1.0f / DISTANCE);
		}

		// めり込んでいる距離を算出
		outInfo.penetration = RADIUS_SUM - DISTANCE;

		return true;
	}

	return false;
}

bool CollisionSphere::CheckSphereVsCapsule(const ColliderBase* sphereCol,
	const ColliderBase* capsuleCol, CollisionInfo& outInfo)
{
	if (!sphereCol || !capsuleCol)
	{
		return false;
	}

	const auto* SPHERE = static_cast<const ColliderSphere*>(sphereCol);
	const auto* CAPSULE = static_cast<const ColliderCapsule*>(capsuleCol);

	if (SPHERE == nullptr || CAPSULE == nullptr)
	{
		return false;
	}

	// 各形状のパラメータ取得
	const VECTOR SPHERE_POS = SPHERE->GetWorldPosition();
	const VECTOR CAP_START_POS = CAPSULE->GetWorldStartPos();
	const VECTOR CAP_END_POS = CAPSULE->GetWorldEndPos();
	const float SPHERE_RADIUS = SPHERE->GetRadius();
	const float CAPSULE_RADIUS = CAPSULE->GetRadius();

	// カプセルの線分上で、球体に最も近い点を算出
	const VECTOR NEAREST_POS = UtilityMath::GetNearestPointOnSegment(CAP_START_POS, CAP_END_POS, SPHERE_POS);

	// 最近接点と球体の中心距離による判定
	const float DIST_SQUARE = static_cast<float>(UtilityMath::SqrMagnitude(SPHERE_POS, NEAREST_POS));
	const float RADIUS_SUM = SPHERE_RADIUS + CAPSULE_RADIUS;

	// 衝突判定
	if (DIST_SQUARE < (RADIUS_SUM * RADIUS_SUM))
	{
		const float DISTANCE = sqrtf(DIST_SQUARE);

		// 衝突情報の設定
		outInfo.myCollider = sphereCol;
		outInfo.hitCollider = capsuleCol;
		outInfo.isActive = true;

		// 衝突位置の計算
		outInfo.hitPosition = UtilityMath::Lerp(NEAREST_POS, SPHERE_POS, UtilityMath::HALF_NUM);

		// 法線ベクトルと押し出し量の計算
		if (DISTANCE > 0.0f)
		{
			outInfo.hitNormal = VScale(VSub(SPHERE_POS, NEAREST_POS), 1.0f / DISTANCE);
		}
		outInfo.penetration = RADIUS_SUM - DISTANCE;

		return true;
	}

	return false;
}

bool CollisionSphere::CheckSphereVsModel(const ColliderBase* sphereCol,
	const ColliderBase* modelCol, CollisionInfo& outInfo)
{
	if (!sphereCol || !modelCol)
	{
		return false;
	}

	const auto* SPHERE = static_cast<const ColliderSphere*>(sphereCol);
	if (SPHERE == nullptr)
	{
		return false;
	}

	const bool IS_HIT = CheckSphereVsModelCore(SPHERE->GetWorldPosition(), SPHERE->GetRadius(),
		modelCol, &outInfo);

	if (IS_HIT)
	{
		outInfo.myCollider = sphereCol;
	}

	return IS_HIT;
}

bool CollisionSphere::CheckHitWave(const ColliderBase* hitCapsuleCol, ColliderBase* waveCol)
{
	if (!hitCapsuleCol || !waveCol)
	{
		return false;
	}

	const auto* CAPSULE = static_cast <const ColliderCapsule*>(hitCapsuleCol);
	const auto* WAVE = static_cast<const ColliderSphere*>(waveCol);

	if (CAPSULE == nullptr || WAVE == nullptr)
	{
		return false;
	}

	const VECTOR WAVE_POS = WAVE->GetWorldPosition();
	const VECTOR CAP_START_POS = CAPSULE->GetWorldStartPos();
	const VECTOR CAP_END_POS = CAPSULE->GetWorldEndPos();

	const VECTOR NEAREST_POS = UtilityMath::GetNearestPointOnSegment(CAP_START_POS, CAP_END_POS, WAVE_POS);

	// Y軸を無視したXZ平面での位置
	const VECTOR WAVE_POS_XZ = VGet(WAVE_POS.x, 0.0f, WAVE_POS.z);
	const VECTOR NEAREST_POS_XZ = VGet(NEAREST_POS.x, 0.0f, NEAREST_POS.z);

	const float DISTANCE = static_cast<float>(UtilityMath::Distance(NEAREST_POS_XZ, WAVE_POS_XZ));
	const float WAVE_RADIUS = WAVE->GetRadius();
	const float TOTAL_THICKNESS = HIT_WAVE_THICKNESS + CAPSULE->GetRadius();

	const bool IS_HIT = std::abs(DISTANCE - WAVE_RADIUS) < TOTAL_THICKNESS;

	if (!IS_HIT)
	{
		return false;
	}

	const float CAPSULE_FOOT_Y = (CAP_START_POS.y < CAP_END_POS.y ? CAP_START_POS.y : CAP_END_POS.y) - CAPSULE->GetRadius();
	const float DIFFERENCE_Y = NEAREST_POS.y - WAVE_POS.y;

	if (CAPSULE_FOOT_Y > (WAVE_POS.y + HIT_WAVE_HEIGHT))
	{
		return false;
	}

	return true;
}

bool CollisionSphere::CheckSphereVsModelCore(const VECTOR& centerPos, float radius,
	const ColliderBase* modelCol, CollisionInfo* outInfo)
{
	if (!modelCol)
	{
		return false;
	}

	const auto* MODEL = static_cast<const ColliderModel*>(modelCol);
	if (MODEL == nullptr)
	{
		return false;
	}

	const int MODEL_HANDLE = MODEL->GetModelHandle();
	if (MODEL_HANDLE == -1)
	{
		return false;
	}

	const MV1_COLL_RESULT_POLY_DIM HIT_RESULT = MV1CollCheck_Sphere(MODEL_HANDLE, -1, centerPos, radius);

	if (HIT_RESULT.HitNum <= 0)
	{
		MV1CollResultPolyDimTerminate(HIT_RESULT);
		return false;
	}

	float maxPenetration = -1.0f;
	int bestIndex = -1;

	const ColliderBase::TAG MODEL_TAG = modelCol->GetCollisionTag();

	for (int i = 0; i < HIT_RESULT.HitNum; ++i)
	{
		if (MODEL->IsExcludedFrame(HIT_RESULT.Dim[i].FrameIndex))
		{
			continue;
		}

		const auto& POLY = HIT_RESULT.Dim[i];

		// タグによる法線フィルタ(カプセル版と同じ基準)
		if (MODEL_TAG == ColliderBase::TAG::STAGE && POLY.Normal.y <= 0.5f)
		{
			continue;
		}
		if (MODEL_TAG == ColliderBase::TAG::WALL && POLY.Normal.y > 0.5f)
		{
			continue;
		}

		const VECTOR POLY_POINT = POLY.Position[0];
		const VECTOR TO_HIT = VSub(POLY_POINT, centerPos);
		const float DIST_ALONG_NORMAL = VDot(TO_HIT, POLY.Normal);
		const float POLY_PENETRATION = radius - DIST_ALONG_NORMAL;

		if (POLY_PENETRATION > maxPenetration)
		{
			maxPenetration = POLY_PENETRATION;
			bestIndex = i;
		}
	}

	if (bestIndex == -1)
	{
		MV1CollResultPolyDimTerminate(HIT_RESULT);
		return false;
	}

	// 呼び出し側が衝突情報を必要としない場合はここで終了
	if (outInfo == nullptr)
	{
		MV1CollResultPolyDimTerminate(HIT_RESULT);
		return true;
	}

	const auto& BEST_HIT = HIT_RESULT.Dim[bestIndex];

	// 呼び出し側で設定する
	outInfo->myCollider = nullptr;
	outInfo->hitCollider = modelCol;

	const VECTOR BEST_POLY_POINT = VScale(
		VAdd(VAdd(BEST_HIT.Position[0], BEST_HIT.Position[1]), BEST_HIT.Position[2]),
		1.0f / 3.0f);

	outInfo->hitPosition = BEST_POLY_POINT;
	outInfo->hitNormal = BEST_HIT.Normal;
	outInfo->isActive = true;

	const VECTOR TO_HIT = VSub(BEST_POLY_POINT, centerPos);
	const float DIST_ALONG_NORMAL = VDot(TO_HIT, BEST_HIT.Normal);

	outInfo->penetration = radius - DIST_ALONG_NORMAL;

	if (outInfo->penetration < 0.0f)
	{
		outInfo->penetration = 0.0f;
	}

	MV1CollResultPolyDimTerminate(HIT_RESULT);
	return true;
}