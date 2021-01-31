#include "pch.h"
#include "ImGuiLayer.h"

#include "API/DX12/DirectX12ImGuiLayer.h"
#include "API/DX12/DirectX12BufferManager.h"
#include "API/DX12/DirectX12TextureMaterialManager.h"

#include "Core/Renderer/TextureMaterialManager.h"
#include "Core/Renderer/TextureMaterial.h"


namespace TruthEngine::Core
{

	std::function<void(uint32_t)> g_MaterialTextureWindowCallBack;

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


	void ImGuiLayer::OpenFileDialog_MultipleSelection(ImGui::FileBrowser* fileBrowser, const char* title, const std::vector<const char*>& FileExtensions)
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


	bool ImGuiLayer::CheckFileDialog_MultipleSelection(ImGui::FileBrowser* fileBrowser, std::vector<std::filesystem::path>& outSelectedFile)
	{
		outSelectedFile.clear();
		fileBrowser->Display();
		if (fileBrowser->HasSelected())
		{
			outSelectedFile = fileBrowser->GetMultiSelected();
			fileBrowser->ClearSelected();
			return true;
		}
		return false;
	}


	void ImGuiLayer::ShowWindowMaterialTexture(const std::function<void(uint32_t)>& callback, bool show)
	{
		m_ShowMaterialTextureWindow = show;

		g_MaterialTextureWindowCallBack = callback;

	}


	bool ImGuiLayer::WindowMaterialTextures()
	{
		static ImGui::FileBrowser s_fileDialogImportMaterialTextures = ImGui::FileBrowser(ImGuiFileBrowserFlags_MultipleSelection);


		if (m_ShowMaterialTextureWindow)
		{
			//m_ShowMaterialTextureWindow = false;
			ImGui::OpenPopup("MaterialTextureWindow");
		}


		if (ImGui::BeginPopupModal("MaterialTextureWindow", &m_ShowMaterialTextureWindow, ImGuiWindowFlags_MenuBar))
		{
			static uint32_t s_selectedTextureID = -1;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Import Texture"))
				{
					static const std::vector<const char*> s_textureFileExtensions = { "*.*", ".jpg", ".jpeg", "*.png" };
					OpenFileDialog(&s_fileDialogImportMaterialTextures, "Import Material Textures", s_textureFileExtensions);

					ImGui::EndMenu();
				}
				if (s_selectedTextureID != -1)
				{
					if (ImGui::BeginMenu("Select"))
					{
						if (s_selectedTextureID != -1)
						{
							g_MaterialTextureWindowCallBack(s_selectedTextureID);
						}

						m_ShowMaterialTextureWindow = false;

						ImGui::EndMenu();
					}
				}
				if (ImGui::BeginMenu("Cancel"))
				{
					//ImGui::CloseCurrentPopup();
					m_ShowMaterialTextureWindow = false;

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			auto materialTextures = TextureMaterialManager::GetInstance()->GetAllTextures();

			static float ItemWidth = 100;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 10.0f, 10.0f });

			auto availContentWidth = ImGui::GetContentRegionAvailWidth();

			float currentPos = 0;

			if (ImGui::BeginChildFrame(ImGui::GetID("materialTexturesContainer"), ImVec2{ availContentWidth , 0 }))
			{


				for (auto& tex : materialTextures)
				{
					if ((currentPos + ItemWidth) < availContentWidth)
					{
						ImGui::SameLine();
					}
					else
					{
						currentPos = 0;
					}

					auto texViewIndex = tex->GetViewIndex();

					bool selected = false;
					if (s_selectedTextureID == texViewIndex)
						selected = true;

					ImGui::PushID(texViewIndex);
					ImGui::BeginGroup();
					TEImGuiRenderImage_MaterialTexture(tex->GetViewIndex(), float2{ ItemWidth, ItemWidth });
					ImGui::SameLine(70.0f);
					ImGui::PushItemWidth(30.0f);
					ImGui::Checkbox("", &selected);
					ImGui::EndGroup();
					if (ImGui::IsItemClicked())
					{
						if (!selected)
							s_selectedTextureID = texViewIndex;
						else
							s_selectedTextureID = -1;
					}
					ImGui::PopID();

					currentPos += ItemWidth;
				}

				ImGui::EndChildFrame();
			}

			ImGui::PopStyleVar();


			static std::vector<std::filesystem::path> s_selectedTextureFiles;
			if (CheckFileDialog_MultipleSelection(&s_fileDialogImportMaterialTextures, s_selectedTextureFiles))
			{
				for (const auto& filePath : s_selectedTextureFiles)
				{
					TextureMaterialManager::GetInstance()->CreateTexture(filePath.generic_string().c_str(), "");
				}
			}

			ImGui::EndPopup();
		}


		return m_ShowMaterialTextureWindow;
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

	bool ImGuiLayer::DrawFloat2ControlUV(const char* label, float2* value, bool chainedUV, float resetValue, float columnWidth)
	{
		bool active = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("U", buttonSize))
		{
			value->x = resetValue;
			active = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##U", &value->x, .01f, .0f, 100.0f, "%.2f"))
		{
			if (chainedUV)
				value->y = value->x;
			active = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("V", buttonSize))
		{
			value->y = resetValue;
			active = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##V", &value->y, .01f, .0f, 100.0f, "%.2f"))
		{
			if (chainedUV)
				value->x = value->y;
			active = true;
		}
		ImGui::PopItemWidth();		

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return active;
	}




	bool ImGuiLayer::m_ShowMaterialTextureWindow = false;

}