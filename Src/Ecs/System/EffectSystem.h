#pragma once

#include <string>
#include <DxLib.h>
#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

/// @brief EffectComponentの再生・追従とEffekseer全体の更新・描画を行うシステム
class EffectSystem : public ISystem
{
public:

	/// @brief コンストラクタ
	EffectSystem(void) = default;

	/// @brief デストラクタ
	~EffectSystem(void) override = default;

	/// @brief エフェクトの再生・座標同期およびEffekseer全体の時間更新を行う
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief Effekseerの3Dエフェクトを一括描画する
	void Draw(void);

	/// @brief 指定エンティティから単発エフェクトを即座に再生するヘルパー関数
	/// @param registry ECSデータベース
	/// @param targetEntity 発生源となるエンティティID
	/// @param resourceKey エフェクトのリソースキー名
	/// @param scale エフェクトの拡大率
	/// @param speed 再生速度
	static void RequestPlayEffect(
		EcsRegistry& registry,
		Entity targetEntity,
		const std::wstring& resourceKey,
		const VECTOR& scale = { 10.0f, 10.0f, 10.0f },
		float speed = 1.0f);
};