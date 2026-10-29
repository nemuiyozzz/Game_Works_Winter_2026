#include "../../../Pch.h"
#include "PlayerActionFacade.h"
#include <cmath>
#include "../../../Engine/System/Input/KeyConfInputManager.h"
#include "../../../Engine/Utility/UtilityMath.h"
#include "../../../Common/Quaternion.h"
#include "../../Component/TransformComponent.h"
#include "../../Component/VelocityComponent.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/StatusComponent.h"
#include "../../Component/PlayerInputComponent.h"
#include "../../Component/PlayerStateComponent.h"
#include "../../Component/EnemyAiComponent.h"
#include "../../Component/CollisionComponent.h"
#include "../AnimationSystem.h"
#include "../PlayerControlSystem.h"

namespace
{
	// 移動倍率・物理判定関連定数
	constexpr float SPRINT_SPEED_MULTIPLIER = 1.75f; // 通常移動に対する疾走速度倍率
	constexpr float EVADE_SPEED_MULTIPLIER = 2.2f;   // 通常移動に対する回避ステップ倍率
	constexpr float ROTATION_SMOOTHNESS = 0.2f;      // 回転補間の滑らかさ
	constexpr float GROUNDED_VELOCITY_EPSILON = 0.5f;// 接地とみなすY速度の許容誤差
	constexpr float STAMINA_RECOVERY_DELAY = 0.8f;   // スタミナ消費後の回復待機秒数

	// アニメーションブレンド時間関連
	constexpr float BLEND_TIME_IDLE = 0.15f;         // 待機アニメーションのブレンド値
	constexpr float BLEND_TIME_MOVE = 0.15f;         // 移動アニメーションのブレンド値
	constexpr float BLEND_TIME_SPRINT = 0.10f;       // 疾走アニメーションのブレンド値
	constexpr float BLEND_TIME_JUMP = 0.20f;         // ジャンプアニメーションのブレンド値
	constexpr float BLEND_TIME_EVADE = 0.10f;        // 回避アニメーションのブレンド値
	constexpr float BLEND_TIME_ATTACK = 0.10f;       // 攻撃アニメーションのブレンド値
}

PlayerActionFacade::PlayerActionFacade(EcsRegistry& registry, Entity entity)
	: registry_(registry)
	, entity_(entity)
{
}

bool PlayerActionFacade::CheckMoveInput(void) const
{
	VECTOR moveDirection = GetInputMoveDirection(); // 入力方向ベクトル
	return (VSize(moveDirection) > 0.0f);
}

bool PlayerActionFacade::CheckSprintHeld(void) const
{
	return KeyConfInputManager::GetInstance().IsPressed(L"DODGE");
}

bool PlayerActionFacade::CheckJumpTriggered(void) const
{
	return KeyConfInputManager::GetInstance().IsTriggerDown(L"JUMP");
}

bool PlayerActionFacade::CheckAttackTriggered(void) const
{
	KeyConfInputManager& input = KeyConfInputManager::GetInstance(); // 入力管理参照
	return input.IsTriggerDown(L"ATTACK_NORMAL") || input.IsTriggerDown(L"ATTACK_SPECIAL");
}

bool PlayerActionFacade::CheckEvadeTriggered(void) const
{
	return KeyConfInputManager::GetInstance().IsTriggerDown(L"DODGE");
}

bool PlayerActionFacade::MoveNormal(void)
{
	if (!registry_.HasComponent<PlayerInputComponent>(entity_))
	{
		return false;
	}
	const PlayerInputComponent& inputInfo =
		registry_.GetComponent<PlayerInputComponent>(entity_); // 入力コンポーネント
	return ApplyCameraRelativeMovement(inputInfo.moveSpeed);
}

bool PlayerActionFacade::MoveSprint(void)
{
	if (!registry_.HasComponent<PlayerInputComponent>(entity_))
	{
		return false;
	}
	const PlayerInputComponent& inputInfo =
		registry_.GetComponent<PlayerInputComponent>(entity_); // 入力コンポーネント
	return ApplyCameraRelativeMovement(inputInfo.moveSpeed * SPRINT_SPEED_MULTIPLIER);
}

