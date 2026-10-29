#pragma once

#include <memory>
#include "../System/PlayerState/IPlayerState.h"

/// @brief プレイヤーの現在ステートとスタミナ・コンボ情報を保持するコンポーネント
struct PlayerStateComponent
{
	// ステート・コンボ管理関連
	std::shared_ptr<IPlayerState> currentState = nullptr; // 現在のステートインスタンス
	float stateTimer = 0.0f;                              // 現在ステートの経過時間
	int comboStep = 0;                                    // 現在のコンボ段数（0〜2）
	bool isNextComboBuffered = false;                     // 次段コンボの先行入力フラグ

	// スタミナ管理関連
	float stamina = 100.0f;                 // 現在のスタミナ
	float maxStamina = 100.0f;              // 最大スタミナ
	float staminaRecoveryRate = 25.0f;      // 秒間のスタミナ自然回復量
	float staminaRecoveryDelayTimer = 0.0f; // スタミナ消費後の回復開始待機タイマー
};