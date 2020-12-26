#pragma once
#include "Core/Layer.h"
//#include "API/DX12/DescriptorHeap.h"

#include "imgui-fileBrowser/imfilebrowser.h"


namespace TruthEngine::Core
{

	class TextureRenderTarget;

	class ImGuiLayer: public Layer
	{


	public:

		static std::shared_ptr<TruthEngine::Core::ImGuiLayer> Factory();


		virtual ~ImGuiLayer() = default;


		virtual void OnAttach() override = 0;


		virtual void OnDetach() override = 0;


		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void OnSceneViewportResize() = 0;

		void OpenFileDialog(const char* title, const std::vector<const char*>& FileExtensions);
		bool CheckFileDialog();

	protected:
		TextureRenderTarget* m_RenderTargetScreenBuffer;
		ImGui::FileBrowser m_FileBrowser;
		std::string m_SelectedFile;
	};
}
