#include "pch.h"
#include "ImGuiNodeEditor.h"

namespace ed = ax::NodeEditor;
using namespace TruthEngine;

static int s_NextId = 1;
static int GetNextId()
{
	return s_NextId++;
}


/*
static ed::LinkId GetNextLinkId()
{
	return ed::LinkId(GetNextId());
}

static void TouchNode(ed::NodeId id)
{
	NodeEditor::s_NodeTouchTime[id] = NodeEditor::s_TouchTime;
}

static float GetTouchProgress(ed::NodeId id)
{
	auto it = NodeEditor::s_NodeTouchTime.find(id);
	if (it != NodeEditor::s_NodeTouchTime.end() && it->second > 0.0f)
		return (NodeEditor::s_TouchTime - it->second) / NodeEditor::s_TouchTime;
	else
		return 0.0f;
}

static void UpdateTouch()
{
	const auto deltaTime = ImGui::GetIO().DeltaTime;
	for (auto& entry : NodeEditor::s_NodeTouchTime)
	{
		if (entry.second > 0.0f)
			entry.second -= deltaTime;
	}
}

static NodeEditor::Node* FindNode(ed::NodeId id)
{
	for (auto& node : NodeEditor::s_Nodes)
		if (node.ID == id)
			return &node;

	return nullptr;
}

static NodeEditor::Link* FindLink(ed::LinkId id)
{
	for (auto& link : NodeEditor::s_Links)
		if (link.ID == id)
			return &link;

	return nullptr;
}

static NodeEditor::Pin* FindPin(ed::PinId id)
{
	if (!id)
		return nullptr;

	for (auto& node : NodeEditor::s_Nodes)
	{
		for (auto& pin : node.Inputs)
			if (pin.ID == id)
				return &pin;

		for (auto& pin : node.Outputs)
			if (pin.ID == id)
				return &pin;
	}

	return nullptr;
}

static bool IsPinLinked(ed::PinId id)
{
	if (!id)
		return false;

	for (auto& link : NodeEditor::s_Links)
		if (link.StartPinID == id || link.EndPinID == id)
			return true;

	return false;
}

static bool CanCreateLink(NodeEditor::Pin* a, NodeEditor::Pin* b)
{
	if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
		return false;

	return true;
}

static void BuildNode(NodeEditor::Node* node)
{
	for (auto& input : node->Inputs)
	{
		input.Node = node;
		input.Kind = NodeEditor::PinKind::Input;
	}

	for (auto& output : node->Outputs)
	{
		output.Node = node;
		output.Kind = NodeEditor::PinKind::Output;
	}
}

static NodeEditor::Node* SpawnInputActionNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "InputAction Fire", ImColor(255, 128, 128));
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Delegate);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Pressed", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Released", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnBranchNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Branch");
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Condition", NodeEditor::PinType::Bool);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "True", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "False", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnDoNNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Do N");
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Enter", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "N", NodeEditor::PinType::Int);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Reset", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Exit", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Counter", NodeEditor::PinType::Int);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnOutputActionNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "OutputAction");
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Sample", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Condition", NodeEditor::PinType::Bool);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Event", NodeEditor::PinType::Delegate);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnPrintStringNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Print String");
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "In String", NodeEditor::PinType::String);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnMessageNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Simple;
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Message", NodeEditor::PinType::String);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnSetTimerNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Set Timer", ImColor(128, 195, 248));
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Object", NodeEditor::PinType::Object);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Function Name", NodeEditor::PinType::Function);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Time", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Looping", NodeEditor::PinType::Bool);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnLessNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "<", ImColor(128, 195, 248));
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Simple;
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Float);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnWeirdNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "o.O", ImColor(128, 195, 248));
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Simple;
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Float);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnTraceByChannelNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Single Line Trace by Channel", ImColor(255, 128, 64));
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Start", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "End", NodeEditor::PinType::Int);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Channel", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Complex", NodeEditor::PinType::Bool);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Actors to Ignore", NodeEditor::PinType::Int);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Draw Debug Type", NodeEditor::PinType::Bool);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "Ignore Self", NodeEditor::PinType::Bool);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Out Hit", NodeEditor::PinType::Float);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "Return Value", NodeEditor::PinType::Bool);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnTreeSequenceNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Sequence");
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Tree;
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnTreeTaskNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Move To");
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Tree;
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnTreeTask2Node()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Random Wait");
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Tree;
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnComment()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Test Comment");
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Comment;
	NodeEditor::s_Nodes.back().Size = ImVec2(300, 200);

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnHoudiniTransformNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Transform");
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Houdini;
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}

static NodeEditor::Node* SpawnHoudiniGroupNode()
{
	NodeEditor::s_Nodes.emplace_back(GetNextId(), "Group");
	NodeEditor::s_Nodes.back().Type = NodeEditor::NodeType::Houdini;
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Inputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);
	NodeEditor::s_Nodes.back().Outputs.emplace_back(GetNextId(), "", NodeEditor::PinType::Flow);

	BuildNode(&NodeEditor::s_Nodes.back());

	return &NodeEditor::s_Nodes.back();
}*/

