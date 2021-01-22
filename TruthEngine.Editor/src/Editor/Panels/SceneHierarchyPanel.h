#pragma once


namespace TruthEngine
{

	namespace Core
	{
		class Scene;
		class EntityNode;
	}

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Core::Scene* context);

		inline void SetContext(Core::Scene* context)
		{
			m_Context = context;
		}

		void OnImGuiRender();

	private:
		void DrawModelEntities() const;
		void DrawCameraEntities() const;
		void DrawLightEntities() const;
		void DrawChilrenNodes(std::vector<Core::EntityNode>& childrenNodes) const;

	private:
		Core::Scene* m_Context = nullptr;

		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow;
	};
}
