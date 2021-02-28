#include "pch.h"
#include "EntityPropertyPanel.h"

#include "Core/ImGui/ImGuiLayer.h"

#include "Core/Entity/Components.h"

#include "Core/Entity/Light/ILight.h"

#include "Core/Entity/Camera/CameraController.h"
#include "Core/Input/InputManager.h"
#include "Core/Entity/Camera/CameraManager.h"

#include "Core/PhysicEngine/PhysicsEngine.h"
#include "Core/Event/EventEntity.h"

using namespace DirectX;

namespace TruthEngine
{


	template<class T, typename UIFunction>
	static void DrawComponent(const char* name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name);
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				//entity.RemoveComponent<T>();
			}
		}
	}


	void EntityPropertyPanel::OnImGuiRender()
	{
		if (!m_Context)
			return;

		DrawImGuizmo();

		DrawTagComponent(m_Context.GetComponent<TagComponent>());
		if (m_Context.HasComponent<TransformComponent>())
		{
			DrawTransformComponent(m_Context.GetComponent<TransformComponent>());
		}
		if (m_Context.HasComponent<MaterialComponent>())
		{
			DrawMaterialComponent(m_Context.GetComponent<MaterialComponent>());
		}
		if (m_Context.HasComponent<CameraComponent>())
		{
			DrawCameraComponent(m_Context.GetComponent<CameraComponent>());
		}
		if (m_Context.HasComponent<LightComponent>())
		{
			DrawLightComponent(m_Context.GetComponent<LightComponent>());
		}
		if (m_Context.HasComponent<MeshComponent>())
		{
			DrawPhysicComponent();
		}
	}


	void EntityPropertyPanel::DrawTagComponent(TagComponent& component)
	{
		char tag[50];
		memset(tag, 0, sizeof(tag));
		std::strcpy(tag, component.GetTag().c_str());
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, ImVec4{ 0.5, .5, .8, .8 });
		if (ImGui::InputText("##tagcomponent", tag, sizeof(tag)))
		{
			component.SetTag(tag);
		}
		ImGui::PopStyleColor();
		ImGui::PopItemWidth();
	}


	inline void EntityPropertyPanel::DrawTransformComponent(TransformComponent& component)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (ImGui::TreeNodeEx("Transform"))
		{
			auto& transform = component.GetTransform();

			//ImGui::DragFloat3("Transform", &transform._41, 1.0f, -1000.0f, 1000.0f, nullptr, 1.0f);
			ImGuiLayer::DrawFloat3Control("Transform", &transform._41);

			ImGui::TreePop();
		}

	}


	inline void EntityPropertyPanel::DrawMaterialComponent(MaterialComponent& component)
	{

		DrawComponent<MaterialComponent>("Material", m_Context, [](MaterialComponent& component)
			{

				auto material = component.GetMaterial();

				{
					auto diffuseColor = material->GetColorDiffuse();

					//if (ImGui::DragFloat4("Diffuse Color", &diffuseColor.x, 0.01f, 0.0f, 1.0f, nullptr, 1.0f))
					ImGui::Text("DiffuseColor: ");
					if (ImGui::ColorEdit4("##materialdiffusecolor", &diffuseColor.x, ImGuiColorEditFlags_Float))
					{
						material->SetColorDiffuse(diffuseColor);
					}
				}

				{
					auto r0 = material->GetFresnelR0().x;

					if (ImGui::DragFloat("FresnelR0", &r0, 0.01f, 0.0f, 1.0f, nullptr, 1.0f))
					{
						material->SetFresnelR0(float3{ r0, r0, r0 });
					}
				}

				{
					auto shininess = material->GetShininess();

					if (ImGui::DragFloat("Shininess", &shininess, 0.1f, 0.0f, 100.0f, nullptr, 1.0f))
					{
						material->SetShininess(shininess);
					}
				}

				{
					auto uvScale = material->GetUVScale();
					static bool s_chainedUVScale = true;
					if (ImGuiLayer::DrawFloat2ControlUV("UV Scale", &uvScale, s_chainedUVScale))
					{
						material->SetUVScale(uvScale);
					}
					ImGui::SameLine();
					ImGui::Checkbox("Chained##uvScale", &s_chainedUVScale);
				}

				{
					static bool s_chainedUVTranslate = true;
					auto uvTranslate = material->GetUVTranslate();
					if (ImGuiLayer::DrawFloat2ControlUV("UV Translate", &uvTranslate, s_chainedUVTranslate))
					{
						material->SetUVTranslate(uvTranslate);
					}
					ImGui::SameLine();
					ImGui::Checkbox("Chained##uvScale", &s_chainedUVTranslate);
				}

				{
					ImGui::SetNextItemWidth(-100);
					ImGui::Text("Diffuse Texture: ");

					ImGui::SameLine();

					uint32_t diffuseIndex = material->GetMapIndexDiffuse();

					if (diffuseIndex == -1)
					{
						ImGui::Button("None");
					}
					else
					{
						if (ImGui::Button("Show##diffuseTexture"))
						{
							ImGui::OpenPopup("showdiffuseTexturepopup");
						}

						if (ImGui::BeginPopup("showdiffuseTexturepopup"))
						{
							TEImGuiRenderImage_MaterialTexture(diffuseIndex, float2{ 150.0f, 150.0f });
							ImGui::EndPopup();
						}
					}

					ImGui::SameLine();
					if (ImGui::Button("Pick Texture##pickdiffuseMap"))
					{
						auto func = [material](uint32_t _diffuseMapIndex)
						{
							material->SetMapIndexDiffuse(_diffuseMapIndex);
						};

						ImGuiLayer::ShowWindowMaterialTexture(func, true);
					}
					ImGui::SameLine();
					if (ImGui::Button("Clear##diffuseMap"))
					{
						material->SetMapIndexDiffuse(-1);
					}

				}


				{

					ImGui::SetNextItemWidth(-100);
					ImGui::Text("Normal Texture: ");

					ImGui::SameLine();

					uint32_t normalIndex = material->GetMapIndexNormal();

					if (normalIndex == -1)
					{
						ImGui::Button("None");
					}
					else
					{
						if (ImGui::Button("Show##normalTexture"))
						{
							ImGui::OpenPopup("shownormalTexturepopup");
						}

						if (ImGui::BeginPopup("shownormalTexturepopup"))
						{
							TEImGuiRenderImage_MaterialTexture(normalIndex, float2{ 150.0f, 150.0f });
							ImGui::EndPopup();
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Pick Texture##picknormalMap"))
					{
						auto func = [material](uint32_t _normalMapIndex)
						{
							material->SetMapIndexNormal(_normalMapIndex);
						};

						ImGuiLayer::ShowWindowMaterialTexture(func, true);
					}
					ImGui::SameLine();
					if (ImGui::Button("Clear##normalMap"))
					{
						ImGui::SameLine();
						material->SetMapIndexNormal(-1);
					}

				}


			});

	}


	inline void EntityPropertyPanel::DrawCameraComponent(CameraComponent& component)
	{
		DrawComponent<CameraComponent>("Camera", m_Context, [](CameraComponent& component)
			{

				auto camera = component.GetCamera();

				bool activeCamera = CameraManager::GetInstance()->GetActiveCamera() == camera;
				if (ImGui::Checkbox("Active", &activeCamera))
				{
					CameraManager::GetInstance()->SetActiveCamera(camera);
				}

				{
					auto position = camera->GetPosition();

					//if (ImGui::DragFloat4("Diffuse Color", &diffuseColor.x, 0.01f, 0.0f, 1.0f, nullptr, 1.0f))
					if (ImGuiLayer::DrawFloat3Control("Camera Position", &position.x))
					{
						camera->SetPosition(position);
					}
				}

				{
					auto look = camera->GetLook();

					if (ImGuiLayer::DrawFloat3Control("Camera Direction", &look.x, -1.0f, 1.0f, 0.01f))
					{
						camera->SetLook(look);
					}
				}

				{
					auto zNearPlane = camera->GetZNearPlane();

					ImGui::Text("Frustum Near Plane Distance: ");
					if (ImGui::DragFloat("##cameraznearplane", &zNearPlane, 0.1f, 1.0f, camera->GetZFarPlane(), "%.3f", 1.0f))
					{
						camera->SetZNearPlane(zNearPlane);
					}
				}

				{
					auto zFarPlane = camera->GetZFarPlane();

					ImGui::Text("Frustum Far Plane Distance: ");
					if (ImGui::DragFloat("##camerazfarplane", &zFarPlane, 0.1f, camera->GetZNearPlane(), 10000.0f, "%.3f", 1.0f))
					{
						camera->SetZFarPlane(zFarPlane);
					}

				}

				if (component.HasCameraController())
				{
					auto cameraController = component.GetCameraController();
					auto controllerSpeed = cameraController->GetSpeed();

					ImGui::Text("Camera Update Speed: ");
					if (ImGui::DragFloat("##cameraspeed", &controllerSpeed, 0.1f, 0.001f, 100.0f, "%.3f", 1.0f))
					{
						cameraController->SetSpeed(controllerSpeed);
					}
				}
			});
	}


	inline void EntityPropertyPanel::DrawLightComponent(LightComponent& component)
	{
		DrawComponent<LightComponent>("Light", m_Context, [](LightComponent& component)
			{


				auto light = component.GetLight();

				{
					auto position = light->GetPosition();

					//ImGui::Text("Light Position: ");
					//if (ImGui::DragFloat3("##lightpostition", &position.x))
					if (ImGuiLayer::DrawFloat3Control("Light Position", &position.x))
					{
						light->SetPosition(position);
					}
				}

				{
					auto direction = light->GetDirection();

					//ImGui::Text("Light Direction: ");
					//if (ImGui::DragFloat3("##lightdirection", &direction.x, 0.01, -1.0f, 1.0f, "%.3f", 1.0f))
					if (ImGuiLayer::DrawFloat3Control("Light Direction", &direction.x, -1.0f, 1.0f, 0.01f))
					{
						light->SetDirection(direction);
					}
				}

				{
					auto diffuseColor = light->GetDiffuseColor();

					ImGui::Text("Light Diffuse Color: ");
					if (ImGui::ColorEdit4("##lightdiffusecolor", &diffuseColor.x, ImGuiColorEditFlags_Float))
					{
						light->SetDiffuseColor(diffuseColor);
					}
				}

				{
					auto ambientColor = light->GetAmbientColor();

					ImGui::Text("Light Ambient Color: ");
					if (ImGui::ColorEdit4("##lightambientcolor", &ambientColor.x, ImGuiColorEditFlags_Float))
					{
						light->SetAmbientColor(ambientColor);
					}
				}

				{
					auto specularColor = light->GetSpecularColor();

					ImGui::Text("Light Specular Color: ");
					if (ImGui::ColorEdit4("##lightspecularcolor", &specularColor.x, ImGuiColorEditFlags_Float))
					{
						light->SetSpecularColor(specularColor);
					}
				}
			});
	}



	void EntityPropertyPanel::DrawImGuizmo()
	{
		//
		////ImGuizmo Rendering
		//
		if (m_Context.HasComponent<TransformComponent>())
		{
			static ImGuizmo::OPERATION _operationMode(ImGuizmo::TRANSLATE);
			static ImGuizmo::MODE _currentGizmoMode(ImGuizmo::LOCAL);

			if (ImGui::Begin("Transform Space"))
			{

				if (ImGui::RadioButton("Local", _currentGizmoMode == ImGuizmo::LOCAL))
				{
					_currentGizmoMode = ImGuizmo::LOCAL;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("World", _currentGizmoMode == ImGuizmo::WORLD))
				{
					_currentGizmoMode = ImGuizmo::WORLD;
				}

				ImGui::End();
			}


			if (InputManager::IsKeyPressed('E'))
			{
				_operationMode = ImGuizmo::SCALE;
			}
			if (InputManager::IsKeyPressed('R'))
			{
				_operationMode = ImGuizmo::ROTATE;
			}
			if (InputManager::IsKeyPressed('T'))
			{
				_operationMode = ImGuizmo::TRANSLATE;
			}

			auto activeCamera = CameraManager::GetInstance()->GetActiveCamera();

			auto& _transform = m_Context.GetComponent<TransformComponent>().GetTransform();

			static auto s_CopyingMesh = false;


			if (ImGuizmo::Manipulate(&activeCamera->GetView()._11, &activeCamera->GetProjection()._11, _operationMode, _currentGizmoMode, &_transform._11, nullptr))
			{
				if (InputManager::IsKeyPressed(VK_SHIFT) && !s_CopyingMesh)
				{
					m_Context.GetScene()->CopyMeshEntity(m_Context);
					s_CopyingMesh = true;
				}

				EventEntityTransform _eventEntityTransform{ m_Context, ETransformType::Scale & ETransformType::Translate };
				TE_INSTANCE_APPLICATION->OnEvent(_eventEntityTransform);
			}

			if (!ImGui::IsMouseDragging(0) && !InputManager::IsKeyPressed(VK_SHIFT))
			{
				s_CopyingMesh = false;
			}


		}
	}


	void EntityPropertyPanel::DrawPhysicComponent()
	{

		bool hasPhysicsComponent = m_Context.HasComponent<PhysicsStaticComponent>();

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx("Physics", treeNodeFlags);
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("...##physicsComponentPopup", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings##physicsComponent");
		}

		bool removeComponent = false;
		bool openAddPhysicsComponent = false;
		if (ImGui::BeginPopup("ComponentSettings##physicsComponent"))
		{
			if (hasPhysicsComponent)
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;
			}
			else
			{
				if (ImGui::MenuItem("Add Component"))
				{
					openAddPhysicsComponent = true;
				}
			}
			ImGui::EndPopup();
		}

		if (openAddPhysicsComponent)
		{
			ImGui::OpenPopup("Setup Physic Component");
		}

		if (ImGui::BeginPopupModal("Setup Physic Component"))
		{
			static TE_PHYSICS_RIGID_TYPE rigidType = TE_PHYSICS_RIGID_TYPE::STATIC;
			static TE_PHYSICS_RIGID_SHAPE rigidshape = TE_PHYSICS_RIGID_SHAPE::BOX;

			ImGui::Text("Static Rigids");
			if (ImGui::RadioButton("Plane", rigidshape == TE_PHYSICS_RIGID_SHAPE::PLANE && rigidType == TE_PHYSICS_RIGID_TYPE::STATIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::STATIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::PLANE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Box##0", rigidshape == TE_PHYSICS_RIGID_SHAPE::BOX && rigidType == TE_PHYSICS_RIGID_TYPE::STATIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::STATIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::BOX;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Sphere##0", rigidshape == TE_PHYSICS_RIGID_SHAPE::SPHERE && rigidType == TE_PHYSICS_RIGID_TYPE::STATIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::STATIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::SPHERE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Convex##0", rigidshape == TE_PHYSICS_RIGID_SHAPE::CONVEX && rigidType == TE_PHYSICS_RIGID_TYPE::STATIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::STATIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::CONVEX;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("TriangledMesh##0", rigidshape == TE_PHYSICS_RIGID_SHAPE::TRIANGLED && rigidType == TE_PHYSICS_RIGID_TYPE::STATIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::STATIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::TRIANGLED;
			}

			ImGui::Text("Dynamic Rigids");
			if (ImGui::RadioButton("Box##1", rigidshape == TE_PHYSICS_RIGID_SHAPE::BOX && rigidType == TE_PHYSICS_RIGID_TYPE::DYNAMIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::DYNAMIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::BOX;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Sphere##1", rigidshape == TE_PHYSICS_RIGID_SHAPE::SPHERE && rigidType == TE_PHYSICS_RIGID_TYPE::DYNAMIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::DYNAMIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::SPHERE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Convex##1", rigidshape == TE_PHYSICS_RIGID_SHAPE::CONVEX && rigidType == TE_PHYSICS_RIGID_TYPE::DYNAMIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::DYNAMIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::CONVEX;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("TriangledMesh##1", rigidshape == TE_PHYSICS_RIGID_SHAPE::TRIANGLED && rigidType == TE_PHYSICS_RIGID_TYPE::DYNAMIC))
			{
				rigidType = TE_PHYSICS_RIGID_TYPE::DYNAMIC;
				rigidshape = TE_PHYSICS_RIGID_SHAPE::TRIANGLED;
			}

			static TEPhysicsRigidDesc _rigidDesc(0.5f, 0.5f, 0.5f, IdentityMatrix);

			ImGui::Text("Rigid Body Properties");
			ImGui::DragFloat("Static Friction", &_rigidDesc.mStaticFriction, 0.01f, 0.0f, 1.0f, "%0.2f", 1.0f);
			ImGui::DragFloat("Dynamic Friction", &_rigidDesc.mDynamicFriction, 0.01f, 0.0f, 1.0f, "%0.2f", 1.0f);
			ImGui::DragFloat("Restitution", &_rigidDesc.mRestitution, 0.01f, 0.0f, 1.0f, "%0.2f", 1.0f);


			if (ImGui::Button("Create"))
			{
				AddPhysicsComponent(rigidType, rigidshape, _rigidDesc);

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (open)
		{

			if (hasPhysicsComponent)
			{

			}
			else if (hasPhysicsComponent)
			{

			}
			


			ImGui::TreePop();
		}



	}


	void EntityPropertyPanel::AddPhysicsComponent(TE_PHYSICS_RIGID_TYPE rigidType, TE_PHYSICS_RIGID_SHAPE rigidshape, TEPhysicsRigidDesc& rigidDesc)
	{
		auto physicsEngine = TE_INSTANCE_PHYSICSENGINE;


		rigidDesc.mTransform = m_Context.GetComponent<TransformComponent>().GetTransform();

		switch (rigidshape)
		{
		case TruthEngine::TE_PHYSICS_RIGID_SHAPE::PLANE:
		{
			TEPhysicsRigidPlaneDesc desc(.0f, 1.0f, .0f, 0.0, rigidDesc);
			switch (rigidType)
			{
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::STATIC:
				physicsEngine->AddRigidStaticPlane(desc, m_Context);
				break;
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::DYNAMIC:
				break;
			}
			break;
		}
		case TruthEngine::TE_PHYSICS_RIGID_SHAPE::BOX:
		{
			const auto& aabb = m_Context.GetComponent<BoundingBoxComponent>().GetBoundingBox();
			auto halfExtents = aabb.Extents;
			TEPhysicsRigidBoxDesc desc(halfExtents, rigidDesc);
			switch (rigidType)
			{
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::STATIC:
				physicsEngine->AddRigidStaticBox(desc, m_Context);
				break;
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::DYNAMIC:
				physicsEngine->AddRigidDynamicBox(desc, m_Context);
				break;
			}
			break;
		}
		case TruthEngine::TE_PHYSICS_RIGID_SHAPE::SPHERE:
		{
			const auto& aabb = m_Context.GetComponent<BoundingBoxComponent>().GetBoundingBox();
			auto halfExtents = aabb.Extents;
			TEPhysicsRigidSphereDesc desc(halfExtents.x, rigidDesc);
			switch (rigidType)
			{
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::STATIC:
				physicsEngine->AddRigidStaticSphere(desc, m_Context);
				break;
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::DYNAMIC:
				physicsEngine->AddRigidDynamicSphere(desc, m_Context);
				break;
			}
			break;
		}
		case TruthEngine::TE_PHYSICS_RIGID_SHAPE::CONVEX:
			break;
		case TruthEngine::TE_PHYSICS_RIGID_SHAPE::TRIANGLED:
		{
			auto mesh = m_Context.GetComponent<MeshComponent>().GetMesh();
			auto rigidTriangleMeshDesc = TEPhysicsRigidTriangleMeshDesc(mesh->GetVertexNum()
				, (void*)mesh->GetVertexBuffer()->GetVertexData<VertexData::Pos>().data()
				, sizeof(VertexData::Pos)
				, mesh->GetIndexNum() / 3
				, (void*)mesh->GetIndexBuffer()->GetIndecies().data()
				, rigidDesc);
			rigidDesc.mTransform = m_Context.GetComponent<TransformComponent>().GetTransform();
			switch (rigidType)
			{
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::STATIC:
				physicsEngine->AddRigidStaticTriangleMesh(rigidTriangleMeshDesc, m_Context);
				break;
			case TruthEngine::TE_PHYSICS_RIGID_TYPE::DYNAMIC:
				physicsEngine->AddRigidDynamicTriangleMesh(rigidTriangleMeshDesc, m_Context);
				break;
			}
			break;
		}
		default:
			break;
		}
	}

}