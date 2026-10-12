#include "../Pch.h"
#include "Fader.h"
#include "../Application.h"
// #include "../Manager/Generic/ResourceManager.h"

Fader::Fader(void)
    : fadeState_(FADE_STATE::NONE),
    alphaValue_(0.0f),
    isPreEnd_(true),
    isEnd_(true),
    fadeImageHandle_(-1)
{
}

Fader::~Fader(void)
{
    if (fadeImageHandle_ != -1)
    {
        DeleteGraph(fadeImageHandle_);
    }
}

Fader::FADE_STATE Fader::GetState(void) const
{
    return fadeState_;
}

bool Fader::IsEnd(void) const
{
    return isEnd_;
}

void Fader::SetFade(FADE_STATE target_state)
{
    fadeState_ = target_state;
    if (fadeState_ != FADE_STATE::NONE)
    {
        isPreEnd_ = false;
        isEnd_ = false;
    }
}

void Fader::Init(void)
{
}

void Fader::LoadFadeImage(void)
{
    // fadeImageHandle_ = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::IMG_ICON);
}

void Fader::Update(void)
{
    if (isEnd_)
    {
        // フェードが完全に終了した状態で画像が残っていれば自動で破棄する
        if (fadeImageHandle_ != -1)
        {
            fadeImageHandle_ = -1;
        }
        return;
    }

    switch (fadeState_)
    {
    case FADE_STATE::NONE:
    {
        return;
    }
    case FADE_STATE::FADE_OUT:
    {
        alphaValue_ += FADE_SPEED_ALPHA;
        if (alphaValue_ > MAXIMUM_FADE_ALPHA)
        {
            // フェード終了
            alphaValue_ = MAXIMUM_FADE_ALPHA;
            if (isPreEnd_)
            {
                // 1フレーム後に終了とする
                isEnd_ = true;
            }
            isPreEnd_ = true;
        }
        break;
    }
    case FADE_STATE::FADE_IN:
    {
        alphaValue_ -= FADE_SPEED_ALPHA;
        if (alphaValue_ < MINIMUM_FADE_ALPHA)
        {
            // フェード終了
            alphaValue_ = MINIMUM_FADE_ALPHA;
            if (isPreEnd_)
            {
                // 1フレーム後に終了とする
                isEnd_ = true;
            }
            isPreEnd_ = true;
        }
        break;
    }
    default:
    {
        return;
    }
    }
}

void Fader::Draw(void)
{
    switch (fadeState_)
    {
    case FADE_STATE::NONE:
    {
        return;
    }
    case FADE_STATE::FADE_OUT:
    case FADE_STATE::FADE_IN:
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alphaValue_));

        if (fadeImageHandle_ != -1)
        {
            // 画像がある場合は背景を黒フェードで塗り、その上に画像を描画
            DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,
                FADE_BLACK_COLOR, true);

            DrawRotaGraph(Application::SCREEN_HALF_X, Application::SCREEN_HALF_Y,
                1.0f, 0.0f, fadeImageHandle_, true);
        }
        else
        {
            // 画像がない場合は通常の黒フェード
            DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,
                FADE_BLACK_COLOR, true);
        }

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        break;
    }
    }
}