#pragma once

#ifdef IMGUI_DISABLE
#undef IMGUI_DISABLE
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace ImGui 
{

	//// Stack Layout
	//static ImGuiLayout* FindLayout(ImGuiID id, ImGuiLayoutType type);
	//static ImGuiLayout* CreateNewLayout(ImGuiID id, ImGuiLayoutType type, ImVec2 size);
	//static void             BeginLayout(ImGuiID id, ImGuiLayoutType type, ImVec2 size, float align);
	//static void             EndLayout(ImGuiLayoutType type);
	//static void             PushLayout(ImGuiLayout* layout);
	//static void             PopLayout(ImGuiLayout* layout);
	//static void             BalanceLayoutSprings(ImGuiLayout& layout);
	//static ImVec2           BalanceLayoutItemAlignment(ImGuiLayout& layout, ImGuiLayoutItem& item);
	//static void             BalanceLayoutItemsAlignment(ImGuiLayout& layout);
	//static void             BalanceChildLayouts(ImGuiLayout& layout);
	//static ImVec2           CalculateLayoutSize(ImGuiLayout& layout, bool collapse_springs);
	//static ImGuiLayoutItem* GenerateLayoutItem(ImGuiLayout& layout, ImGuiLayoutItemType type);
	//static float            CalculateLayoutItemAlignmentOffset(ImGuiLayout& layout, ImGuiLayoutItem& item);
	//static void             TranslateLayoutItem(ImGuiLayoutItem& item, const ImVec2& offset);
	//static void             BeginLayoutItem(ImGuiLayout& layout);
	//static void             EndLayoutItem(ImGuiLayout& layout);
	//static void             AddLayoutSpring(ImGuiLayout& layout, float weight, float spacing);
	//static void             SignedIndent(float indent);

}