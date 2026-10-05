#pragma once

#include "../EcsRegistry.h"

class ISystem
{
public:

	/// @brief デストラクタ 
	virtual ~ISystem(void) = default;

	/// @brief システムごとの更新処理
	/// @param registry ECSデータベース
	virtual void Update(EcsRegistry& registry) = 0;
};