#include "../../Pch.h"
#include "ShaderSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/ShaderComponent.h"
#include "../Component/ModelComponent.h"
#include "../Component/ImageComponent.h"

ShaderSystem::ShaderSystem(void)
	: shaderRenderer_(std::make_unique<ShaderRenderer>())
{
}

void ShaderSystem::Initialize(void)
{
	shaderRenderer_->Initialize();
}

void ShaderSystem::Release(void)
{
	shaderRenderer_->Release();
	renderCommandQueue_.clear();
}

void ShaderSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<TransformComponent, ShaderComponent>();

	for (Entity entity : entities)
	{
		QueueDrawCommand(registry, entity);
	}
}

void ShaderSystem::QueueDrawCommand(EcsRegistry& registry, Entity entity)
{
	// 必須コンポーネント（Transform と Shader）の確認
	if (!registry.HasComponent<TransformComponent>(entity) ||
		!registry.HasComponent<ShaderComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	ShaderComponent& shader = registry.GetComponent<ShaderComponent>(entity);

	RenderCommand command{};
	command.vertexShaderHandleId = shader.vertexShaderHandle_;
	command.pixelShaderHandleId = shader.pixelShaderHandle_;
	command.textureHandleId = shader.textureHandle_;
	command.normalMapHandleId = shader.normalMapHandle_;
	command.isClamp = shader.isClamp_;

	command.vertexParameterData = shader.vertexParameterData_;
	command.vertexParameterSize = static_cast<int>(shader.vertexParameterData_.size());
	command.pixelParameterData = shader.pixelParameterData_;
	command.pixelParameterSize = static_cast<int>(shader.pixelParameterData_.size());

	// 3Dモデルを持つエンティティの場合
	if (registry.HasComponent<ModelComponent>(entity))
	{
		ModelComponent& model = registry.GetComponent<ModelComponent>(entity);

		command.renderType = RENDER_TYPE::DRAW_3D;
		command.modelHandleId = model.modelHandle_;
		command.positionX = 0.0f;
		command.positionY = 0.0f;
		command.scaleSize = 1.0f;

		renderCommandQueue_.push_back(command);
	}
	// 2D画像を持つエンティティの場合
	else if (registry.HasComponent<ImageComponent>(entity))
	{
		ImageComponent& image = registry.GetComponent<ImageComponent>(entity);

		// 2Dの場合は画像のハンドルをテクスチャとして扱う（ShaderRendererの仕様に合わせる）
		if (command.textureHandleId == -1)
		{
			command.textureHandleId = image.imageHandle_;
		}

		command.renderType = RENDER_TYPE::DRAW_2D;
		command.modelHandleId = -1;
		command.positionX = transform.position_.x;
		command.positionY = transform.position_.y;
		command.scaleSize = transform.scale_.x;

		renderCommandQueue_.push_back(command);
	}
}

void ShaderSystem::ExecuteDrawCommands(void)
{
	if (renderCommandQueue_.empty())
	{
		return;
	}

	// シェーダの切り替えコストを減らすためのソート
	std::sort(renderCommandQueue_.begin(), renderCommandQueue_.end(),
		[](const RenderCommand& leftCommand, const RenderCommand& rightCommand)
		{
			if (leftCommand.vertexShaderHandleId != rightCommand.vertexShaderHandleId)
			{
				return leftCommand.vertexShaderHandleId < rightCommand.vertexShaderHandleId;
			}
			return leftCommand.pixelShaderHandleId < rightCommand.pixelShaderHandleId;
		});

	shaderRenderer_->BeginBatch();

	for (const auto& renderCommand : renderCommandQueue_)
	{
		shaderRenderer_->ExecuteCommand(renderCommand);
	}

	shaderRenderer_->EndBatch();
	renderCommandQueue_.clear();
}
