#include "../../Pch.h"
#include "DebugGuiSystem.h"
#include "../EcsRegistry.h"
#include "../Component/TransformComponent.h"
#include "../Component/VelocityComponent.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/CollisionComponent.h"
#include "../Component/Collision2DComponent.h"
#include "../Component/HierarchyComponent.h"
#include "../../Engine/Physics/Collider/ColliderSphere.h"
#include "../../Engine/Physics/Collider/ColliderCapsule.h"
#include "../../Engine/Physics/Collider/ColliderBox.h"
#include "../../Engine/Physics/Collider/ColliderLine.h"
#include "../../Engine/Physics/Collider/ColliderModel.h"
#include "../../Engine/Physics/Collider2D/Collider2DBase.h"
#include "../../Engine/Physics/Collider2D/Collider2DBox.h"
#include "../../Engine/Physics/Collider2D/Collider2DCircle.h"
#include "../../Engine/System/ExternalLoader/ActorDataLoader.h"
#include "../../Engine/System/ExternalLoader/StageDataLoader.h"
#include "../../Common/Quaternion.h"
#include "../../../Libs/ImGui/imgui.h"
#include "../../Engine/Utility/UtilityMath.h"
#include "../Component/ModelComponent.h"
#include "../Component/StatusComponent.h"

namespace
{
	// コライダー調整用のドラッグ感度・最小値関連
	constexpr float DRAG_SPEED_POSITION = 1.0f;  // 座標ドラッグ時の変化量
	constexpr float DRAG_SPEED_SIZE = 0.5f;      // サイズ・半径ドラッグ時の変化量
	constexpr float MIN_COLLIDER_SIZE = 0.1f;    // 半径やサイズの最小値
	constexpr float MAX_COLLIDER_SIZE = 5000.0f; // 半径やサイズの最大値
}

DebugGuiSystem::DebugGuiSystem(void)
	: selectedEntity_(NULL_ENTITY)
{
}

void DebugGuiSystem::Update(EcsRegistry& registry)
{
#ifdef _DEBUG
	auto entities = registry.GetEntitiesWith<DebugGuiComponent>();

	if (entities.empty())
	{
		return;
	}

	// まだ何も選択されていない、または選択中のエンティティが破棄された場合は先頭を選択する
	if (selectedEntity_ == NULL_ENTITY ||
		!registry.HasComponent<DebugGuiComponent>(selectedEntity_))
	{
		selectedEntity_ = entities.front();
	}

	// Hierarchyウィンドウ（エンティティ一覧）を描画
	DrawHierarchyGui(registry, entities);

	// Inspectorウィンドウ（選択中エンティティのパラメータ）を描画
	if (selectedEntity_ != NULL_ENTITY &&
		registry.HasComponent<DebugGuiComponent>(selectedEntity_))
	{
		DrawInspectorGui(registry, selectedEntity_);
	}
#endif // _DEBUG
}

void DebugGuiSystem::DrawHierarchyGui(
	EcsRegistry& registry, const std::vector<Entity>& entities)
{
	ImGui::Begin("Hierarchy");

	auto drawEntityNode = [&](auto& self, Entity currentEntity)-> void
		{
			if (!registry.HasComponent<DebugGuiComponent>(currentEntity))
			{
				return;
			}

			const DebugGuiComponent& debugGui =
				registry.GetComponent<DebugGuiComponent>(currentEntity);
			bool hasChildren = false;

			if (registry.HasComponent<HierarchyComponent>(currentEntity))
			{
				const auto& hierarchy = registry.GetComponent<HierarchyComponent>(currentEntity);

				for (Entity child : hierarchy.children)
				{
					if (registry.HasComponent<DebugGuiComponent>(child))
					{
						hasChildren = true;
						break;
					}
				}
			}

			ImGuiTreeNodeFlags nodeFlags =
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			if (selectedEntity_ == currentEntity)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
			}

			if (!hasChildren)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			}

			ImGui::PushID(static_cast<int>(currentEntity));

			bool isNodeOpen = ImGui::TreeNodeEx(debugGui.windowTitle.c_str(), nodeFlags);

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				selectedEntity_ = currentEntity;
			}

			if (hasChildren && isNodeOpen)
			{
				const auto& hierarchy = registry.GetComponent<HierarchyComponent>(currentEntity);

				for (Entity child : hierarchy.children)
				{
					self(self, child);
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		};

	for (Entity entity : entities)
	{
		bool isRootNode = true;

		if (registry.HasComponent<HierarchyComponent>(entity))
		{
			Entity parentEntity = registry.GetComponent<HierarchyComponent>(entity).parent;

			if (parentEntity != NULL_ENTITY &&
				registry.HasComponent<DebugGuiComponent>(parentEntity))
			{
				isRootNode = false;
			}
		}

		if (isRootNode)
		{
			drawEntityNode(drawEntityNode, entity);
		}
	}

	ImGui::End();
}