//void NodeEditor::BuildNodes()
//{
//	for (auto& node : s_Nodes)
//		BuildNode(&node);
//}

void NodeEditor::Init()
{
	ed::Config config;
	config.SettingsFile = "Simple.json";

	auto editor = ed::CreateEditor(&config);

	m_EditorContext.reset(editor);
	//ed::SetCurrentEditor(m_EditorContext.get());

	/*Node* node;
	node = SpawnInputActionNode();      ed::SetNodePosition(node->ID, ImVec2(-252, 220));
	node = SpawnBranchNode();           ed::SetNodePosition(node->ID, ImVec2(-300, 351));
	node = SpawnDoNNode();              ed::SetNodePosition(node->ID, ImVec2(-238, 504));
	node = SpawnOutputActionNode();     ed::SetNodePosition(node->ID, ImVec2(71, 80));
	node = SpawnSetTimerNode();         ed::SetNodePosition(node->ID, ImVec2(168, 316));

	node = SpawnTreeSequenceNode();     ed::SetNodePosition(node->ID, ImVec2(1028, 329));
	node = SpawnTreeTaskNode();         ed::SetNodePosition(node->ID, ImVec2(1204, 458));
	node = SpawnTreeTask2Node();        ed::SetNodePosition(node->ID, ImVec2(868, 538));

	node = SpawnComment();              ed::SetNodePosition(node->ID, ImVec2(112, 576));
	node = SpawnComment();              ed::SetNodePosition(node->ID, ImVec2(800, 224));

	node = SpawnLessNode();             ed::SetNodePosition(node->ID, ImVec2(366, 652));
	node = SpawnWeirdNode();            ed::SetNodePosition(node->ID, ImVec2(144, 652));
	node = SpawnMessageNode();          ed::SetNodePosition(node->ID, ImVec2(-348, 698));
	node = SpawnPrintStringNode();      ed::SetNodePosition(node->ID, ImVec2(-69, 652));

	node = SpawnHoudiniTransformNode(); ed::SetNodePosition(node->ID, ImVec2(500, -70));
	node = SpawnHoudiniGroupNode();     ed::SetNodePosition(node->ID, ImVec2(500, 42));

	ed::NavigateToContent();

	BuildNodes();

	s_Links.push_back(Link(GetNextLinkId(), s_Nodes[5].Outputs[0].ID, s_Nodes[6].Inputs[0].ID));
	s_Links.push_back(Link(GetNextLinkId(), s_Nodes[5].Outputs[0].ID, s_Nodes[7].Inputs[0].ID));

	s_Links.push_back(Link(GetNextLinkId(), s_Nodes[14].Outputs[0].ID, s_Nodes[15].Inputs[0].ID));*/


}