void PlayerActionFacade::MoveInAir(void)
{
	if (!registry_.HasComponent<PlayerInputComponent>(entity_))
	{
		return;
	}
	const PlayerInputComponent& inputInfo =
		registry_.GetComponent<PlayerInputComponent>(entity_); // 入力コンポーネント
	float speedMultiplier = CheckSprintHeld() ? SPRINT_SPEED_MULTIPLIER : 1.0f; // 速度倍率
	ApplyCameraRelativeMovement(inputInfo.moveSpeed * speedMultiplier);
}

void PlayerActionFacade::ExecuteEvadeStep(void)
{
	if (!registry_.HasComponent<PlayerInputComponent>(entity_))
	{
		return;
	}
	const PlayerInputComponent& inputInfo =
		registry_.GetComponent<PlayerInputComponent>(entity_); // 入力コンポーネント
	float evadeSpeed = inputInfo.moveSpeed * EVADE_SPEED_MULTIPLIER; // 回避ステップ速度

	if (!ApplyCameraRelativeMovement(evadeSpeed))
	{
		ExecuteForwardStep(EVADE_SPEED_MULTIPLIER);
	}
}

void PlayerActionFacade::ExecuteForwardStep(float forwardSpeedRatio)
{
	if (!registry_.HasComponent<TransformComponent>(entity_) ||
		!registry_.HasComponent<VelocityComponent>(entity_) ||
		!registry_.HasComponent<PlayerInputComponent>(entity_))
	{
		return;
	}

	const TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(entity_); // トランスフォーム参照
	const PlayerInputComponent& inputInfo =
		registry_.GetComponent<PlayerInputComponent>(entity_); // 入力コンポーネント
	VelocityComponent& velocityData =
		registry_.GetComponent<VelocityComponent>(entity_); // 速度コンポーネント

	float stepSpeed = inputInfo.moveSpeed * forwardSpeedRatio; // 踏み込み速度
	float forwardAngle = transform.rotationEuler.y - DX_PI_F;  // モデルの正面角度
	velocityData.velocity.x = std::sinf(forwardAngle) * stepSpeed;
	velocityData.velocity.z = std::cosf(forwardAngle) * stepSpeed;
}

void PlayerActionFacade::DecelerateHorizontalMovement(float decelerationRate)
{
	if (!registry_.HasComponent<VelocityComponent>(entity_))
	{
		return;
	}

	VelocityComponent& velocityData =
		registry_.GetComponent<VelocityComponent>(entity_); // 速度コンポーネント
	velocityData.velocity.x *= decelerationRate;
	velocityData.velocity.z *= decelerationRate;
}

void PlayerActionFacade::StopHorizontalMovement(void)
{
	if (!registry_.HasComponent<VelocityComponent>(entity_))
	{
		return;
	}

	VelocityComponent& velocityData =
		registry_.GetComponent<VelocityComponent>(entity_); // 速度コンポーネント
	velocityData.velocity.x = 0.0f;
	velocityData.velocity.z = 0.0f;
}

void PlayerActionFacade::ExecuteJumpVelocity(void)
{
	if (!registry_.HasComponent<PlayerInputComponent>(entity_) ||
		!registry_.HasComponent<VelocityComponent>(entity_))
	{
		return;
	}

	const PlayerInputComponent& inputInfo =
		registry_.GetComponent<PlayerInputComponent>(entity_); // 入力コンポーネント
	VelocityComponent& velocityData =
		registry_.GetComponent<VelocityComponent>(entity_); // 速度コンポーネント

	velocityData.velocity.y = inputInfo.jumpPower;
}

void PlayerActionFacade::ExecuteDownwardDive(float downwardSpeedRatio)
{
	if (!registry_.HasComponent<PlayerInputComponent>(entity_) ||
		!registry_.HasComponent<VelocityComponent>(entity_))
	{
		return;
	}

	const PlayerInputComponent& inputInfo =
		registry_.GetComponent<PlayerInputComponent>(entity_); // 入力コンポーネント
	VelocityComponent& velocityData =
		registry_.GetComponent<VelocityComponent>(entity_); // 速度コンポーネント

	velocityData.velocity.y = -std::abs(inputInfo.jumpPower * downwardSpeedRatio);
}

bool PlayerActionFacade::CheckGrounded(void) const
{
	if (!registry_.HasComponent<VelocityComponent>(entity_))
	{
		return true;
	}

	const VelocityComponent& velocityData =
		registry_.GetComponent<VelocityComponent>(entity_); // 速度コンポーネント
	return (std::abs(velocityData.velocity.y) <= GROUNDED_VELOCITY_EPSILON);
}

