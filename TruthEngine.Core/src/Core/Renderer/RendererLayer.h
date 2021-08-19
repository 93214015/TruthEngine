#pragma once
#include "Core/Layer.h"

#include "BufferManager.h"
#include "RendererCommand.h"
#include "RenderPassStack.h"

#include "RenderPass/RenderPass_ForwardRendering.h"
#include "RenderPass/RenderPass_GenerateShadowMap.h"
#include "RenderPass/RenderPass_PostProcessing_HDR.h"
#include "RenderPass/RenderPass_GenerateGBuffers.h"
#include "RenderPass/RenderPass_DeferredShading.h"
#include "RenderPass/RenderPass_GenerateCubeMap.h"
#include "RenderPass/RenderPass_GenerateIBL.h"


namespace TruthEngine
{
	class ModelManager;
	class ImGuiLayer;
	class EventWindowResize;
	class EventSceneViewportResize;
	class EventEntityAddMaterial;
	class EventEntityUpdateMaterial;
	class EventEntityUpdateLight;
	class EventEntityAddLight;

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

		void SetEnabledEnvironmentMap(bool _EnabledEnvironmentMap);

		const float3& GetAmbientLightStrength() const;
		void SetAmbientLightStrength(const float3& _AmbientLightStrength);


		const float3& GetEnvironmentMapMultiplier()const;
		const void SetEnvironmentMapMultiplier(const float3& _EnvironmentMapMultiplier);
		


		inline bool IsEnvironmentMapEnabled()const noexcept
		{
			return m_EnabledEnvironmentMap;
		}

		inline bool IsEnabledHDR()const noexcept
		{
			return m_IsEnabledHDR;
		}

		void SetHDR(bool _EnableHDR);

	private:
		void RegisterEvents();

		void OnWindowResize(const EventWindowResize& event);
		void OnSceneViewportResize(const EventSceneViewportResize& event);
		void OnAddMaterial(const EventEntityAddMaterial& event);
		void OnUpdateMaterial(const EventEntityUpdateMaterial& event);
		void OnUpdateLight(const EventEntityUpdateLight& event);
		void OnAddLight(const EventEntityAddLight& event);

		void _ChangeUnfrequentBuffer_LightDirectionalCount(uint32_t _LightDirectionalCount);
		void _ChangeUnfrequentBuffer_LightSpotCount(uint32_t _LightSpotCount);

		void InitRenderPasses();
		void InitTextures();
		void InitBuffers();

	private:
		RendererCommand m_RendererCommand;
		RendererCommand m_RendererCommand_BackBuffer;


		RenderPassStack m_RenderPassStack;

		std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

		std::shared_ptr<RenderPass_ForwardRendering> m_RenderPass_ForwardRendering;
		std::shared_ptr<RenderPass_GenerateShadowMap> m_RenderPass_GenerateShadowMap;
		std::shared_ptr<RenderPass_PostProcessing_HDR> m_RenderPass_PostProcessing_HDR;
		std::shared_ptr<RenderPass_GenerateGBuffers> m_RenderPass_GenerateGBuffers;
		std::shared_ptr<RenderPass_DeferredShading> m_RenderPass_DeferredShading;
		std::shared_ptr<RenderPass_GenerateCubeMap> m_RenderPass_GenerateCubeMap;
		std::shared_ptr<RenderPass_GenerateIBL> m_RenderPass_GenerateIBL;

		ModelManager* m_ModelManagers;

		BufferManager* m_BufferManager;

		std::vector<const Model3D*> m_Model3DQueue;

		RenderTargetView m_RTVBackBuffer, m_RTVSceneBuffer, m_RTVSceneBufferHDR;
		DepthStencilView m_DSVSceneBuffer;

		ConstantBufferUpload<ConstantBuffer_Data_Per_Frame>* m_CB_PerFrame;
		ConstantBufferUpload<ConstantBuffer_Data_LightData>* m_CB_LightData;
		ConstantBufferUpload<ConstantBuffer_Data_Materials>* m_CB_Materials;
		ConstantBufferUpload<ConstantBuffer_Data_UnFrequent>* m_CB_UnFrequent;
		ConstantBufferUpload<ConstantBuffer_Data_Bones>* m_CB_Bones;

		ConstantBufferDirect<ConstantBuffer_Data_EnvironmentMap>* m_CB_EnvironmentMap;

		std::map<int, int> m_Map_DLightToCBuffer;
		std::map<int, int> m_Map_SLightToCBuffer;

		bool m_EnabledImGuiLayer = true;
		bool m_EnabledEnvironmentMap = false;
		bool m_IsEnabledHDR = false;

		TimerProfile_OneSecond m_TimerRenderLayerUpdate;

	};

}
