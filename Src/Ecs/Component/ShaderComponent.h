#pragma once

#include <vector>

/// @brief シェーダのハンドルやパラメータを保持する純粋なデータコンポーネント
struct ShaderComponent
{
	// シェーダのハンドル関連
	int vertexShaderHandle_ = -1; // 頂点シェーダのID
	int pixelShaderHandle_ = -1;  // ピクセルシェーダのID
	int textureHandle_ = -1;      // メインテクスチャのハンドル
	int normalMapHandle_ = -1;    // ノーマルマップのハンドル
	bool isClamp_ = false;           // テクスチャをクランプするか

	// シェーダのパラメータ関連
	std::vector<unsigned char> vertexParameterData_; // 頂点シェーダ用パラメータデータ
	std::vector<unsigned char> pixelParameterData_;  // ピクセルシェーダ用パラメータデータ

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