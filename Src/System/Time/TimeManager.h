#pragma once

#include <unordered_map>
#include <string>
#include <chrono>

/// @brief ゲーム全体の時間管理、タイマー機能を制御するシングルトンクラス
class TimeManager
{
public:

	/// @brief 明示的にインスタンスを生成する
	static void CreateInstance(void);

	/// @brief 静的インスタンスを取得する
	/// @return TimeManagerインスタンスの参照
	static TimeManager& GetInstance(void);

	/// @brief インスタンスを破棄する
	static void DestroyInstance(void);

	/// @brief 各種数値を初期状態にリセットする
	void Reset(void);

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief 現在のゲーム内総時間を取得する
	/// @return 総時間（秒）
	float GetGameTime(void) const;

	/// @brief 現在のゲーム内(時)を取得する
	/// @return 時間
	int GetGameHour(void) const;

	/// @brief 現在のゲーム内(分)を取得する
	/// @return 分
	int GetGameMinute(void) const;

	/// @brief 現在のゲーム内(秒)を取得する
	/// @return 秒
	int GetGameSecond(void) const;

	/// @brief ゲーム内時間を直接設定する
	/// @param targetTime 設定する時間
	void SetGameTime(float targetTime);

	/// @brief 指定したIDでタイマーを開始する
	/// @param timerId タイマー識別用文字列
	/// @param duration 持続時間
	void StartTimer(const std::string& timerId, float duration);

	/// @brief タイマーが終了しているか確認する
	/// @param timerId タイマー識別用文字列
	/// @return 終了していればtrue
	bool IsTimerFinished(const std::string& timerId) const;

	/// @brief タイマーをリセットする
	/// @param timerId タイマー識別用文字列
	void ResetTimer(const std::string& timerId);

	/// @brief ポーズ状態を設定する
	/// @param isPaused ポーズ中ならtrue
	void SetPaused(bool isPaused) { isPaused_ = isPaused; }

	/// @brief 現在ポーズ中か取得する
	/// @return ポーズ中ならtrue
	bool IsPaused(void) const { return isPaused_; }

	/// @brief 前フレームからの経過時間を取得する
	/// @return デルタタイム
	float GetDeltaTime(void) const;

private:

	// タイマー構造体
	struct Timer
	{
		float timeLeft; // 残り時間
		float duration; // 設定された持続時間
	};

	// 定数関連
	static constexpr int SECONDS_PER_MINUTE = 60; // 1分あたりの秒数
	static constexpr int MINUTES_PER_HOUR = 60;   // 1時間あたりの分数
	static constexpr int SECONDS_PER_HOUR = 3600; // 1時間あたりの秒数

	// シングルトン用インスタンス
	static TimeManager* instance_;

	// 時間管理関連
	std::chrono::steady_clock::time_point previousTime_; // 前フレームのシステム時刻
	float gameTime_;                                     // ゲーム開始時からの累積時間
	float gameSpeed_;                                    // 時間の進行速度（1.0が等倍）
	float deltaTime_;                                    // 前フレームからの経過時間

	// ポーズフラグ
	bool isPaused_;

	// 実行中のタイマーリスト
	std::unordered_map<std::string, Timer> timers_;

	/// @brief コンストラクタ
	TimeManager(void) = default;

	/// @brief デストラクタ
	~TimeManager(void) = default;

	/// @brief インスタンスのコピー禁止
	TimeManager(const TimeManager&) = delete;

	/// @brief 代入演算子の禁止
	TimeManager& operator=(const TimeManager&) = delete;
};