void DebugGuiSystem::DrawInspectorGui(EcsRegistry& registry, Entity entity)
{
	ImGui::Begin("Inspector");

	const DebugGuiComponent& debugGuiData =
		registry.GetComponent<DebugGuiComponent>(entity);

	ImGui::Text("Target : %s (Entity ID: %d)",
		debugGuiData.windowTitle.c_str(), static_cast<int>(entity));
	ImGui::Separator();

	if (registry.HasComponent<CameraComponent>(entity))
	{
		DrawCameraGui(registry, entity);
	}
	else if (registry.HasComponent<TransformComponent>(entity))
	{
		DrawTransformGui(registry, entity);
	}

	if (registry.HasComponent<PlayerInputComponent>(entity))
	{
		DrawPlayerMovementGui(registry, entity);
	}

	if (registry.HasComponent<CollisionComponent>(entity))
	{
		DrawCollisionGui(registry, entity);
	}

	if (registry.HasComponent<Collision2DComponent>(entity))
	{
		DrawCollision2DGui(registry, entity);
	}

	if (debugGuiData.saveType != DEBUG_SAVE_TYPE::NONE)
	{
		DrawSaveButtonGui(registry, entity, debugGuiData);
	}

	if (registry.HasComponent<StatusComponent>(entity))
	{
		DrawStatusGui(registry, entity);
	}

	ImGui::End();
}

