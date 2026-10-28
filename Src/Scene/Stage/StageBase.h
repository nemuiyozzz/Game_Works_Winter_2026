#pragma once

#include <vector>
#include <string>
#include <Dxlib.h>

#include "../../Engine/System/ExternalLoader/StageDataLoader.h"
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

/// @brief ステージの共通処理やロード管理を行う基底クラス
class StageBase
{
public:

	/// @brief コンストラクタ
	/// @param registry ECSデータベース
	StageBase(EcsRegistry& registry);

	/// @brief デストラクタ 
	virtual ~StageBase(void);

	/// @brief 非同期ロード処理
	virtual void Load(void) = 0;

	/// @brief ロード完了時の処理
	virtual void EndLoad(void) {};

	/// @brief 初期化処理 
	virtual void Initialize(void);

	/// @brief 更新処理
	virtual void Update(void) = 0;

	/// @brief 描画処理 
	virtual void Draw(void) = 0;

	/// @brief 解放処理 
	virtual void Release(void);

	/// @brief プレイヤーの初期スポーンを取得する 
	/// @return 初期スポーン座標
	virtual VECTOR GetPlayerSpawnPosition(void) const = 0;

protected:

	// ECSデータベース関連
	EcsRegistry& ecsRegistry_;          // シーン側のECSレジストリ参照
	std::vector<Entity> stageEntities_; // 生成したエンティティ一覧

	// ステージファイル・データ関連
	StageDataLoader::LoadedStageData stageData_; // バイナリから読み込んだステージ情報
	std::wstring csvFilePath_;                   // 読み込んだCSVファイルパス
	std::wstring binaryFilePath_;                // 読み込んだバイナリファイルパス

	/// @brief エンティティを生成し、管理リストに登録する
	/// @return 生成したエンティティID
	Entity CreateStageEntity(void);

	/// @brief CSVとバイナリからステージデータを読み込む
	/// @param csvFilePath 開発用CSVファイルのパス
	/// @param binaryFilePath 実行用バイナリファイルのパス
	void LoadStageDataFile(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);
};