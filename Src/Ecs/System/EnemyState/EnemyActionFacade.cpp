#include "../../../Pch.h"
#include "EnemyActionFacade.h"
#include "../../../Common/Quaternion.h"
#include "../../../Engine/Utility/UtilityMath.h"
#include "../../Component/TransformComponent.h"
#include "../../Component/VelocityComponent.h"
#include "../../Component/StatusComponent.h"
#include "../../Component/AnimationComponent.h"
#include "../AnimationSystem.h"

namespace
{
	// 移動・距離判定関連定数
	constexpr float HOME_ARRIVAL_DISTANCE_SQ = 25.0f; // 初期位置へ到着したとみなす二乗距離
	constexpr float DISTANCE_EPSILON = 0.0001f;       // ゼロ除算防止用の微小値
	constexpr float ROTATION_SMOOTHNESS = 0.15f;      // 振り向き回転の補間係数

	// アニメーション番号・ブレンド関連定数
	constexpr int ENEMY_ANIM_IDLE = 0;                // 待機アニメーション番号
	constexpr int ENEMY_ANIM_MOVE = 1;                // 移動アニメーション番号
	constexpr int ENEMY_ANIM_ATTACK = 2;              // 攻撃アニメーション番号
	constexpr float BLEND_TIME_DEFAULT = 0.15f;       // アニメーションブレンド時間

	// デバッグ描画関連定数
	constexpr int DEBUG_COLOR_CONE = 0xffff00;        // 未発見時の扇形視野カラー（黄色）
	constexpr int DEBUG_COLOR_SPHERE = 0xff3333;      // 発見時の球状アグロ範囲カラー（赤色）
	constexpr int DEBUG_SPHERE_DIV_NUM = 12;          // デバッグ球のポリゴン分割数
	constexpr float DEBUG_DRAW_HEIGHT_OFFSET = 20.0f; // 地面に埋まらないための描画Yオフセット
}

EnemyActionFacade::EnemyActionFacade(EcsRegistry& registry, Entity enemyEntity)
	: registry_(registry)
	, enemyEntity_(enemyEntity)
{
}

void EnemyActionFacade::UpdateTimersAndInitializeHome(float deltaTime)
{
	EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	const TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(enemyEntity_); // トランスフォーム参照

	// StageBattle::Initialize でランダム座標が設定された後の初回フレームで初期位置を記録する
	if (!enemyComp.isHomeInitialized)
	{
		enemyComp.homePosition = transform.position;
		enemyComp.isHomeInitialized = true;
	}

	enemyComp.stateTimer += deltaTime;
	if (enemyComp.attackCooldownTimer > 0.0f)
	{
		enemyComp.attackCooldownTimer -= deltaTime;
	}
}

bool EnemyActionFacade::CheckDead(void) const
{
	if (!registry_.HasComponent<StatusComponent>(enemyEntity_))
	{
		return false;
	}
	const StatusComponent& status =
		registry_.GetComponent<StatusComponent>(enemyEntity_); // ステータス参照
	return (status.currentHp <= 0);
}

EnemyComponent::AI_STATE EnemyActionFacade::GetCurrentState(void) const
{
	return registry_.GetComponent<EnemyComponent>(enemyEntity_).currentState;
}

void EnemyActionFacade::ChangeState(EnemyComponent::AI_STATE nextState)
{
	EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	enemyComp.currentState = nextState;
	enemyComp.stateTimer = 0.0f;

	// AnimationComponent が付与されているモデルの場合のみアニメーションを切り替える
	if (!registry_.HasComponent<AnimationComponent>(enemyEntity_))
	{
		return;
	}

	switch (nextState)
	{
	case EnemyComponent::AI_STATE::IDLE:
		AnimationSystem::Play(
			registry_, enemyEntity_, ENEMY_ANIM_IDLE, true, BLEND_TIME_DEFAULT);
		break;

	case EnemyComponent::AI_STATE::CHASE:
	case EnemyComponent::AI_STATE::RETURN:
		AnimationSystem::Play(
			registry_, enemyEntity_, ENEMY_ANIM_MOVE, true, BLEND_TIME_DEFAULT);
		break;

	case EnemyComponent::AI_STATE::ATTACK:
		AnimationSystem::Play(
			registry_, enemyEntity_, ENEMY_ANIM_ATTACK, false, BLEND_TIME_DEFAULT);
		break;
	}
}

