#pragma once

#include <vector>

/// @brief シェーダのハンドルやパラメータを保持する純粋なデータコンポーネント
struct ShaderComponent
{
	// シェーダのハンドル関連
	int vertexShaderHandle_; // 頂点シェーダのID（-1で不使用）
	int pixelShaderHandle_;  // ピクセルシェーダのID
	int textureHandle_;      // メインテクスチャのハンドル
	int normalMapHandle_;    // ノーマルマップのハンドル
	bool isClamp_;           // テクスチャをクランプするか

	// シェーダのパラメータ関連
	std::vector<unsigned char> vertexParameterData_; // 頂点シェーダ用パラメータデータ
	std::vector<unsigned char> pixelParameterData_;  // ピクセルシェーダ用パラメータデータ

	/// @brief コンストラクタ
	ShaderComponent(void)
		: vertexShaderHandle_(-1)
		, pixelShaderHandle_(-1)
		, textureHandle_(-1)
		, normalMapHandle_(-1)
		, isClamp_(false)
	{
	}

	/// @brief 頂点シェーダ用のパラメータ構造体をセットする
	/// @tparam ParameterType 定数バッファの構造体型
	/// @param parameters 設定するパラメータ
	template <typename ParameterType>
	void SetVertexParameter(const ParameterType& parameters)
	{
		const unsigned char* bytePointer = reinterpret_cast<const unsigned char*>(&parameters);
		vertexParameterData_.assign(bytePointer, bytePointer + sizeof(ParameterType));
	}

	/// @brief ピクセルシェーダ用のパラメータ構造体をセットする
	/// @tparam ParameterType 定数バッファの構造体型
	/// @param parameters 設定するパラメータ
	template <typename ParameterType>
	void SetPixelParameter(const ParameterType& parameters)
	{
		const unsigned char* bytePointer = reinterpret_cast<const unsigned char*>(&parameters);
		pixelParameterData_.assign(bytePointer, bytePointer + sizeof(ParameterType));
	}
};