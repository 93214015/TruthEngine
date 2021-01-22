#include "pch.h"
#include "ImGuiLayer.h"

#include "API/DX12/DirectX12ImGuiLayer.h"
#include "API/DX12/DirectX12BufferManager.h"
#include "../Renderer/TextureMaterialManager.h"
#include "API/DX12/DirectX12TextureMaterialManager.h"



namespace TruthEngine::Core
{

	void TEImGuiRenderImage(uint32_t viewIndex, const float2& size, const float2& uv0, const float2& uv1)
	{
		ImTextureID id;

		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12:
		{
			static auto dx12BufferManager = API::DirectX12::DirectX12BufferManager::GetInstance().get();
			id = (ImTextureID)dx12BufferManager->GetDescriptorHeapSRV().GetGPUHandle(viewIndex).ptr;
			break;
		}
		default:
			throw;
		}

		ImGui::Image(id, size, uv0, uv1);
	}

	void TEImGuiRenderImage_MaterialTexture(uint32_t viewIndex, const float2& size, const float2& uv0, const float2& uv1)
	{
		ImTextureID id;
		static TextureMaterialManager* _textureMaterialManager = TextureMaterialManager::GetInstance();

		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12:
		{
			static auto dx12BufferManager = API::DirectX12::DirectX12BufferManager::GetInstance().get();
			static auto offset = static_cast<API::DirectX12::DirectX12TextureMaterialManager*>(_textureMaterialManager)->GetIndexOffset();
			id = (ImTextureID)dx12BufferManager->GetDescriptorHeapSRV().GetGPUHandle(offset + viewIndex).ptr;
			break;
		}
		default:
			throw;
		}

		ImGui::Image(id, size, uv0, uv1);
	}


	std::shared_ptr<TruthEngine::Core::ImGuiLayer> ImGuiLayer::Factory()
	{
		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12:
			return std::make_shared<API::DirectX12::DirectX12ImGuiLayer>();
		default:
			return nullptr;
		}
	}

	void ImGuiLayer::OpenFileDialog(ImGui::FileBrowser* fileBrowser, const char* title, const std::vector<const char*>& FileExtensions)
	{
		
		fileBrowser->SetTitle(title);
		fileBrowser->SetTypeFilters(FileExtensions);

		fileBrowser->Open();
	}

	bool ImGuiLayer::CheckFileDialog(ImGui::FileBrowser* fileBrowser, std::string& outSelectedFile)
	{
		fileBrowser->Display();
		if (fileBrowser->HasSelected())
		{
			outSelectedFile = fileBrowser->GetSelected().string();
			fileBrowser->ClearSelected();
			return true;
		}
		return false;
	}


	bool ImGuiLayer::DrawFloat3Control(const char* label, float* value, float minValue /* = 0.0f*/, float maxValue /* = 0.0f*/, float speed /* = 1.0f*/, float resetValue /*= 0.0f*/, float columnWidth /*= 100.0f*/)
	{
		bool active = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			value[0] = resetValue;
			active = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value[0], speed, minValue, maxValue, "%.2f"))
		{
			active = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			value[1] = resetValue;
			active = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value[1], speed, minValue, maxValue, "%.2f"))
		{
			active = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			value[2] = resetValue;
			active = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &value[2], speed, minValue, maxValue, "%.2f"))
		{
			active = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return active;
	}

}