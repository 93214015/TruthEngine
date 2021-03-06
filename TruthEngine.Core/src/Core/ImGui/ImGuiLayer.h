#pragma once
#include "Core/Layer.h"
//#include "API/DX12/DescriptorHeap.h"

#include "imgui-fileBrowser/imfilebrowser.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Event/EventRenderer.h"

namespace TruthEngine
{

	class TextureRenderTarget;

	void TEImGuiRenderImage(uint32_t viewIndex, const float2& size, const float2& uv0 = { 0.0f, 0.0f }, const float2& uv1 = {1.0f, 1.0f});
	void TEImGuiRenderImage_MaterialTexture(uint32_t viewIndex, const float2& size, const float2& uv0 = { 0.0f, 0.0f }, const float2& uv1 = { 1.0f, 1.0f });



	class ImGuiLayer: public Layer
	{


	public:

		static std::shared_ptr<TruthEngine::ImGuiLayer> Factory();


		virtual ~ImGuiLayer() = default;


		virtual void OnAttach() override = 0;


		virtual void OnDetach() override = 0;


		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void OnRendererTextureResize(const EventRendererTextureResize& event) = 0;
		virtual void OnRendererViewportResize(const EventRendererViewportResize& event) = 0;

		//virtual void RenderSceneViewport() = 0;
		virtual void RenderSceneViewport(const ImVec2& viewportSize) = 0;

		void OpenFileDialog(ImGui::FileBrowser* fileBrowser, const char* title, const std::vector<const char*>& FileExtensions);
		void OpenFileDialog_MultipleSelection(ImGui::FileBrowser* fileBrowser, const char* title, const std::vector<const char*>& FileExtensions);
		bool CheckFileDialog(ImGui::FileBrowser* fileBrowser, std::string& outSelectedFile);
		bool CheckFileDialog_MultipleSelection(ImGui::FileBrowser* fileBrowser, std::vector<std::filesystem::path>& outSelectedFile);


		inline void ShowDemoWindow(bool show)
		{
			m_ShowDemoWindow = show;
		}

		static void ShowWindowMaterialTexture(const std::function<void(uint32_t)>& callback, bool show = true);
		static bool DrawFloat3Control(const char* label, float* value, float minValue = 0.0f, float maxValue = 0.0f, float speed = 1.0f, float resetValue = 0.0f, float columnWidth = 100.0f);
		static bool DrawFloat2ControlUV(const char* label, float2* value, bool chainedUV = true, float resetValue = 0.0f, float columnWidth = 100.0f);

	protected:
		bool WindowMaterialTextures();

	protected:
		TextureRenderTarget* m_RenderTargetScreenBuffer;
		RenderTargetView m_RTVBackBuffer;
		ImGui::FileBrowser m_FileBrowser;
		std::string m_SelectedFile;
		bool m_ShowDemoWindow = false;

		static bool m_ShowMaterialTextureWindow;

	};
}
