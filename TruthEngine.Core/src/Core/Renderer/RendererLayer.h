#pragma once
#include "Core/Layer.h"

#include "Renderer3D.h"
#include "BufferManager.h"
#include "RendererCommand.h"



namespace TruthEngine::Core
{
	class ModelManager;
	class ImGuiLayer;

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


	private:
		RendererCommand m_RendererCommand;

		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

		std::shared_ptr<Renderer3D> m_Renderer3D;

		std::shared_ptr<ModelManager> m_ModelManagers;

		std::shared_ptr<BufferManager> m_BufferManager;

		std::vector<const Model3D*> m_Model3DQueue;

		RenderTargetView m_RTVBackBuffer;

		bool m_EnabledImGuiLayer = true;

	};

}
