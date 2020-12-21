#pragma once
#include "Core/Layer.h"

#include "BufferManager.h"
#include "RendererCommand.h"

#include "RenderPass/RenderPass_ForwardRendering.h"


namespace TruthEngine::Core
{
	class ModelManager;
	class ImGuiLayer;
	class EventWindowResize;

	template<class T> class ConstantBufferUpload;
	class ConstantBuffer_Data_Per_Frame;

	class RendererLayer : public Layer
	{

	public:

		RendererLayer();
		virtual ~RendererLayer();


		RendererLayer(const RendererLayer& renderer);
		RendererLayer& operator=(const RendererLayer& renderer);


		void OnAttach() override;

		void OnDetach() override;

		void OnUpdate(double deltaFrameTime) override;

		bool BeginImGuiLayer();
		void EndImGuiLayer();

		void BeginRendering();
		void EndRendering();


	private:
		void OnResize(const EventWindowResize& event);

		void InitGPUResource();

	private:
		RendererCommand m_RendererCommand;

		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

		std::shared_ptr<RenderPass_ForwardRendering> m_Renderer3D;

		std::shared_ptr<ModelManager> m_ModelManagers;

		std::shared_ptr<BufferManager> m_BufferManager;

		std::vector<const Model3D*> m_Model3DQueue;

		RenderTargetView m_RTVBackBuffer;

		ConstantBufferUpload<ConstantBuffer_Data_Per_Frame>* m_CB_PerFrame;

		bool m_EnabledImGuiLayer = true;

	};

}
