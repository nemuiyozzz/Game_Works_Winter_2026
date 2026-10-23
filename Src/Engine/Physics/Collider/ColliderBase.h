#pragma once

#include <DxLib.h>
#include <vector>

struct TransformComponent;

/// @brief コライダーの基底クラス
class ColliderBase
{
public:

	// 形状
	enum class SHAPE
	{
		NONE,        // 無効
		LINE,        // 線分
		SPHERE,      // 球
		CAPSULE,     // カプセル
		MODEL,       // モデル
		BOX,         // 3Dボックス
	};

	// 衝突種別
	enum class TAG
	{
		STAGE,			// ステージ
		PLAYER,			// プレイヤー
		PLAYER_SWORD,	// プレイヤーの剣
		ENEMY,			// エネミー
		WALL,           // 壁
	};

	/// @brief コンストラクタ
	/// @param shapeType コライダー形状
	/// @param collisionTag 衝突種別
	/// @param followTarget 追従対象のTransformComponent
	ColliderBase(SHAPE shapeType, TAG collisionTag, const TransformComponent* followTarget);

	/// @brief デストラクタ
	virtual ~ColliderBase(void) = default;

	/// @brief デバッグ描画を行う
	void Draw(void) const;

	/// @brief 追従先を取得する
	/// @return TransformComponentポインタ
	const TransformComponent* GetFollowTarget(void) const;

	/// @brief 追従先を設定する
	/// @param followTarget 新しいTransformComponent
	void SetFollowTarget(const TransformComponent* followTarget);

	/// @brief コライダー形状を取得する
	/// @return 形状種別
	SHAPE GetShapeType(void) const;

	/// @brief 衝突タグを取得する
	/// @return タグ種別
	TAG GetCollisionTag(void) const;

	/// @brief 有効状態を取得する
	/// @return true: 有効 / false: 無効
	bool IsActive(void) const;

	/// @brief 有効状態を設定する
	/// @param isActive trueで有効化
	void SetActive(bool isActive);

	/// @brief 押し出し判定するかどうか
	/// @return trueなら押し出す
	bool IsTrigger(void) const;

	/// @brief 押し出し判定の設定
	/// @param isTrigger trueなら押し出す、falseなら押し出さない
	void SetTrigger(bool isTrigger);

	/// @brief 半径を設定する
	/// @param radius 新しい半径の値
	virtual void SetRadius(float radius) {}

	/// @brief 衝突対象にタグを追加する
	/// @param targetTag 追加するタグ
	void AddTargetTag(TAG targetTag);

	/// @brief 指定したタグが衝突対象に含まれているか確認する
	/// @param targetTag 確認するタグ
	/// @return 含まれていれば true
	bool IsTargetTag(TAG targetTag) const;

protected:

	// デバック表示の色
	static constexpr int DEBUG_COLOR_ACTIVE = 0xff0000;      // 有効なコライダーは赤色
	static constexpr int DEBUG_COLOR_INACTIVE = 0xaaaaaa;    // 無効なコライダーは灰色

	// 形状
	SHAPE shapeType_;

	// 衝突種別
	TAG collisionTag_;

	// 追従先
	const TransformComponent* followTarget_;

	// 有効フラグ
	bool isActive_;

	// 押し出しするかどうかのフラグ
	bool isTrigger_;

	// 衝突対象のタグリスト
	std::vector<TAG> targetTags_;

	/// @brief ローカル座標をワールド座標に変換する
	/// @param localPosition ローカル座標
	/// @return ワールド座標
	VECTOR TransformLocalToWorld(const VECTOR& localPosition) const;

	/// @brief デバッグ描画
	/// @param color 描画色
	virtual void DrawDebug(int color) const = 0;
};