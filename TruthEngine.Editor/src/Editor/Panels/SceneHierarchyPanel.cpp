#include "pch.h"
#include "SceneHierarchyPanel.h"

#include "Core/Entity/Scene.h"
#include "Core/Entity/Components.h"

#include "Core/Entity/Camera/CameraManager.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Application.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Entity/Light/LightManager.h"

using namespace TruthEngine;

namespace TruthEngine
{
	SceneHierarchyPanel::SceneHierarchyPanel(Scene* context)
		: m_Context(context)
	{
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(.9f, .15f, .4, .8));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(.9f, .3f, .4, 1));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(.9f, .5f, .4, 1));
		if (ImGui::Button("DeSelect", ImVec2{ 60.0, 20.0 }))
		{
			m_Context->ClearSelectedEntity();
		}
		if (auto _selectedEntity = m_Context->GetSelectedEntity(); _selectedEntity)
		{
			ImGui::Text("Selected Entity: %s", _selectedEntity.GetComponent<TagComponent>().GetTag());
		}
		ImGui::PopStyleColor(3);


		/*if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemFocused())
		{
			m_Context->ClearSelectedEntity();
		}*/

		DrawModelEntities();
		DrawCameraEntities();
		DrawLightEntities();

	}

	void SceneHierarchyPanel::DrawModelEntities() const
	{

		auto isHeaderOpen = ImGui::CollapsingHeader("Models", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + (GImGui->Style.FramePadding.y * 2.0f);
		ImGui::SameLine(contentRegionAvailable.x - 80);

		if (ImGui::Button("Add Mesh", ImVec2{ 80, lineHeight }))
		{
			ImGui::OpenPopup("AddMeshPopup");
		}


		if (ImGui::BeginPopupModal("AddMeshPopup"))
		{
			auto modelManager = ModelManager::GetInstance();

			static uint16_t _ModelNamePostfix = 0;
			static char _ModelNameBuffer[50];


			if (!m_Context->GetSelectedEntity())
			{
				ImGui::InputText("Model Name", _ModelNameBuffer, sizeof(_ModelNameBuffer));
			}

			ImGui::Text("Primitves");

			static auto primitiveType = TE_PRIMITIVE_TYPE::BOX;

			if (ImGui::RadioButton("Box", primitiveType == TE_PRIMITIVE_TYPE::BOX))
			{
				primitiveType = TE_PRIMITIVE_TYPE::BOX;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Sphere", primitiveType == TE_PRIMITIVE_TYPE::SPHERE))
			{
				primitiveType = TE_PRIMITIVE_TYPE::SPHERE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Cylinder", primitiveType == TE_PRIMITIVE_TYPE::CYLINDER))
			{
				primitiveType = TE_PRIMITIVE_TYPE::CYLINDER;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Plane", primitiveType == TE_PRIMITIVE_TYPE::PLANE))
			{
				primitiveType = TE_PRIMITIVE_TYPE::PLANE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("RoundedBox", primitiveType == TE_PRIMITIVE_TYPE::ROUNDEDBOX))
			{
				primitiveType = TE_PRIMITIVE_TYPE::ROUNDEDBOX;
			}



			static float3 _Size = { 1.0f, 1.0f, 1.0f };
			static float _Radius = 1.0f;
			static int3  _Slices{ 16,16,16 };
			static int3  _Segments{ 16,16,16 };

			auto _LambdaSize3 = [&size = _Size]() { ImGui::InputFloat3("Size ", &size.x);  };
			auto _LambdaSize2 = [&size = _Size]() { ImGui::InputFloat2("Size ", &size.x);  };
			auto _LambdaSize = [&size = _Size]() { ImGui::InputFloat("Size ", &size.x);  };
			auto _LambdaRadius = [&radius = _Radius]() { ImGui::InputFloat("Radius ", &radius);  };
			auto _LambdaSlices3 = [&slices = _Slices]() { ImGui::InputInt3("Slices ", &slices.x);  };
			auto _LambdaSlices = [&slices = _Slices]() { ImGui::InputInt("Slices ", &slices.x);  };
			auto _LambdaSegments3 = [&segments = _Segments]() { ImGui::InputInt3("Segments ", &segments.x);  };
			auto _LambdaSegments2 = [&segments = _Segments]() { ImGui::InputInt2("Segments ", &segments.x);  };
			auto _LambdaSegments = [&segments = _Segments]() { ImGui::InputInt("Segments ", &segments.x);  };

			switch (primitiveType)
			{
			case TruthEngine::TE_PRIMITIVE_TYPE::BOX:
				_LambdaSize3();
				_LambdaSegments3();
				break;
			case TruthEngine::TE_PRIMITIVE_TYPE::ROUNDEDBOX:
				_LambdaSize3();
				_LambdaRadius();
				_LambdaSlices();
				_LambdaSegments3();
				break;
			case TruthEngine::TE_PRIMITIVE_TYPE::SPHERE:
				_LambdaRadius();
				_LambdaSlices();
				_LambdaSegments();
				break;
			case TruthEngine::TE_PRIMITIVE_TYPE::CYLINDER:
			case TruthEngine::TE_PRIMITIVE_TYPE::CAPPEDCYLINDER:
				_LambdaSize();
				_LambdaRadius();
				_LambdaSlices();
				_LambdaSegments();
				break;
			case TruthEngine::TE_PRIMITIVE_TYPE::PLANE:
				_LambdaSize2();
				_LambdaSegments2();
				break;
			default:
				break;
			}



			if (ImGui::Button("Add"))
			{

				std::string _MeshName = "Primitive_" + std::to_string(_ModelNamePostfix);
				_ModelNamePostfix++;

				Entity _MeshEntity = m_Context->AddPrimitiveMesh(_MeshName.c_str(), primitiveType, _Size, _Radius, _Slices, _Segments, nullptr /*_ModelEntity*/);

				m_Context->SelectEntity(_MeshEntity);

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (!isHeaderOpen)
		{
			return;
		}

		//auto g = m_Context->ViewEntities<MeshComponent>();
		//auto _ModelEntityCollection = m_Context->ViewEntities<ModelComponent>();
		auto _MeshEntityCollection = m_Context->ViewEntities<MeshComponent>();

		if (_MeshEntityCollection.empty())
			return;

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.36f, 1.0f, .57f, .31f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.36f, 1.0f, .57f, .6f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ 0.36f, 1.0f, .57f, .9f });
		int _buttomID = 1;


		ImGui::Indent();

		for (auto entity : _MeshEntityCollection)
		{
			auto& tag = m_Context->GetComponent<TagComponent>(entity);

			auto is_open = ImGui::TreeNodeEx(tag.GetTag(), flags | (entity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(Entity{ entity });
			}

			if (is_open)
			{
				/*std::vector<Entity>& _MeshEntityCollection = m_Context->GetComponent<ModelComponent>(entity).GetMeshEntities();

				if (_MeshEntityCollection.size() > 0)
				{

					ImGui::Indent();

					for (Entity& _MeshEntity : _MeshEntityCollection)
					{
						auto& _MeshTag = m_Context->GetComponent<TagComponent>(_MeshEntity);

						if (ImGui::TreeNodeEx(_MeshTag.GetTag(), flags | (_MeshEntity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0)))
						{
							ImGui::TreePop();
						}


						if (ImGui::IsItemClicked())
						{
							m_Context->SelectEntity(_MeshEntity);
						}
					}

					ImGui::Unindent();
				}*/

				ImGui::TreePop();
			}
		}

		ImGui::Unindent();

		ImGui::PopStyleColor(3);

	}

	/*void SceneHierarchyPanel::DrawModelEntities() const
	{
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + (GImGui->Style.FramePadding.y * 2.0f);

		auto isHeaderOpen = ImGui::CollapsingHeader("Models", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);


		ImGui::SameLine(contentRegionAvailable.x - 80);

		if (ImGui::Button("Add Model", ImVec2{ 80, lineHeight }))
		{
			auto entity = m_Context->AddEntity("Empty Model");
			entity.AddComponent<ModelComponent>();
			m_Context->SelectEntity(entity);
		}

		if (!isHeaderOpen)
		{
			return;
		}

		auto g = m_Context->ViewEntities<ModelComponent>();

		if (g.size() < 1)
			return;

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.36f, 1.0f, .57f, .31f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.36f, 1.0f, .57f, .6f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ 0.36f, 1.0f, .57f, .9f });
		int _buttomID = 1;

		ImGui::Indent();

		for (auto entity : g)
		{

			auto& tag = m_Context->GetComponent<TagComponent>(entity);


			auto is_open = ImGui::TreeNodeEx(tag.GetTag().c_str(), flags | (entity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(Entity{ m_Context, entity });
			}

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			ImGui::PushID(_buttomID);
			ImGui::SameLine(contentRegionAvailable.x - 80);
			if (ImGui::Button("Add Mesh", ImVec2{ 80, lineHeight }))
			{
				ImGui::OpenPopup("AddMeshPopup");
			}


			if (ImGui::BeginPopupModal("AddMeshPopup"))
			{
				auto modelManager = ModelManager::GetInstance();

				ImGui::Text("Primitves");
				static float primitiveSize = 1.0f;
				static auto primitiveType = TE_PRIMITIVE_TYPE::BOX;
				ImGui::InputFloat("Primitive Size/Radius", &primitiveSize);
				if (ImGui::RadioButton("Box", primitiveType == TE_PRIMITIVE_TYPE::BOX))
				{
					primitiveType = TE_PRIMITIVE_TYPE::BOX;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Sphere", primitiveType == TE_PRIMITIVE_TYPE::SPHERE))
				{
					primitiveType = TE_PRIMITIVE_TYPE::SPHERE;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Cylinder", primitiveType == TE_PRIMITIVE_TYPE::CYLINDER))
				{
					primitiveType = TE_PRIMITIVE_TYPE::CYLINDER;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Plane", primitiveType == TE_PRIMITIVE_TYPE::PLANE))
				{
					primitiveType = TE_PRIMITIVE_TYPE::PLANE;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("RoundedBox", primitiveType == TE_PRIMITIVE_TYPE::ROUNDEDBOX))
				{
					primitiveType = TE_PRIMITIVE_TYPE::ROUNDEDBOX;
				}

				if (ImGui::Button("Add"))
				{

					auto entityMesh = modelManager->GeneratePrimitiveMesh(primitiveType, primitiveSize, IdentityMatrix, m_Context->GetSelectedEntity());



					m_Context->SelectEntity(entityMesh);

					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
			_buttomID++;



			if (is_open)
			{

				auto& modelChildrenNodes = m_Context->GetChildrenNodes(entity);

				DrawChilrenNodes(modelChildrenNodes);

				ImGui::TreePop();
			}

		}

		ImGui::Unindent();

		ImGui::PopStyleColor(3);

	}*/

	void SceneHierarchyPanel::DrawCameraEntities() const
	{

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + (GImGui->Style.FramePadding.y * 2.0f);

		auto isHeaderOpen = ImGui::CollapsingHeader("Cameras", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);

		ImGui::SameLine(contentRegionAvailable.x - 80);

		if (ImGui::Button("Add Camera", ImVec2{ 80, lineHeight }))
		{
			ImGui::OpenPopup("AddCameraComponent");
		}

		if (ImGui::BeginPopup("AddCameraComponent"))
		{
			static auto cameraType = TE_CAMERA_TYPE::Perspective;

			if (ImGui::RadioButton("Perspective", cameraType == TE_CAMERA_TYPE::Perspective))
			{
				cameraType = TE_CAMERA_TYPE::Perspective;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Orthograohic", cameraType == TE_CAMERA_TYPE::Orthographic))
			{
				cameraType = TE_CAMERA_TYPE::Orthographic;
			}


			if (ImGui::Button("OK"))
			{
				auto cameraManager = CameraManager::GetInstance();
				Camera* camera;
				switch (cameraType)
				{
				case TruthEngine::TE_CAMERA_TYPE::Perspective:
					camera = cameraManager->CreatePerspectiveFOV("", float3{ .0f, .10f, -10.0f }, float3{ .0f, .0f, .0f }, float3{ .0f, 1.0f, .0f }, DirectX::XM_PIDIV4, TE_INSTANCE_APPLICATION->GetSceneViewportAspectRatio(), 1.0f, 100.0f, false);
					break;
				case TruthEngine::TE_CAMERA_TYPE::Orthographic:
					camera = cameraManager->CreateOrthographicCenterOff("", float3{ .0f, 20.0f, -20.0f }, float3{ .0f, .0f, .0f }, float3{ .0f, 1.0f, .0f }, -20.0f, 20.0f, 20.0f, -20.0f, 1.0, 100.0f);
					break;
				default:
					break;
				}
				auto cameraEntity = m_Context->AddEntity("DefaultCameraEntity");
				cameraEntity.AddComponent<CameraComponent>(camera);

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (!isHeaderOpen)
		{
			return;
		}

		auto g = m_Context->ViewEntities<CameraComponent>();

		if (g.size() < 1)
			return;

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ .8, .26, .98, .31 });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ .8, .26, .98, .6 });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ .8, .26, .98, .9 });

		ImGui::Indent();

		for (auto entity : g)
		{
			auto& tag = m_Context->GetComponent<TagComponent>(entity);

			auto is_open = ImGui::TreeNodeEx(tag.GetTag(), flags | (entity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(Entity{ entity });
			}

			if (is_open)
			{
				/*auto& CameraChildrenNodes = m_Context->GetChildrenNodes(entity);

				DrawChilrenNodes(CameraChildrenNodes);*/

				ImGui::TreePop();
			}
		}

		ImGui::Unindent();

		ImGui::PopStyleColor(3);
	}

	void SceneHierarchyPanel::DrawLightEntities() const
	{
		static char s_LightNameBuffer[50];
		static float3 s_LightPosition = float3(0.0f, 0.0f, 0.0f);

		auto isHeaderOpen = ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + (GImGui->Style.FramePadding.y * 2.0f);
		ImGui::SameLine(contentRegionAvailable.x - 80);

		if (ImGui::Button("Add Light", ImVec2{ 80, lineHeight }))
		{
			ImGui::OpenPopup("AddLightPopup");
		}

		if (ImGui::BeginPopupModal("AddLightPopup"))
		{
			ImGui::InputText("Light Name: ", s_LightNameBuffer, sizeof(s_LightNameBuffer));

			static auto s_LightType = TE_LIGHT_TYPE::Point;

			if (ImGui::RadioButton("Point", s_LightType == TE_LIGHT_TYPE::Point))
			{
				s_LightType = TE_LIGHT_TYPE::Point;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Spot", s_LightType == TE_LIGHT_TYPE::Spot))
			{
				s_LightType = TE_LIGHT_TYPE::Spot;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Directional", s_LightType == TE_LIGHT_TYPE::Directional))
			{
				s_LightType = TE_LIGHT_TYPE::Directional;
			}

			ImGuiLayer::DrawFloat3Control("AddLightEntityPosition", &s_LightPosition.x);

			if (ImGui::Button("Add##AddLightPopup"))
			{
				ILight* _ILight = nullptr;
				switch (s_LightType)
				{
				case TE_LIGHT_TYPE::Directional:
					m_Context->AddLightEntity_Directional(s_LightNameBuffer, s_LightPosition, 0.1f, float3(0.3f, 0.3f, 0.3f), 1.0f, float3(0.0f, 0.0f, 1.0f), false, float4(1.0f, 2.0f, 10.0f, 100.0f));
					break;
				case TE_LIGHT_TYPE::Spot:
					m_Context->AddLightEntity_Spot(s_LightNameBuffer, s_LightPosition, 0.1f, float3(0.3f, 0.3f, 0.3f), 1.0f, float3(0.0f, 0.0f, 1.0f), false, 10.0f, 100.0f, 45.0f, 90.0f);
					break;
				case TE_LIGHT_TYPE::Point:
					m_Context->AddLightEntity_Point(s_LightNameBuffer, s_LightPosition, 0.1f, float3(0.3f, 0.3f, 0.3f), 1.0f, false, 1.0f, 0.007f, 0.0002f);
					break;
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel##AddLightPopup"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}


		if (!isHeaderOpen)
		{
			return;
		}

		auto g = m_Context->ViewEntities<LightComponent>();

		if (g.size() < 1)
			return;

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ .97, 1.0, 0.0, .45 });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ .97, 1.0, 0.0, .65 });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ .97, 1.0, 0.0, .85 });

		ImGui::Indent();

		for (auto lightEntity : g)
		{
			auto& tag = m_Context->GetComponent<TagComponent>(lightEntity);

			auto is_open = ImGui::TreeNodeEx(tag.GetTag(), flags | (lightEntity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(Entity{ lightEntity });
			}

			if (is_open)
			{
				/*auto& CameraChildrenNodes = m_Context->GetChildrenNodes(lightEntity);

				DrawChilrenNodes(CameraChildrenNodes);*/

				ImGui::TreePop();
			}
		}

		ImGui::Unindent();

		ImGui::PopStyleColor(3);
	}

	void SceneHierarchyPanel::DrawChilrenNodes(std::vector<EntityNode>& childrenNodes) const
	{
		for (auto& childNode : childrenNodes)
		{
			const char* meshTag = m_Context->GetComponent<TagComponent>(childNode.mEntity).GetTag();

			bool is_open = ImGui::TreeNodeEx(meshTag, flags | (childNode.mEntity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(childNode.mEntity);
			}

			if (is_open)
			{
				DrawChilrenNodes(childNode.mChildren);

				ImGui::TreePop();
			}
		}
	}


}