void DebugGuiSystem::DrawTransformGui(EcsRegistry& registry, Entity entity)
{
	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		constexpr float DRAG_SPEED_ROTATION = 0.1f;   // 回転ドラッグ時の変化量
		constexpr float MIN_ROTATION_DEG = -360.0f;   // 回転の最小角度
		constexpr float MAX_ROTATION_DEG = 360.0f;    // 回転の最大角度

		bool hasParent = false;
		if (registry.HasComponent<HierarchyComponent>(entity))
		{
			if (registry.GetComponent<HierarchyComponent>(entity).parent != NULL_ENTITY)
			{
				hasParent = true;
			}
		}

		if (hasParent)
		{
			ImGui::DragFloat3("Local Position", &transform.localPosition.x, DRAG_SPEED_POSITION);
		}
		else
		{
			ImGui::DragFloat3("Position", &transform.position.x, DRAG_SPEED_POSITION);
		}

		ImGui::DragFloat3("Scale", &transform.scale.x, 0.05f);

		float rotationDegree[3] =
		{
			UtilityMath::Rad2DegF(transform.rotationEuler.x),
			UtilityMath::Rad2DegF(transform.rotationEuler.y),
			UtilityMath::Rad2DegF(transform.rotationEuler.z)
		};

		if (ImGui::DragFloat3("Rotation(deg)", rotationDegree,
			DRAG_SPEED_ROTATION, MIN_ROTATION_DEG, MAX_ROTATION_DEG, "%.2f"))
		{
			transform.rotationEuler.x = UtilityMath::Deg2RadF(rotationDegree[0]);
			transform.rotationEuler.y = UtilityMath::Deg2RadF(rotationDegree[1]);
			transform.rotationEuler.z = UtilityMath::Deg2RadF(rotationDegree[2]);
			transform.rotation_ = Quaternion::Euler(
				transform.rotationEuler.x,
				transform.rotationEuler.y,
				transform.rotationEuler.z);
		}

		ImGui::Separator();

		if (!registry.HasComponent<HierarchyComponent>(entity))
		{
			if (ImGui::Button("Add Hierarchy (Make Parent/Child)"))
			{
				registry.AddComponent<HierarchyComponent>(entity, HierarchyComponent{});
			}
		}
		else
		{
			HierarchyComponent& hierarchy = registry.GetComponent<HierarchyComponent>(entity);

			int parentId = (hierarchy.parent == NULL_ENTITY) ? -1
				: static_cast<int>(hierarchy.parent);
			
			if (ImGui::InputInt("Parent Entity ID", &parentId))
			{
				if (hierarchy.parent != NULL_ENTITY &&
					registry.HasComponent<HierarchyComponent>(hierarchy.parent))
				{
					auto& oldParentHier = registry.GetComponent<HierarchyComponent>(hierarchy.parent);
					oldParentHier.children.erase(
						std::remove(oldParentHier.children.begin(), oldParentHier.children.end(), entity),
						oldParentHier.children.end());
				}

				if (parentId < 0)
				{
					hierarchy.parent = NULL_ENTITY;

					// 親を解除したときは、ローカル座標をそのままワールド座標に戻す
					if (registry.HasComponent<TransformComponent>(entity))
					{
						auto& t = registry.GetComponent<TransformComponent>(entity);
						t.localPosition = t.position;
					}
				}
				else
				{
					Entity newParent = static_cast<Entity>(parentId);
					hierarchy.parent = newParent;

					if (!registry.HasComponent<HierarchyComponent>(newParent))
					{
						registry.AddComponent<HierarchyComponent>(newParent, HierarchyComponent{});
					}
					
					registry.GetComponent<HierarchyComponent>(newParent).children.push_back(entity);

					if (registry.HasComponent<TransformComponent>(entity) &&
						registry.HasComponent<TransformComponent>(newParent))
					{
						auto& childTransform = registry.GetComponent<TransformComponent>(entity);
						auto& parentTransform = registry.GetComponent<TransformComponent>(newParent);

						VECTOR offset = VSub(childTransform.position, parentTransform.position);

						Quaternion invParentRot = parentTransform.rotation_.Inverse();
						childTransform.localPosition = invParentRot.PosAxis(offset);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::Button("Duplicate Hierarchy (Copy All)"))
			{
				// 再帰的にエンティティとコンポーネントを複製するラムダ関数
				auto duplicateEntity = [&](auto& self, Entity srcEntity) -> Entity
					{
						Entity dstEntity = registry.CreateEntity();

						// Transformのコピー
						if (registry.HasComponent<TransformComponent>(srcEntity))
						{
							registry.AddComponent<TransformComponent>(
								dstEntity, registry.GetComponent<TransformComponent>(srcEntity));
						}
						// モデルのコピー（同じハンドルを使い回す）
						if (registry.HasComponent<ModelComponent>(srcEntity))
						{
							registry.AddComponent<ModelComponent>(
								dstEntity, registry.GetComponent<ModelComponent>(srcEntity));
						}
						// ImGui表示と保存用データのコピー
						if (registry.HasComponent<DebugGuiComponent>(srcEntity))
						{
							registry.AddComponent<DebugGuiComponent>(
								dstEntity, registry.GetComponent<DebugGuiComponent>(srcEntity));
						}
						// 階層（子）のコピー
						if (registry.HasComponent<HierarchyComponent>(srcEntity))
						{
							HierarchyComponent srcHier = registry.GetComponent<HierarchyComponent>(srcEntity);
							HierarchyComponent dstHier{};

							for (Entity child : srcHier.children)
							{
								// 子供も再帰的に複製する
								Entity duplicatedChild = self(self, child);
								dstHier.children.push_back(duplicatedChild);

								// 複製された子供の親を、複製された新しい親（自分）に向ける
								if (registry.HasComponent<HierarchyComponent>(duplicatedChild))
								{
									registry.GetComponent<HierarchyComponent>(duplicatedChild).parent = dstEntity;
								}
							}
							registry.AddComponent<HierarchyComponent>(dstEntity, dstHier);
						}
						return dstEntity;
					};

				// 自分自身をルートとして複製処理を実行
				Entity duplicatedRoot = duplicateEntity(duplicateEntity, entity);

				// コピーした村が重ならないように、少しX方向にずらして配置する
				if (registry.HasComponent<TransformComponent>(duplicatedRoot))
				{
					registry.GetComponent<TransformComponent>(duplicatedRoot).localPosition.x += 300.0f;
				}
			}
		}
	}
}

void DebugGuiSystem::DrawPlayerMovementGui(EcsRegistry& registry, Entity entity)
{
	PlayerInputComponent& playerInput =
		registry.GetComponent<PlayerInputComponent>(entity);

	if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat("Move Speed", &playerInput.moveSpeed);
		ImGui::InputFloat("Jump Power", &playerInput.jumpPower);

		if (registry.HasComponent<VelocityComponent>(entity))
		{
			VelocityComponent& velocity = registry.GetComponent<VelocityComponent>(entity);
			ImGui::InputFloat("Max Speed", &velocity.maxSpeed);
		}
	}
}

