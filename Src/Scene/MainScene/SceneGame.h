#pragma once

#include <memory>
#include <vector>

#include "../SceneBase.h"
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"
#include "../../Ecs/System/ISystem.h"
#include "../../Ecs/System/ModelSystem.h"
#include "../Stage/StageManager.h"

/// @brief ゲーム本編のシーンクラス
class SceneGame : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneGame(void);

	/// @brief デストラクタ
	~SceneGame(void) override;

	/// @brief リソースのロード処理
	void Load(void) override;

	/// @brief 初期化処理
	void Initialize(void) override;

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 描画処理
	void Draw(void) override;

	/// @brief 解放処理
	void Release(void) override;

private:

	// ECS・システム関連
	EcsRegistry ecsRegistry_;                             // ECSデータベース
	std::vector<std::shared_ptr<ISystem>> updateSystems_; // 更新用システム一覧
	std::shared_ptr<ModelSystem> modelSystem_;            // 3Dモデル描画システム

	// エンティティ関連
	Entity playerEntity_; // プレイヤーエンティティID
	Entity cameraEntity_; // カメラエンティティID

	// ステージ管理オブジェクト
	std::unique_ptr<StageManager> stageManager_;
};