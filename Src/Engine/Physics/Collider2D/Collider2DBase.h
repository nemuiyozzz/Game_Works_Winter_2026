#pragma once

/// @brief 2Dコライダーの共通親クラス
class Collider2DBase
{
public:

	/// @brief 2D形状の種別
	enum class SHAPE_2D
	{
		CIRCLE,  // 円
		BOX,     // 矩形
	};

	/// @brief 衝突の識別タグ
	enum class TAG_2D
	{
		NONE,                    // 未設定
	};

	/// @brief コンストラクタ
	/// @param _shapeType 2D形状の種類
	/// @param _collisionTag 衝突の2D識別タグ
	Collider2DBase(SHAPE_2D shapeType, TAG_2D collisionTag);

	/// @brief デストラクタ
	virtual ~Collider2DBase(void) = default;

	/// @brief 2D形状タイプの取得
	/// @return 2D形状タイプ
	SHAPE_2D GetShapeType(void) const;

	/// @brief 2Dタグの取得 
	/// @return 衝突2Dタグ
	TAG_2D GetCollisionTag(void) const;

	/// @brief デバッグ描画
	/// @param _color 描画色
	virtual void DrawDebug(int color) const = 0;

protected:

	// コライダー設定関連
	SHAPE_2D shapeType_;      // 2D形状の種別
	TAG_2D collisionTag_;     // 衝突の2D識別タグ
};