#pragma once

#include "../SceneBase.h"

/// @brief リザルトシーンクラス
class SceneResult : public SceneBase
{
public:

	/// @brief コンストラクタ
	SceneResult(void);

	/// @brief デストラクタ
	~SceneResult(void) override;

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

protected:

	/// @brief ImGui用の更新処理
	void UpdateGui(void) override;
};