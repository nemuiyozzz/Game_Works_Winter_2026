#pragma once

#include <vector>

/// @brief アニメーションの読み込みモード
enum class ANIM_MODE
{
	INTERNAL, // モデル内部に含まれるアニメーション
	EXTERNAL, // 外部ファイルから読み込むアニメーション
};

/// @brief 1つのアニメーション再生に必要な設定データ
struct AnimData
{
	bool isRegistered = false;            // このインデックスにデータが登録されているか
	int modelHandle = -1;                 // 外部アニメーション用モデルハンドル
	int attachNumber = -1;                // DxLibのアタッチ番号
	int animationIndex = 0;               // アニメーションインデックス
	float playSpeed = 1.0f;               // 再生速度
	float totalTime = 0.0f;               // アニメーションの総時間
	float currentStep = 0.0f;             // アニメーションの現在の進行時間
	ANIM_MODE mode = ANIM_MODE::INTERNAL; // アニメーションモード
};

/// @brief アニメーションの管理状態を保持するコンポーネント
struct AnimationComponent
{
	// 登録済みアニメーションの配列（タイプIDをそのままインデックスとして使用）
	std::vector<AnimData> animations;

	// 再生状態関連
	int playType = -1;   // 現在再生中のアニメーションタイプ
	int previousType = -1; // 1つ前に再生していたアニメーションタイプ（ブレンド用）

	// フラグ関連
	bool isLoop = false;
	bool isPaused = false;

	// ブレンド（フェード遷移）関連
	float blendTime = 0.0f;   // 切り替え時の総補完時間
	float blendTimer = 0.0f;  // 現在の補完進行度
};