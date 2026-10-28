#pragma once

#include <string>
#include <DxLib.h>
#include "../../Engine/Utility/UtilityMath.h"

/// @brief Effekseerエフェクトの状態とパラメータを保持するコンポーネント
struct EffectComponent
{
	// リソース・ハンドル関連
	std::wstring resourceKey = L""; // ResourceManagerで取得するキー名
	int playHandle = -1;            // 再生中のEffekseerインスタンスハンドル

	// トランスフォーム・速度関連
	VECTOR localOffset = UtilityMath::VECTOR_ZERO; // エンティティ座標からのローカルズレ
	VECTOR scale = { 10.0f, 10.0f, 10.0f };        // エフェクトの拡大率
	float speed = 1.0f;                            // 再生速度

	// 再生制御フラグ関連
	bool isPlayRequested = true; // 再生リクエスト（trueにすると再生開始）
	bool isLoop = false;         // 再生終了時に自動で再再生するか
	bool isFollow = true;        // エンティティの座標・回転に毎フレーム追従するか
};