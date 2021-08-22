#include "pch.h"
#include "RendererLayer.h"

#include "SwapChain.h"
#include "ConstantBuffer.h"


#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Event/EventApplication.h"
#include "Core/Event/EventRenderer.h"
#include "Core/Event/EventEntity.h"
#include "Core/Renderer/GraphicDevice.h"

#include "Core/AnimationEngine/AnimationManager.h"

#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Entity/Camera/CameraPerspective.h"

#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Light/LightDirectional.h"

#include "Core/Timer.h"
#include "Core/ThreadPool.h"

namespace TruthEngine
{

	RendererLayer::RendererLayer() : m_ImGuiLayer(ImGuiLayer::Factory())
		, m_RenderPass_ForwardRendering(std::make_shared<RenderPass_ForwardRendering>(this))
		, m_RenderPass_GenerateShadowMap(std::make_shared<RenderPass_GenerateShadowMap>(this, 4096))
		, m_RenderPass_PostProcessing_HDR(std::make_shared<RenderPass_PostProcessing_HDR>(this))
		, m_RenderPass_GenerateGBuffers(std::make_shared<RenderPass_GenerateGBuffers>(this))
		, m_RenderPass_DeferredShading(std::make_shared<RenderPass_DeferredShading>(this))
		, m_RenderPass_GenerateCubeMap(std::make_shared<RenderPass_GenerateCubeMap>(this))
		, m_RenderPass_GenerateIBLAmbient(std::make_shared<RenderPass_GenerateIBLAmbient>(this))
		, m_RenderPass_GenerateIBLSpecular(std::make_shared<RenderPass_GenerateIBLSpecular>(this))
	{
	}
	RendererLayer::~RendererLayer() = default;

	RendererLayer::RendererLayer(const RendererLayer& renderer) = default;
	RendererLayer& RendererLayer::operator=(const RendererLayer& renderer) = default;


	void RendererLayer::OnAttach()
	{
		//Settings::SetMSAA(TE_SETTING_MSAA::X4);


		m_BufferManager = TE_INSTANCE_BUFFERMANAGER;

		m_BufferManager->Init(1000, 1000, 20, 10);

		// init singleton object of dx12 swap chain
		TE_INSTANCE_SWAPCHAIN->Init(TE_INSTANCE_APPLICATION->GetClientWidth(), TE_INSTANCE_APPLICATION->GetClientHeight(), TE_INSTANCE_APPLICATION->GetWindow(), TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum());

		m_RendererCommand.Init(TE_IDX_RENDERPASS::NONE, TE_IDX_SHADERCLASS::NONE);
		m_RendererCommand_BackBuffer.Init(TE_IDX_RENDERPASS::NONE, TE_IDX_SHADERCLASS::NONE);

		m_ModelManagers = TE_INSTANCE_MODELMANAGER;

		InitTextures();
		InitBuffers();

		/*m_RendererCommand.AddUpdateTask([&CB_LightData = m_CB_LightData]()
			{
				auto _lightManager = LightManager::GetInstace();
				auto _light0 = _lightManager->GetDirectionalLight("dlight_0");
				const auto& lightdata = _light0->GetDirectionalLightData();
				*(CB_LightData->GetData()) = ConstantBuffer_Data_LightData(lightdata.Diffuse, lightdata.Ambient, lightdata.Specular, lightdata.Direction, lightdata.LightSize, lightdata.Position, static_cast<uint32_t>(lightdata.CastShadow), lightdata.Range, _lightManager->GetShadowTransform(_light0));
		});*/

		m_ImGuiLayer->OnAttach();

		InitRenderPasses();

		RegisterEvents();
	}

	void RendererLayer::OnDetach()
	{
		m_RenderPassStack.PopAll();

		m_ImGuiLayer->OnDetach();

		TE_INSTANCE_SWAPCHAIN->Release();

		m_BufferManager->Release();
	}

