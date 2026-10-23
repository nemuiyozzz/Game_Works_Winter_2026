#pragma once

#include <memory>
#include "../SceneBase.h"
#include "../Stage/StageManager.h"
#include "../../Ecs/EcsWorld.h"
#include "../../Ecs/Entity.h"

/// @brief ゲーム本編のシーンクラス
class SceneGame : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneGame(void);

	/// @brief デストラクタ
	~SceneGame(void) override;

	/// @brief リソースの読み込み処理
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

	// ECS・ステージ管理関連
	EcsWorld ecsWorld_;                          // ECS統括ファサード
	std::unique_ptr<StageManager> stageManager_; // ステージ管理

	// 主要エンティティID関連
	Entity playerEntity_; // プレイヤーエンティティID
	Entity cameraEntity_; // カメラエンティティID

	int defeatedEnemyCount_;
};