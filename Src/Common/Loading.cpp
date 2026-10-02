#include "../Pch.h"
#include "Loading.h"

#include "../Application.h"

Loading::Loading(void)
	: isLoading_(false)
	, progress_(MINIMUM_PROGRESS)
{
}

Loading::~Loading(void)
{
}

void Loading::Initialize(void)
{
	isLoading_ = false;
	progress_ = MINIMUM_PROGRESS;
}

void Loading::StartAsyncLoad(std::function<void()> loadFunction)
{
	SetUseASyncLoadFlag(true);
	if (isLoading_)
	{
		return;
	}

	Initialize();
	isLoading_ = true;

	if (loadFunction)
	{
		loadFunction();
	}
	SetUseASyncLoadFlag(false);
}

void Loading::Update(void)
{
	if (!isLoading_)
	{
		return;
	}

	const int LOAD_COUNT = GetASyncLoadNum();  // 残りの非同期ロード数
	const float PROGRESS_SPEED = 0.5f;         // 1フレームごとの進捗増加量
	const float WAIT_PROGRESS = 99.9f;         // ロード完了待ちの進捗率

	if (progress_ < MAXIMUM_PROGRESS)
	{
		progress_ += PROGRESS_SPEED;
	}

	if (progress_ >= MAXIMUM_PROGRESS)
	{
		if (LOAD_COUNT == 0)
		{
			EndAsyncLoad();
		}
		else
		{
			progress_ = WAIT_PROGRESS;
		}
	}
}

void Loading::Draw(void)
{
	const unsigned int BLACK_COLOR = GetColor(0, 0, 0);
	const unsigned int WHITE_COLOR = GetColor(255, 255, 255);

	// 背景を黒で塗りつぶす
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, BLACK_COLOR, true);

	// 画面中央付近に「ロード中...」と描画
	DrawString(Application::SCREEN_HALF_X - 40, Application::SCREEN_HALF_Y, L"ロード中...", WHITE_COLOR);
}

void Loading::EndAsyncLoad(void)
{
	isLoading_ = false;
	progress_ = MAXIMUM_PROGRESS;
}

bool Loading::IsLoading(void) const
{
	return isLoading_;
}

int Loading::GetProgress(void) const
{
	return static_cast<int>(progress_);
}

void Loading::SetProgress(float progress)
{
	if (progress < MINIMUM_PROGRESS)
	{
		progress = MINIMUM_PROGRESS;
	}
	if (progress > MAXIMUM_PROGRESS)
	{
		progress = MAXIMUM_PROGRESS;
	}
	progress_ = progress;
}