void DebugGuiSystem::DrawCollisionGui(EcsRegistry& registry, Entity entity)
{
	CollisionComponent& collision = registry.GetComponent<CollisionComponent>(entity);

	if (ImGui::CollapsingHeader("Colliders (3D)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Hit Entities Count : %d",
			static_cast<int>(collision.hitEntities.size()));

		for (size_t index = 0; index < collision.colliders.size(); ++index)
		{
			auto& collider = collision.colliders[index];
			if (collider == nullptr)
			{
				continue;
			}

			ImGui::PushID(static_cast<int>(index));
			ImGui::Separator();

			bool isActive = collider->IsActive();
			if (ImGui::Checkbox("Active", &isActive))
			{
				collider->SetActive(isActive);
			}

			ImGui::SameLine();

			bool isTrigger = collider->IsTrigger();
			if (ImGui::Checkbox("IsTrigger", &isTrigger))
			{
				collider->SetTrigger(isTrigger);
			}

			switch (collider->GetShapeType())
			{
			case ColliderBase::SHAPE::SPHERE:
			{
				auto* sphere = static_cast<ColliderSphere*>(collider.get());
				ImGui::Text("[3D Collider #%d : Sphere]", static_cast<int>(index));

				VECTOR localPos = sphere->GetLocalPosition();
				if (ImGui::DragFloat3("Local Pos", &localPos.x, DRAG_SPEED_POSITION))
				{
					sphere->SetLocalPosition(localPos);
				}

				float radius = sphere->GetRadius();
				if (ImGui::DragFloat("Radius", &radius,
					DRAG_SPEED_SIZE, MIN_COLLIDER_SIZE, MAX_COLLIDER_SIZE))
				{
					sphere->SetRadius(radius);
				}
			}
			break;

			case ColliderBase::SHAPE::CAPSULE:
			{
				auto* capsule = static_cast<ColliderCapsule*>(collider.get());
				ImGui::Text("[3D Collider #%d : Capsule]", static_cast<int>(index));

				VECTOR startPos = capsule->GetLocalStartPos();
				if (ImGui::DragFloat3("Local Start", &startPos.x, DRAG_SPEED_POSITION))
				{
					capsule->SetLocalStartPos(startPos);
				}

				VECTOR endPos = capsule->GetLocalEndPos();
				if (ImGui::DragFloat3("Local End", &endPos.x, DRAG_SPEED_POSITION))
				{
					capsule->SetLocalEndPos(endPos);
				}

				float radius = capsule->GetRadius();
				if (ImGui::DragFloat("Radius", &radius,
					DRAG_SPEED_SIZE, MIN_COLLIDER_SIZE, MAX_COLLIDER_SIZE))
				{
					capsule->SetRadius(radius);
				}
			}
			break;

			case ColliderBase::SHAPE::BOX:
			{
				auto* box = static_cast<ColliderBox*>(collider.get());
				ImGui::Text("[3D Collider #%d : Box]", static_cast<int>(index));

				VECTOR localCenter = box->GetLocalCenter();
				if (ImGui::DragFloat3("Local Center", &localCenter.x, DRAG_SPEED_POSITION))
				{
					box->SetLocalCenter(localCenter);
				}

				VECTOR size = box->GetSize();
				if (ImGui::DragFloat3("Size (W/H/D)", &size.x,
					DRAG_SPEED_SIZE, MIN_COLLIDER_SIZE, MAX_COLLIDER_SIZE))
				{
					box->SetSize(size);
				}
			}
			break;

			case ColliderBase::SHAPE::LINE:
			{
				auto* line = static_cast<ColliderLine*>(collider.get());
				ImGui::Text("[3D Collider #%d : Line]", static_cast<int>(index));

				VECTOR startPos = line->GetLocalStartPos();
				if (ImGui::DragFloat3("Local Start", &startPos.x, DRAG_SPEED_POSITION))
				{
					line->SetLocalStartPos(startPos);
				}

				VECTOR endPos = line->GetLocalEndPos();
				if (ImGui::DragFloat3("Local End", &endPos.x, DRAG_SPEED_POSITION))
				{
					line->SetLocalEndPos(endPos);
				}
			}
			break;

			case ColliderBase::SHAPE::MODEL:
			{
				auto* modelCollider = static_cast<ColliderModel*>(collider.get());
				ImGui::Text("[3D Collider #%d : Model] (Handle: %d)",
					static_cast<int>(index), modelCollider->GetModelHandle());
			}
			break;

			default:
				break;
			}

			ImGui::PopID();
		}
	}
}

