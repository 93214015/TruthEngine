#include "pch.h"
#include "ImGuiLayer.h"

#include "API/DX12/DirectX12ImGuiLayer.h"



namespace TruthEngine::Core
{



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

	void ImGuiLayer::OpenFileDialog(const char* title, const std::vector<const char*>& FileExtensions)
	{
		
		m_FileBrowser.SetTitle(title);
		m_FileBrowser.SetTypeFilters(FileExtensions);

		m_FileBrowser.Open();		
	}

	bool ImGuiLayer::CheckFileDialog()
	{
		if (m_FileBrowser.HasSelected())
		{
			m_SelectedFile = m_FileBrowser.GetSelected().string();
			m_FileBrowser.ClearSelected();
			return true;
		}
		return false;
	}

}