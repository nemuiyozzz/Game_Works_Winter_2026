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
	if (isLoading_)
	{
		return;
	}

	Initialize();
	isLoading_ = true;

	SetUseASyncLoadFlag(true);

	if (loadFunction)
	{
		loadFunction();
	}
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
		// 進行度が100%になり、かつDXライブラリの非同期ロードが完全に0件になったら終了
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

	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, BLACK_COLOR, true);

	DrawString(Application::SCREEN_HALF_X - 40, Application::SCREEN_HALF_Y, L"ロード中...", WHITE_COLOR);
}

void Loading::EndAsyncLoad(void)
{
	isLoading_ = false;
	progress_ = MAXIMUM_PROGRESS;

	SetUseASyncLoadFlag(false);
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