#pragma once

#include <functional>

/// @brief ローディング画面を制御するクラス
class Loading
{
public:

	/// @brief コンストラクタ
	Loading(void);

	/// @brief デストラクタ
	~Loading(void);

	/// @brief 初期化する
	void Initialize(void);

	/// @brief 更新する
	void Update(void);

	/// @brief 描画する
	void Draw(void);

	/// @brief 非同期ロードを開始する
	/// @param loadFunction ロード中に実行する関数オブジェクト
	void StartAsyncLoad(std::function<void()> loadFunction);

	/// @brief ロード完了処理を行う
	void EndAsyncLoad(void);

	/// @brief ロード中か確認する
	/// @return bool ロード中ならtrue
	bool IsLoading(void) const;

	/// @brief 進捗率を取得する
	/// @return int 進捗率
	int GetProgress(void) const;

	/// @brief 進捗率を設定する
	/// @param progress 設定する進捗率
	void SetProgress(float progress);

private:

	// 定数関連
	static constexpr float MAXIMUM_PROGRESS = 100.0f; // 進捗率の最大値
	static constexpr float MINIMUM_PROGRESS = 0.0f;   // 進捗率の最小値

	// 状態関連
	bool isLoading_; // ロード中フラグ
	float progress_; // 進捗率

};