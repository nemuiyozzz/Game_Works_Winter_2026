#pragma once

#include <string>
#include <memory>

class FpsController;

/// @brief アプリケーション管理クラス
class Application
{
public:
	// スクリーンサイズ関連
	static constexpr int SCREEN_ASPECT = 100;					// スクリーンアスペクト非乗数
	static constexpr int SCREEN_SIZE_X = 16 * SCREEN_ASPECT;	// スクリーンサイズの横幅
	static constexpr int SCREEN_SIZE_Y = 9 * SCREEN_ASPECT;		// スクリーンサイズの縦
	static constexpr int SCREEN_HALF_X = SCREEN_SIZE_X / 2;		// スクリーンサイズの横幅の半分
	static constexpr int SCREEN_HALF_Y = SCREEN_SIZE_Y / 2;		// スクリーンサイズの縦幅の半分

	// フレームレート
	static constexpr int FRAME_RATE = 60;

	// データパス関連
	static const std::wstring PATH_DATA;			// データフォルダパス
	static const std::wstring PATH_IMAGE;		// 画像フォルダパス
	static const std::wstring PATH_MODEL;		// モデルフォルダパス
	static const std::wstring PATH_ANIM;			// アニメーションフォルダパス
	static const std::wstring PATH_EFFECT;		// エフェクトフォルダパス
	static const std::wstring PATH_SOUND;		// サウンドフォルダパス
	static const std::wstring PATH_CSV;			// CSVフォルダパス
	static const std::wstring PATH_SHADER;		// シェーダフォルダパス
	static const std::wstring PATH_KEY_CONFIG;	// キーコンフィグフォルダパス
	static const std::wstring PATH_SENSITIVITY;	// 感度フォルダパス

	/// @brief インスタンスを明示的に生成
	static void CreateInstance(void);

	/// @brief インスタンスの取得 
	/// @return アプリケーションのインスタンス
	static Application& GetInstance(void);
	
	/// @brief リソースの破棄 
	void DestroyInstance(void);

	/// @brief 初期化 
	void Initialize(void);

	/// @brief ゲームループ 
	void Run(void);

	/// @brief 初期化成功 / 初期化失敗 
	/// @return 失敗していればtrue
	bool GetInitFail(void) const { return isInitFail_; }

	/// @brief 解放成功 / 解放失敗 
	/// @return 失敗してればtrue
	bool GetReleaseFail(void) const { return isReleaseFail_; }

	/// @brief ゲーム終了 
	void GameEnd(void);

	/// @brief ゲーム終了フラグの取得 
	/// @return ゲーム終了フラグ
	bool GetGameEnd(void) const { return isGameEnd_; }

private:

	// 静的インスタンス
	static Application* instance_;

	// fps制御クラス
	std::unique_ptr<FpsController> fpsController_;

	// 状態フラグ関連
	bool isInitFail_;		// 初期化失敗フラグ
	bool isReleaseFail_;	// 解放失敗フラグ
	bool isGameEnd_;		// ゲーム終了フラグ

	/// @brief デフォルトコンストラクタ 
	Application(void);

	/// @brief デストラクタ 
	~Application(void) = default;

	/// @brief エフェクシアの初期化 
	bool InitEffekseer(void);
 
	// コピーコンストラクタの削除
	Application(const Application&) = delete; 

	// コピー代入演算子の削除
	Application& operator=(const Application&) = delete; 

	// ムーブコンストラクタの削除
	Application(Application&&) = delete; 

	// ムーブ代入演算子の削除
	Application& operator=(Application&&) = delete; 
};