	void RendererLayer::OnUpdate(double deltaFrameTime)
	{

		m_TimerRenderLayerUpdate.Start();

		auto data_perFrame = m_CB_PerFrame->GetData();

		auto activeCamera = CameraManager::GetInstance()->GetActiveCamera();

		float4x4A _cascadedShadowTransforms[4];

		auto _lightManager = LightManager::GetInstace();
		static auto _dirLight0 = _lightManager->GetDirectionalLight("SunLight");
		if (_dirLight0)
		{
			auto cameraCascaded = _dirLight0->GetCamera();
			cameraCascaded->UpdateFrustums(GetActiveScene(), activeCamera, true);
			_lightManager->GetCascadedShadowTransform(cameraCascaded, _cascadedShadowTransforms);
		}


		*data_perFrame = ConstantBuffer_Data_Per_Frame(activeCamera->GetViewProj(), activeCamera->GetViewInv(), activeCamera->GetProjectionValues(), activeCamera->GetPosition(), _cascadedShadowTransforms);

		//
		////Use MultiThreaded Rendering
		//
		static std::vector<std::future<void>> m_futures;
		m_futures.clear();
		for (auto renderPass : m_RenderPassStack)
		{
			renderPass->OnUpdate(deltaFrameTime);
			renderPass->BeginScene();
			auto f = [renderPass]() { renderPass->Render(); };
			m_futures.emplace_back(std::move(ThreadPool::GetInstance()->Queue(f)));
		}

		for (auto& f : m_futures)
		{
			f.wait();
		}

		for (auto renderPass : m_RenderPassStack)
		{
			renderPass->EndScene();
		}


		//
		////Use SingleThreaded Rendering
		//
		/*for (auto renderPass : m_RenderPassStack)
		{
			renderPass->BeginScene();
			renderPass->Render();
			renderPass->EndScene();
		}*/



		m_TimerRenderLayerUpdate.End();
	}

	bool RendererLayer::BeginImGuiLayer()
	{
		if (m_EnabledImGuiLayer)
		{
			m_ImGuiLayer->Begin();
		}
		return m_EnabledImGuiLayer;
	}

	void RendererLayer::EndImGuiLayer()
	{
		if (m_EnabledImGuiLayer)
		{
			m_ImGuiLayer->End();
		}
	}

	void RendererLayer::BeginRendering()
	{

		if (SA_Animation* _DefaultAnimation = TE_INSTANCE_ANIMATIONMANAGER->GetAnimation(0); _DefaultAnimation)
		{

			auto _AnimationTransforms = _DefaultAnimation->GetTransform();

			m_RendererCommand.AddUpdateTaskJustCurrentFrame([_AnimationTransforms, this]()
				{
					auto _Dest = m_CB_Bones->GetData()->mBones;
					auto _Src = _AnimationTransforms->data();
					auto _Size = sizeof(float4x4) * _AnimationTransforms->size();
					memcpy(_Dest, _Src, _Size);
				});
		}

		auto swapchain = TE_INSTANCE_SWAPCHAIN;
		m_RendererCommand_BackBuffer.BeginGraphics();
		m_RendererCommand_BackBuffer.SetRenderTarget(swapchain, m_RTVBackBuffer);
		m_RendererCommand_BackBuffer.ClearRenderTarget(swapchain, m_RTVBackBuffer);
		m_RendererCommand_BackBuffer.End();


		m_RendererCommand.BeginGraphics();

		m_RendererCommand.SetRenderTarget(m_RTVSceneBuffer);
		m_RendererCommand.SetRenderTarget(m_RTVSceneBufferHDR);
		m_RendererCommand.SetDepthStencil(m_DSVSceneBuffer);

		m_RendererCommand.ClearRenderTarget(m_RTVSceneBuffer);
		m_RendererCommand.ClearRenderTarget(m_RTVSceneBufferHDR);
		m_RendererCommand.ClearDepthStencil(m_DSVSceneBuffer);

		m_RendererCommand.End();
	}

	void RendererLayer::EndRendering()
	{
		m_RendererCommand.BeginGraphics();
		m_RendererCommand.EndAndPresent();
	}

	void RendererLayer::OnImGuiRender()
	{
		for (auto renderPass : m_RenderPassStack)
		{
			renderPass->OnImGuiRender();
		}

		if (ImGui::Begin("RendererLayer"))
		{
			ImGui::Text("RendererLayer Update Time: %0.3f ms", m_TimerRenderLayerUpdate.GetAverageTime());
		}
		ImGui::End();
	}

