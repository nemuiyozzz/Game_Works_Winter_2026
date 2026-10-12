#pragma once

#include <string>

/// @brief フェード処理クラス
class Fader
{
public:

    /// @brief フェード処理の状態を表す列挙型
    enum class FADE_STATE
    {
        NONE,
        FADE_OUT, /// 徐々に暗転
        FADE_IN   /// 徐々に明転
    };

    // 構造体や列挙型の後に定数を定義する
    static constexpr float FADE_SPEED_ALPHA = 4.0f;            /// フェードが進む速さ
    static constexpr float MAXIMUM_FADE_ALPHA = 255.0f;        /// 透明度の最大値
    static constexpr float MINIMUM_FADE_ALPHA = 0.0f;          /// 透明度の最小値
    static constexpr unsigned int FADE_BLACK_COLOR = 0x000000; /// フェードの黒色


    /// @brief コンストラクタ
    Fader(void);

    /// @brief デストラクタ
    ~Fader(void);

    /// @brief 状態の取得
    /// @return 現在のフェード状態
    FADE_STATE GetState(void) const;

    /// @brief フェード処理が終了しているか
    /// @return 終了している場合はtrue
    bool IsEnd(void) const;

    /// @brief 指定フェードを開始する
    /// @param target_state 開始するフェードの状態
    void SetFade(FADE_STATE target_state);

    /// @brief 初期化
    void Init(void);

    /// @brief 更新
    void Update(void);

    /// @brief 描画
    void Draw(void);

    /// @brief 初回用のフェード画像を読み込む
    void LoadFadeImage(void);

private:
    // 状態関連
    FADE_STATE fadeState_; /// 現在の状態
    float alphaValue_;     /// 透明度
    bool isPreEnd_;       /// 1フレーム判定用
    bool isEnd_;           /// フェード処理の終了判定

    /// フェードに使用する画像ハンドル
    int fadeImageHandle_;
};