void NodeEditor::Shutdown()
{
	if (m_EditorContext)
	{
		ed::DestroyEditor(m_EditorContext.get());
		m_EditorContext.reset();
	}
}

//static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
//{
//	using namespace ImGui;
//	ImGuiContext& g = *GImGui;
//	ImGuiWindow* window = g.CurrentWindow;
//	ImGuiID id = window->GetID("##Splitter");
//	ImRect bb;
//	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
//	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
//	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
//}

void NodeEditor::NewFrame()
{
	//UpdateTouch();

	//auto& io = ImGui::GetIO();

	ed::SetCurrentEditor(m_EditorContext.get());


	ed::Begin("NodeEditor");
	{
		auto cursorTopLeft = ImGui::GetCursorScreenPos();

		for (auto& node : m_Nodes)
		{
			ed::BeginNode(node.ID);
			{
				ImGui::Text(node.Name.c_str());
				for (auto& pinInput : node.Inputs)
				{
					DrawPinIcon(pinInput, false, (int)(255));
					ed::BeginPin(pinInput.ID, ed::PinKind::Input);
					ImGui::Text(pinInput.Name.c_str());
					ed::EndPin();
				}
			}
			ed::EndNode();


		}

		ed::BeginNode(10256);
		{
			ImGui::Text("SimpleNode2");
			ed::BeginPin(236, ed::PinKind::Output);
			ImGui::Text("outpu->");
			ed::EndPin();
		}
		ed::EndNode();

	}
	ed::End();
	ed::SetCurrentEditor(nullptr);

}

TruthEngine::NodeEditor::Node* TruthEngine::NodeEditor::AddNode(const char* name, const ImVec2& size, const ImColor& color, NodeType nodeType)
{
	return &m_Nodes.emplace_back(GetNextId(), name, color);
}

TruthEngine::NodeEditor::Pin* TruthEngine::NodeEditor::AddPin(const char* _name, Node* _node, PinKind _pinKind, PinType _pinType)
{
	Pin* _pin;
	switch (_pinKind)
	{
	case TruthEngine::NodeEditor::PinKind::Output:
		_pin = &_node->Outputs.emplace_back(Pin(GetNextId(), _name, _node, _pinType, PinKind::Output));
		
		break;
	case TruthEngine::NodeEditor::PinKind::Input:
		_pin = &_node->Inputs.emplace_back(Pin(GetNextId(), _name, _node, _pinType, PinKind::Input));
		break;
	default:
		break;
	}

	return _pin;
}

ImColor TruthEngine::NodeEditor::GetIconColor(PinType type)
{
	switch (type)
	{
	default:
	case PinType::Flow:     return ImColor(255, 255, 255);
	case PinType::Bool:     return ImColor(220, 48, 48);
	case PinType::Int:      return ImColor(68, 201, 156);
	case PinType::Float:    return ImColor(147, 226, 74);
	case PinType::String:   return ImColor(124, 21, 153);
	case PinType::Object:   return ImColor(51, 150, 215);
	case PinType::Function: return ImColor(218, 0, 183);
	case PinType::Delegate: return ImColor(255, 48, 48);
	}
};

void TruthEngine::NodeEditor::DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
	using namespace ax::Drawing;

	IconType iconType;
	ImColor  color = GetIconColor(pin.Type);
	color.Value.w = alpha / 255.0f;
	switch (pin.Type)
	{
	case PinType::Flow:     iconType = IconType::Flow;   break;
	case PinType::Bool:     iconType = IconType::Circle; break;
	case PinType::Int:      iconType = IconType::Circle; break;
	case PinType::Float:    iconType = IconType::Circle; break;
	case PinType::String:   iconType = IconType::Circle; break;
	case PinType::Object:   iconType = IconType::Circle; break;
	case PinType::Function: iconType = IconType::Circle; break;
	case PinType::Delegate: iconType = IconType::Square; break;
	default:
		return;
	}

	ax::Widgets::Icon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)), iconType, connected, color, ImColor(32, 32, 32, alpha));
}