void DebugGuiSystem::DrawCollision2DGui(EcsRegistry& registry, Entity entity)
{
	Collision2DComponent& collision2D =
		registry.GetComponent<Collision2DComponent>(entity);

	if (ImGui::CollapsingHeader("Colliders (2D)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Hit 2D Entities Count : %d",
			static_cast<int>(collision2D.hitEntities.size()));

		for (size_t index = 0; index < collision2D.colliders.size(); ++index)
		{
			auto& collider2D = collision2D.colliders[index];
			if (collider2D == nullptr)
			{
				continue;
			}

			ImGui::PushID(static_cast<int>(index));
			ImGui::Separator();

			switch (collider2D->GetShapeType())
			{
			case Collider2DBase::SHAPE_2D::CIRCLE:
			{
				auto* circle2D = static_cast<Collider2DCircle*>(collider2D.get());
				ImGui::Text("[2D Collider #%d : Circle]", static_cast<int>(index));

				Vector2F centerPos = circle2D->GetWorldCenterPos();
				if (ImGui::DragFloat2("Center Pos", &centerPos.x_, DRAG_SPEED_POSITION))
				{
					circle2D->SetCenterPos(centerPos);
				}

				float radius = circle2D->GetRadius();
				if (ImGui::DragFloat("Radius", &radius,
					DRAG_SPEED_SIZE, MIN_COLLIDER_SIZE, MAX_COLLIDER_SIZE))
				{
					circle2D->SetRadius(radius);
				}
			}
			break;

			case Collider2DBase::SHAPE_2D::BOX:
			{
				auto* box2D = static_cast<Collider2DBox*>(collider2D.get());
				ImGui::Text("[2D Collider #%d : Box]", static_cast<int>(index));

				Vector2F centerPos = box2D->GetCenterPos();
				if (ImGui::DragFloat2("Center Pos", &centerPos.x_, DRAG_SPEED_POSITION))
				{
					box2D->SetCenterPos(centerPos);
				}

				float width = box2D->GetWidth();
				if (ImGui::DragFloat("Width", &width,
					DRAG_SPEED_SIZE, MIN_COLLIDER_SIZE, MAX_COLLIDER_SIZE))
				{
					box2D->SetWidth(width);
				}

				float height = box2D->GetHeight();
				if (ImGui::DragFloat("Height", &height,
					DRAG_SPEED_SIZE, MIN_COLLIDER_SIZE, MAX_COLLIDER_SIZE))
				{
					box2D->SetHeight(height);
				}
			}
			break;

			default:
				break;
			}

			ImGui::PopID();
		}
	}
}

