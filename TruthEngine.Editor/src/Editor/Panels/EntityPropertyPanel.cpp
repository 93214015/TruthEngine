#include "pch.h"
#include "EntityPropertyPanel.h"

#include "Core/ImGui/ImGuiLayer.h"

#include "Core/Entity/Components.h"

#include "Core/Entity/Light/ILight.h"

#include "Core/Entity/Camera/CameraController.h"
#include "Core/Input/InputManager.h"
#include "Core/Entity/Camera/CameraManager.h"

#include "Core/PhysicEngine/PhysicsEngine.h"

namespace TruthEngine
{

	using namespace Core;




	template<class T, typename UIFunction>
	static void DrawComponent(const char* name, Core::Entity entity, UIFunction uiFunction)
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


	void EntityPropertyPanel::DrawTagComponent(Core::TagComponent& component)
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


	inline void EntityPropertyPanel::DrawTransformComponent(Core::TransformComponent& component)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (ImGui::TreeNodeEx("Transform"))
		{
			auto& transform = component.GetTransform();

			//ImGui::DragFloat3("Transform", &transform._41, 1.0f, -1000.0f, 1000.0f, nullptr, 1.0f);
			Core::ImGuiLayer::DrawFloat3Control("Transform", &transform._41);

			ImGui::TreePop();
		}

	}


	inline void EntityPropertyPanel::DrawMaterialComponent(Core::MaterialComponent& component)
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
					ImGui::SetNextItemWidth(-100);
					ImGui::Text("Diffuse Texture: ");
					
					ImGui::SameLine();

					uint32_t diffuseIndex = material->GetMapIndexDiffuse();

					if ( diffuseIndex == -1)
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
							TEImGuiRenderImage_MaterialTexture(diffuseIndex, float2{150.0f, 150.0f});
							ImGui::EndPopup();
						}
					}

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

					if (ImGui::Button("Clear##normalMap"))
					{
						ImGui::SameLine();
						material->SetMapIndexNormal(-1);
					}

				}


			});

	}


	inline void EntityPropertyPanel::DrawCameraComponent(Core::CameraComponent& component)
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
					if (Core::ImGuiLayer::DrawFloat3Control("Camera Position", &position.x))
					{
						camera->SetPosition(position);
					}
				}

				{
					auto look = camera->GetLook();

					if (Core::ImGuiLayer::DrawFloat3Control("Camera Direction", &look.x, -1.0f, 1.0f, 0.01f))
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
					if (ImGui::DragFloat("##camerazfarplane", &zFarPlane, 0.1f, 1.0f, camera->GetZFarPlane(), "%.3f", 1.0f))
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


	inline void EntityPropertyPanel::DrawLightComponent(Core::LightComponent& component)
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

			auto activeCamera = Core::CameraManager::GetInstance()->GetActiveCamera();

			auto& transform = m_Context.GetComponent<TransformComponent>().GetTransform();

			ImGuizmo::Manipulate(&activeCamera->GetView()._11, &activeCamera->GetProjection()._11, _operationMode, _currentGizmoMode, &transform._11, nullptr);


			/*auto& transform = selectedEntity.GetComponent<TransformComponent>().GetTransform();

			auto& bb = selectedEntity.GetComponent<Core::BoundingBoxComponent>().GetBoundingBox();

			auto bbTranslate = DirectX::XMMatrixTranslation(bb.Center.x, bb.Center.y, bb.Center.z);

			float4x4 t;
			XMStoreFloat4x4(&t, XMMatrixMultiply(DirectX::XMLoadFloat4x4(&transform), bbTranslate));

			float4x4 deltaMatrix;

			if (ImGuizmo::Manipulate(&activeCamera->GetView()._11, &activeCamera->GetProjection()._11, operationMode, ImGuizmo::MODE::WORLD, &t._11, &deltaMatrix._11))
			{
				transform = deltaMatrix * transform;
			}*/

		}
	}


	void EntityPropertyPanel::DrawPhysicComponent()
	{
		if (m_Context.HasComponent<PhysicsStaticComponent>())
		{

		}
		else if (m_Context.HasComponent<PhysicsDynamicComponent>())
		{

		}
		else
		{
			if (ImGui::Button("Add Physic Component"))
			{
				ImGui::OpenPopup("Setup Physic Component");
			}

			if (ImGui::BeginPopupModal("Setup Physic Component"))
			{
				static TE_PHYSICS_RIGID_TYPE rigidType = Core::TE_PHYSICS_RIGID_TYPE::STATIC;
				static TE_PHYSICS_RIGID_SHAPE rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::BOX;

				ImGui::Text("Static Rigids");
				if (ImGui::RadioButton("Plane", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::PLANE && rigidType == Core::TE_PHYSICS_RIGID_TYPE::STATIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::STATIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::PLANE;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Box##0", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::BOX && rigidType == Core::TE_PHYSICS_RIGID_TYPE::STATIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::STATIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::BOX;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Sphere##0", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::SPHERE && rigidType == Core::TE_PHYSICS_RIGID_TYPE::STATIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::STATIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::SPHERE;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Convex##0", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::CONVEX && rigidType == Core::TE_PHYSICS_RIGID_TYPE::STATIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::STATIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::CONVEX;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("TriangledMesh##0", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::TRIANGLED && rigidType == Core::TE_PHYSICS_RIGID_TYPE::STATIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::STATIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::TRIANGLED;
				}

				ImGui::Text("Dynamic Rigids");
				if (ImGui::RadioButton("Box##1", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::BOX && rigidType == Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::BOX;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Sphere##1", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::SPHERE && rigidType == Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::SPHERE;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Convex##1", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::CONVEX && rigidType == Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::CONVEX;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("TriangledMesh##1", rigidshape == Core::TE_PHYSICS_RIGID_SHAPE::TRIANGLED && rigidType == Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC))
				{
					rigidType = Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC;
					rigidshape = Core::TE_PHYSICS_RIGID_SHAPE::TRIANGLED;
				}


				if (ImGui::Button("Create"))
				{
					AddPhysicsComponent(rigidType, rigidshape);

					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

		}
	}


	void EntityPropertyPanel::AddPhysicsComponent(TE_PHYSICS_RIGID_TYPE rigidType, TE_PHYSICS_RIGID_SHAPE rigidshape)
	{
		auto physicsEngine = TE_INSTANCE_PHYSICSENGINE;

		switch (rigidshape)
		{
		case TruthEngine::Core::TE_PHYSICS_RIGID_SHAPE::PLANE:
		{
			TEPhysicsRigidDesc rigidDesc(.5f, .5f, 0.0f, IdentityMatrix);
			TEPhysicsRigidPlaneDesc desc(.0f, 1.0f, .0f, 0.0, rigidDesc);
			switch (rigidType)
			{
			case TruthEngine::Core::TE_PHYSICS_RIGID_TYPE::STATIC:
				m_Context.AddComponent<PhysicsStaticComponent>(physicsEngine->AddRigidStaticPlane(desc, m_Context));
				break;
			case TruthEngine::Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC:
				break;
			}
			break;
		}
		case TruthEngine::Core::TE_PHYSICS_RIGID_SHAPE::BOX:
		{
			auto& transform = m_Context.GetComponent<TransformComponent>().GetTransform();
			auto& aabb = m_Context.GetComponent<BoundingBoxComponent>().GetBoundingBox();
			auto halfExtents = aabb.Extents;
			TEPhysicsRigidDesc rigidDesc(.5f, .5f, 0.0f, transform);
			TEPhysicsRigidBoxDesc desc(halfExtents.x, halfExtents.y, halfExtents.z, rigidDesc);
			switch (rigidType)
			{
			case TruthEngine::Core::TE_PHYSICS_RIGID_TYPE::STATIC:
				m_Context.AddComponent<PhysicsStaticComponent>(physicsEngine->AddRigidStaticBox(desc, m_Context));
				break;
			case TruthEngine::Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC:
				m_Context.AddComponent<PhysicsDynamicComponent>(physicsEngine->AddRigidDynamicBox(desc, m_Context));
				break;
			}
			break;
		}
		case TruthEngine::Core::TE_PHYSICS_RIGID_SHAPE::SPHERE:
		{
			auto& transform = m_Context.GetComponent<TransformComponent>().GetTransform();
			auto& aabb = m_Context.GetComponent<BoundingBoxComponent>().GetBoundingBox();
			auto halfExtents = aabb.Extents;
			TEPhysicsRigidDesc rigidDesc(.5f, .5f, 0.0f, transform);
			TEPhysicsRigidSphereDesc desc(halfExtents.x, rigidDesc);
			switch (rigidType)
			{
			case TruthEngine::Core::TE_PHYSICS_RIGID_TYPE::STATIC:
				m_Context.AddComponent<PhysicsStaticComponent>(physicsEngine->AddRigidStaticSphere(desc, m_Context));
				break;
			case TruthEngine::Core::TE_PHYSICS_RIGID_TYPE::DYNAMIC:
				m_Context.AddComponent<PhysicsDynamicComponent>(physicsEngine->AddRigidDynamicSphere(desc, m_Context));
				break;
			}
			break;
		}
		case TruthEngine::Core::TE_PHYSICS_RIGID_SHAPE::CONVEX:
			break;
		case TruthEngine::Core::TE_PHYSICS_RIGID_SHAPE::TRIANGLED:
			break;
		default:
			break;
		}
	}

}