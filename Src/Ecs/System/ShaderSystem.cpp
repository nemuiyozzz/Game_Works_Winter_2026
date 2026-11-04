#include "../../Pch.h"
#include "ShaderSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/ShaderComponent.h"
#include "../Component/ModelComponent.h"
#include "../Component/ImageComponent.h"
#include "../Component/PlayerInputComponent.h"

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
	VECTOR playerPos = VGet(0, 0, 0);
	auto players = registry.GetEntitiesWith<TransformComponent, PlayerInputComponent>();

	if (!players.empty())
	{
		playerPos = registry.GetComponent<TransformComponent>(players.front()).position;
	}

	for (Entity entity : entities)
	{
		QueueDrawCommand(registry, entity, playerPos);
	}
}

void ShaderSystem::QueueDrawCommand(EcsRegistry& registry, Entity entity, const VECTOR& basePos)
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
	command.vertexShaderHandleId = shader.vertexShaderHandle;
	command.pixelShaderHandleId = shader.pixelShaderHandle;
	command.textureHandleId = shader.textureHandle;
	command.normalMapHandleId = shader.normalMapHandle;
	command.isClamp = shader.isClamp;

	command.vertexParameterData = shader.vertexParameterData;
	command.vertexParameterSize = static_cast<int>(shader.vertexParameterData.size());
	command.pixelParameterData = shader.pixelParameterData;
	command.pixelParameterSize = static_cast<int>(shader.pixelParameterData.size());

	// 3Dモデルを持つエンティティの場合
	if (registry.HasComponent<ModelComponent>(entity))
	{
		ModelComponent& model = registry.GetComponent<ModelComponent>(entity);

		command.renderType = RENDER_TYPE::DRAW_3D;
		command.modelHandleId = model.modelHandle;
		command.worldMatrix = transform.combinedMatrix;
		command.positionX = 0.0f;
		command.positionY = 0.0f;
		command.scaleSize = 1.0f;

		constexpr float CULLING_DISTANCE = 3000.0f;
		constexpr float CULLING_DIST_SQ = CULLING_DISTANCE * CULLING_DISTANCE;

		if (model.frameInfos.size() > 1)
		{
			for (const auto& frame : model.frameInfos)
			{
				VECTOR worldCenter = VTransform(frame.localCenterPos, transform.combinedMatrix);

				float diffX = worldCenter.x - basePos.x;
				float diffY = worldCenter.y - basePos.y;
				float diffZ = worldCenter.z - basePos.z;
				float distSq = (diffX * diffX) + (diffY * diffY) + (diffZ * diffZ);

				if (distSq <= CULLING_DIST_SQ)
				{
					command.targetFrameIndices.push_back(frame.frameIndex);
				}
			}

			if (command.targetFrameIndices.empty())
			{
				return;
			}
		}

		renderCommandQueue_.push_back(command);
	}
	else if (registry.HasComponent<ImageComponent>(entity))
	{
		ImageComponent& image = registry.GetComponent<ImageComponent>(entity);

		// 2Dの場合は画像のハンドルをテクスチャとして扱う（ShaderRendererの仕様に合わせる）
		if (command.textureHandleId == -1)
		{
			command.textureHandleId = image.imageHandle;
		}

		command.renderType = RENDER_TYPE::DRAW_2D;
		command.modelHandleId = -1;
		command.positionX = transform.position.x;
		command.positionY = transform.position.y;
		command.scaleSize = transform.scale.x;

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
