#pragma once
#include "Core/Layer.h"

#include "BufferManager.h"
#include "RendererCommand.h"
#include "RenderPassStack.h"

#include "RenderPass/RenderPass_ForwardRendering.h"
#include "RenderPass/RenderPass_GenerateShadowMap.h"


namespace TruthEngine
{
	class ModelManager;
	class ImGuiLayer;
	class EventWindowResize;
	class EventSceneViewportResize;
	class EventEntityAddMaterial;
	class EventEntityUpdateMaterial;
	class EventEntityUpdateLight;

	template<class T> class ConstantBufferUpload;
	struct ConstantBuffer_Data_Per_Frame;

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

		void OnImGuiRender();

		inline ImGuiLayer* GetImGuiLayer()
		{
			return m_ImGuiLayer.get();
		}


	private:
		void RegisterEvents();

		void OnWindowResize(const EventWindowResize& event);
		void OnSceneViewportResize(const EventSceneViewportResize& event);
		void OnAddMaterial(const EventEntityAddMaterial& event);
		void OnUpdateMaterial(const EventEntityUpdateMaterial& event);
		void OnUpdateLight(const EventEntityUpdateLight& event);

		void InitTextures();
		void InitBuffers();

	private:
		RendererCommand m_RendererCommand;

		RenderPassStack m_RenderPassStack;

		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

		std::shared_ptr<RenderPass_ForwardRendering> m_RenderPass_ForwardRendering;
		std::shared_ptr<RenderPass_GenerateShadowMap> m_RenderPass_GenerateShadowMap;

		std::shared_ptr<ModelManager> m_ModelManagers;

		BufferManager* m_BufferManager;

		std::vector<const Model3D*> m_Model3DQueue;

		RenderTargetView m_RTVBackBuffer, m_RTVSceneBuffer;

		ConstantBufferUpload<ConstantBuffer_Data_Per_Frame>* m_CB_PerFrame;
		ConstantBufferUpload<ConstantBuffer_Data_Per_DLight>* m_CB_PerDLight;
		ConstantBufferUpload<ConstantBuffer_Data_Materials>* m_CB_Materials;

		bool m_EnabledImGuiLayer = true;

		TimerProfile_OneSecond m_TimerRenderLayerUpdate;

	};

}
