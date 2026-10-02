#pragma once
#include <vector>
#include <chrono>
#include <DxLib.h>

/// @brief FPSを制御・計測するクラス
class FpsController 
{
public:
    /// @brief コンストラクタ (DxLib_Initの前に呼ぶこと)
    /// @param fixed_fps 固定するFPSの値
    FpsController(int fixed_fps);

    /// @brief デストラクタ
    ~FpsController(void);

    /// @brief 1フレームごとのFPS制御と待機処理 (ScreenFlipの後に呼ぶこと)
    void Wait(void);

    /// @brief 画面に現在のFPSを表示する
    void Draw(void);

    /// @brief 現在のFPSを取得する
    /// @return 現在のFPS値
    float GetFps(void) const { return current_fps; }

private:
    // FPS設定関連
    static constexpr int MAXIMUM_FPS = 1200;       // 最大FPS値
    static constexpr int AVERAGE_FPS_COUNT = 60;   // 平均FPS計算に使用するフレーム数

    // 描画関連
    const unsigned int TEXT_COLOR = GetColor(255, 255, 255); // 平均FPSの描画色
    const int POSITION_X = 20;                               // 平均FPSの描画X位置
    const int POSITION_Y = 20;                               // 平均FPSの描画Y位置

    // 時間管理関連
    const int target_fps;          // 指定された固定フレームレート
    const double ideal_frame_time; // 1フレームの理想時間(秒)
    float current_fps;             // 計測された現在のFPS

    std::vector<double> frame_time_list;                          // 平均FPS計測用の時間リスト(秒単位)
    std::chrono::high_resolution_clock::time_point previous_time; // 前フレームの計測時間
};