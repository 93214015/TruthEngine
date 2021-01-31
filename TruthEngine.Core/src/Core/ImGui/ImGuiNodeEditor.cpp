#include "pch.h"
#include "ImGuiNodeEditor.h"

namespace ed = ax::NodeEditor;
using namespace TruthEngine;

static int s_NextId = 1;
static int GetNextId()
{
	return s_NextId++;
}

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
}

void NodeEditor::BuildNodes()
{
	for (auto& node : s_Nodes)
		BuildNode(&node);
}

void NodeEditor::Init()
{
	ed::Config config;

	m_EditorContext.reset(ed::CreateEditor(&config));
	ed::SetCurrentEditor(m_EditorContext.get());

	Node* node;
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

	s_Links.push_back(Link(GetNextLinkId(), s_Nodes[14].Outputs[0].ID, s_Nodes[15].Inputs[0].ID));


}

void NodeEditor::Shutdown()
{
	if (m_EditorContext)
	{
		ed::DestroyEditor(m_EditorContext.get());
		m_EditorContext.reset();
	}
}


static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}


void NodeEditor::NewFrame()
{
	UpdateTouch();

	auto& io = ImGui::GetIO();

	ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

	ed::SetCurrentEditor(m_Editor);

	//auto& style = ImGui::GetStyle();

# if 0
	{
		for (auto x = -io.DisplaySize.y; x < io.DisplaySize.x; x += 10.0f)
		{
			ImGui::GetWindowDrawList()->AddLine(ImVec2(x, 0), ImVec2(x + io.DisplaySize.y, io.DisplaySize.y),
				IM_COL32(255, 255, 0, 255));
		}
	}
# endif

	static ed::NodeId contextNodeId = 0;
	static ed::LinkId contextLinkId = 0;
	static ed::PinId  contextPinId = 0;
	static bool createNewNode = false;
	static Pin* newNodeLinkPin = nullptr;
	static Pin* newLinkPin = nullptr;

	static float leftPaneWidth = 400.0f;
	static float rightPaneWidth = 800.0f;
	Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

	ShowLeftPane(leftPaneWidth - 4.0f);

	ImGui::SameLine(0.0f, 12.0f);

	ed::Begin("Node editor");
	{
		auto cursorTopLeft = ImGui::GetCursorScreenPos();

		util::BlueprintNodeBuilder builder(s_HeaderBackground, Application_GetTextureWidth(s_HeaderBackground), Application_GetTextureHeight(s_HeaderBackground));

		for (auto& node : s_Nodes)
		{
			if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
				continue;

			const auto isSimple = node.Type == NodeType::Simple;

			bool hasOutputDelegates = false;
			for (auto& output : node.Outputs)
				if (output.Type == PinType::Delegate)
					hasOutputDelegates = true;

			builder.Begin(node.ID);
			if (!isSimple)
			{
				builder.Header(node.Color);
				ImGui::Spring(0);
				ImGui::TextUnformatted(node.Name.c_str());
				ImGui::Spring(1);
				ImGui::Dummy(ImVec2(0, 28));
				if (hasOutputDelegates)
				{
					ImGui::BeginVertical("delegates", ImVec2(0, 28));
					ImGui::Spring(1, 0);
					for (auto& output : node.Outputs)
					{
						if (output.Type != PinType::Delegate)
							continue;

						auto alpha = ImGui::GetStyle().Alpha;
						if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
							alpha = alpha * (48.0f / 255.0f);

						ed::BeginPin(output.ID, ed::PinKind::Output);
						ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
						ed::PinPivotSize(ImVec2(0, 0));
						ImGui::BeginHorizontal(output.ID.AsPointer());
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
						if (!output.Name.empty())
						{
							ImGui::TextUnformatted(output.Name.c_str());
							ImGui::Spring(0);
						}
						DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
						ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
						ImGui::EndHorizontal();
						ImGui::PopStyleVar();
						ed::EndPin();

						//DrawItemRect(ImColor(255, 0, 0));
					}
					ImGui::Spring(1, 0);
					ImGui::EndVertical();
					ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
				}
				else
					ImGui::Spring(0);
				builder.EndHeader();
			}

			for (auto& input : node.Inputs)
			{
				auto alpha = ImGui::GetStyle().Alpha;
				if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
					alpha = alpha * (48.0f / 255.0f);

				builder.Input(input.ID);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
				DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
				ImGui::Spring(0);
				if (!input.Name.empty())
				{
					ImGui::TextUnformatted(input.Name.c_str());
					ImGui::Spring(0);
				}
				if (input.Type == PinType::Bool)
				{
					ImGui::Button("Hello");
					ImGui::Spring(0);
				}
				ImGui::PopStyleVar();
				builder.EndInput();
			}

			if (isSimple)
			{
				builder.Middle();

				ImGui::Spring(1, 0);
				ImGui::TextUnformatted(node.Name.c_str());
				ImGui::Spring(1, 0);
			}

			for (auto& output : node.Outputs)
			{
				if (!isSimple && output.Type == PinType::Delegate)
					continue;

				auto alpha = ImGui::GetStyle().Alpha;
				if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
					alpha = alpha * (48.0f / 255.0f);

				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
				builder.Output(output.ID);
				if (output.Type == PinType::String)
				{
					static char buffer[128] = "Edit Me\nMultiline!";
					static bool wasActive = false;

					ImGui::PushItemWidth(100.0f);
					ImGui::InputText("##edit", buffer, 127);
					ImGui::PopItemWidth();
					if (ImGui::IsItemActive() && !wasActive)
					{
						ed::EnableShortcuts(false);
						wasActive = true;
					}
					else if (!ImGui::IsItemActive() && wasActive)
					{
						ed::EnableShortcuts(true);
						wasActive = false;
					}
					ImGui::Spring(0);
				}
				if (!output.Name.empty())
				{
					ImGui::Spring(0);
					ImGui::TextUnformatted(output.Name.c_str());
				}
				ImGui::Spring(0);
				DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
				ImGui::PopStyleVar();
				builder.EndOutput();
			}

			builder.End();
		}

		for (auto& node : s_Nodes)
		{
			if (node.Type != NodeType::Tree)
				continue;

			const float rounding = 5.0f;
			const float padding = 12.0f;

			const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

			ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
			ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
			ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
			ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

			ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
			ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
			ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
			ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
			ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
			ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
			ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);
			ed::BeginNode(node.ID);

			ImGui::BeginVertical(node.ID.AsPointer());
			ImGui::BeginHorizontal("inputs");
			ImGui::Spring(0, padding * 2);

			ImRect inputsRect;
			int inputAlpha = 200;
			if (!node.Inputs.empty())
			{
				auto& pin = node.Inputs[0];
				ImGui::Dummy(ImVec2(0, padding));
				ImGui::Spring(1, 0);
				inputsRect = ImGui_GetItemRect();

				ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
				ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
				ed::PushStyleVar(ed::StyleVar_PinCorners, 12);
				ed::BeginPin(pin.ID, ed::PinKind::Input);
				ed::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
				ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
				ed::EndPin();
				ed::PopStyleVar(3);

				if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
					inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
			}
			else
				ImGui::Dummy(ImVec2(0, padding));

			ImGui::Spring(0, padding * 2);
			ImGui::EndHorizontal();

			ImGui::BeginHorizontal("content_frame");
			ImGui::Spring(1, padding);

			ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
			ImGui::Dummy(ImVec2(160, 0));
			ImGui::Spring(1);
			ImGui::TextUnformatted(node.Name.c_str());
			ImGui::Spring(1);
			ImGui::EndVertical();
			auto contentRect = ImGui_GetItemRect();

			ImGui::Spring(1, padding);
			ImGui::EndHorizontal();

			ImGui::BeginHorizontal("outputs");
			ImGui::Spring(0, padding * 2);

			ImRect outputsRect;
			int outputAlpha = 200;
			if (!node.Outputs.empty())
			{
				auto& pin = node.Outputs[0];
				ImGui::Dummy(ImVec2(0, padding));
				ImGui::Spring(1, 0);
				outputsRect = ImGui_GetItemRect();

				ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
				ed::BeginPin(pin.ID, ed::PinKind::Output);
				ed::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
				ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
				ed::EndPin();
				ed::PopStyleVar();

				if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
					outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
			}
			else
				ImGui::Dummy(ImVec2(0, padding));

			ImGui::Spring(0, padding * 2);
			ImGui::EndHorizontal();

			ImGui::EndVertical();

			ed::EndNode();
			ed::PopStyleVar(7);
			ed::PopStyleColor(4);

			auto drawList = ed::GetNodeBackgroundDrawList(node.ID);

			//const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
			//const auto unitSize    = 1.0f / fringeScale;

			//const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
			//{
			//    if ((col >> 24) == 0)
			//        return;
			//    drawList->PathRect(a, b, rounding, rounding_corners);
			//    drawList->PathStroke(col, true, thickness);
			//};

			drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
			//ImGui::PopStyleVar();
			drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
				IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
			//ImGui::PopStyleVar();
			drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			drawList->AddRect(
				contentRect.GetTL(),
				contentRect.GetBR(),
				IM_COL32(48, 128, 255, 100), 0.0f);
			//ImGui::PopStyleVar();
		}

		for (auto& node : s_Nodes)
		{
			if (node.Type != NodeType::Houdini)
				continue;

			const float rounding = 10.0f;
			const float padding = 12.0f;


			ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
			ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(125, 125, 125, 200));
			ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(229, 229, 229, 60));
			ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

			const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

			ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
			ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
			ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
			ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
			ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
			ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
			ed::PushStyleVar(ed::StyleVar_PinRadius, 6.0f);
			ed::BeginNode(node.ID);

			ImGui::BeginVertical(node.ID.AsPointer());
			if (!node.Inputs.empty())
			{
				ImGui::BeginHorizontal("inputs");
				ImGui::Spring(1, 0);

				ImRect inputsRect;
				int inputAlpha = 200;
				for (auto& pin : node.Inputs)
				{
					ImGui::Dummy(ImVec2(padding, padding));
					inputsRect = ImGui_GetItemRect();
					ImGui::Spring(1, 0);
					inputsRect.Min.y -= padding;
					inputsRect.Max.y -= padding;

					//ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
					//ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
					ed::PushStyleVar(ed::StyleVar_PinCorners, 15);
					ed::BeginPin(pin.ID, ed::PinKind::Input);
					ed::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
					ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
					ed::EndPin();
					//ed::PopStyleVar(3);
					ed::PopStyleVar(1);

					auto drawList = ImGui::GetWindowDrawList();
					drawList->AddRectFilled(inputsRect.GetTL(), inputsRect.GetBR(),
						IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 15);
					drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
						IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 15);

					if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
						inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
				}

				//ImGui::Spring(1, 0);
				ImGui::EndHorizontal();
			}

			ImGui::BeginHorizontal("content_frame");
			ImGui::Spring(1, padding);

			ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
			ImGui::Dummy(ImVec2(160, 0));
			ImGui::Spring(1);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			ImGui::TextUnformatted(node.Name.c_str());
			ImGui::PopStyleColor();
			ImGui::Spring(1);
			ImGui::EndVertical();
			auto contentRect = ImGui_GetItemRect();

			ImGui::Spring(1, padding);
			ImGui::EndHorizontal();

			if (!node.Outputs.empty())
			{
				ImGui::BeginHorizontal("outputs");
				ImGui::Spring(1, 0);

				ImRect outputsRect;
				int outputAlpha = 200;
				for (auto& pin : node.Outputs)
				{
					ImGui::Dummy(ImVec2(padding, padding));
					outputsRect = ImGui_GetItemRect();
					ImGui::Spring(1, 0);
					outputsRect.Min.y += padding;
					outputsRect.Max.y += padding;

					ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
					ed::BeginPin(pin.ID, ed::PinKind::Output);
					ed::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
					ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
					ed::EndPin();
					ed::PopStyleVar();

					auto drawList = ImGui::GetWindowDrawList();
					drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR(),
						IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 15);
					drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
						IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 15);


					if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
						outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
				}

				ImGui::EndHorizontal();
			}

			ImGui::EndVertical();

			ed::EndNode();
			ed::PopStyleVar(7);
			ed::PopStyleColor(4);

			auto drawList = ed::GetNodeBackgroundDrawList(node.ID);

			//const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
			//const auto unitSize    = 1.0f / fringeScale;

			//const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
			//{
			//    if ((col >> 24) == 0)
			//        return;
			//    drawList->PathRect(a, b, rounding, rounding_corners);
			//    drawList->PathStroke(col, true, thickness);
			//};

			//drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
			//    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			//drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
			//    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
			//ImGui::PopStyleVar();
			//drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
			//    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
			////ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			//drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
			//    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
			////ImGui::PopStyleVar();
			//drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
			//drawList->AddRect(
			//    contentRect.GetTL(),
			//    contentRect.GetBR(),
			//    IM_COL32(48, 128, 255, 100), 0.0f);
			//ImGui::PopStyleVar();
		}

		for (auto& node : s_Nodes)
		{
			if (node.Type != NodeType::Comment)
				continue;

			const float commentAlpha = 0.75f;

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
			ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
			ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
			ed::BeginNode(node.ID);
			ImGui::PushID(node.ID.AsPointer());
			ImGui::BeginVertical("content");
			ImGui::BeginHorizontal("horizontal");
			ImGui::Spring(1);
			ImGui::TextUnformatted(node.Name.c_str());
			ImGui::Spring(1);
			ImGui::EndHorizontal();
			ed::Group(node.Size);
			ImGui::EndVertical();
			ImGui::PopID();
			ed::EndNode();
			ed::PopStyleColor(2);
			ImGui::PopStyleVar();

			if (ed::BeginGroupHint(node.ID))
			{
				//auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
				auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

				//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

				auto min = ed::GetGroupMin();
				//auto max = ed::GetGroupMax();

				ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
				ImGui::BeginGroup();
				ImGui::TextUnformatted(node.Name.c_str());
				ImGui::EndGroup();

				auto drawList = ed::GetHintBackgroundDrawList();

				auto hintBounds = ImGui_GetItemRect();
				auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

				drawList->AddRectFilled(
					hintFrameBounds.GetTL(),
					hintFrameBounds.GetBR(),
					IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

				drawList->AddRect(
					hintFrameBounds.GetTL(),
					hintFrameBounds.GetBR(),
					IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

				//ImGui::PopStyleVar();
			}
			ed::EndGroupHint();
		}

		for (auto& link : s_Links)
			ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

		if (!createNewNode)
		{
			if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
			{
				auto showLabel = [](const char* label, ImColor color)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
					auto size = ImGui::CalcTextSize(label);

					auto padding = ImGui::GetStyle().FramePadding;
					auto spacing = ImGui::GetStyle().ItemSpacing;

					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

					auto rectMin = ImGui::GetCursorScreenPos() - padding;
					auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

					auto drawList = ImGui::GetWindowDrawList();
					drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
					ImGui::TextUnformatted(label);
				};

				ed::PinId startPinId = 0, endPinId = 0;
				if (ed::QueryNewLink(&startPinId, &endPinId))
				{
					auto startPin = FindPin(startPinId);
					auto endPin = FindPin(endPinId);

					newLinkPin = startPin ? startPin : endPin;

					if (startPin->Kind == PinKind::Input)
					{
						std::swap(startPin, endPin);
						std::swap(startPinId, endPinId);
					}

					if (startPin && endPin)
					{
						if (endPin == startPin)
						{
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}
						else if (endPin->Kind == startPin->Kind)
						{
							showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
							ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
						}
						//else if (endPin->Node == startPin->Node)
						//{
						//    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
						//    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
						//}
						else if (endPin->Type != startPin->Type)
						{
							showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
							ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
						}
						else
						{
							showLabel("+ Create Link", ImColor(32, 45, 32, 180));
							if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
							{
								s_Links.emplace_back(Link(GetNextId(), startPinId, endPinId));
								s_Links.back().Color = GetIconColor(startPin->Type);
							}
						}
					}
				}

				ed::PinId pinId = 0;
				if (ed::QueryNewNode(&pinId))
				{
					newLinkPin = FindPin(pinId);
					if (newLinkPin)
						showLabel("+ Create Node", ImColor(32, 45, 32, 180));

					if (ed::AcceptNewItem())
					{
						createNewNode = true;
						newNodeLinkPin = FindPin(pinId);
						newLinkPin = nullptr;
						ed::Suspend();
						ImGui::OpenPopup("Create New Node");
						ed::Resume();
					}
				}
			}
			else
				newLinkPin = nullptr;

			ed::EndCreate();

			if (ed::BeginDelete())
			{
				ed::LinkId linkId = 0;
				while (ed::QueryDeletedLink(&linkId))
				{
					if (ed::AcceptDeletedItem())
					{
						auto id = std::find_if(s_Links.begin(), s_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
						if (id != s_Links.end())
							s_Links.erase(id);
					}
				}

				ed::NodeId nodeId = 0;
				while (ed::QueryDeletedNode(&nodeId))
				{
					if (ed::AcceptDeletedItem())
					{
						auto id = std::find_if(s_Nodes.begin(), s_Nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
						if (id != s_Nodes.end())
							s_Nodes.erase(id);
					}
				}
			}
			ed::EndDelete();
		}

		ImGui::SetCursorScreenPos(cursorTopLeft);
	}

# if 1
	auto openPopupPosition = ImGui::GetMousePos();
	ed::Suspend();
	if (ed::ShowNodeContextMenu(&contextNodeId))
		ImGui::OpenPopup("Node Context Menu");
	else if (ed::ShowPinContextMenu(&contextPinId))
		ImGui::OpenPopup("Pin Context Menu");
	else if (ed::ShowLinkContextMenu(&contextLinkId))
		ImGui::OpenPopup("Link Context Menu");
	else if (ed::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create New Node");
		newNodeLinkPin = nullptr;
	}
	ed::Resume();

	ed::Suspend();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("Node Context Menu"))
	{
		auto node = FindNode(contextNodeId);

		ImGui::TextUnformatted("Node Context Menu");
		ImGui::Separator();
		if (node)
		{
			ImGui::Text("ID: %p", node->ID.AsPointer());
			ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
			ImGui::Text("Inputs: %d", (int)node->Inputs.size());
			ImGui::Text("Outputs: %d", (int)node->Outputs.size());
		}
		else
			ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
		ImGui::Separator();
		if (ImGui::MenuItem("Delete"))
			ed::DeleteNode(contextNodeId);
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Pin Context Menu"))
	{
		auto pin = FindPin(contextPinId);

		ImGui::TextUnformatted("Pin Context Menu");
		ImGui::Separator();
		if (pin)
		{
			ImGui::Text("ID: %p", pin->ID.AsPointer());
			if (pin->Node)
				ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
			else
				ImGui::Text("Node: %s", "<none>");
		}
		else
			ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Link Context Menu"))
	{
		auto link = FindLink(contextLinkId);

		ImGui::TextUnformatted("Link Context Menu");
		ImGui::Separator();
		if (link)
		{
			ImGui::Text("ID: %p", link->ID.AsPointer());
			ImGui::Text("From: %p", link->StartPinID.AsPointer());
			ImGui::Text("To: %p", link->EndPinID.AsPointer());
		}
		else
			ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
		ImGui::Separator();
		if (ImGui::MenuItem("Delete"))
			ed::DeleteLink(contextLinkId);
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Create New Node"))
	{
		auto newNodePostion = openPopupPosition;
		//ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

		//auto drawList = ImGui::GetWindowDrawList();
		//drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

		Node* node = nullptr;
		if (ImGui::MenuItem("Input Action"))
			node = SpawnInputActionNode();
		if (ImGui::MenuItem("Output Action"))
			node = SpawnOutputActionNode();
		if (ImGui::MenuItem("Branch"))
			node = SpawnBranchNode();
		if (ImGui::MenuItem("Do N"))
			node = SpawnDoNNode();
		if (ImGui::MenuItem("Set Timer"))
			node = SpawnSetTimerNode();
		if (ImGui::MenuItem("Less"))
			node = SpawnLessNode();
		if (ImGui::MenuItem("Weird"))
			node = SpawnWeirdNode();
		if (ImGui::MenuItem("Trace by Channel"))
			node = SpawnTraceByChannelNode();
		if (ImGui::MenuItem("Print String"))
			node = SpawnPrintStringNode();
		ImGui::Separator();
		if (ImGui::MenuItem("Comment"))
			node = SpawnComment();
		ImGui::Separator();
		if (ImGui::MenuItem("Sequence"))
			node = SpawnTreeSequenceNode();
		if (ImGui::MenuItem("Move To"))
			node = SpawnTreeTaskNode();
		if (ImGui::MenuItem("Random Wait"))
			node = SpawnTreeTask2Node();
		ImGui::Separator();
		if (ImGui::MenuItem("Message"))
			node = SpawnMessageNode();
		ImGui::Separator();
		if (ImGui::MenuItem("Transform"))
			node = SpawnHoudiniTransformNode();
		if (ImGui::MenuItem("Group"))
			node = SpawnHoudiniGroupNode();

		if (node)
		{
			BuildNodes();

			createNewNode = false;

			ed::SetNodePosition(node->ID, newNodePostion);

			if (auto startPin = newNodeLinkPin)
			{
				auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

				for (auto& pin : pins)
				{
					if (CanCreateLink(startPin, &pin))
					{
						auto endPin = &pin;
						if (startPin->Kind == PinKind::Input)
							std::swap(startPin, endPin);

						s_Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
						s_Links.back().Color = GetIconColor(startPin->Type);

						break;
					}
				}
			}
		}

		ImGui::EndPopup();
	}
	else
		createNewNode = false;
	ImGui::PopStyleVar();
	ed::Resume();
# endif
}

const float TruthEngine::NodeEditor::s_TouchTime = 1.0f;

std::vector<TruthEngine::NodeEditor::Node> TruthEngine::NodeEditor::s_Nodes;

std::vector<TruthEngine::NodeEditor::Link> TruthEngine::NodeEditor::s_Links;

ImTextureID TruthEngine::NodeEditor::s_HeaderBackground = nullptr;

ImTextureID TruthEngine::NodeEditor::s_SaveIcon = nullptr;

ImTextureID TruthEngine::NodeEditor::s_RestoreIcon = nullptr;

std::map<ImGuiNodeEditor::NodeId, float, TruthEngine::NodeEditor::NodeIdLess> TruthEngine::NodeEditor::s_NodeTouchTime;
