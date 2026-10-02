#pragma once
#include <vector>
#include <unordered_map>
#include <DxLib.h>
#include <string>
#include <array>

#include "../../Common/Vector2.h"

/// @brief 入力管理クラス
class KeyConfInputManager
{
public:

	/// @brief マウスの感度設定
	struct MouseSensitivity
	{
		float x = 1.0f;         // X軸感度
		float y = 1.0f;         // Y軸感度
		bool invertX = false;   // X軸反転
		bool invertY = false;   // Y軸反転
	};

	/// @brief 右スティックの感度設定
	struct RStickSensitivity
	{
		float x = 1.0f;          // X軸感度
		float y = 1.0f;          // Y軸感度
		bool invertX = false;    // X軸反転
		bool invertY = false;    // Y軸反転
		float deadZone = 0.35f;  // デットゾーンのしきい値
	};

	/// @brief インスタンの生成
	static void CreateInstance(void);

	/// @brief インスタンスを取得 
	/// @return インスタンスの参照
	static KeyConfInputManager& GetInstance(void);

	/// @brief インスタンスの破棄
	void DestroyInstance(void);

	/// @brief 更新処理
	void Update(void);

	/// @brief 押し続けている間 true を返す
	/// @param _name イベント名
	/// @return 押されていれば true
	bool isPressed(const std::wstring& _name) const;

	/// @brief 離した瞬間のみ true を返す
	/// @param _name イベント名
	/// @return 離した瞬間のみ true
	bool isTrigerUp(const std::wstring& _name) const;

	/// @brief 押した瞬間のみ true を返す
	/// @param _name イベント名
	/// @return 押した瞬間のみ true
	bool isTrigerDown(const std::wstring& _name) const;

	/// @brief マウスカーソルの座標を取得 
	/// @return スクリーン座標のマウスカーソル位置
	Vector2 GetMousePosition(void) const;

	/// @brief マウスの移動量を取得し、カーソルを画面中央に固定・非表示にする
	/// @return 前フレームからの移動量
	Vector2F GetMouseVelocityAndFixCenter(void);

	/// @brief マウスの感度設定を変更する
	/// @param _sensitivity 設定する感度データ
	void SetMouseSensitivity(const MouseSensitivity& _sensitivity);

	/// @brief 現在のマウス感度設定を取得する
	/// @return 現在の感度設定
	const MouseSensitivity& GetMouseSensitivity(void) const;

	/// @brief 左スティックの入力方向を正規化したXZベクトルで返す
	/// @return 正規化済みの方向ベクトル
	VECTOR GetLeftStickDirection(void) const;

	/// @brief 右スティックの入力量を返す
	/// @return 右スティックの入力量
	Vector2F GetRIghtStick(void) const;

	/// @brief 左スティックの生の入力値を返す
	/// @return 左スティックの生の入力値
	Vector2F GetLeftStickRaw(void) const;

	/// @brief 右スティックの生の入力値を返す
	/// @return 右スティックの生の入力値
	Vector2F GetRightStickRaw(void) const;

	/// @brief 右スティックの感度設定を変更する
	/// @param _sensitivity 設定する感度データ
	void SetRStickSensitivity(const RStickSensitivity& _sensitivity);

	/// @brief 現在の右スティックの感度設定を取得する
	/// @return 現在の感度設定
	const RStickSensitivity& GetRStickSensitivity(void) const;

	/// @brief 現在の入力テーブルをファイルに保存する
	void SaveInputTable(void);

	/// @brief ファイルから入力テーブルを読み込む
	void LoadInputTable(void);

	/// @brief 感度設定をファイルに保存する
	void SaveSensitivitySettings(void) const;

	/// @brief 感度設定をファイルから読み込む
	void LoadSensitivitySettings(void);

	/// @brief 使用するパッドの番号を設定する
	/// @param _padNo DxLib定数
	void SetUsePadNo(int _padNo);

	/// @brief マウスホイールの回転量を取得する
	/// @return 前フレームからの回転量 
	int GetMouseWheel(void) const;

private:

	/// @brief 入力デバイス名
	enum class INPUT_TYPE
	{
		KEY_BOARD,      // キーボード
		MOUSE,          // マウス
		JOYPAD,         // ジョイパット
		XINPUT_ANALOG,  // XInputのアナログ入力
	};

	/// @brief 実入力情報
	struct InputInfo
	{
		INPUT_TYPE type = INPUT_TYPE::KEY_BOARD; // 入力デバイスの種類
		unsigned int id = -1;                    // キーコードやボタンIDなどの識別子
	};

	/// @brief 保存データのヘッダ構造
	struct KeyConfigHeader
	{
		char     signature[4] = { "" };  // ファイル識別ようのシグネチャ
		float    version = 1.0f;         // バージョン
		uint32_t dataNum = 0;            // 入力イベントの数
	};