void DebugGuiSystem::DrawCameraGui(EcsRegistry& registry, Entity entity)
{
	CameraComponent& cameraData = registry.GetComponent<CameraComponent>(entity);

	if (ImGui::CollapsingHeader("Camera Mode", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int currentMode = (cameraData.mode == CAMERA_MODE::TRACKING) ? 0 : 1;
		if (ImGui::RadioButton("Tracking", &currentMode, 0))
		{
			cameraData.mode = CAMERA_MODE::TRACKING;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Free", &currentMode, 1))
		{
			cameraData.mode = CAMERA_MODE::FREE;
		}
	}

	if (ImGui::CollapsingHeader("Tracking & Rotation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const float SLIDER_MIN_DISTANCE = 50.0f;    // 距離の最小値
		const float SLIDER_MAX_DISTANCE = 3000.0f;  // 距離の最大値
		const float SLIDER_MIN_ROT_SPEED = 0.001f;  // 回転速度の最小値
		const float SLIDER_MAX_ROT_SPEED = 0.2f;    // 回転速度の最大値
		const float SLIDER_MIN_FREE_SPEED = 0.5f;   // フリー移動速度の最小値
		const float SLIDER_MAX_FREE_SPEED = 50.0f;  // フリー移動速度の最大値
		const float SLIDER_MIN_ANGLE_H = -180.0f;   // 水平角度の最小値
		const float SLIDER_MAX_ANGLE_H = 180.0f;    // 水平角度の最大値
		const float SLIDER_MIN_ANGLE_V = -80.0f;    // 垂直角度の最小値
		const float SLIDER_MAX_ANGLE_V = 80.0f;     // 垂直角度の最大値

		if (registry.HasComponent<TransformComponent>(entity))
		{
			TransformComponent& cameraTransform =
				registry.GetComponent<TransformComponent>(entity);
			ImGui::InputFloat3("Camera Pos", &cameraTransform.position.x);
		}

		ImGui::InputFloat3("Target Offset", &cameraData.targetOffset.x);
		ImGui::SliderFloat("Distance", &cameraData.distance,
			SLIDER_MIN_DISTANCE, SLIDER_MAX_DISTANCE);
		ImGui::SliderFloat("Rotation Speed", &cameraData.rotationSpeed,
			SLIDER_MIN_ROT_SPEED, SLIDER_MAX_ROT_SPEED);
		ImGui::SliderFloat("Free Speed", &cameraData.freeMoveSpeed,
			SLIDER_MIN_FREE_SPEED, SLIDER_MAX_FREE_SPEED);

		float angleHDegree = UtilityMath::Rad2DegF(cameraData.angleH);
		if (ImGui::SliderFloat("Angle H(deg)", &angleHDegree,
			SLIDER_MIN_ANGLE_H, SLIDER_MAX_ANGLE_H))
		{
			cameraData.angleH = UtilityMath::Deg2RadF(angleHDegree);
		}

		float angleVDegree = UtilityMath::Rad2DegF(cameraData.angleV);
		if (ImGui::SliderFloat("Angle V(deg)", &angleVDegree,
			SLIDER_MIN_ANGLE_V, SLIDER_MAX_ANGLE_V))
		{
			cameraData.angleV = UtilityMath::Deg2RadF(angleVDegree);
		}
	}

	if (ImGui::CollapsingHeader("Lens & Clip"))
	{
		const float SLIDER_MIN_FOV = 30.0f;  // 視野角の最小値
		const float SLIDER_MAX_FOV = 120.0f; // 視野角の最大値

		float fovDegree = UtilityMath::Rad2DegF(cameraData.fov);
		if (ImGui::SliderFloat("FOV(deg)", &fovDegree, SLIDER_MIN_FOV, SLIDER_MAX_FOV))
		{
			cameraData.fov = UtilityMath::Deg2RadF(fovDegree);
		}

		ImGui::InputFloat("Near Z", &cameraData.nearZ);
		ImGui::InputFloat("Far Z", &cameraData.farZ);
	}
}

void DebugGuiSystem::DrawStatusGui(EcsRegistry& registry, Entity entity)
{
	StatusComponent& status = registry.GetComponent<StatusComponent>(entity);

	if (ImGui::CollapsingHeader("Status & Level", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Level : %d (EXP: %d / %d)",
			status.level, status.currentExperience, status.nextLevelExperience);

		ImGui::InputInt("Current HP", &status.currentHp);
		ImGui::InputInt("Max HP", &status.maxHp);
		ImGui::InputInt("Attack Power", &status.attackPower);
		ImGui::InputInt("Defense Power", &status.defensePower);

		ImGui::Separator();

		if (registry.HasComponent<PlayerInputComponent>(entity))
		{
			ImGui::InputInt("Next Level EXP", &status.nextLevelExperience);
			ImGui::InputInt("Growth HP", &status.growthHp);
			ImGui::InputInt("Growth Attack", &status.growthAttack);
			ImGui::InputInt("Growth Defense", &status.growthDefense);
		}
		else
		{
			ImGui::InputInt("Reward EXP", &status.rewardExperience);
		}
	}
}

void DebugGuiSystem::DrawSaveButtonGui(
	EcsRegistry& registry, Entity entity, const DebugGuiComponent& debugGui)
{
	ImGui::Separator();

	if (debugGui.saveType == DEBUG_SAVE_TYPE::PLAYER)
	{
		if (ImGui::Button("Save Player Data"))
		{
			PlayerBinaryData saveData{};
			if (registry.HasComponent<TransformComponent>(entity))
			{
				saveData.scale = registry.GetComponent<TransformComponent>(entity).scale;
			}
			
			if (registry.HasComponent<PlayerInputComponent>(entity))
			{
				const auto& input = registry.GetComponent<PlayerInputComponent>(entity);
				saveData.moveSpeed = input.moveSpeed;
				saveData.jumpPower = input.jumpPower;
			}
			
			if (registry.HasComponent<VelocityComponent>(entity))
			{
				saveData.maxSpeed =
					registry.GetComponent<VelocityComponent>(entity).maxSpeed;
			}
			
			if (registry.HasComponent<CollisionComponent>(entity))
			{
				const auto& collision = registry.GetComponent<CollisionComponent>(entity);
				for (const auto& collider : collision.colliders)
				{
					if (collider == nullptr)
					{
						continue;
					}

					if (collider->GetShapeType() == ColliderBase::SHAPE::CAPSULE)
					{
						auto* capsule = static_cast<ColliderCapsule*>(collider.get());
						saveData.capsuleStartPos = capsule->GetLocalStartPos();
						saveData.capsuleEndPos = capsule->GetLocalEndPos();
						saveData.capsuleRadius = capsule->GetRadius();
					}
					else if (collider->GetShapeType() == ColliderBase::SHAPE::LINE)
					{
						auto* line = static_cast<ColliderLine*>(collider.get());
						saveData.rayStartPos = line->GetLocalStartPos();
						saveData.rayEndPos = line->GetLocalEndPos();
					}
				}
			}

			if (registry.HasComponent<StatusComponent>(entity))
			{
				const auto& status = registry.GetComponent<StatusComponent>(entity);
				saveData.maxHp = status.maxHp;
				saveData.attackPower = status.attackPower;
				saveData.defensePower = status.defensePower;
				saveData.nextLevelExperience = status.nextLevelExperience;
				saveData.growthHp = status.growthHp;
				saveData.growthAttack = status.growthAttack;
				saveData.growthDefense = status.growthDefense;
			}

			ActorDataLoader::SavePlayerData(
				debugGui.csvFilePath, debugGui.binaryFilePath, saveData);
		}
	}
	else if (debugGui.saveType == DEBUG_SAVE_TYPE::ENEMY)
	{
		if (ImGui::Button("Save Enemy Data"))
		{
			EnemyBinaryData saveData{};

			// スケールの取得
			if (registry.HasComponent<TransformComponent>(entity))
			{
				saveData.scale = registry.GetComponent<TransformComponent>(entity).scale;
			}

			// コライダーの座標・サイズの取得
			if (registry.HasComponent<CollisionComponent>(entity))
			{
				const auto& collision = registry.GetComponent<CollisionComponent>(entity);
				for (const auto& collider : collision.colliders)
				{
					if (collider == nullptr)
					{
						continue;
					}
					if (collider->GetShapeType() == ColliderBase::SHAPE::CAPSULE)
					{
						auto* capsule = static_cast<ColliderCapsule*>(collider.get());
						saveData.capsuleStartPos = capsule->GetLocalStartPos();
						saveData.capsuleEndPos = capsule->GetLocalEndPos();
						saveData.capsuleRadius = capsule->GetRadius();
					}
					else if (collider->GetShapeType() == ColliderBase::SHAPE::LINE)
					{
						auto* line = static_cast<ColliderLine*>(collider.get());
						saveData.rayStartPos = line->GetLocalStartPos();
						saveData.rayEndPos = line->GetLocalEndPos();
					}
				}
			}

			// ステータスの取得
			if (registry.HasComponent<StatusComponent>(entity))
			{
				const auto& status = registry.GetComponent<StatusComponent>(entity);
				saveData.maxHp = status.maxHp;
				saveData.attackPower = status.attackPower;
				saveData.defensePower = status.defensePower;
				saveData.rewardExperience = status.rewardExperience;
			}

			// CSVとバイナリに書き出し
			ActorDataLoader::SaveEnemyData(
				debugGui.csvFilePath, debugGui.binaryFilePath, saveData);
		}
	else if (debugGui.saveType == DEBUG_SAVE_TYPE::CAMERA)
	{
		if (ImGui::Button("Save Camera Data"))
		{
			if (registry.HasComponent<CameraComponent>(entity))
			{
				const auto& cameraData = registry.GetComponent<CameraComponent>(entity);
				CameraBinaryData saveData{};
				saveData.targetOffset = cameraData.targetOffset;
				saveData.distance = cameraData.distance;
				saveData.angleHorizontal = cameraData.angleH;
				saveData.angleVertical = cameraData.angleV;
				saveData.rotationSpeed = cameraData.rotationSpeed;
				saveData.freeMoveSpeed = cameraData.freeMoveSpeed;
				saveData.fov = cameraData.fov;
				saveData.nearZ = cameraData.nearZ;
				saveData.farZ = cameraData.farZ;

				ActorDataLoader::SaveCameraData(
					debugGui.csvFilePath, debugGui.binaryFilePath, saveData);
			}
		}
	}
	else if (debugGui.saveType == DEBUG_SAVE_TYPE::STAGE)
	{
		if (ImGui::Button("Save Stage Layout (All Stage Objects)"))
		{
			LoadedStageData saveStageData{};

			// 既存のスポーン座標が消えないように読み込んで引き継ぐ
			LoadedStageData existingData{};
			if (StageDataLoader::LoadFromBinary(debugGui.binaryFilePath, existingData))
			{
				saveStageData.playerSpawnPosition = existingData.playerSpawnPosition;
			}

			auto allGuiEntities = registry.GetEntitiesWith<DebugGuiComponent>();
			std::vector<Entity> stageEntitiesToSave;

			for (Entity targetEntity : allGuiEntities)
			{
				const auto& targetGui = registry.GetComponent<DebugGuiComponent>(targetEntity);
				if (targetGui.saveType == DEBUG_SAVE_TYPE::STAGE &&
					targetGui.csvFilePath == debugGui.csvFilePath &&
					registry.HasComponent<TransformComponent>(targetEntity))
				{
					stageEntitiesToSave.push_back(targetEntity);
				}
			}

			for (Entity targetEntity : stageEntitiesToSave)
			{
				const auto& targetGui = registry.GetComponent<DebugGuiComponent>(targetEntity);
				const auto& transform = registry.GetComponent<TransformComponent>(targetEntity);

				StageObjectBinaryData objectData{};
				wcsncpy_s(objectData.resourceKey, targetGui.stageResourceKey.c_str(), MAX_RESOURCE_KEY_LENGTH - 1);

				objectData.position = transform.localPosition;
				objectData.rotation = transform.rotationEuler;
				objectData.scale = transform.scale;
				objectData.parentIndex = -1;

				if (registry.HasComponent<HierarchyComponent>(targetEntity))
				{
					Entity parentEntity = registry.GetComponent<HierarchyComponent>(targetEntity).parent;
					if (parentEntity != NULL_ENTITY)
					{
						auto it = std::find(stageEntitiesToSave.begin(), stageEntitiesToSave.end(), parentEntity);
						if (it != stageEntitiesToSave.end())
						{
							objectData.parentIndex = static_cast<int>(std::distance(stageEntitiesToSave.begin(), it));
						}
					}
				}

				// ボックスコライダーの情報を保存データに反映
				if (registry.HasComponent<CollisionComponent>(targetEntity))
				{
					const auto& collision = registry.GetComponent<CollisionComponent>(targetEntity);
					for (const auto& collider : collision.colliders)
					{
						if (collider != nullptr &&
							collider->GetShapeType() == ColliderBase::SHAPE::BOX)
						{
							auto* box = static_cast<ColliderBox*>(collider.get());
							objectData.boxCenter = box->GetLocalCenter();
							objectData.boxSize = box->GetSize();
							break;
						}
					}
				}

				saveStageData.objects.push_back(objectData);
			}

			StageDataLoader::SaveStageData(debugGui.csvFilePath, debugGui.binaryFilePath, saveStageData);
		}
	}
}
