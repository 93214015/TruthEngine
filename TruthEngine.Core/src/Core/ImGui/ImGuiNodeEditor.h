#pragma once
#include "ImGuiNodeEditor/imgui_node_editor.h"
#include "ImGuiNodeEditor/examples/blueprints-example/utilities/widgets.h"

namespace ImGuiNodeEditor = ax::NodeEditor;


namespace TruthEngine
{
	class NodeEditor
	{
	public:
		enum class PinType
		{
			Flow,
			Bool,
			Int,
			Float,
			String,
			Object,
			Function,
			Delegate,
		};

		enum class PinKind
		{
			Output,
			Input
		};

		enum class NodeType
		{
			Blueprint,
			Simple,
			Tree,
			Comment,
			Houdini
		};

		struct Node;

		struct Pin
		{
			ImGuiNodeEditor::PinId   ID;
			Node* Node;
			std::string Name;
			PinType     Type;
			PinKind     Kind;

			Pin(int id, const char* name, PinType type) :
				ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
			{
			}
		};

		struct Node
		{
			ImGuiNodeEditor::NodeId ID;
			std::string Name;
			std::vector<Pin> Inputs;
			std::vector<Pin> Outputs;
			ImColor Color;
			NodeType Type;
			ImVec2 Size;

			std::string State;
			std::string SavedState;

			Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
				ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
			{
			}
		};

		struct Link
		{
			ImGuiNodeEditor::LinkId ID;

			ImGuiNodeEditor::PinId StartPinID;
			ImGuiNodeEditor::PinId EndPinID;

			ImColor Color;

			Link(ImGuiNodeEditor::LinkId id, ImGuiNodeEditor::PinId startPinId, ImGuiNodeEditor::PinId endPinId) :
				ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
			{
			}
		};

		struct NodeIdLess
		{
			bool operator()(const ImGuiNodeEditor::NodeId& lhs, const ImGuiNodeEditor::NodeId& rhs) const
			{
				return lhs.AsPointer() < rhs.AsPointer();
			}
		};

		void Init();
		void Shutdown();
		void NewFrame();

	private:
		void BuildNodes();
		
	public:

		static const int            s_PinIconSize = 24;
		static const float          s_TouchTime;
		static std::vector<Node>    s_Nodes;
		static std::vector<Link>    s_Links;
		static ImTextureID          s_HeaderBackground;
		//static ImTextureID        s_SampleImage = nullptr;
		static ImTextureID          s_SaveIcon;
		static ImTextureID          s_RestoreIcon;
		static std::map<ImGuiNodeEditor::NodeId, float, NodeIdLess> s_NodeTouchTime;

	private:

		std::shared_ptr<ImGuiNodeEditor::EditorContext> m_EditorContext;

	};
}
