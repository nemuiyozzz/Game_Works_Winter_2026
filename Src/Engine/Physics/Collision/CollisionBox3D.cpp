#include "../../../Pch.h"
#include "CollisionBox3D.h"
#include "../Collider/ColliderBox.h"
#include "../Collider/ColliderSphere.h"
#include "../../../Ecs/Component/TransformComponent.h"
#include "CollisionInfo.h"
#include "../../Utility/UtilityMath.h"
#include "../Collider/ColliderCapsule.h"

namespace
{
	bool CheckAxisOverlap(const VECTOR& axis,
		const VECTOR& centerA, const VECTOR& halfSizeA, const VECTOR* axesA,
		const VECTOR& centerB, const VECTOR& halfSizeB, const VECTOR* axesB,
		float& outOverlap)
	{
		const float AXIS_LENGTH_SQUARE = VDot(axis, axis);

		if (AXIS_LENGTH_SQUARE < 1e-6f)
		{
			outOverlap = 9999.0f;
			return true;
		}

		const VECTOR NORMALIZED_AXIS = VScale(axis, 1.0f / sqrtf(AXIS_LENGTH_SQUARE));

		// Aの投影半径を計算
		const float RADIUS_A =
			std::abs(VDot(axesA[0], NORMALIZED_AXIS)) * halfSizeA.x +
			std::abs(VDot(axesA[1], NORMALIZED_AXIS)) * halfSizeA.y +
			std::abs(VDot(axesA[2], NORMALIZED_AXIS)) * halfSizeA.z;

		// Bの投影半径を計算
		const float RADIUS_B =
			std::abs(VDot(axesB[0], NORMALIZED_AXIS)) * halfSizeB.x +
			std::abs(VDot(axesB[1], NORMALIZED_AXIS)) * halfSizeB.y +
			std::abs(VDot(axesB[2], NORMALIZED_AXIS)) * halfSizeB.z;

		// 中心点同士の距離を軸に投影
		const float DISTANCE = std::abs(VDot(VSub(centerB, centerA), NORMALIZED_AXIS));

		// 重なり具合を算出
		outOverlap = (RADIUS_A + RADIUS_B) - DISTANCE;

		// 重なりが0より大きければ衝突している
		return outOverlap > 0.0f;
	}
}

bool CollisionBox3D::CheckBoxVsBox(const ColliderBase* boxA, const ColliderBase* boxB,
	CollisionInfo& outInfo)
{
	if (!boxA || !boxB) { return false; }

	const auto* BOX_A = static_cast<const ColliderBox*>(boxA);
	const auto* BOX_B = static_cast<const ColliderBox*>(boxB);

	if (BOX_A == nullptr || BOX_B == nullptr) { return false; }

	const TransformComponent* TRANSFORM_A = BOX_A->GetFollowTarget();
	const TransformComponent* TRANSFORM_B = BOX_B->GetFollowTarget();

	if (!TRANSFORM_A || !TRANSFORM_B) { return false; }

	const VECTOR CENTER_A = BOX_A->GetWorldCenter();
	const VECTOR CENTER_B = BOX_B->GetWorldCenter();

	const VECTOR HALF_SIZE_A = VScale(BOX_A->GetSize(), 0.5f);
	const VECTOR HALF_SIZE_B = VScale(BOX_B->GetSize(), 0.5f);

	// ボックスのローカル軸を取得
	VECTOR axesA[3] = {
		TRANSFORM_A->rotation_.RotatePosition(VGet(1.0f, 0.0f, 0.0f)),
		TRANSFORM_A->rotation_.RotatePosition(VGet(0.0f, 1.0f, 0.0f)),
		TRANSFORM_A->rotation_.RotatePosition(VGet(0.0f, 0.0f, 1.0f))
	};
	VECTOR axesB[3] = {
		TRANSFORM_B->rotation_.RotatePosition(VGet(1.0f, 0.0f, 0.0f)),
		TRANSFORM_B->rotation_.RotatePosition(VGet(0.0f, 1.0f, 0.0f)),
		TRANSFORM_B->rotation_.RotatePosition(VGet(0.0f, 0.0f, 1.0f))
	};

	// 判定する15本の分離軸
	VECTOR testAxes[15] = 
	{
		axesA[0], axesA[1], axesA[2], // Aの面法線
		axesB[0], axesB[1], axesB[2], // Bの面法線
		VCross(axesA[0], axesB[0]), VCross(axesA[0], axesB[1]), VCross(axesA[0], axesB[2]),
		VCross(axesA[1], axesB[0]), VCross(axesA[1], axesB[1]), VCross(axesA[1], axesB[2]),
		VCross(axesA[2], axesB[0]), VCross(axesA[2], axesB[1]), VCross(axesA[2], axesB[2])
	};

	float minimumOverlap = 99999.0f;
	VECTOR hitNormal = UtilityMath::VECTOR_ZERO;

	for (int i = 0; i < 15; ++i)
	{
		float overlap = 0.0f;

		if (!CheckAxisOverlap(testAxes[i], CENTER_A, HALF_SIZE_A, axesA,
			CENTER_B, HALF_SIZE_B, axesB, overlap))
		{
			// 1つでも重ならない軸があれば、完全に分離している
			return false;
		}

		// 最も重なりが小さい軸を押し戻し方向として記録する
		if (overlap < minimumOverlap)
		{
			minimumOverlap = overlap;
			hitNormal = testAxes[i];

			// 正規化して安全性を担保
			const float LENGTH = UtilityMath::MagnitudeF(hitNormal);
			if (LENGTH > 0.0001f)
			{
				hitNormal = VScale(hitNormal, 1.0f / LENGTH);
			}
		}
	}

	// 押し戻しベクトルが常にAからBへ向かうように向きを補正する
	const VECTOR DIRECTION_A_TO_B = VSub(CENTER_B, CENTER_A);
	if (VDot(DIRECTION_A_TO_B, hitNormal) > 0.0f)
	{
		hitNormal = VScale(hitNormal, -1.0f);
	}

	outInfo.myCollider = boxA;
	outInfo.hitCollider = boxB;
	outInfo.isActive = true;
	outInfo.penetration = minimumOverlap;
	outInfo.hitNormal = hitNormal;

	// 衝突位置の近似
	outInfo.hitPosition = VAdd(CENTER_A, VScale(DIRECTION_A_TO_B, 0.5f));

	return true;
}

