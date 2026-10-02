#include "../Pch.h"
#include "SceneManager.h"
#include "SceneBase.h"
#include "../Application.h"
#include "../System/Input/KeyConfInputManager.h"
#include "../Common/Fader.h"
#include "../Common/Loading.h"
#include "../Scene/MainScene/SceneTitle.h"

// // TODO: 後ほど作成するファイル群
// #include "../../Camera/Camera.h"
// #include "../../Object/Collision/CollisionController.h"
// #include "../Decoration/SoundManager.h"
// #include "../System/TimeManager.h"
// #include "../System/NetManager.h"
// #include "../../Shader/ShaderController.h"
// #include "../../ImGUI/GuiController.h"

SceneManager* SceneManager::instance_ = nullptr;

void SceneManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new SceneManager();
    }
}

SceneManager& SceneManager::GetInstance(void)
{
    return *instance_;
}

void SceneManager::DestroyInstance(void)
{
    if (instance_)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

SceneManager::SceneManager(void)
    : isSceneChanging_(false)
    , isFirstFrame_(true)
    , sceneMutex_()
    , firstFadeImageHandle_(-1)
    , oldScene_(nullptr)
    , nextScene_(nullptr)
{
    // camera_ = std::make_unique<Camera>();
    fader_ = std::make_unique<Fader>();
    loading_ = std::make_unique<Loading>();
    scenes_ = std::list<std::shared_ptr<SceneBase>>();
}

SceneManager::~SceneManager(void)
{
}

void SceneManager::Initialize(void)
{
    SetMouseDispFlag(true);

    // 各種マネージャーの初期化（未作成のものはコメントアウト）
    // NetManager::CreateInstance();
    // SoundManager::CreateInstance();
    // SoundManager::GetInstance().Initialize();
    // TimeManager::CreateInstance();
    // ShaderController::CreateInstance();
    // ShaderController::GetInstance().Initialize();
    loading_->Initialize();
    // CollisionController::CreateInstance();
    // CollisionController::GetInstance().Initialize();
    // GuiController::CreateInstance();
}

void SceneManager::Init3D(void)
{
    constexpr COLOR_F BACKGROUND_COLOR = { 0.0f, 0.0f, 0.0f }; // 背景色
    const float AMBIENT_VALUE = 0.8f;                          // 環境光の強さ
    const float LIGHT_MID_VALUE = 0.5f;                        // 鏡面光・環境光のベース値
    const int FOG_COLOR = 5;                                   // フォグの色
    const float FOG_START_DISTANCE = 10000.0f;                 // フォグ開始距離
    const float FOG_END_DISTANCE = 20000.0f;                   // フォグ終了距離

    // 背景色を設定する
    SetBackgroundColor(static_cast<int>(BACKGROUND_COLOR.r)
        , static_cast<int>(BACKGROUND_COLOR.g)
        , static_cast<int>(BACKGROUND_COLOR.b));

    // Zバッファを有効にする
    SetUseZBuffer3D(true);
    SetWriteZBuffer3D(true);

    // バックカリングを有効にする
    SetUseBackCulling(true);

    // ライティングを有効にする
    SetUseLighting(true);
    SetLightEnable(true);

    SetGlobalAmbientLight(GetColorF(AMBIENT_VALUE, AMBIENT_VALUE, AMBIENT_VALUE, 1.0f));

    ChangeLightTypeDir(VGet(0.0f, -1.0f, 1.0f));
    SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
    SetLightSpcColor(GetColorF(LIGHT_MID_VALUE, LIGHT_MID_VALUE, LIGHT_MID_VALUE, 1.0f));
    SetLightAmbColor(GetColorF(LIGHT_MID_VALUE, LIGHT_MID_VALUE, LIGHT_MID_VALUE, 1.0f));

    // フォグを設定する
    SetFogEnable(true);
    SetFogColor(FOG_COLOR, FOG_COLOR, FOG_COLOR);
    SetFogStartEnd(FOG_START_DISTANCE, FOG_END_DISTANCE);
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> _scene)
{
    // CollisionController::GetInstance().Clear();
    // if (camera_)
    // {
    //     CollisionController::GetInstance().RegisterActor(camera_.get());
    // }
    // SoundManager::GetInstance().StopAllBGM();

    nextScene_ = _scene;
    isSceneChanging_ = true;

    fader_->SetFade(Fader::FADE_STATE::FADE_OUT);

    // 非同期ロード開始
    loading_->StartAsyncLoad([_scene]()
        {
            _scene->Load();
        });
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> _scene)
{
    scenes_.push_back(_scene);

    _scene->Load();
    _scene->EndLoad();
    _scene->Initialize();
}

void SceneManager::PopScene(void)
{
    if (scenes_.size() > 1)
    {
        scenes_.back()->Release();
        scenes_.pop_back();
    }
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> _scene)
{
    scenes_.clear();

    // CollisionController::GetInstance().Clear();
    // SoundManager::GetInstance().StopAllBGM();

    isSceneChanging_ = true;
    scenes_.push_back(_scene);

    loading_->StartAsyncLoad([_scene]()
        {
            _scene->Load();
        });
}

void SceneManager::Update(void)
{
    if (isFirstFrame_)
    {
        isFirstFrame_ = false;

        Init3D();
        // if (camera_)
        // {
        //     camera_->Init();
        // }
        fader_->LoadFadeImage();

        ChangeScene(std::make_shared<SceneTitle>());
    }

    // TimeManager::GetInstance().Update();
    // NetManager::GetInstance().Update();

    if (Application::GetInstance().GetGameEnd())
    {
        return;
    }

    if (isSceneChanging_)
    {
        fader_->Update();
        loading_->Update();

        constexpr float LOAD_COMPLETE_THRESHOLD = 100.0f;

        const bool isLoadFinished =
            (loading_->GetProgress() >= LOAD_COMPLETE_THRESHOLD && !loading_->IsLoading());
        const bool isFadeOutFinished =
            (fader_->GetState() == Fader::FADE_STATE::FADE_OUT && fader_->IsEnd());

        if (isLoadFinished && isFadeOutFinished && nextScene_ != nullptr)
        {
            nextScene_->EndLoad();
            nextScene_->Initialize();
            scenes_.push_back(nextScene_);

            for (auto& sceneElement : scenes_)
            {
                if (sceneElement != nextScene_)
                {
                    oldScene_ = sceneElement;
                }
            }

            scenes_.remove_if([this](const auto& targetElement)
                {
                    return targetElement != nextScene_;
                });

            nextScene_ = nullptr;
            fader_->SetFade(Fader::FADE_STATE::FADE_IN);
        }

        if (nextScene_ == nullptr && fader_->GetState() == Fader::FADE_STATE::FADE_IN && fader_->IsEnd())
        {
            fader_->SetFade(Fader::FADE_STATE::NONE);
            isSceneChanging_ = false;
        }
        return;
    }

    if (oldScene_)
    {
        oldScene_->Release();
        oldScene_ = nullptr;
    }

    if (scenes_.empty())
    {
        return;
    }

    auto& currentScene = scenes_.back();
    if (currentScene)
    {
        currentScene->Update();
    }

    // CollisionController::GetInstance().Update();
    // if (camera_)
    // {
    //     camera_->ResolveCollision();
    // }
}

void SceneManager::Draw(void)
{
    // ClearDrawScreen();

    if (!scenes_.empty())
    {
        // if (camera_)
        // {
        //     camera_->SetBeforeDraw();
        // }

        for (auto& sceneElement : scenes_)
        {
            if (sceneElement)
            {
                sceneElement->Draw();
            }
        }
    }

    // フェードを先に描画する
    fader_->Draw();

    // フェードアウトが終わってからロード画面を上に重ねる
    if (isSceneChanging_ && fader_->GetState() == Fader::FADE_STATE::FADE_OUT && fader_->IsEnd())
    {
        loading_->Draw();
    }
}

void SceneManager::Release(void)
{
    if (loading_->IsLoading())
    {
        loading_->EndAsyncLoad();
    }

    for (auto& sceneElement : scenes_)
    {
        sceneElement->Release();
    }
    scenes_.clear();

    // camera_.reset();

    // SoundManager::GetInstance().DestroyInstance();
    // ShaderController::GetInstance().DestroyInstance();
    // TimeManager::GetInstance().DestroyInstance();

    // CollisionController::GetInstance().DestroyInstance();
    // GuiController::DestroyInstance();
    // NetManager::GetInstance().DestroyInstance();
}