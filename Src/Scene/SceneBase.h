#pragma once

#include "../Ecs/EcsRegistry.h"

/// @brief シーンの基底クラス
class SceneBase
{
public:

	/// @brief コンストラクタ
	SceneBase(void);

	/// @brief 仮想デストラクタ
	virtual ~SceneBase(void) = default;

	/// @brief リソースの非同期ロード処理
	virtual void Load(void) {}

	/// @brief ロード完了時の処理
	virtual void EndLoad(void) {}

	/// @brief 初期化処理
	virtual void Initialize(void) {}

	/// @brief 更新処理
	virtual void Update(void) {}

	/// @brief 描画処理
	virtual void Draw(void) {}

	/// @brief 解放処理
	virtual void Release(void) {}

protected:

	/// @brief ImGui用の更新処理
	virtual void UpdateGui(void) {}

	// 各シーン専用のECSデータベース
	EcsRegistry ecsRegistry_; 
};