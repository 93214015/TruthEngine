#include "pch.h"
#include "SceneHierarchyPanel.h"

#include "Core/Entity/Scene.h"
#include "Core/Entity/Components.h"

#include "Core/Entity/Camera/CameraManager.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Application.h"
#include "Core/Entity/Model/ModelManager.h"

using namespace TruthEngine::Core;

namespace TruthEngine
{
	SceneHierarchyPanel::SceneHierarchyPanel(Core::Scene* context)
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
			ImGui::Text("Selected Entity: %s", _selectedEntity.GetComponent<TagComponent>().GetTag().c_str());
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
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + (GImGui->Style.FramePadding.y * 2.0f);

		auto isHeaderOpen = ImGui::CollapsingHeader("Meshes", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);

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
				auto entityMesh = modelManager->GeneratePrimitiveMesh(primitiveType, primitiveSize);

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

		if (!isHeaderOpen)
		{
			return;
		}

		auto g = m_Context->ViewEntities<Core::MeshComponent>();

		if (g.size() < 1)
			return;

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.36f, 1.0f, .57f, .31f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.36f, 1.0f, .57f, .6f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ 0.36f, 1.0f, .57f, .9f });
		int _buttomID = 1;



		ImGui::Indent();

		for (auto entity : g)
		{

			auto& tag = m_Context->GetComponent<Core::TagComponent>(entity);


			auto is_open = ImGui::TreeNodeEx(tag.GetTag().c_str(), flags | (entity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(Entity{ m_Context, entity });
			}



			if (is_open)
			{

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

		auto g = m_Context->ViewEntities<Core::ModelComponent>();

		if (g.size() < 1)
			return;

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.36f, 1.0f, .57f, .31f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.36f, 1.0f, .57f, .6f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ 0.36f, 1.0f, .57f, .9f });
		int _buttomID = 1;

		ImGui::Indent();

		for (auto entity : g)
		{

			auto& tag = m_Context->GetComponent<Core::TagComponent>(entity);


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
				case TruthEngine::Core::TE_CAMERA_TYPE::Perspective:
					camera = cameraManager->CreatePerspectiveFOV("", float3{ .0f, .10f, -10.0f }, float3{ .0f, .0f, .0f }, float3{ .0f, 1.0f, .0f }, DirectX::XM_PIDIV4, TE_INSTANCE_APPLICATION->GetSceneViewportAspectRatio(), 1.0f, 100.0f);
					break;
				case TruthEngine::Core::TE_CAMERA_TYPE::Orthographic:
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

		auto g = m_Context->ViewEntities<Core::CameraComponent>();

		if (g.size() < 1)
			return;

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ .8, .26, .98, .31 });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ .8, .26, .98, .6 });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ .8, .26, .98, .9 });

		ImGui::Indent();

		for (auto entity : g)
		{
			auto& tag = m_Context->GetComponent<Core::TagComponent>(entity);

			auto is_open = ImGui::TreeNodeEx(tag.GetTag().c_str(), flags | (entity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(Entity{ m_Context, entity });
			}

			if (is_open)
			{
				auto& CameraChildrenNodes = m_Context->GetChildrenNodes(entity);

				DrawChilrenNodes(CameraChildrenNodes);

				ImGui::TreePop();
			}
		}

		ImGui::Unindent();

		ImGui::PopStyleColor(3);
	}

	void SceneHierarchyPanel::DrawLightEntities() const
	{
		auto isHeaderOpen = ImGui::CollapsingHeader("Lights");

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + (GImGui->Style.FramePadding.y * 2.0f);
		ImGui::SameLine(contentRegionAvailable.x - 80);

		if (ImGui::Button("Add Light", ImVec2{ 80, lineHeight }))
		{
			// 			auto entity = m_Context->AddEntity("Empty Light");
			// 			entity.AddComponent<LightComponent>();
			// 			m_Context->SelectEntity(entity);
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
			auto& tag = m_Context->GetComponent<Core::TagComponent>(lightEntity);

			auto is_open = ImGui::TreeNodeEx(tag.GetTag().c_str(), flags | (lightEntity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
			{
				m_Context->SelectEntity(Entity{ m_Context, lightEntity });
			}

			if (is_open)
			{
				auto& CameraChildrenNodes = m_Context->GetChildrenNodes(lightEntity);

				DrawChilrenNodes(CameraChildrenNodes);

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
			auto meshTag = m_Context->GetComponent<TagComponent>(childNode.mEntity).GetTag();

			auto is_open = ImGui::TreeNodeEx(meshTag.c_str(), flags | (childNode.mEntity == m_Context->GetSelectedEntity() ? ImGuiTreeNodeFlags_Selected : 0));

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