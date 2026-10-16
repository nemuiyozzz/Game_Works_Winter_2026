#pragma once

#include <vector>
#include "ISystem.h"
#include "../Entity.h"
#include "../Component/DebugGuiComponent.h"

class EcsRegistry;

/// @brief 選択したエンティティのImGuiパラメータを2ウィンドウで表示・編集するシステム
class DebugGuiSystem : public ISystem
{
public:

	/// @brief コンストラクタ
	DebugGuiSystem(void);

	/// @brief デストラクタ
	~DebugGuiSystem(void) override = default;

	/// @brief 更新処理（ImGuiのウィンドウ構築と保存判定）
	/// @param registry ECSレジストリ
	void Update(EcsRegistry& registry) override;

private:

	// 現在選択されているエンティティID
	Entity selectedEntity_;

	/// @brief エンティティ選択リスト（Hierarchyウィンドウ）を描画する
	/// @param registry ECSレジストリ
	/// @param entities 表示対象のエンティティ一覧
	void DrawHierarchyGui(EcsRegistry& registry, const std::vector<Entity>& entities);

	/// @brief 選択中エンティティの詳細パラメータ（Inspectorウィンドウ）を描画する
	/// @param registry ECSレジストリ
	/// @param entity 選択中のエンティティ
	void DrawInspectorGui(EcsRegistry& registry, Entity entity);

	/// @brief TransformComponentのGUI項目を描画する
	/// @param registry ECSレジストリ
	/// @param entity 対象エンティティ
	void DrawTransformGui(EcsRegistry& registry, Entity entity);

	/// @brief プレイヤー移動系コンポーネントのGUI項目を描画する
	/// @param registry ECSレジストリ
	/// @param entity 対象エンティティ
	void DrawPlayerMovementGui(EcsRegistry& registry, Entity entity);

	/// @brief CameraComponentのGUI項目を描画する
	/// @param registry ECSレジストリ
	/// @param entity 対象エンティティ
	void DrawCameraGui(EcsRegistry& registry, Entity entity);

	/// @brief 各種バイナリ・CSVへの保存ボタンを描画する
	/// @param registry ECSレジストリ
	/// @param entity 対象エンティティ
	/// @param debugGui デバッグGUIコンポーネントの設定データ
	void DrawSaveButtonGui(
		EcsRegistry& registry, Entity entity, const DebugGuiComponent& debugGui);
};