#pragma once

#include <memory>

#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

class StageBase;
class Fader;
class Loading;

/// @brief ステージの切り替えとフェード・ロード進行を管理するクラス
class StageManager
{
public:

	/// @brief コンストラクタ
	/// @param registry ECSデータベースの参照
	/// @param playerEntity 初期位置へ移動させるプレイヤーのエンティティID
	StageManager(EcsRegistry& registry, Entity playerEntity);

	/// @brief デストラクタ 
	~StageManager(void);

	/// @brief 初期化処理 
	void Initialize(void);

	/// @brief 更新処理 
	void Update(void);

	/// @brief ステージの描画処理 
	void DrawStage(void);

	/// @brief 演出の描画処理 
	void DrawOverlay(void);

	/// @brief 解放処理 
	void Release(void);

	/// @brief ステージを変更する
	/// @param nextStage 次に実行するステージのポインタ
	void ChangeStage(std::shared_ptr<StageBase> nextStage);

	/// @brief 初回起動時など、フェードを挟まず即座にステージをセットアップする
	/// @param firstStage 最初に実行するステージのポインタ
	void SetupFirstStage(std::shared_ptr<StageBase> firstStage);

	/// @brief ステージ切り替え中かどうかを判定する
	/// @return 切り替え中であればtrue
	bool GetStageChanging(void) const { return isStageChangeing_; }

private:

	// ECS関連
	EcsRegistry& ecsRegistry_;	// ECSデータベース
	Entity playerEntity_;		// プレイヤーエンティティID

	// ステージ管理関連
	std::shared_ptr<StageBase> currentStage_;	// 現在実行中のステージ
	std::shared_ptr<StageBase> nextStage_;		// 次に遷移するステージ
	std::shared_ptr<StageBase> oldStage_;		// 古いステージ
	bool isStageChangeing_;						// ステージ切り替え中フラグ

	// 演出・ロード関連
	std::unique_ptr<Fader> fader_;				// フェード管理オブジェクト
	std::unique_ptr<Loading> loading_;			// ローディング管理オブジェクト

	/// @brief プレイヤーの座標と速度を現在のステージの初期位置にリセットする
	void ResetPlayerTransform(void);
};

