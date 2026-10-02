#pragma once
#include <list>
#include <memory>
#include <mutex>
#include <chrono>

class SceneBase;
// class Camera;
class Fader;
class Loading;    

/// @brief シーンのスタック管理、遷移を管理するクラス
class SceneManager
{
public:

    /// @brief インスタンスを明示的に生成する
    static void CreateInstance(void);

    /// @brief インスタンスを取得する
    /// @return SceneManagerの参照
    static SceneManager& GetInstance(void);

    /// @brief インスタンスを破棄する
    static void DestroyInstance(void);

    /// @brief システムおよび3D描画設定を初期化する
    void Initialize(void);

    /// @brief 現在のシーンおよびデルタタイムの更新
    void Update(void);

    /// @brief シーンの描画処理を呼び出す
    void Draw(void);

    /// @brief 全シーンの解放処理
    void Release(void);

    /// @brief シーンを変更する
    /// @param _scene 次に実行するシーンのポインタ
    void ChangeScene(std::shared_ptr<SceneBase> _scene);

    /// @brief シーンを積む
    /// @param _scene 重ねるシーンのポインタ
    void PushScene(std::shared_ptr<SceneBase> _scene);

    /// @brief シーンを外す
    void PopScene(void);

    /// @brief シーンをジャンプする
    /// @param _scene 遷移先シーンのポインタ
    void JumpScene(std::shared_ptr<SceneBase> _scene);

    // /// @brief 現在管理しているカメラを取得する
    // /// @return カメラオブジェクトの共有ポインタ
    // const std::unique_ptr<Camera>& GetCamera(void) const;

    /// @brief ロード中かどうかを判定する
    /// @return ロード中であれば true
    bool IsSceneChanging(void) const { return isSceneChanging_; }

private:

    // シングルトン用インスタンス
    static SceneManager* instance_;

    // シーン管理関連
    std::list<std::shared_ptr<SceneBase>> scenes_; // シーンを保持する
    std::mutex sceneMutex_;                        // シーンアクセスを保護するミューテックス
    std::shared_ptr<SceneBase> nextScene_;         // 次に遷移するシーン
    std::shared_ptr<SceneBase> oldScene_;          // 解放対象の古いシーン
    bool isSceneChanging_;                         // シーン切り替え中フラグ
    bool isFirstFrame_;                            // 最初のフレームかどうかのフラグ

    // カメラオブジェクト
    // std::unique_ptr<Camera> camera_;               

    // フェード関連
    std::unique_ptr<Fader> fader_;    // フェード管理オブジェクト
    int firstFadeImageHandle_;        // 初回フェード用の画像ハンドル

    // ローディング管理オブジェクト
    std::unique_ptr<Loading> loading_; 

    /// @brief 3D描画に関する初期設定
    void Init3D(void);

    /// @brief コンストラクタ
    SceneManager(void);

    /// @brief デストラクタ
    ~SceneManager(void);

    /// @brief コピーコンストラクタを禁止する
    SceneManager(const SceneManager&) = delete;

    /// @brief 代入演算子を禁止する
    SceneManager& operator=(const SceneManager&) = delete;

    /// @brief ムーブコンストラクタを禁止する
    SceneManager(SceneManager&&) = delete;

    /// @brief ムーブ代入演算子を禁止する
    SceneManager& operator=(SceneManager&&) = delete;
};