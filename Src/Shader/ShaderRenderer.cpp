#include "../Pch.h"
#include "ShaderRenderer.h"
#include "../Application.h" 

ShaderRenderer::ShaderRenderer(void)
	: currentVertexShaderHandleId_(-1)
	, currentPixelShaderHandleId_(-1)
	, currentTexture0HandleId_(-1)
	, currentTexture1HandleId_(-1)
	, constantBufferMap_()
{
}

ShaderRenderer::~ShaderRenderer(void)
{
	Release();
}

void ShaderRenderer::Initialize(void)
{
}

void ShaderRenderer::BeginBatch(void)
{
	// キャッシュを初期化する
	currentVertexShaderHandleId_ = -1;
	currentPixelShaderHandleId_ = -1;
	currentTexture0HandleId_ = -1;
	currentTexture1HandleId_ = -1;
}

void ShaderRenderer::ExecuteCommand(const RenderCommand& _renderCommand)
{
	if (_renderCommand.vertexShaderHandleId != -1 && _renderCommand.vertexParameterSize > 0)
	{
		UpdateAndSetConstantBuffer(_renderCommand.vertexParameterData.data(), _renderCommand.vertexParameterSize, DX_SHADERTYPE_VERTEX, CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER);
	}

	if (_renderCommand.pixelParameterSize > 0)
	{
		UpdateAndSetConstantBuffer(_renderCommand.pixelParameterData.data(), _renderCommand.pixelParameterSize, DX_SHADERTYPE_PIXEL, CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER);
	}

	// テクスチャのバインド
	if (_renderCommand.textureHandleId != -1)
	{
		if (currentTexture0HandleId_ != _renderCommand.textureHandleId)
		{
			SetUseTextureToShader(0, _renderCommand.textureHandleId);
			currentTexture0HandleId_ = _renderCommand.textureHandleId;
		}
	}
	else
	{
		if (currentTexture0HandleId_ != -1)
		{
			SetUseTextureToShader(0, -1);
			currentTexture0HandleId_ = -1;
		}
	}

	if (_renderCommand.normalMapHandleId != -1)
	{
		if (currentTexture1HandleId_ != _renderCommand.normalMapHandleId)
		{
			SetUseTextureToShader(1, _renderCommand.normalMapHandleId);
			currentTexture1HandleId_ = _renderCommand.normalMapHandleId;
		}
	}
	else
	{
		if (currentTexture1HandleId_ != -1)
		{
			SetUseTextureToShader(1, -1);
			currentTexture1HandleId_ = -1;
		}
	}

	// 頂点シェーダの設定
	if (_renderCommand.vertexShaderHandleId != -1)
	{
		if (currentVertexShaderHandleId_ != _renderCommand.vertexShaderHandleId)
		{
			SetUseVertexShader(_renderCommand.vertexShaderHandleId);
			currentVertexShaderHandleId_ = _renderCommand.vertexShaderHandleId;
		}
	}
	else
	{
		if (currentVertexShaderHandleId_ != -1)
		{
			SetUseVertexShader(-1);
			currentVertexShaderHandleId_ = -1;
		}
	}

	// ピクセルシェーダの設定
	if (_renderCommand.pixelShaderHandleId != -1)
	{
		// 前回と違うシェーダが指定された時だけセットする
		if (currentPixelShaderHandleId_ != _renderCommand.pixelShaderHandleId)
		{
			SetUsePixelShader(_renderCommand.pixelShaderHandleId);
			currentPixelShaderHandleId_ = _renderCommand.pixelShaderHandleId;
		}
	}
	else
	{
		if (currentPixelShaderHandleId_ != -1)
		{
			SetUsePixelShader(-1);
			currentPixelShaderHandleId_ = -1;
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	if (_renderCommand.renderType == RENDER_TYPE::DRAW_2D)
	{
		const float DRAW_WIDTH = static_cast<float>(Application::SCREEN_SIZE_X) * _renderCommand.scaleSize;
		const float DRAW_HEIGHT = static_cast<float>(Application::SCREEN_SIZE_Y) * _renderCommand.scaleSize;

		std::array<VERTEX2DSHADER, 4> vertices;
		ApplyVertices(vertices, DRAW_WIDTH, DRAW_HEIGHT);

		for (auto& vertex : vertices)
		{
			vertex.pos.x += _renderCommand.positionX;
			vertex.pos.y += _renderCommand.positionY;
		}

		DrawPrimitive2DToShader(vertices.data(), 4, DX_PRIMTYPE_TRIANGLESTRIP);
	}
	else if (_renderCommand.renderType == RENDER_TYPE::DRAW_3D)
	{
		if (_renderCommand.isClamp)
		{
			SetTextureAddressModeUV(DX_TEXADDRESS_CLAMP, DX_TEXADDRESS_CLAMP);
		}
		else
		{
			SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);
		}

		MV1SetUseOrigShader(true);
		MV1DrawModel(_renderCommand.modelHandleId);
		MV1SetUseOrigShader(false);

		SetTextureAddressModeUV(DX_TEXADDRESS_CLAMP, DX_TEXADDRESS_CLAMP);
	}
}

void ShaderRenderer::EndBatch(void)
{
	// 全ての描画が終わった後にまとめて状態をアンバインドする
	SetUseTextureToShader(0, -1);
	SetUseTextureToShader(1, -1);
	SetUsePixelShader(-1);
	SetUseVertexShader(-1);

	SetShaderConstantBuffer(-1, DX_SHADERTYPE_VERTEX, CONSTANT_BUFFER_SLOT_BEGIN_VERTEX_SHADER);
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, CONSTANT_BUFFER_SLOT_BEGIN_PIXEL_SHADER);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// キャッシュをリセットする
	currentVertexShaderHandleId_ = -1;
	currentPixelShaderHandleId_ = -1;
	currentTexture0HandleId_ = -1;
	currentTexture1HandleId_ = -1;
}

