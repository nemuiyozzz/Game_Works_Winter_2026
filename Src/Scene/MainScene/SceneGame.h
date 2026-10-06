#pragma once

#include <vector>
#include <memory>
#include "../SceneBase.h"
#include "../../Ecs/Entity.h"
#include "../../Ecs/System/ISystem.h"
#include "../../Ecs/System/ModelSystem.h"

/// @brief ゲーム本編シーンクラス
class SceneGame : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneGame(void);

	/// @brief デストラクタ
	~SceneGame(void) override;

	/// @brief 非同期ロード処理
	void Load(void) override;

	/// @brief ロード完了時の処理
	void EndLoad(void) override {}

	/// @brief 初期化処理
	void Initialize(void) override;

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 描画処理
	void Draw(void) override;

	/// @brief 解放処理
	void Release(void) override;

protected:

	/// @brief ImGui用の更新処理
	void UpdateGui(void) override {}

private:

	// ECSシステムパイプライン
	std::vector<std::shared_ptr<ISystem>> updateSystems_;
	std::shared_ptr<ModelSystem> modelSystem_;

	// エンティテID関連
	Entity playerEntity_;	// プレイヤー
	Entity cameraEntity_;	// カメラ
};