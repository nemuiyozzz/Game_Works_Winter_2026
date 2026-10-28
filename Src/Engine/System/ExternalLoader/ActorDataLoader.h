#pragma once

#include <string>
#include <DxLib.h>

/// @brief プレイヤーやカメラ・エネミーのパラメータをCSV変換・バイナリ読み書きするクラス
class ActorDataLoader
{
public:

	// アクター1体が保持できる最大コライダー数
	static constexpr int MAX_ACTOR_COLLIDERS = 4;

#pragma pack(push, 1)

	/// @brief 任意のコライダー1つ分の形状とパラメータを保持する汎用バイナリ構造体
	struct ColliderSlotBinaryData
	{
		// 形状・判定設定関連
		int shapeType = 0;               // 形状（0:NONE,1:LINE,2:SPHERE,3:CAPSULE,4:MODEL,5:BOX）
		int isTrigger = 1;               // 押し出し判定か（0:false, 1:true）
		int selfTag = -1;                // 自身のタグ（-1ならアクター標準タグを使用）
		unsigned int targetTagMask = 0;  // 衝突対象タグのビットマスク（0なら標準設定）

		// 座標・サイズパラメータ関連
		VECTOR vectorParameter1 = { 0.0f, 0.0f, 0.0f };  // Sphere/Box:中心, Capsule/Line:始点
		VECTOR vectorParameter2 = { 0.0f, 50.0f, 0.0f }; // Box:サイズ, Capsule/Line:終点
		float radius = 10.0f;                            // Sphere/Capsule:半径
	};

	/// @brief プレイヤーの初期パラメータを保持するバイナリ構造体
	struct PlayerBinaryData
	{
		// ファイル識別子
		char signature[4] = { 'P', 'L', 'R', '2' };

		// 移動・トランスフォーム関連
		VECTOR scale = { 50.0f, 50.0f, 50.0f }; // モデルのスケール
		float moveSpeed = 100.0f;               // 移動速度
		float jumpPower = 150.0f;               // ジャンプ力
		float maxSpeed = -1.0f;                 // 最大速度制限

		// ステータス・レベル成長関連
		int maxHp = 100;              // 最大体力
		int attackPower = 25;         // 攻撃力
		int defensePower = 5;         // 防御力
		int nextLevelExperience = 50; // 初期の次レベル必要経験値
		int growthHp = 20;            // レベルアップ時のHP上昇量
		int growthAttack = 5;         // レベルアップ時の攻撃力上昇量
		int growthDefense = 2;        // レベルアップ時の防御力上昇量

		// 汎用コライダー関連
		int colliderCount = 2; // 保有するコライダー数
		ColliderSlotBinaryData colliders[MAX_ACTOR_COLLIDERS] = // コライダーデータ配列
		{
			{ 3, 1, 1, 25,{ 0.0f, 30.0f, 0.0f },{ 0.0f, 120.0f, 0.0f }, 30.0f },
			{ 1, 1, 1, 17,{ 0.0f, 50.0f, 0.0f },{ 0.0f, -5.0f, 0.0f }, 0.0f },
			{},
			{}
		};
	};

	/// @brief カメラの初期パラメータを保持するバイナリ構造体
	struct CameraBinaryData
	{
		// ファイル識別子
		char signature[4] = { 'C', 'A', 'M', '2' };

		// パラメータ関連
		VECTOR targetOffset = { 0.0f, 50.0f, 0.0f };      // 注視点のズレ
		float distance = 300.0f;                          // ターゲットからの距離
		float angleHorizontal = 0.0f;                     // 初期水平角度
		float angleVertical = 15.0f * (DX_PI_F / 180.0f); // 初期垂直角度
		float rotationSpeed = 0.05f;                      // 回転速度
		float freeMoveSpeed = 2.0f;                       // フリーカメラ移動速度
		float fov = 60.0f * (DX_PI_F / 180.0f);           // 視野角
		float nearZ = 1.0f;                               // ニアクリップ
		float farZ = 20000.0f;                            // ファークリップ

		// 汎用コライダー関連
		int colliderCount = 0;                                      // 保有するコライダー数
		ColliderSlotBinaryData colliders[MAX_ACTOR_COLLIDERS] = {}; // コライダーデータ配列
	};