float EnemyActionFacade::GetStateTimer(void) const
{
	return registry_.GetComponent<EnemyComponent>(enemyEntity_).stateTimer;
}

void EnemyActionFacade::SetPlayerDetected(bool isDetected)
{
	registry_.GetComponent<EnemyComponent>(enemyEntity_).isPlayerDetected = isDetected;
}

void EnemyActionFacade::SetAttackCooldown(float cooldownTime)
{
	registry_.GetComponent<EnemyComponent>(enemyEntity_).attackCooldownTimer = cooldownTime;
}

bool EnemyActionFacade::CheckPlayerInConeVision(const VECTOR& playerPosition) const
{
	const EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	const TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(enemyEntity_); // トランスフォーム参照

	VECTOR toPlayer = VSub(playerPosition, transform.position); // プレイヤーへのベクトル
	float distanceSq = VDot(toPlayer, toPlayer); // 3次元二乗距離
	float maxSightDistSq = enemyComp.sightDistance * enemyComp.sightDistance; // 索敵距離二乗

	if (distanceSq > maxSightDistSq || distanceSq <= DISTANCE_EPSILON)
	{
		return false;
	}

	float inverseDistance = 1.0f / std::sqrtf(distanceSq); // 正規化用の逆数
	VECTOR toPlayerDir = VScale(toPlayer, inverseDistance); // 正規化した方向ベクトル

	float forwardAngle = transform.rotationEuler.y - DX_PI_F; // 正面方向の角度
	VECTOR forwardDir = VGet(std::sinf(forwardAngle), 0.0f, std::cosf(forwardAngle)); // 正面ベクトル

	float dotProduct = VDot(forwardDir, toPlayerDir); // 内積（コサイン値）
	float thresholdCos = std::cosf(
		UtilityMath::Deg2RadF(enemyComp.sightHalfAngle)); // 視野角の閾値コサイン

	return (dotProduct >= thresholdCos);
}

bool EnemyActionFacade::CheckPlayerInSphericalAggro(const VECTOR& playerPosition) const
{
	const EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	const TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(enemyEntity_); // トランスフォーム参照

	VECTOR diff = VSub(playerPosition, transform.position); // 3次元差分ベクトル
	float distanceSq = VDot(diff, diff); // 3次元二乗距離
	float aggroRadiusSq = enemyComp.aggroRadius * enemyComp.aggroRadius; // アグロ半径の二乗

	return (distanceSq <= aggroRadiusSq);
}

bool EnemyActionFacade::CheckCanStartAttack(const VECTOR& playerPosition) const
{
	const EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	if (enemyComp.attackCooldownTimer > 0.0f)
	{
		return false;
	}

	const TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(enemyEntity_); // トランスフォーム参照
	float diffX = playerPosition.x - transform.position.x; // X軸差分
	float diffZ = playerPosition.z - transform.position.z; // Z軸差分
	float distanceSq = (diffX * diffX) + (diffZ * diffZ);  // 水平二乗距離
	float attackRangeSq = enemyComp.attackRange * enemyComp.attackRange; // 攻撃距離二乗

	return (distanceSq <= attackRangeSq);
}

bool EnemyActionFacade::CheckArrivedAtHome(void) const
{
	const EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	const TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(enemyEntity_); // トランスフォーム参照

	float diffX = enemyComp.homePosition.x - transform.position.x; // 帰還X差分
	float diffZ = enemyComp.homePosition.z - transform.position.z; // 帰還Z差分
	float distanceSq = (diffX * diffX) + (diffZ * diffZ);          // 帰還二乗距離

	return (distanceSq <= HOME_ARRIVAL_DISTANCE_SQ);
}

void EnemyActionFacade::MoveTowardsPlayer(const VECTOR& playerPosition)
{
	MoveTowardsPosition(playerPosition);
}

void EnemyActionFacade::MoveTowardsHome(void)
{
	const EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	MoveTowardsPosition(enemyComp.homePosition);
}

void EnemyActionFacade::StopHorizontalMovement(void)
{
	if (!registry_.HasComponent<VelocityComponent>(enemyEntity_))
	{
		return;
	}

	VelocityComponent& velocity =
		registry_.GetComponent<VelocityComponent>(enemyEntity_); // 速度コンポーネント
	velocity.velocity.x = 0.0f;
	velocity.velocity.z = 0.0f;
}

