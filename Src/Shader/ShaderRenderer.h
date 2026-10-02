#pragma once

#include <DxLib.h>
#include <unordered_map>
#include <array>
#include "RenderCommand.h"

/// @brief シェーダー描画を管理するレンダラークラス
class ShaderRenderer
{
public:

	// スロット関連定数
	static constexpr int CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER = 7; // 頂点シェーダ用定数バッファの開始スロット
	static constexpr int CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER = 4;  // ピクセルシェーダ用定数バッファの開始スロット

	/// @brief コンストラクタ
	ShaderRenderer(void);

	/// @brief デストラクタ
	~ShaderRenderer(void);

	/// @brief 初期化処理
	void Initialize(void);

	/// @brief バッチ処理の開始（描画ループの直前に呼ぶ）
	void BeginBatch(void);

	/// @brief 描画コマンドの実行
	/// @param _renderCommand 描画コマンド情報
	void ExecuteCommand(const RenderCommand& _renderCommand);

	/// @brief バッチ処理の終了（すべての描画が終わった後に呼ぶ）
	void EndBatch(void);

	/// @brief 解放処理
	void Release(void);

private:

	/// @brief 定数バッファを更新してシェーダに設定
	/// @param _parameterPointer パラメータデータへのポインタ
	/// @param _parameterSize パラメータサイズ
	/// @param _shaderType シェーダタイプ
	/// @param _slotBegin スロット開始番号
	/// @return 使用した定数バッファハンドル
	int UpdateAndSetConstantBuffer(const void* _parameterPointer, int _parameterSize, int _shaderType, int _slotBegin);

	/// @brief 2D描画用の頂点データを適用
	/// @param _vertices 頂点配列
	/// @param _width 描画幅
	/// @param _height 描画高さ
	void ApplyVertices(std::array<VERTEX2DSHADER, 4>& _vertices, float _width, float _height) const;

	// 定数バッファマップ
	std::unordered_map<int, int> constantBufferMap_;                      

	// キャッシュ・状態管理関連
	int currentVertexShaderHandleId_;                                     // 現在設定されている頂点シェーダのハンドル
	int currentPixelShaderHandleId_;                                      // 現在設定されているピクセルシェーダのハンドル
	int currentTexture0HandleId_;                                         // 現在設定されているテクスチャ0のハンドル
	int currentTexture1HandleId_;                                         // 現在設定されているテクスチャ1のハンドル
};