	/// @brief エネミーの初期パラメータを保持するバイナリ構造体
	struct EnemyBinaryData
	{
		// ファイル識別子
		char signature[4] = { 'E', 'N', 'M', '2' };

		// モデルのスケール
		VECTOR scale = { 30.0f, 30.0f, 30.0f };

		// ステータス関連
		int maxHp = 50;            // 最大体力
		int attackPower = 10;      // 攻撃力
		int defensePower = 2;      // 防御力
		int rewardExperience = 25; // 撃破時に得られる経験値

		// 汎用コライダー関連
		int colliderCount = 2; // 保有するコライダー数
		ColliderSlotBinaryData colliders[MAX_ACTOR_COLLIDERS] = // コライダーデータ配列
		{
			{ 3, 1, 3, 31,{ 0.0f, 10.0f, 0.0f },{ 0.0f, 30.0f, 0.0f }, 5.0f },
			{ 1, 1, 3, 17,{ 0.0f, 50.0f, 0.0f },{ 0.0f, -5.0f, 0.0f }, 0.0f },
			{},
			{}
		};
	};

#pragma pack(pop)

	/// @brief プレイヤーデータを読み込む
	/// @param csvFilePath 開発用CSVファイルのパス
	/// @param binaryFilePath 実行用バイナリファイルのパス
	/// @param outData 読み込んだデータの格納先
	/// @return 読み込みに成功したらtrue
	static bool LoadPlayerData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, PlayerBinaryData& outData);

	/// @brief プレイヤーデータをバイナリとCSVの両方に上書き保存する
	/// @param csvFilePath 保存先CSVファイルのパス
	/// @param binaryFilePath 保存先バイナリファイルのパス
	/// @param inData 保存するプレイヤーデータ
	/// @return 保存に成功したらtrue
	static bool SavePlayerData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, const PlayerBinaryData& inData);

	/// @brief カメラデータを読み込む
	/// @param csvFilePath 開発用CSVファイルのパス
	/// @param binaryFilePath 実行用バイナリファイルのパス
	/// @param outData 読み込んだデータの格納先
	/// @return 読み込みに成功したらtrue
	static bool LoadCameraData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, CameraBinaryData& outData);

	/// @brief カメラデータをバイナリとCSVの両方に上書き保存する
	/// @param csvFilePath 保存先CSVファイルのパス
	/// @param binaryFilePath 保存先バイナリファイルのパス
	/// @param inData 保存するカメラデータ
	/// @return 保存に成功したらtrue
	static bool SaveCameraData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, const CameraBinaryData& inData);

	/// @brief エネミーデータを読み込む
	/// @param csvFilePath 開発用CSVファイルのパス
	/// @param binaryFilePath 実行用バイナリファイルのパス
	/// @param outData 読み込んだデータの格納先
	/// @return 読み込みに成功したらtrue
	static bool LoadEnemyData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, EnemyBinaryData& outData);

	/// @brief エネミーデータをバイナリとCSVの両方に上書き保存する
	/// @param csvFilePath 保存先CSVファイルのパス
	/// @param binaryFilePath 保存先バイナリファイルのパス
	/// @param inData 保存するエネミーデータ
	/// @return 保存に成功したらtrue
	static bool SaveEnemyData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, const EnemyBinaryData& inData);

private:

	/// @brief プレイヤー用CSVをバイナリへ変換して保存する
	/// @param csvFilePath CSVファイルのパス
	/// @param binaryFilePath バイナリファイルのパス
	/// @return 変換に成功したらtrue
	static bool ConvertPlayerCsvToBinary(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);

	/// @brief カメラ用CSVをバイナリへ変換して保存する
	/// @param csvFilePath CSVファイルのパス
	/// @param binaryFilePath バイナリファイルのパス
	/// @return 変換に成功したらtrue
	static bool ConvertCameraCsvToBinary(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);

	/// @brief エネミー用CSVをバイナリへ変換して保存する
	/// @param csvFilePath CSVファイルのパス
	/// @param binaryFilePath バイナリファイルのパス
	/// @return 変換に成功したらtrue
	static bool ConvertEnemyCsvToBinary(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);
};