	void RendererLayer::SetHDR(bool _EnableHDR)
	{
		m_IsEnabledHDR = _EnableHDR;

		InitRenderPasses();
	}

	void RendererLayer::RegisterEvents()
	{
		auto listener_windowResize = [this](Event& event) {  OnWindowResize(static_cast<EventWindowResize&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::WindowResize, listener_windowResize);

		auto listener_sceneViewportResize = [this](Event& event) {  OnSceneViewportResize(static_cast<EventSceneViewportResize&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::SceneViewportResize, listener_sceneViewportResize);

		auto listener_addMaterial = [this](Event& event) { OnAddMaterial(static_cast<EventEntityAddMaterial&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityAddMaterial, listener_addMaterial);

		auto listener_updateMaterial = [this](Event& event) { OnUpdateMaterial(static_cast<EventEntityUpdateMaterial&>(event)); };
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityUpdatedMaterial, listener_updateMaterial);

		auto listener_updateLight = [this](Event& event)
		{
			OnUpdateLight(static_cast<EventEntityUpdateLight&>(event));
		};
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityUpdatedLight, listener_updateLight);

		auto _Listener_AddLight = [this](Event& event)
		{
			OnAddLight(static_cast<EventEntityAddLight&>(event));
		};
		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::EntityAddLight, _Listener_AddLight);
	}

	void RendererLayer::OnWindowResize(const EventWindowResize& event)
	{
		GraphicDevice::GetPrimaryDevice()->WaitForGPU();

		auto width = event.GetWidth();
		auto height = event.GetHeight();

		m_RendererCommand.ResizeSwapChain(TE_INSTANCE_SWAPCHAIN, width, height, &m_RTVBackBuffer, nullptr);

		EventRendererTextureResize _EventRendererTextureResize(width, height, TE_IDX_GRESOURCES::Texture_RT_BackBuffer);
		TE_INSTANCE_APPLICATION->OnEvent(_EventRendererTextureResize);

	}