bool PlayerActionFacade::TryConsumeStamina(float cost)
{
	if (!registry_.HasComponent<PlayerStateComponent>(entity_))
	{
		return true;
	}

	PlayerStateComponent& stateComponent =
		registry_.GetComponent<PlayerStateComponent>(entity_); // ステートコンポーネント
	if (stateComponent.stamina < cost)
	{
		return false;
	}

	stateComponent.stamina -= cost;
	if (stateComponent.stamina < 0.0f)
	{
		stateComponent.stamina = 0.0f;
	}
	stateComponent.staminaRecoveryDelayTimer = STAMINA_RECOVERY_DELAY;
	return true;
}

bool PlayerActionFacade::CheckInCombat(void) const
{
	std::vector<Entity> enemies =
		registry_.GetEntitiesWith<EnemyAiComponent>(); // エネミー一覧
	for (Entity enemyEntity : enemies)
	{
		const EnemyAiComponent& enemyAi =
			registry_.GetComponent<EnemyAiComponent>(enemyEntity); // エネミーAI参照
		if (enemyAi.isPlayerDetected)
		{
			return true;
		}
	}
	return false;
}

void PlayerActionFacade::SetInvincibleTimer(float invincibleTime)
{
	if (registry_.HasComponent<StatusComponent>(entity_))
	{
		StatusComponent& status =
			registry_.GetComponent<StatusComponent>(entity_); // ステータス参照
		status.invincibleTimer = invincibleTime;
	}
}

void PlayerActionFacade::SetSwordColliderActive(bool isActive)
{
	if (!registry_.HasComponent<CollisionComponent>(entity_))
	{
		return;
	}

	CollisionComponent& collision =
		registry_.GetComponent<CollisionComponent>(entity_); // 衝突コンポーネント
	for (auto& collider : collision.colliders)
	{
		if (collider != nullptr &&
			collider->GetCollisionTag() == ColliderBase::TAG::PLAYER_SWORD)
		{
			collider->SetActive(isActive);
		}
	}
}

float PlayerActionFacade::GetStateTimer(void) const
{
	if (!registry_.HasComponent<PlayerStateComponent>(entity_))
	{
		return 0.0f;
	}
	return registry_.GetComponent<PlayerStateComponent>(entity_).stateTimer;
}

void PlayerActionFacade::SetupComboStep(int comboStep)
{
	if (!registry_.HasComponent<PlayerStateComponent>(entity_))
	{
		return;
	}
	PlayerStateComponent& stateComponent =
		registry_.GetComponent<PlayerStateComponent>(entity_);
	stateComponent.comboStep = comboStep;
	stateComponent.isNextComboBuffered = false;
}

void PlayerActionFacade::SetNextComboBuffered(bool isBuffered)
{
	if (registry_.HasComponent<PlayerStateComponent>(entity_))
	{
		registry_.GetComponent<PlayerStateComponent>(entity_).isNextComboBuffered = isBuffered;
	}
}

bool PlayerActionFacade::CheckNextComboBuffered(void) const
{
	if (!registry_.HasComponent<PlayerStateComponent>(entity_))
	{
		return false;
	}
	return registry_.GetComponent<PlayerStateComponent>(entity_).isNextComboBuffered;
}

bool PlayerActionFacade::CheckAnimationEnded(void) const
{
	return AnimationSystem::IsEnd(registry_, entity_);
}

void PlayerActionFacade::PlayIdleAnimation(void)
{
	AnimationSystem::Play(
		registry_,
		entity_,
		static_cast<int>(PlayerControlSystem::PLAYER_ANIM::IDLE),
		true,
		BLEND_TIME_IDLE);
}

void PlayerActionFacade::PlayMoveAnimation(void)
{
	AnimationSystem::Play(
		registry_,
		entity_,
		static_cast<int>(PlayerControlSystem::PLAYER_ANIM::MOVE),
		true,
		BLEND_TIME_MOVE);
}

void PlayerActionFacade::PlaySprintAnimation(void)
{
	AnimationSystem::Play(
		registry_,
		entity_,
		static_cast<int>(PlayerControlSystem::PLAYER_ANIM::MOVE),
		true,
		BLEND_TIME_SPRINT);
}

