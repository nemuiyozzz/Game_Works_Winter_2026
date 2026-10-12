#include "../../../Pch.h"
#include "FpsController.h"

FpsController::FpsController(int fixed_fps)
    : targetFps_(fixed_fps > MAXIMUM_FPS ? MAXIMUM_FPS : fixed_fps)
    , idealFrameTime_(1.0f / static_cast<double>(targetFps_))
    , currentFps_(0.0f)
    , frameTimeList_()
    , previousTime_()
{
    previousTime_ = std::chrono::high_resolution_clock::now();

    // DxLibの垂直同期待ちを無効化
    SetWaitVSyncFlag(false);
}

FpsController::~FpsController(void)
{
}

void FpsController::Wait(void) 
{
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_duration = current_time - previousTime_;
    double elapsed_time_seconds = elapsed_duration.count();

    // 経過時間が理想時間よりも短ければ待機
    if (elapsed_time_seconds < idealFrameTime_) 
    {
        double wait_milli_seconds = (idealFrameTime_ - elapsed_time_seconds) * 1000.0;

        // Sleepで待ち時間分を待機
        if (wait_milli_seconds >= 1.0)
        {
            WaitTimer(static_cast<int>(wait_milli_seconds));
        }

        // 指定時間になるまでbusyになるが待つ
        while (elapsed_time_seconds < idealFrameTime_)
        {
            current_time = std::chrono::high_resolution_clock::now();
            elapsed_duration = current_time - previousTime_;
            elapsed_time_seconds = elapsed_duration.count();
        }
    }

    previousTime_ = current_time;

    // FPS計測(指定された最新フレーム数分の平均)
    frameTimeList_.emplace_back(elapsed_time_seconds);
    if (frameTimeList_.size() > AVERAGE_FPS_COUNT)
    {
        frameTimeList_.erase(frameTimeList_.begin());
    }

    double total_time = 0.0;
    for (double recorded_time : frameTimeList_) 
    {
        total_time += recorded_time;
    }

    // 平均FPSの算出
    currentFps_ = static_cast<float>(frameTimeList_.size() / total_time);
}

void FpsController::Draw(void) 
{
    DrawFormatString(POSITION_X, POSITION_Y, TEXT_COLOR, L"FPS : %.2f", currentFps_);
}