#pragma once

#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

/// @brief CameraSystemのデータをもとにカメラを更新するシステム
class CameraSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	CameraSystem(void) = default;

	/// @brief デストラクタ 
	~CameraSystem(void) = default;

	/// @brief カメラを持つすべてのエンティティの座標を計算し、DxLibに適用する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;
};