void EnemyActionFacade::DrawDebugVisionRange(void) const
{
#ifdef _DEBUG
	const EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	const TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(enemyEntity_); // トランスフォーム参照

	VECTOR originPos = transform.position; // 描画基点座標
	originPos.y += DEBUG_DRAW_HEIGHT_OFFSET;

	if (enemyComp.isPlayerDetected)
	{
		DrawSphere3D(
			originPos,
			enemyComp.aggroRadius,
			DEBUG_SPHERE_DIV_NUM,
			DEBUG_COLOR_SPHERE,
			DEBUG_COLOR_SPHERE,
			FALSE);
	}
	else
	{
		float forwardAngle = transform.rotationEuler.y - DX_PI_F; // 正面角度
		float halfAngleRad = UtilityMath::Deg2RadF(enemyComp.sightHalfAngle); // 半視野角

		float leftAngle = forwardAngle - halfAngleRad;  // 左端の視野角
		float rightAngle = forwardAngle + halfAngleRad; // 右端の視野角

		VECTOR leftEdgePos = VGet(
			originPos.x + std::sinf(leftAngle) * enemyComp.sightDistance,
			originPos.y,
			originPos.z + std::cosf(leftAngle) * enemyComp.sightDistance); // 左端到達点

		VECTOR rightEdgePos = VGet(
			originPos.x + std::sinf(rightAngle) * enemyComp.sightDistance,
			originPos.y,
			originPos.z + std::cosf(rightAngle) * enemyComp.sightDistance); // 右端到達点

		VECTOR centerEdgePos = VGet(
			originPos.x + std::sinf(forwardAngle) * enemyComp.sightDistance,
			originPos.y,
			originPos.z + std::cosf(forwardAngle) * enemyComp.sightDistance); // 中央到達点

		DrawLine3D(originPos, leftEdgePos, DEBUG_COLOR_CONE);
		DrawLine3D(originPos, rightEdgePos, DEBUG_COLOR_CONE);
		DrawLine3D(leftEdgePos, centerEdgePos, DEBUG_COLOR_CONE);
		DrawLine3D(centerEdgePos, rightEdgePos, DEBUG_COLOR_CONE);
	}
#endif // _DEBUG
}

void EnemyActionFacade::MoveTowardsPosition(const VECTOR& targetPosition)
{
	const EnemyComponent& enemyComp =
		registry_.GetComponent<EnemyComponent>(enemyEntity_); // エネミーコンポーネント参照
	TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(enemyEntity_); // トランスフォーム参照

	float diffX = targetPosition.x - transform.position.x; // X軸差分
	float diffZ = targetPosition.z - transform.position.z; // Z軸差分
	float horizontalDistSq = (diffX * diffX) + (diffZ * diffZ); // 水平二乗距離

	if (horizontalDistSq <= DISTANCE_EPSILON)
	{
		StopHorizontalMovement();
		return;
	}

	float inverseDist = 1.0f / std::sqrtf(horizontalDistSq); // 正規化用逆数
	float dirX = diffX * inverseDist; // X方向単位成分
	float dirZ = diffZ * inverseDist; // Z方向単位成分

	if (registry_.HasComponent<VelocityComponent>(enemyEntity_))
	{
		VelocityComponent& velocity =
			registry_.GetComponent<VelocityComponent>(enemyEntity_); // 速度コンポーネント
		velocity.velocity.x = dirX * enemyComp.moveSpeed;
		velocity.velocity.z = dirZ * enemyComp.moveSpeed;
	}

	float targetAngle = std::atan2f(dirX, dirZ) + DX_PI_F; // 目標Y軸角度
	float currentAngle = transform.rotationEuler.y;        // 現在角度
	float diffAngle = targetAngle - currentAngle;          // 角度差分
	float shortestAngle = std::atan2f(std::sinf(diffAngle), std::cosf(diffAngle)); // 最短回転角
	float newAngle = currentAngle + (shortestAngle * ROTATION_SMOOTHNESS); // 補間後角度

	transform.rotationEuler = VGet(0.0f, newAngle, 0.0f);
	transform.rotation_ = Quaternion::CreateEuler(0.0f, newAngle, 0.0f);
}