#pragma once

#include <map>
#include <DxLib.h>

#include "../Entity.h"

/// @brief アニメーションの読み込みモード
enum class ANIM_MODE
{
	INTERNAL,	// モデル内部に含まれるアニメーション
	EXTERNAL,	// 外部ファイルから読み込むアニメーション
};

/// @brief 1つのアニメーション再生に必要な設定データ
struct AnimData
{
	int model = -1;							// 外部アニメーション用モデルハンドル
	int attachNo = -1;						// DxLibのアタッチ番号
	int animIndex = 0;						// アニメーションインデックス
	float speed = 1.0f;						// 再生速度
	float totalTime = 0.0f;					// アニメーションの総時間
	float step = 0.0f;						// アニメーションの現在の進行時間
	ANIM_MODE mode = ANIM_MODE::INTERNAL;	// アニメーションモード
};

/// @brief アニメーションの管理状態を保持するコンポーネント
struct AnimationComponent
{
	// 登録済みアニメーションのマップ
	std::map<int, AnimData> animations;

	// 再生状態関連
	int playType = -1;	// 現在再生中のアニメーションタイプ
	int prevType = -1;	// 1つ前に再生していたアニメーションタイプ

	// フラグ関連
	bool isLoop = false;
	bool isPaused = false;

	// ブレンド関連
	float blendTime = 0.0f;		// 切り替え時の総補完時間
	float blendTimer = 0.0f;	// 現在の補完進行度
};