void ShaderRenderer::Release(void)
{
	for (auto& pair : constantBufferMap_)
	{
		if (pair.second != -1)
		{
			DeleteShaderConstantBuffer(pair.second);
		}
	}
	constantBufferMap_.clear();
}

int ShaderRenderer::UpdateAndSetConstantBuffer(const void* _parameterPointer, int _parameterSize, int _shaderType, int _slotBegin)
{
	if (_parameterPointer == nullptr || _parameterSize <= 0)
	{
		return -1;
	}

	int constantBufferHandle = -1;

	if (constantBufferMap_.find(_parameterSize) == constantBufferMap_.end())
	{
		constantBufferHandle = CreateShaderConstantBuffer(_parameterSize);
		constantBufferMap_[_parameterSize] = constantBufferHandle;
	}
	else
	{
		constantBufferHandle = constantBufferMap_[_parameterSize];
	}

	if (constantBufferHandle == -1)
	{
		return -1;
	}

	void* bufferPointer = GetBufferShaderConstantBuffer(constantBufferHandle);
	if (bufferPointer != nullptr)
	{
		std::memcpy(bufferPointer, _parameterPointer, _parameterSize);
		UpdateShaderConstantBuffer(constantBufferHandle);
		SetShaderConstantBuffer(constantBufferHandle, _shaderType, _slotBegin);
	}

	return constantBufferHandle;
}

void ShaderRenderer::ApplyVertices(std::array<VERTEX2DSHADER, 4>& _vertices, float _width, float _height) const
{
	for (auto& vertex : _vertices)
	{
		vertex.rhw = 1.0f;
		vertex.pos.z = 0.0f;
		vertex.dif = GetColorU8(255, 255, 255, 255);
		vertex.spc = GetColorU8(0, 0, 0, 0);
		vertex.su = 1.0f;
		vertex.sv = 1.0f;
	}

	// 左上
	_vertices[0].pos = VGet(0.0f, 0.0f, 0.0f);
	_vertices[0].u = 0.0f;
	_vertices[0].v = 0.0f;

	// 右上
	_vertices[1].pos = VGet(_width, 0.0f, 0.0f);
	_vertices[1].u = 1.0f;
	_vertices[1].v = 0.0f;

	// 左下
	_vertices[2].pos = VGet(0.0f, _height, 0.0f);
	_vertices[2].u = 0.0f;
	_vertices[2].v = 1.0f;

	// 右下
	_vertices[3].pos = VGet(_width, _height, 0.0f);
	_vertices[3].u = 1.0f;
	_vertices[3].v = 1.0f;
}