bool CollisionBox3D::CheckBoxVsSphere(const ColliderBase* boxCol, const ColliderBase* sphereCol,
	CollisionInfo& outInfo)
{
	if (!boxCol || !sphereCol) { return false; }

	const auto* BOX = static_cast<const ColliderBox*>(boxCol);
	const auto* SPHERE = static_cast<const ColliderSphere*>(sphereCol);

	if (BOX == nullptr || SPHERE == nullptr) { return false; }

	const TransformComponent* TRANSFORM = BOX->GetFollowTarget();
	if (!TRANSFORM) { return false; }

	const VECTOR BOX_CENTER = BOX->GetWorldCenter();
	const VECTOR SPHERE_CENTER = SPHERE->GetWorldPosition();
	const VECTOR HALF_SIZE = VScale(BOX->GetSize(), 0.5f);
	const float SPHERE_RADIUS = SPHERE->GetRadius();

	// ボックスのローカル軸を取得
	VECTOR axisRight = TRANSFORM->rotation_.RotatePosition(VGet(1.0f, 0.0f, 0.0f));
	VECTOR axisUp = TRANSFORM->rotation_.RotatePosition(VGet(0.0f, 1.0f, 0.0f));
	VECTOR axisForward = TRANSFORM->rotation_.RotatePosition(VGet(0.0f, 0.0f, 1.0f));

	// 球体の中心座標を、ボックス中心からの相対ベクトルに変換
	VECTOR offset = VSub(SPHERE_CENTER, BOX_CENTER);

	// ボックスのローカル空間に投影
	float localX = VDot(offset, axisRight);
	float localY = VDot(offset, axisUp);
	float localZ = VDot(offset, axisForward);

	// 最も近い点をボックスの半幅の範囲内にクランプする
	float closestX = std::max(-HALF_SIZE.x, std::min(localX, HALF_SIZE.x));
	float closestY = std::max(-HALF_SIZE.y, std::min(localY, HALF_SIZE.y));
	float closestZ = std::max(-HALF_SIZE.z, std::min(localZ, HALF_SIZE.z));

	// クランプしたローカル座標をワールド座標の最近接点に戻す
	VECTOR closestPoint = BOX_CENTER;
	closestPoint = VAdd(closestPoint, VScale(axisRight, closestX));
	closestPoint = VAdd(closestPoint, VScale(axisUp, closestY));
	closestPoint = VAdd(closestPoint, VScale(axisForward, closestZ));

	// 最近接点と球体の中心との距離を計算
	VECTOR distanceVector = VSub(SPHERE_CENTER, closestPoint);
	float distanceSquare = UtilityMath::SqrMagnitude(distanceVector);

	if (distanceSquare < (SPHERE_RADIUS * SPHERE_RADIUS))
	{
		float distance = sqrtf(distanceSquare);

		outInfo.myCollider = boxCol;
		outInfo.hitCollider = sphereCol;
		outInfo.isActive = true;
		outInfo.hitPosition = closestPoint;
		outInfo.penetration = SPHERE_RADIUS - distance;

		if (distance > 0.0001f)
		{
			outInfo.hitNormal = VScale(distanceVector, 1.0f / distance);
		}
		else
		{
			// 完全に中心が一致してしまった場合のフォールバック
			outInfo.hitNormal = VGet(0.0f, 1.0f, 0.0f);
		}

		return true;
	}

	return false;
}