	/// @brief スティックの入力値
	struct StickInfo
	{
		int leftStickX = 0;     // 左スティック 左右
		int leftStickY = 0;     // 左スティック 上下
		int rightStickX = 0;    // 右スティック 左右
		int rightStickY = 0;    // 右スティック 上下
	};

	/// @brief 感度設定ファイルのヘッダ構造
	struct SensitivityHeader
	{
		char  signature[4] = { "" };   // ファイル識別用のシグネチャ
		float version = 1.0f;          // バージョン
	};

	/// @brief XInputのアナログ入力をボタンとして扱うためのID
	enum class XINPUT_ANALOG_ID
	{
		LEFT_TRIGGER,      // 左トリガー
		RIGHT_TRIGGER,     // 右トリガー
		LEFT_STICK_UP,     // 左ステック上
		LEFT_STICK_DOWN,   // 左ステック下
		LEFT_STICK_LEFT,   // 左ステック左
		LEFT_STICK_RIGHT,  // 左ステック右
		RIGHT_STICK_UP,    // 右スティック上
		RIGHT_STICK_DOWN,  // 右スティック下
		RIGHT_STICK_LEFT,  // 右スティック左
		RIGHT_STICK_RIGHT, // 右スティック右
		LEFT_SHOULDER,     // Lボタン/LB
		RIGHT_SHOULDER,    // Rボタン/RB
		LEFT_STICK_PUSH,   // 左スティック押し込み
		RIGHT_STICK_PUSH,  // 右スティック押し込み
	};

	// XInputのアナログ値の最大値
	static constexpr float XINPUT_VAL_MAX = 32767.0f;

	/// @brief 左スティックのデッドゾーンしきい値
	static constexpr float LEFT_STICK_DEAD_ZONE = 0.35f;

	// 入力テーブル関連
	using InputTable_t = std::unordered_map<std::wstring, std::vector<InputInfo>>;
	InputTable_t inputTable_;                                   // イベント名と実入力のマッピングテーブル
	std::unordered_map<std::wstring, bool> currentInputState_;   // 現フレームの入力状態(イベント名 → 押されているか)
	std::unordered_map<std::wstring, bool> previousInputState_;  // 前フレームの入力状態(トリガー判定に使用)

	// スティック関連
	StickInfo stickInfo_;                  // 現フレームのスティック入力値
	RStickSensitivity rStickSensitivity_;  // 右スティックの感度設定

	// マウス関連
	Vector2 mousePosition_;               // マウスカーソルの座標
	MouseSensitivity mouseSensitivity_;   // マウス感度
	int mouseWheelVol_;                   // マウスホイールの回転量

	// 使用するパッドの番号
	int usePadNo_;

	// シングルトン用インスタンス
	static KeyConfInputManager* instance_;

	/// @brief コンストラクタ
	KeyConfInputManager(void);

	/// @brief デストラクタ 
	~KeyConfInputManager(void) = default;

	/// @brief 入力テーブルをデフォルト設定で初期化する
	void InitInputTable(void);

	/// @brief スティックの入力値を正規化し、デッドゾーン処理を行う共通関数
	/// @param _rawX 生のX入力値
	/// @param _rawY 生のY入力値
	/// @param _deadZone 適用するデッドゾーン
	/// @param _outNormalX 正規化後のX値を受け取る参照
	/// @param _outNormalY 正規化後のY値を受け取る参照
	void CalculateNormalizedStick(int _rawX, int _rawY, float _deadZone, float& _outNormalX, float& _outNormalY) const;

	/// @brief 右スティックの入力値にデッドゾーンと感度を適用し正規化する
	/// @param _rightStickX スティックのX入力値
	/// @param _rightStickY スティックのY入力値
	/// @param _outNormalX 処理後のX値
	/// @param _outNormalY 処理後のY値
	void ApplyRightStickSensitivity(int _rightStickX, int _rightStickY, float& _outNormalX, float& _outNormalY) const;

	/// @brief キーボードの入力判定を行う
	/// @param _inputInfo 判定する入力情報
	/// @param _keyState キーボードの生データ
	/// @return 押されていれば true
	bool CheckKeyboardInput(const InputInfo& _inputInfo,
		const std::array<char, 256>& _keyState) const;

	/// @brief XInputのアナログ入力トリガーやスティックの判定を行う
	/// @param _inputInfo 判定する入力情報
	/// @param _xInputState XInputの生データ
	/// @return 閾値を超えていれば true
	bool CheckXInputAnalog(const InputInfo& _inputInfo, const XINPUT_STATE& _xInputState) const;

	// コピー禁止 
	KeyConfInputManager(const KeyConfInputManager&) = delete;
	KeyConfInputManager& operator=(const KeyConfInputManager&) = delete;

	// ムーブ禁止
	KeyConfInputManager(KeyConfInputManager&&) = delete;
	KeyConfInputManager& operator=(KeyConfInputManager&&) = delete;
};