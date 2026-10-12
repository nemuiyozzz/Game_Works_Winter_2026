#include "../../Pch.h"
#include "FpsController.h"

FpsController::FpsController(int fixed_fps)
    : target_fps(fixed_fps > MAXIMUM_FPS ? MAXIMUM_FPS : fixed_fps)
    , ideal_frame_time(1.0f / static_cast<double>(target_fps))
    , current_fps(0.0f)
    , frame_time_list()
    , previous_time()
{
    previous_time = std::chrono::high_resolution_clock::now();

    // DxLibの垂直同期待ちを無効化
    SetWaitVSyncFlag(false);
}

FpsController::~FpsController(void)
{
}

void FpsController::Wait(void) 
{
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_duration = current_time - previous_time;
    double elapsed_time_seconds = elapsed_duration.count();

    // 経過時間が理想時間よりも短ければ待機
    if (elapsed_time_seconds < ideal_frame_time) 
    {
        double wait_milli_seconds = (ideal_frame_time - elapsed_time_seconds) * 1000.0;

        // Sleepで待ち時間分を待機
        if (wait_milli_seconds >= 1.0)
        {
            WaitTimer(static_cast<int>(wait_milli_seconds));
        }

        // 指定時間になるまでbusyになるが待つ
        while (elapsed_time_seconds < ideal_frame_time)
        {
            current_time = std::chrono::high_resolution_clock::now();
            elapsed_duration = current_time - previous_time;
            elapsed_time_seconds = elapsed_duration.count();
        }
    }

    previous_time = current_time;

    // FPS計測(指定された最新フレーム数分の平均)
    frame_time_list.emplace_back(elapsed_time_seconds);
    if (frame_time_list.size() > AVERAGE_FPS_COUNT)
    {
        frame_time_list.erase(frame_time_list.begin());
    }

    double total_time = 0.0;
    for (double recorded_time : frame_time_list) 
    {
        total_time += recorded_time;
    }

    // 平均FPSの算出
    current_fps = static_cast<float>(frame_time_list.size() / total_time);
}

void FpsController::Draw(void) 
{
    DrawFormatString(POSITION_X, POSITION_Y, TEXT_COLOR, L"FPS : %.2f", current_fps);
}