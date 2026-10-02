#pragma once
#include <string>
#include <vector>

/// @brief 画像、モデル、サウンドなどのリソース管理を行うクラス
class Resource
{
public:

    /// @brief リソースの種類を定義する列挙型
    enum class RESOURCE_TYPE
    {
        NONE,          // 未設定
        IMAGE,         // 単一画像
        IMAGES,        // 分割画像
        MASK,          // マスク画像
        MODEL,         // 3Dモデル
        ANIMATION,     // アニメーション
        EFFEKSEER,     // エフェクト
        SOUND,         // 音源
        VERTEX_SHADER, // 頂点シェーダ (追加)
        PIXEL_SHADER,  // ピクセルシェーダ (追加)
    };

    /// @brief デフォルトコンストラクタ
    Resource(void);

    /// @brief 通常リソース用コンストラクタ
    /// @param targetType リソースの種類
    /// @param filePath ファイルパス
    Resource(RESOURCE_TYPE targetType, const std::wstring& filePath);

    /// @brief 分割画像リソース用コンストラクタ
    /// @param targetType リソースの種類
    /// @param filePath ファイルパス
    /// @param divisionX 横方向の分割数
    /// @param divisionY 縦方向の分割数
    /// @param sizeX 1枚あたりの横幅
    /// @param sizeY 1枚あたりの縦幅
    Resource(RESOURCE_TYPE targetType, const std::wstring& filePath,
        int divisionX, int divisionY, int sizeX, int sizeY);

    /// @brief デストラクタ
    ~Resource(void);

    /// @brief リソースの読み込み処理を実行
    void Load(void);

    /// @brief リソースの解放処理を実行
    void Release(void);

    /// @brief 複数画像ハンドルを外部配列にコピーする
    /// @param outputImages コピー先のポインタ
    void CopyHandle(int* outputImages);

    /// @brief リソースのハンドルIDを取得する
    /// @return ハンドルID
    int GetHandleId(void) const;

    // リソース情報関連
    RESOURCE_TYPE resourceType_; /// リソースタイプ
    std::wstring filePath_;       /// リソースのファイルパス

    // 画像およびモデル関連
    int handleId_;               /// 画像やモデルのハンドルID
    int* handleIdArray_;         /// 分割画像用ハンドル配列
    int divisionCountX_;         /// 分割画像の横方向の分割数
    int divisionCountY_;         /// 分割画像の縦方向の分割数
    int imageSizeX_;             /// 各分割画像の横幅（ピクセル単位）
    int imageSizeY_;             /// 各分割画像の縦幅（ピクセル単位）

    // 複製されたモデルのハンドルリスト
    std::vector<int> duplicatedModelIdList_;
};