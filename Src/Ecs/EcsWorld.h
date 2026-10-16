#pragma once

#include <vector>
#include <memory>
#include "EcsRegistry.h"

class ISystem;
class ModelSystem;

/// @brief EcsRegistryと全Systemの実行順序を一元管理するファサードクラス
class EcsWorld
{
public:

	/// @brief コンストラクタ
	EcsWorld(void) = default;

	/// @brief デストラクタ
	~EcsWorld(void) = default;

	/// @brief 各種システムの生成と登録を行う
	void Initialize(void);

	/// @brief 更新用システムを一括実行する
	void Update(void);

	/// @brief 描画用システムを一括実行する
	void Draw(void);

	/// @brief 管理しているシステムを解放する
	void Release(void);

	/// @brief 内部のECSレジストリを取得する
	/// @return EcsRegistryの参照
	EcsRegistry& GetRegistry(void);

private:

	// ECS管理関連
	EcsRegistry ecsRegistry_;                             // エンティティ・コンポーネント管理
	std::vector<std::shared_ptr<ISystem>> updateSystems_; // 更新用システム一覧
	std::shared_ptr<ModelSystem> modelSystem_;            // 3Dモデル描画システム
};