#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>
#include <cstdint>

#include "Resource.h"

/// @brief 画像、モデル、音源などの全リソースを一括管理するクラス
class ResourceManager
{
public:

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

    /// @brief リソースの読み込み
    /// @param targetKey リソース識別
    /// @return Resourceオブジェクト
    Resource Load(const std::wstring& targetKey);

    /// @brief モデルリソースを複製して読み込む
    /// @param targetKey モデルのリソース識別
    /// @return 複製されたモデルのハンドルID
    int LoadModelDuplicate(const std::wstring& targetKey);

    /// @brief ロード済みリソースのハンドルを取得する
    /// @param targetId リソース識別子
    /// @return ハンドルID
    int GetHandleId(const std::wstring& targetKey);

    /// @brief 現在登録されているリソース一覧をバイナリファイルへ保存する
    /// @param binaryFilePath 保存先バイナリファイルパス
    /// @return 成功すればtrue
    bool SaveBinary(const std::wstring& binaryFilePath) const;

private:

    /// @brief バイナリファイルのヘッダ構造体
    struct ResourceBinaryHeader
    {
        char signature[4] = { 'R', 'E', 'S', 'B' }; // 識別シグネチャ
        uint32_t version = 1;                       // バージョン番号
        uint32_t recordCount = 0;                   // 登録リソース数
    };

    /// @brief 登録用リソース要素構造体
    struct RegisteredResourceEntry
    {
        std::wstring key = L"";    // リソース識別キー
        Resource resource{};       // リソースデータ
    };

    /// @brief 読み込み済みリソース要素構造体
    struct LoadedResourceEntry
    {
        std::wstring key = L"";              // リソース識別キー
        std::unique_ptr<Resource> resource;  // 読み込み済みリソースの実体
    };

    // 静的インスタンス
    static ResourceManager* instance_;

    // リソース管理関連
    std::vector<RegisteredResourceEntry> registeredResources_;              // 登録対象リスト
    std::vector<LoadedResourceEntry> loadedResources_;                      // 読込済対象リスト
    std::unordered_map<std::wstring, Resource::RESOURCE_TYPE> typeTable_;   // 型変換表

    // スレッドセーフ用ミューテックス
    std::mutex resourceMutex_;

    /// @brief コンストラクタ
    ResourceManager(void) {};

    /// @brief デストラクタ
    ~ResourceManager(void) = default;

    /// @brief コピー・ムーブの禁止
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    /// @brief 文字列からRESOURCE_TYPEへ変換するテーブルを初期化する
    void InitTypeTable(void);

    /// @brief CSVまたはバイナリからリソース定義を読み込む
    /// @param csvFilePath CSVファイルパス
    /// @param binaryFilePath バイナリファイルパス
    void LoadResourceList(
        const std::wstring& csvFilePath, const std::wstring& binaryFilePath);

    /// @brief CSVファイルからリソース定義を読み込む
    /// @param csvFilePath CSVファイルパス
    /// @return 成功すればtrue
    bool LoadCsv(const std::wstring& csvFilePath);

    /// @brief バイナリファイルからリソース定義を読み込む
    /// @param binaryFilePath バイナリファイルパス
    /// @return 成功すればtrue
    bool LoadBinary(const std::wstring& binaryFilePath);

    /// @brief CSVが存在しない場合に初期サンプルCSVを生成する
    /// @param csvFilePath 生成するCSVファイルパス
    void CreateDefaultCsv(const std::wstring& csvFilePath) const;

    /// @brief リソースの種類に応じたベースフォルダパスを取得する
    /// @param targetType リソースの種類
    /// @return ベースフォルダパス
    std::wstring GetBaseFolderPath(Resource::RESOURCE_TYPE targetType) const;

    /// @brief リソースをリストへ登録する
    /// @param keyName リソース識別
    /// @param newResource 登録するResourceデータ
    void RegisterResource(const std::wstring& keyName, const Resource& newResource);

    /// @brief 登録済みリストから指定キーのリソースを探す
    /// @param targetKey リソース識別
    /// @return 見つかればポインタ、無ければnullptr
    Resource* FindRegisteredResource(const std::wstring& targetKey);

    /// @brief 読み込み済みリストから指定キーのリソースを探す
    /// @param targetKey リソース識別
    /// @return 見つかればポインタ、無ければnullptr
    Resource* FindLoadedResource(const std::wstring& targetKey);

    /// @brief 内部読み込み処理の実体
    /// @param targetKey リソース識別
    /// @return 内部管理用ポインタ
    Resource* ExecuteLoad(const std::wstring& targetKey);
};