void PlayerActionFacade::PlayJumpAnimation(void)
{
	AnimationSystem::Play(
		registry_,
		entity_,
		static_cast<int>(PlayerControlSystem::PLAYER_ANIM::JUMP),
		false,
		BLEND_TIME_JUMP);
}

void PlayerActionFacade::PlayEvadeAnimation(void)
{
	AnimationSystem::Play(
		registry_,
		entity_,
		static_cast<int>(PlayerControlSystem::PLAYER_ANIM::MOVE),
		false,
		BLEND_TIME_EVADE);
}

void PlayerActionFacade::PlayAttackAnimation(void)
{
	AnimationSystem::Play(
		registry_,
		entity_,
		static_cast<int>(PlayerControlSystem::PLAYER_ANIM::ATTACK),
		false,
		BLEND_TIME_ATTACK);
}

bool PlayerActionFacade::ApplyCameraRelativeMovement(float moveSpeed)
{
	if (!registry_.HasComponent<TransformComponent>(entity_) ||
		!registry_.HasComponent<VelocityComponent>(entity_))
	{
		return false;
	}

	VECTOR moveDirection = GetInputMoveDirection(); // 入力方向ベクトル
	if (VSize(moveDirection) <= 0.0f)
	{
		return false;
	}

	float cameraAngleH = 0.0f; // カメラの水平角度
	std::vector<Entity> cameras =
		registry_.GetEntitiesWith<CameraComponent>(); // カメラ一覧

	if (!cameras.empty())
	{
		const CameraComponent& camera =
			registry_.GetComponent<CameraComponent>(cameras[0]); // カメラ参照
		cameraAngleH = camera.angleH;
	}

	float cosH = std::cosf(cameraAngleH); // 水平角コサイン
	float sinH = std::sinf(cameraAngleH); // 水平角サイン

	VECTOR rotatedDir{}; // カメラ基準の移動方向ベクトル
	rotatedDir.x = (moveDirection.x * cosH) - (moveDirection.z * sinH);
	rotatedDir.y = 0.0f;
	rotatedDir.z = (moveDirection.x * sinH) + (moveDirection.z * cosH);
	rotatedDir = UtilityMath::VNormalize(rotatedDir);

	VelocityComponent& velocityData =
		registry_.GetComponent<VelocityComponent>(entity_); // 速度コンポーネント
	VECTOR moveVelocity = VScale(rotatedDir, moveSpeed);    // 移動速度ベクトル
	velocityData.velocity.x = moveVelocity.x;
	velocityData.velocity.z = moveVelocity.z;

	TransformComponent& transform =
		registry_.GetComponent<TransformComponent>(entity_); // トランスフォーム参照
	float targetAngle = std::atan2f(rotatedDir.x, rotatedDir.z) + DX_PI_F; // 目標角度
	float currentAngle = transform.rotationEuler.y; // 現在角度
	float diffAngle = targetAngle - currentAngle;   // 角度差分
	float shortestAngle = std::atan2f(std::sinf(diffAngle), std::cosf(diffAngle)); // 最短回転角
	float newAngle = currentAngle + (shortestAngle * ROTATION_SMOOTHNESS); // 補間後角度

	transform.rotationEuler = VGet(0.0f, newAngle, 0.0f);
	transform.rotation_ = Quaternion::CreateEuler(0.0f, newAngle, 0.0f);

	return true;
}

VECTOR PlayerActionFacade::GetInputMoveDirection(void) const
{
	KeyConfInputManager& input = KeyConfInputManager::GetInstance(); // 入力管理参照
	VECTOR moveDirection = input.GetLeftStickDirection(); // スティック入力方向

	if (VSize(moveDirection) == 0.0f)
	{
		float inputX = 0.0f; // 左右入力
		float inputZ = 0.0f; // 前後入力

		if (input.IsPressed(L"UP"))
		{
			inputZ += 1.0f;
		}
		if (input.IsPressed(L"DOWN"))
		{
			inputZ -= 1.0f;
		}
		if (input.IsPressed(L"RIGHT"))
		{
			inputX += 1.0f;
		}
		if (input.IsPressed(L"LEFT"))
		{
			inputX -= 1.0f;
		}

		VECTOR wasdDirection = VGet(inputX, 0.0f, inputZ); // キー入力ベクトル
		if (VSize(wasdDirection) > 0.0f)
		{
			moveDirection = UtilityMath::VNormalize(wasdDirection);
		}
	}

	return moveDirection;
}