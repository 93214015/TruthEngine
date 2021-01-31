#pragma once
#include "ImGuiNodeEditor/imgui_node_editor.h"
#include "ImGuiNodeEditor/examples/blueprints-example/utilities/widgets.h"
#include "ImGuiNodeEditor/examples/blueprints-example/utilities/builders.h"

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
			Node* mNode;
			std::string Name;
			PinType     Type;
			PinKind     Kind;

			Pin(int id, const char* name, Node* _node, PinType type, PinKind _kind) :
				ID(id), mNode(_node), Name(name), Type(type), Kind(_kind)
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
		Node* AddNode(const char* name, const ImVec2& size = ImVec2{.0f, .0f}, const ImColor& color = ImColor(255, 255, 255), NodeType nodeType = NodeType::Simple);
		Pin* AddPin(const char* _name, Node* _node, PinKind _pinKind, PinType _pinType);

	private:
		void BuildNodes();
		
		ImColor GetIconColor(PinType type);
		void DrawPinIcon(const Pin& pin, bool connected, int alpha);

	public:

		int                  m_NextId = 1;
		const int            m_PinIconSize = 24;
		std::vector<Node>    m_Nodes;
		std::vector<Link>    m_Links;
		ImTextureID          m_HeaderBackground = nullptr;
		ImTextureID          m_SaveIcon = nullptr;
		ImTextureID          m_RestoreIcon = nullptr;
		const float          m_TouchTime = 1.0f;
		std::map<ax::NodeEditor::NodeId, float, NodeIdLess> m_NodeTouchTime;

	private:

		std::shared_ptr<ImGuiNodeEditor::EditorContext> m_EditorContext;

	};
}