	void RendererLayer::OnSceneViewportResize(const EventSceneViewportResize& event)
	{
		auto width = event.GetWidth();
		auto height = event.GetHeight();

		m_RendererCommand.ResizeRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, width, height, &m_RTVSceneBuffer, nullptr);
		m_RendererCommand.ResizeRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, width, height, &m_RTVSceneBufferHDR, nullptr);

		m_RendererCommand.ResizeDepthStencil(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, width, height, &m_DSVSceneBuffer, nullptr);

		EventRendererViewportResize _EventRendererViewportResize(width, height);
		TE_INSTANCE_APPLICATION->OnEvent(_EventRendererViewportResize);
	}

	void RendererLayer::OnAddMaterial(const EventEntityAddMaterial& event)
	{
		m_RendererCommand.AddUpdateTask([event, &CB_MAterials = m_CB_Materials]()
		{
			auto material = event.GetMaterial();
			auto& cbMaterialData = CB_MAterials->GetData()->MaterialArray[material->GetID()];
			cbMaterialData = ConstantBuffer_Data_Materials::Material(
				material->GetColorDiffuse()
				, material->GetRoughness()
				, material->GetMetallic()
				, material->GetAmbientOcclusion()
				, material->GetUVScale()
				, material->GetUVTranslate()
				, material->GetMapIndexDiffuse()
				, material->GetMapIndexNormal()
				, material->GetMapIndexDisplacement()
				, material->GetMapIndexSpecular()
				, material->GetMapIndexRoughness()
				, material->GetMapIndexMetallic()
				, material->GetMapIndexAmbientOcclusion()
			);
		});
	}

	void RendererLayer::OnUpdateMaterial(const EventEntityUpdateMaterial& event)
	{
		m_RendererCommand.AddUpdateTask([event, &CB_MAterials = m_CB_Materials]()
		{
			auto material = event.GetMaterial();
			auto& cbMaterialData = CB_MAterials->GetData()->MaterialArray[material->GetID()];
			cbMaterialData = ConstantBuffer_Data_Materials::Material(
				material->GetColorDiffuse()
				, material->GetRoughness()
				, material->GetMetallic()
				, material->GetAmbientOcclusion()
				, material->GetUVScale()
				, material->GetUVTranslate()
				, material->GetMapIndexDiffuse()
				, material->GetMapIndexNormal()
				, material->GetMapIndexDisplacement()
				, material->GetMapIndexSpecular()
				, material->GetMapIndexRoughness()
				, material->GetMapIndexMetallic()
				, material->GetMapIndexAmbientOcclusion()
			);
		});
	}

	void RendererLayer::OnUpdateLight(const EventEntityUpdateLight& event)
	{
		auto _Light = event.GetLight();
		auto _LightType = _Light->GetLightType();
		auto _LightID = _Light->GetID();

		switch (_LightType)
		{
		case TE_LIGHT_TYPE::Directional:
		{
			auto _Itr = m_Map_DLightToCBuffer.find(_LightID);
			if (_Itr == m_Map_DLightToCBuffer.end())
			{
				throw;
				return;
			}

			int _BufferIndex = _Itr->second;

			m_RendererCommand.AddUpdateTask([_BufferIndex, _Light = static_cast<LightDirectional*>(event.GetLight()), CB_Lights = m_CB_LightData]()
			{
				const auto& _LightData = _Light->GetDirectionalLightData();

				CB_Lights->GetData()->mDLights[_BufferIndex] = ConstantBuffer_Struct_DLight
				(
					_LightData.Strength
					, _LightData.LightSize
					, _LightData.Direction
					, static_cast<uint32_t>(_LightData.CastShadow)
					, _LightData.Position
				);

			});

			break;
		}
		case TE_LIGHT_TYPE::Spot:
		{

			auto _Itr = m_Map_SLightToCBuffer.find(_LightID);
			if (_Itr == m_Map_SLightToCBuffer.end())
			{
				throw;
				return;
			}

			int _BufferIndex = _Itr->second;

			m_RendererCommand.AddUpdateTask([_BufferIndex, _Light = static_cast<LightSpot*>(event.GetLight()), CB_Lights = m_CB_LightData]()
			{
				const auto& _LightData = _Light->GetLightData();

				CB_Lights->GetData()->mSLights[_BufferIndex] = ConstantBuffer_Struct_SLight
				(
					_LightData.ShadowTransform
					, _LightData.Strength
					, _LightData.LightSize
					, _LightData.Direction
					, static_cast<uint32_t>(_LightData.CastShadow)
					, _LightData.Position
					, _LightData.FalloffStart
					, _LightData.FalloffEnd
					, _LightData.SpotOuterConeCos
					, _LightData.SpotOuterConeAngleRangeCosRcp
				);

			});

			break;
		}
		case TE_LIGHT_TYPE::Point:
			break;
		default:
			break;
		}


	}

	void RendererLayer::OnAddLight(const EventEntityAddLight& event)
	{
		ILight* _Light = event.GetLight();
		TE_LIGHT_TYPE _LightType = _Light->GetLightType();
		uint32_t _LightID = _Light->GetID();

		switch (_LightType)
		{
		case TE_LIGHT_TYPE::Directional:
		{
			size_t _BufferIndex = m_Map_DLightToCBuffer.size();

			m_Map_DLightToCBuffer[_LightID] = _BufferIndex;

			m_RendererCommand.AddUpdateTask([_BufferIndex, _Light = static_cast<LightDirectional*>(event.GetLight()), CB_Lights = m_CB_LightData]()
			{
				const auto& _LightData = _Light->GetDirectionalLightData();

				CB_Lights->GetData()->mDLights[_BufferIndex] = ConstantBuffer_Struct_DLight
				(
					_LightData.Strength
					, _LightData.LightSize
					, _LightData.Direction
					, static_cast<uint32_t>(_LightData.CastShadow)
					, _LightData.Position
				);

			});

			auto _DLightCount = TE_INSTANCE_LIGHTMANAGER->GetLightDirectionalCount();

			_ChangeUnfrequentBuffer_LightDirectionalCount(_DLightCount);

			break;
		}
		case TE_LIGHT_TYPE::Spot:
		{
			size_t _BufferIndex = m_Map_SLightToCBuffer.size();

			m_Map_SLightToCBuffer[_LightID] = _BufferIndex;

			m_RendererCommand.AddUpdateTask([_BufferIndex, _Light = static_cast<LightSpot*>(event.GetLight()), CB_Lights = m_CB_LightData]()
			{
				const auto& _LightData = _Light->GetLightData();

				CB_Lights->GetData()->mSLights[_BufferIndex] = ConstantBuffer_Struct_SLight
				(
					_LightData.ShadowTransform
					, _LightData.Strength
					, _LightData.LightSize
					, _LightData.Direction
					, static_cast<uint32_t>(_LightData.CastShadow)
					, _LightData.Position
					, _LightData.FalloffStart
					, _LightData.FalloffEnd
					, _LightData.SpotOuterConeCos
					, _LightData.SpotOuterConeAngleRangeCosRcp
				);

			});

			auto _SLightCount = TE_INSTANCE_LIGHTMANAGER->GetLightSpotCount();

			_ChangeUnfrequentBuffer_LightSpotCount(_SLightCount);

			break;
		}
		case TE_LIGHT_TYPE::Point:
			throw;
			break;
		default:
			throw;
			break;
		}

	}

	void RendererLayer::SetEnabledEnvironmentMap(bool _EnabledEnvironmentMap)
	{
		m_EnabledEnvironmentMap = _EnabledEnvironmentMap;

		m_RendererCommand.AddUpdateTask([_EnabledEnvironmentMap, CB_UnFrequent = m_CB_UnFrequent]()
			{
				CB_UnFrequent->GetData()->mEnabledEnvironmentMap = static_cast<uint32_t>(_EnabledEnvironmentMap);
			});
	}

	const float3& RendererLayer::GetAmbientLightStrength() const
	{
		return m_CB_UnFrequent->GetData()->mAmbientLightStrength;
	}

	void RendererLayer::SetAmbientLightStrength(const float3& _AmbientLightStrength)
	{
		m_RendererCommand.AddUpdateTask([_AmbientLightStrength, CB_UnFrequent = m_CB_UnFrequent]()
			{
				CB_UnFrequent->GetData()->mAmbientLightStrength = _AmbientLightStrength;
			});
	}

	const float3& RendererLayer::GetEnvironmentMapMultiplier() const
	{
		return m_CB_EnvironmentMap->GetData()->mEnvironmentMapMultiplier;
	}

	const void RendererLayer::SetEnvironmentMapMultiplier(const float3& _EnvironmentMapMultiplier)
	{
		m_RendererCommand.AddUpdateTask([_EnvironmentMapMultiplier, CB_EnvironmentMap = m_CB_EnvironmentMap]()
			{
				CB_EnvironmentMap->GetData()->mEnvironmentMapMultiplier = _EnvironmentMapMultiplier;
			});
	}

	void RendererLayer::_ChangeUnfrequentBuffer_LightDirectionalCount(uint32_t _LightDirectionalCount)
	{
		m_RendererCommand.AddUpdateTask([_LightDirectionalCount, CB_UnFrequent = m_CB_UnFrequent]()
			{
				CB_UnFrequent->GetData()->mDLightCount = _LightDirectionalCount;
			});
	}

	void RendererLayer::_ChangeUnfrequentBuffer_LightSpotCount(uint32_t _LightSpotCount)
	{
		m_RendererCommand.AddUpdateTask([_LightSpotCount, CB_UnFrequent = m_CB_UnFrequent]()
			{
				CB_UnFrequent->GetData()->mSLightCount = _LightSpotCount;
			});
	}

	void RendererLayer::InitRenderPasses()
	{
		//////////////////////////////////////////////////
		//Generate Environment Cube Map
		//////////////////////////////////////////////////

		/*m_RenderPass_GenerateCubeMap->Initialize(TE_IDX_GRESOURCES::Texture_RT_CubeMap, 2048, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, "E:\\3DModels\\2021\\Textures\\IBL\\Desert_Highway\\Road_to_MonumentValley_Ref.hdr");
		m_RenderPass_GenerateCubeMap->OnAttach();
		m_RenderPass_GenerateCubeMap->BeginScene();
		m_RenderPass_GenerateCubeMap->Render();
		m_RenderPass_GenerateCubeMap->EndScene();

		m_RendererCommand.SaveTextureToFile(TE_IDX_GRESOURCES::Texture_RT_CubeMap, "E:\\3DModels\\2021\\Textures\\GeneratedEnvironmentMap_DesertHighway.dds");*/

		//////////////////////////////////////////////////
		//Generate IBL Ambient
		//////////////////////////////////////////////////

		/*m_RendererCommand.LoadTextureFromFile(TE_IDX_GRESOURCES::Texture_InputCreateIBLAmbient, "E:\\3DModels\\2021\\Textures\\GeneratedEnvironmentMap_DesertHighway.dds");

		m_RenderPass_GenerateIBLAmbient->Initialize(TE_IDX_GRESOURCES::Texture_RT_IBL_Ambient, 512, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, "E:\\3DModels\\2021\\Textures\\GeneratedIBLAmbient_DesertHighway.dds");
		m_RenderPass_GenerateIBLAmbient->OnAttach();
		m_RenderPass_GenerateIBLAmbient->BeginScene();
		m_RenderPass_GenerateIBLAmbient->Render();
		m_RenderPass_GenerateIBLAmbient->EndScene();*/


		//////////////////////////////////////////////////
		//Generate IBL Specular
		//////////////////////////////////////////////////


		/*
		m_RendererCommand.LoadTextureFromFile(TE_IDX_GRESOURCES::Texture_InputCreateIBLSpecular, "E:\\3DModels\\2021\\Textures\\GeneratedEnvironmentMap_DesertHighway.dds");

		m_RenderPass_GenerateIBLSpecular->Initialize(128, 512, 5, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT);
		m_RenderPass_GenerateIBLSpecular->OnAttach();
		m_RenderPass_GenerateIBLSpecular->BeginScene();
		m_RenderPass_GenerateIBLSpecular->Render();
		m_RenderPass_GenerateIBLSpecular->EndScene();

		m_RendererCommand.SaveTextureToFile(TE_IDX_GRESOURCES::Texture_RT_IBL_Specular_Prefilter, "E:\\3DModels\\2021\\Textures\\GeneratedIBLMapSpecular_DesertHighway.dds");
		m_RendererCommand.SaveTextureToFile(TE_IDX_GRESOURCES::Texture_RT_IBL_Specular_BRDF, "E:\\3DModels\\2021\\Textures\\GeneratedIBLMapSpecularBRDF.dds");
		*/

		/*m_RenderPass_GenerateCubeMap->OnDetach();
		m_RenderPass_GenerateIBL->OnDetach(); */


		m_RenderPassStack.PopAll();

		m_RenderPassStack.PushRenderPass(m_RenderPass_GenerateShadowMap.get());
		m_RenderPassStack.PushRenderPass(m_RenderPass_ForwardRendering.get());
		//m_RenderPassStack.PushRenderPass(m_RenderPass_GenerateGBuffers.get());
		//m_RenderPassStack.PushRenderPass(m_RenderPass_DeferredShading.get());

		if (m_IsEnabledHDR)
			m_RenderPassStack.PushRenderPass(m_RenderPass_PostProcessing_HDR.get());
	}

	void RendererLayer::InitTextures()
	{
		auto _ViewportWidth = TE_INSTANCE_APPLICATION->GetClientWidth();
		auto _ViewportHeight = TE_INSTANCE_APPLICATION->GetClientHeight();

		{
			auto _TextureSceneBuffer = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, _ViewportWidth, _ViewportHeight, 1, TE_RESOURCE_FORMAT::R8G8B8A8_UNORM, ClearValue_RenderTarget{ 1.0f, 1.0f, 1.0f, 1.0f }, true, false);
			m_RendererCommand.CreateRenderTargetView(_TextureSceneBuffer, &m_RTVSceneBuffer);
		}
		{
			auto _TextureSceneBufferHDR = m_RendererCommand.CreateRenderTarget(TE_IDX_GRESOURCES::Texture_RT_SceneBufferHDR, _ViewportWidth, _ViewportHeight, 1, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, ClearValue_RenderTarget{ 0.0f, 0.0f, 0.0f, 1.0f }, true, false);
			m_RendererCommand.CreateRenderTargetView(_TextureSceneBufferHDR, &m_RTVSceneBufferHDR);
		}

		m_RendererCommand.CreateRenderTargetView(TE_INSTANCE_SWAPCHAIN, &m_RTVBackBuffer);

		//m_RendererCommand.CreateTextureCubeMap(TE_IDX_GRESOURCES::Texture_CubeMap_Environment, "E:\\3DModels\\2021\\Textures\\GeneratedEnvironmentMap.dds");
		//m_RendererCommand.CreateTextureCubeMap(TE_IDX_GRESOURCES::Texture_CubeMap_IBLAmbient, "E:\\3DModels\\2021\\Textures\\IBL\\GeneratedIBL.dds");
		//m_RendererCommand.CreateTextureCubeMap(TE_IDX_GRESOURCES::Texture_CubeMap_Environment, "K:\\EBook\\Game Programming\\Source Codes\\d3d12book-master\\d3d12book-master\\Textures\\grasscube1024.dds");
		//m_RendererCommand.CreateTextureCubeMap(TE_IDX_TEXTURE::CUBEMAP_ENVIRONMENT, "K:\\Downloads\\3D\\EnvironmentMap_BabylonJs_Sample1\\textures\\SpecularHDR.dds");
		//m_RendererCommand.CreateTextureCubeMap(TE_IDX_TEXTURE::CUBEMAP_ENVIRONMENT, "K:\\Downloads\\3D\\EnvironmentMap_BabylonJs_Forest\\textures\\forest.dds");

		auto _TextureDepthStencil = m_RendererCommand.CreateDepthStencil(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer, _ViewportWidth, _ViewportHeight, 1, TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 1.0f, 0 }, true, false);
		m_RendererCommand.CreateDepthStencilView(_TextureDepthStencil, &m_DSVSceneBuffer);

		m_RendererCommand.LoadTextureFromFile(TE_IDX_GRESOURCES::Texture_CubeMap_Environment, "E:\\3DModels\\2021\\Textures\\GeneratedEnvironmentMap_DesertHighway.dds");
		m_RendererCommand.LoadTextureFromFile(TE_IDX_GRESOURCES::Texture_CubeMap_IBLAmbient, "E:\\3DModels\\2021\\Textures\\GeneratedIBLAmbient_DesertHighway.dds");
		m_RendererCommand.LoadTextureFromFile(TE_IDX_GRESOURCES::Texture_CubeMap_IBLSpecular, "E:\\3DModels\\2021\\Textures\\GeneratedIBLMapSpecular_DesertHighway.dds");
		m_RendererCommand.LoadTextureFromFile(TE_IDX_GRESOURCES::Texture_PrecomputedBRDF, "E:\\3DModels\\2021\\Textures\\GeneratedIBLMapSpecularBRDF.dds");
	}

	void RendererLayer::InitBuffers()
	{
		m_CB_PerFrame = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_Per_Frame>(TE_IDX_GRESOURCES::CBuffer_PerFrame);
		m_CB_LightData = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_LightData>(TE_IDX_GRESOURCES::CBuffer_LightData);
		m_CB_Materials = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_Materials>(TE_IDX_GRESOURCES::CBuffer_Materials);
		m_CB_UnFrequent = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_UnFrequent>(TE_IDX_GRESOURCES::CBuffer_UnFrequent);
		m_CB_Bones = m_RendererCommand.CreateConstantBufferUpload<ConstantBuffer_Data_Bones>(TE_IDX_GRESOURCES::CBuffer_Bones);
		m_CB_EnvironmentMap = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_EnvironmentMap>(TE_IDX_GRESOURCES::Constant_EnvironmentMap);
	}

}

