#pragma once

/// @brief 2D画像のハンドル情報を保持する純粋なデータコンポーネント
struct ImageComponent
{
	// 画像のハンドルID
	int imageHandle_;

	// 透過処理を有効にするかどうかのフラグ
	bool isTransparent_; 

	/// @brief コンストラクタ
	ImageComponent(void)
		: imageHandle_(-1)
		, isTransparent_(true)
	{
	}
};