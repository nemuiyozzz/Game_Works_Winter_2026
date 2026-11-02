#pragma once

#include <DxLib.h>

/// @brief エネミーのAI状態や索敵・追跡パラメータを保持するコンポーネント
struct EnemyComponent
{
	// エネミーの行動状態種別
	enum class AI_STATE
	{
		IDLE,   // 未発見
		CHASE,  // 発見・交戦中
		ATTACK, // 攻撃中
		RETURN  // 初期位置への帰還中
	};

	// 状態・タイマー関連
	AI_STATE currentState = AI_STATE::IDLE; // 現在の行動状態
	bool isPlayerDetected = false;          // プレイヤー発見フラグ
	float stateTimer = 0.0f;                // 現在ステートの経過時間
	float attackCooldownTimer = 0.0f;       // 攻撃後のクールダウンタイマー

	// 索敵・交戦維持範囲関連
	float sightDistance = 60.0f; // 未発見時の前方索敵距離
	float sightHalfAngle = 10.0f; // 未発見時の前方視野角の半分
	float aggroRadius = 50.0f;   // 発見後の交戦維持半径
	float attackRange = 10.0f;    // 攻撃を開始する距離

	// 移動・帰還パラメータ関連
	VECTOR homePosition = { 0.0f, 0.0f, 0.0f }; // スポーン時の初期位置
	float moveSpeed = 65.0f;                    // 追跡・帰還時の移動速度
	bool isHomeInitialized = false;             // 初期位置の記録完了フラグ
};