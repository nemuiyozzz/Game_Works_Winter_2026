#include "../../Pch.h"
#include "TimeManager.h"

// 静的インスタンスの初期化
TimeManager* TimeManager::instance_ = nullptr;

void TimeManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new TimeManager();
	}
}

TimeManager& TimeManager::GetInstance(void)
{
	return *instance_;
}

void TimeManager::DestroyInstance(void)
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void TimeManager::Reset(void)
{
	gameTime_ = 0.0f;
	timers_.clear();
	previousTime_ = std::chrono::steady_clock::now();
}

void TimeManager::Initialize(void)
{
	gameTime_ = 0.0f;
	gameSpeed_ = 1.0f;
	timers_.clear();
	isPaused_ = false;
	deltaTime_ = 0.0f;

	// 初期化時の時間を記録
	previousTime_ = std::chrono::steady_clock::now();
}

void TimeManager::Update(void)
{
	// 現在時刻を取得し、前フレームからの経過時間を計算
	auto currentTime = std::chrono::steady_clock::now();

	std::chrono::duration<float> elapsedTime = currentTime - previousTime_;

	previousTime_ = currentTime;

	if (isPaused_)
	{
		deltaTime_ = 0.0f;
		return;
	}

	deltaTime_ = elapsedTime.count();

	// ゲーム内時間の更新
	gameTime_ += deltaTime_ * gameSpeed_;

	// 登録されている全タイマーの更新
	for (auto& pair : timers_)
	{
		Timer& timer = pair.second;
		if (timer.timeLeft > 0.0f)
		{
			timer.timeLeft -= deltaTime_;
		}
	}
}

float TimeManager::GetGameTime(void) const
{
	return gameTime_;
}

int TimeManager::GetGameHour(void) const
{
	return static_cast<int>(gameTime_) / SECONDS_PER_HOUR;
}

int TimeManager::GetGameMinute(void) const
{
	return (static_cast<int>(gameTime_) / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR;
}

int TimeManager::GetGameSecond(void) const
{
	return static_cast<int>(gameTime_) % SECONDS_PER_MINUTE;
}

void TimeManager::SetGameTime(float targetTime)
{
	gameTime_ = targetTime;
}

void TimeManager::StartTimer(const std::string& timerId, float duration)
{
	// 新規追加または上書きでタイマーを開始
	timers_[timerId] = { duration, duration };
}

bool TimeManager::IsTimerFinished(const std::string& timerId) const
{
	auto it = timers_.find(timerId);

	// タイマーが存在しない場合は終了しているとみなす
	if (it == timers_.end())
	{
		return true;
	}

	return it->second.timeLeft <= 0.0f;
}

void TimeManager::ResetTimer(const std::string& timerId)
{
	auto it = timers_.find(timerId);
	if (it != timers_.end())
	{
		// 開始時の持続時間で残り時間をリセット
		it->second.timeLeft = it->second.duration;
	}
}

float TimeManager::GetDeltaTime(void) const
{
#ifdef _DEBUG
	// デバッグ用の固定デルタタイム
	const float FIXED_DELTA_TIME = 1.0f / 60.0f;
	return FIXED_DELTA_TIME;
#else
	return deltaTime_;
#endif
}