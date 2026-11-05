#pragma once

#include "../SceneBase.h"
#include "../../Ecs/EcsWorld.h"
#include "../../Common/Vector2.h"
#include <vector>

/// @brief タイトルシーンクラス
class SceneTitle : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneTitle(void);

	/// @brief デストラクタ
	~SceneTitle(void) override;

	/// @brief リソースの非同期ロード処理
	void Load(void) override;

	/// @brief ロード完了時の処理
	void EndLoad(void) override;

	/// @brief 初期化処理
	void Initialize(void) override;

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 描画処理
	void Draw(void) override;

	/// @brief 解放処理
	void Release(void) override;

private:

	EcsWorld ecsWorld_;

	// メニュー制御関連
	int selectedMenuIndex_ = 0;          // 0: NewGame, 1: QuitGame
	bool isMouseMode_ = true;            // マウスで操作しているか
	Vector2F prevMousePos_ = { 0.0f, 0.0f }; // 前回のマウス座標

	// エンティティ
	Entity cursorEntity_ = NULL_ENTITY;  // マウスカーソル
	Entity newGameEntity_ = NULL_ENTITY; // NewGameボタン
	Entity quitGameEntity_ = NULL_ENTITY; // QuitGameボタン

	// 分割画像ハンドル保持用
	std::vector<int> menuHandles_;
};