bool CollisionBox3D::CheckBoxVsCapsule(const ColliderBase* boxCol, const ColliderBase* capsuleCol,
	CollisionInfo& outInfo)
{
	if (!boxCol || !capsuleCol) { return false; }

	const auto* BOX = static_cast<const ColliderBox*>(boxCol);
	const auto* CAPSULE = static_cast<const ColliderCapsule*>(capsuleCol);

	if (BOX == nullptr || CAPSULE == nullptr) { return false; }

	const TransformComponent* TRANSFORM = BOX->GetFollowTarget();
	if (!TRANSFORM) { return false; }

	const VECTOR BOX_CENTER = BOX->GetWorldCenter();
	const VECTOR HALF_SIZE = VScale(BOX->GetSize(), 0.5f);

	// ボックスのローカル軸を取得
	VECTOR boxAxes[3] = {
		TRANSFORM->rotation_.RotatePosition(VGet(1.0f, 0.0f, 0.0f)),
		TRANSFORM->rotation_.RotatePosition(VGet(0.0f, 1.0f, 0.0f)),
		TRANSFORM->rotation_.RotatePosition(VGet(0.0f, 0.0f, 1.0f))
	};

	// カプセルの情報を取得
	const VECTOR CAP_START = CAPSULE->GetWorldStartPos();
	const VECTOR CAP_END = CAPSULE->GetWorldEndPos();
	const float CAP_RADIUS = CAPSULE->GetRadius();

	VECTOR capHalfVec = VScale(VSub(CAP_END, CAP_START), 0.5f);
	const VECTOR CAP_CENTER = VAdd(CAP_START, capHalfVec);

	const float CAP_HALF_LEN = UtilityMath::MagnitudeF(capHalfVec);

	// カプセルの線分方向ベクトル
	VECTOR capDir = VGet(0.0f, 1.0f, 0.0f);
	if (CAP_HALF_LEN >= 1e-4f)
	{
		capDir = VScale(capHalfVec, 1.0f / CAP_HALF_LEN);
	}

	// 判定する7本の分離軸
	VECTOR testAxes[7] = {
		boxAxes[0], boxAxes[1], boxAxes[2],
		capDir,
		VCross(boxAxes[0], capDir),
		VCross(boxAxes[1], capDir),
		VCross(boxAxes[2], capDir)
	};

	float minimumOverlap = 99999.0f;
	VECTOR hitNormal = UtilityMath::VECTOR_ZERO;

	for (int i = 0; i < 7; ++i)
	{
		float axisLenSqr = UtilityMath::SqrMagnitude(testAxes[i]);

		// 軸が平行で外積ベクトルが作れなかった場合はスキップ
		if (axisLenSqr < 1e-6f) { continue; }

		VECTOR axis = VScale(testAxes[i], 1.0f / sqrtf(axisLenSqr));

		// 軸に対するボックスの投影半径
		float rBox = std::abs(VDot(boxAxes[0], axis)) * HALF_SIZE.x +
			std::abs(VDot(boxAxes[1], axis)) * HALF_SIZE.y +
			std::abs(VDot(boxAxes[2], axis)) * HALF_SIZE.z;

		// 軸に対するカプセルの投影半径
		float rCap = std::abs(VDot(capHalfVec, axis)) + CAP_RADIUS;

		// 中心同士の距離の投影
		float distance = std::abs(VDot(VSub(CAP_CENTER, BOX_CENTER), axis));

		// 重なりを計算
		float overlap = (rBox + rCap) - distance;

		// 1つでも重なっていない軸があれば衝突していない
		if (overlap <= 0.0f)
		{
			return false;
		}

		// 最も浅い重なりを押し戻し方向として記録
		if (overlap < minimumOverlap)
		{
			minimumOverlap = overlap;
			hitNormal = axis;
		}
	}

	// 法線の向きを ボックス -> カプセル に統一する
	const VECTOR DIRECTION = VSub(CAP_CENTER, BOX_CENTER);
	if (VDot(DIRECTION, hitNormal) < 0.0f)
	{
		hitNormal = VScale(hitNormal, -1.0f);
	}

	outInfo.myCollider = boxCol;
	outInfo.hitCollider = capsuleCol;
	outInfo.isActive = true;
	outInfo.penetration = minimumOverlap;
	outInfo.hitNormal = hitNormal;

	// 近似の衝突座標
	outInfo.hitPosition = VAdd(BOX_CENTER, VScale(DIRECTION, 0.5f));

	return true;
}
