#include "CollisionLine.h"
#include "../Collider/ColliderLine.h"
#include "../Collider/ColliderModel.h"
#include "CollisionInfo.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// 中間座標計算用の係数
	constexpr float HALF_SCALE = 0.5f;
}

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

	// 線分の中間点を基準に、最も近い最大4フレームを抽出する
	const VECTOR LINE_CENTER = VScale(VAdd(START_POS, END_POS), HALF_SCALE);
	int targetFrames[ColliderModel::NEAREST_FRAME_COUNT]{}; // 判定対象フレーム配列
	int frameCount = 0;                                     // 抽出されたフレーム数
	MODEL->FindNearestFrames(LINE_CENTER, targetFrames, frameCount);

	bool hasValidHit = false;           // 有効な衝突があったか
	float maxPenetration = -1.0f;       // 最大めり込み量（終点からの距離）
	MV1_COLL_RESULT_POLY bestHitPoly{}; // 最も深く当たったポリゴン情報

	for (int frameIdx = 0; frameIdx < frameCount; ++frameIdx)
	{
		const MV1_COLL_RESULT_POLY HIT_RESULT = MV1CollCheck_Line(
			MODEL_HANDLE, targetFrames[frameIdx], START_POS, END_POS);

		if (HIT_RESULT.HitFlag == 1)
		{
			if (MODEL->IsExcludedFrame(HIT_RESULT.FrameIndex))
			{
				continue;
			}

			const float PENETRATION = UtilityMath::MagnitudeF(
				VSub(END_POS, HIT_RESULT.HitPosition));

			if (PENETRATION > maxPenetration)
			{
				maxPenetration = PENETRATION;
				bestHitPoly = HIT_RESULT;
				hasValidHit = true;
			}
		}
	}

	if (!hasValidHit)
	{
		return false;
	}

	outInfo.myCollider = lineCol;
	outInfo.hitCollider = modelCol;
	outInfo.isActive = true;

	outInfo.hitPosition = bestHitPoly.HitPosition;
	outInfo.hitNormal = bestHitPoly.Normal;
	outInfo.penetration = maxPenetration;

	return true;
}