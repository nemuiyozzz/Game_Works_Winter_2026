#pragma once

#include <DxLib.h>

/// @brief エネミーの索敵・球状アグロ・追跡パラメータを保持するコンポーネント
struct EnemyAiComponent
{
	// エネミーの行動状態種別
	enum class AI_STATE
	{
		IDLE,   // 未発見（前方の扇形視野で索敵中）
		CHASE,  // 発見・交戦中（球状範囲で全周囲追跡）
		ATTACK, // 攻撃モーション中
		RETURN  // 初期位置への帰還中
	};

	// 状態・タイマー関連
	AI_STATE currentState = AI_STATE::IDLE; // 現在のAIステート
	bool isPlayerDetected = false;          // プレイヤー発見フラグ（ダッシュのスタミナ消費連動）
	float stateTimer = 0.0f;                // ステート経過時間
	float attackCooldownTimer = 0.0f;       // 攻撃後のクールダウンタイマー

	// 索敵（扇形）・交戦維持（球状）範囲関連
	float sightDistance = 400.0f; // 未発見時の前方索敵距離（扇形の半径）
	float sightHalfAngle = 45.0f; // 未発見時の前方視野角の半分（度数法：左右45度）
	float aggroRadius = 700.0f;   // 発見後の交戦維持半径（球状：回り込んでも見失わない）
	float attackRange = 80.0f;    // 攻撃を開始する距離

	// 移動・帰還パラメータ関連
	VECTOR homePosition = { 0.0f, 0.0f, 0.0f }; // スポーン時の初期位置
	float moveSpeed = 65.0f;                    // 追跡・帰還時の移動速度
	bool isHomeInitialized = false;             // 初期位置の記録完了フラグ
};