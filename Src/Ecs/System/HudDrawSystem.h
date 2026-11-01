#pragma once

#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

/// @brief プレイヤーのHUDを2D描画するシステムクラス
class HudDrawSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	HudDrawSystem(void) = default;

	/// @brief デストラクタ 
	~HudDrawSystem(void) override = default;

	/// @brief HUDの描画を更新する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;
};

