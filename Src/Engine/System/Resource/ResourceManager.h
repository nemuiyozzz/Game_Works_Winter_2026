#pragma once
#include <map>
#include <mutex>
#include "Resource.h"

/// @brief 画像、モデル、音源などの全リソースを一括管理するクラス
class ResourceManager
{
public:
    /// @brief 管理するリソースの識別子定義
    enum class RESOURCE_ID
    {
        NONE = -1,

        IMAGE_SAMPLE,         // 単一画像サンプル
        IMAGES_SAMPLE,        // 分割画像サンプル
        MODEL_SAMPLE,         // 3Dモデルサンプル
        ANIMATION_SAMPLE,     // アニメーションサンプル
        EFFECT_SAMPLE,        // エフェクトサンプル
        SOUND_SAMPLE,         // サウンドサンプル
        SHADER_VERTEX_SAMPLE, // 頂点シェーダサンプル
        SHADER_PIXEL_SAMPLE,  // ピクセルシェーダサンプル
        MOVIE_SAMPLE,         // 映像サンプル
    };

    /// @brief 明示的にインスタンスを生成する
    static void CreateInstance(void);

    /// @brief 静的インスタンスの取得
    /// @return ResourceManagerの参照
    static ResourceManager& GetInstance(void);

    /// @brief システム全体の初期化（リソースのパス登録）
    void Initialize(void);

    /// @brief シーン切り替え時などのリソース解放
    void Release(void);

    /// @brief プログラム終了時のリソース完全破棄
    static void DestroyInstance(void);

    /// @brief リソースの読み込み（未ロードなら実行）
    /// @param targetId リソース識別子
    /// @return Resourceオブジェクト
    Resource Load(RESOURCE_ID targetId);

    /// @brief モデルリソースを複製して読み込む
    /// @param targetId モデルのリソース識別子
    /// @return 複製されたモデルのハンドルID
    int LoadModelDuplicate(RESOURCE_ID targetId);

    /// @brief ロード済みリソースのハンドルを取得する
    /// @param targetId リソース識別子
    /// @return ハンドルID
    int GetHandleId(RESOURCE_ID targetId);

private:

    // 静的インスタンス
    static ResourceManager* instance_;

    // リソース管理関連
    std::map<RESOURCE_ID, Resource> registeredResourcesMap_; /// 登録対象
    std::map<RESOURCE_ID, Resource*> loadedResourcesMap_;    /// 読み込み済み対象

    // スレッドセーフ用ミューテックス
    std::mutex resourceMutex_;

    /// @brief コンストラクタ
    ResourceManager(void);

    /// @brief デストラクタ
    ~ResourceManager(void) = default;

    /// @brief コピー・ムーブの禁止
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    /// @brief 内部読み込み処理の実体
    /// @param targetId リソース識別子
    /// @return 内部管理用ポインタ
    Resource* ExecuteLoad(RESOURCE_ID targetId);
};