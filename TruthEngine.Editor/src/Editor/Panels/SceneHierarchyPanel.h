#pragma once


namespace TruthEngine
{

	class Scene;
	struct EntityNode;

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Scene* context);

		inline void SetContext(Scene* context)
		{
			m_Context = context;
		}

		void OnImGuiRender();

	private:
		void DrawModelEntities() const;
		void DrawCameraEntities() const;
		void DrawLightEntities() const;
		void DrawChilrenNodes(std::vector<EntityNode>& childrenNodes) const;

	private:
		Scene* m_Context = nullptr;

		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow;
	};
}
