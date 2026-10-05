#pragma once

#include <memory>
#include <vector>
#include "ISystem.h"
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"
#include "../../Shader/RenderCommand.h"
#include "../../Shader/ShaderRenderer.h"

/// @brief シェーダを用いた描画命令を収集し、バッチ処理を実行するシステム
class ShaderSystem : public ISystem
{
public:

	/// @brief コンストラクタ
	ShaderSystem(void);

	/// @brief デストラクタ
	~ShaderSystem(void) = default;

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief 解放処理
	void Release(void);

	/// @brief 描画可能なエンティティをすべて検索し、描画命令をキューに登録する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief ECSデータベースから対象を検索し、描画命令をキューに登録する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void QueueDrawCommand(EcsRegistry& registry, Entity entity);

	/// @brief 溜まった描画命令をソートして一括実行する（描画フェーズの最後に呼ぶ）
	void ExecuteDrawCommands(void);

private:

	std::unique_ptr<ShaderRenderer> shaderRenderer_; // シェーダの描画クラス
	std::vector<RenderCommand> renderCommandQueue_;  // 描画命令のリスト

};