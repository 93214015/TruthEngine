#include "pch.h"
#include "RenderPass_PostProcessing_HDR.h"

#include "Core/Renderer/RendererLayer.h"
#include "Core/Renderer/ShaderManager.h"

#include "Core/Event/EventRenderer.h"
#include <Core/Entity/Camera/Camera.h>
#include <Core/Entity/Camera/CameraManager.h>

TruthEngine::RenderPass_PostProcessing_HDR::RenderPass_PostProcessing_HDR(RendererLayer* _RendererLayer)
	: RenderPass(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, _RendererLayer)
	, mAdaptationPercentage(1.0)
{
}

void TruthEngine::RenderPass_PostProcessing_HDR::OnImGuiRender()
{
	static auto _CBFinalPassData = mConstantBufferFinalPass->GetData();
	static float _MiddleGrey = _CBFinalPassData->mMiddleGrey;
	static float _WhiteSquare = 1.5f;
	static float _DOFFarRange = 60.0f;


	if (ImGui::Begin("PostProcessing HDR RenderPass"))
	{

		static const char* _ToneMappingsListStr[] = { "Reinhard", "ACES" };
		static const char* _CurrentToneMappingItem = _ToneMappingsListStr[0];

		if (ImGui::BeginCombo("Tone Mapping: ", _CurrentToneMappingItem))
		{
			if (ImGui::Selectable(_ToneMappingsListStr[0], mPipelineFinalPass_Selected == &mPipelineFinalPass_ReinhardToneMapping))
			{
				mPipelineFinalPass_Selected = &mPipelineFinalPass_ReinhardToneMapping;
				_CurrentToneMappingItem = _ToneMappingsListStr[0];
			}
			if (ImGui::Selectable(_ToneMappingsListStr[1], mPipelineFinalPass_Selected == &mPipelineFinalPass_ACESToneMapping))
			{
				mPipelineFinalPass_Selected = &mPipelineFinalPass_ACESToneMapping;
				_CurrentToneMappingItem = _ToneMappingsListStr[1];
			}

			ImGui::EndCombo();
		}


		if (ImGui::DragFloat("Middle Grey:", &_MiddleGrey, 0.00001, 0.0f, 1.0f, "%.5f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mMiddleGrey = _MiddleGrey; });
		}
		if (ImGui::DragFloat("Luminance White Square:", &_WhiteSquare, 0.01, 0.0f, 10.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->SetLumWhiteSqr(_WhiteSquare); });
		}
		if (ImGui::DragFloat("Per Second Adaptation Percentage:", &mAdaptationPercentage, 0.01, 0.0f, 10.f, "%.2f"))
		{
		}
		if (ImGui::DragFloat("Bloom Threshold:", &mBloomThreshold, 0.01, 0.0f, 10.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferDownScaling->GetData()->gBloomThreshold = mBloomThreshold; });
		}
		if (ImGui::DragFloat("Bloom Scale:", &mBloomScale, 0.01, 0.0f, 10.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mBloomScale = mBloomScale; });
		}
		if (ImGui::DragFloat("DOF Far Start:", &mDOFFarStart, 1.0, 1.0f, 0.0f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mDOFFarValues.x = mDOFFarStart; });
		}
		if (ImGui::DragFloat("DOF Far Range:", &mDOFFarRange, 0.01, 0.0001f, 0.0f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mDOFFarValues.y = (1.0f / mDOFFarRange); });
		}
	}
	ImGui::End();
}

void TruthEngine::RenderPass_PostProcessing_HDR::OnUpdate(double _DeltaTime)
{
	mAdaptation = mAdaptationPercentage < 0.0001f ? 1.0f : _DeltaTime * mAdaptationPercentage;
	mAdaptation = mAdaptation < 0.9999f ? mAdaptation : 0.9999f;
	mRendererCommand_DownScaling_SecondPass.AddUpdateTask([this]() {mConstantBufferDownScaling->GetData()->gAdaption = mAdaptation; });
}

void TruthEngine::RenderPass_PostProcessing_HDR::BeginScene()
{

	mRendererCommand_DownScaling_FirstPass.BeginCompute(&mPipelineDownScalingFirstPass);


	mRendererCommand_DownScaling_SecondPass.BeginCompute(&mPipelineDownScalingSecondPass);


	mRendererCommand_BloomPass.BeginCompute(&mPipelineBloomPass);


	mRendererCommand_BlurPassHorz.BeginCompute(&mPipelineBlurPassHorz);


	mRendererCommand_BlurPassVert.BeginCompute(&mPipelineBlurPassVert);


	mRendererCommand_FinalPass.BeginGraphics(mPipelineFinalPass_Selected);
	mRendererCommand_FinalPass.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneSDR());
	mRendererCommand_FinalPass.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());


}

void TruthEngine::RenderPass_PostProcessing_HDR::EndScene()
{
	mRendererCommand_DownScaling_FirstPass.End();

	mRendererCommand_DownScaling_SecondPass.End();

	mRendererCommand_BloomPass.End();

	mRendererCommand_BlurPassHorz.End();

	mRendererCommand_BlurPassVert.End();

	mRendererCommand_FinalPass.End();
}

void TruthEngine::RenderPass_PostProcessing_HDR::Render()
{

	//mRendererCommand_DownScaling_FirstPass.SetPipelineCompute(mPipelineDownScalingFirstPass);
	mRendererCommand_DownScaling_FirstPass.ExecutePendingCommands();
	mRendererCommand_DownScaling_FirstPass.Dispatch(mGroupNum, 1, 1);


	static uint32_t _BufferIndex = 0;
	GraphicResource* _CurrentBuffer = _BufferIndex == 0 ? mBufferRWAverageLumSecondPass0 : mBufferRWAverageLumSecondPass1;
	GraphicResource* _PrevBuffer = _BufferIndex == 0 ? mBufferRWAverageLumSecondPass1 : mBufferRWAverageLumSecondPass0;
	mRendererCommand_DownScaling_SecondPass.SetDirectUnorderedAccessViewCompute(_CurrentBuffer, 0);
	mRendererCommand_DownScaling_SecondPass.SetDirectShaderResourceViewCompute(_PrevBuffer, 0);
	//mRendererCommand_DownScaling_SecondPass.SetPipelineCompute(mPipelineDownScalingSecondPass); // The Pipeline is set in the BeginScene() Method
	mRendererCommand_DownScaling_SecondPass.ExecutePendingCommands();
	mRendererCommand_DownScaling_SecondPass.Dispatch(1, 1, 1);


	mRendererCommand_BloomPass.SetDirectShaderResourceViewCompute(_CurrentBuffer, 1);
	//mRendererCommand_BloomPass.SetPipelineCompute(mPipelineBloomPass);  // The Pipeline is set in the BeginScene() Method
	mRendererCommand_BloomPass.ExecutePendingCommands();
	mRendererCommand_BloomPass.Dispatch(mGroupNum, 1, 1);

	uint32_t horz = static_cast<uint32_t>(ceil(mSceneViewQuarterSize[0] / (128.0f - 12.0f)));
	uint32_t vert = static_cast<uint32_t>(ceil(mSceneViewQuarterSize[1] / (128.0f - 12.0f)));

	//Horz Pipeline is set in BeginScene() Stage
	mRendererCommand_BlurPassHorz.ExecutePendingCommands();
	mRendererCommand_BlurPassHorz.Dispatch(horz, mSceneViewQuarterSize[1], 1);

	mRendererCommand_BlurPassVert.ExecutePendingCommands();
	mRendererCommand_BlurPassVert.Dispatch(mSceneViewQuarterSize[0], vert, 1);

	mRendererCommand_FinalPass.SetDirectShaderResourceViewGraphics(_CurrentBuffer, 4);
	//mRendererCommand_FinalPass.SetPipelineGraphics(mPipelineFinalPass_ReinhardToneMapping);
	mRendererCommand_FinalPass.ExecutePendingCommands();
	mRendererCommand_FinalPass.Draw(4, 0);

	_BufferIndex = (_BufferIndex + 1) % 2;
}


void TruthEngine::RenderPass_PostProcessing_HDR::InitRendererCommand()
{
	mRendererCommand_DownScaling_FirstPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_FIRSTPASS);
	mRendererCommand_DownScaling_SecondPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_SECONDPASS);
	mRendererCommand_BloomPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_BLOOMPASS);
	mRendererCommand_BlurPassHorz.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_HORZ);
	mRendererCommand_BlurPassVert.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_VERT);
	mRendererCommand_FinalPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_FINALPASS);
}

void TruthEngine::RenderPass_PostProcessing_HDR::InitTextures()
{
	const Application* _App = TE_INSTANCE_APPLICATION;

	uint32_t _ViewportWidth = _App->GetSceneViewportWidth();
	uint32_t _ViewportHeight = _App->GetSceneViewportHeight();

	/*{
		uint32_t _Width = ceil(static_cast<float>(_ViewportWidth) / 4.0f);
		uint32_t _Height = ceil(static_cast<float>(_ViewportHeight) / 4.0f);
		mRWTextureDownScaledHDR = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_DownScaledHDR, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
		mRWTextureBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_Bloom, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
		mRWTextureBluredBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBlured, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
		mRWTextureBluredBloomHorz = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBluredHorz, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	}*/


	ResizedViewport(_ViewportWidth, _App->GetSceneViewportHeight());
}

void TruthEngine::RenderPass_PostProcessing_HDR::InitBuffers()
{
	mBufferRWAverageLumFirstPass = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, 4, (mGroupNum != 0 ? mGroupNum : 1), false);

	mBufferRWAverageLumSecondPass0 = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass0, 4, 1, false);
	mBufferRWAverageLumSecondPass1 = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass1, 4, 1, false);

	mConstantBufferDownScaling = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_DownScaling>(TE_IDX_GRESOURCES::CBuffer_HDR_DownScaling);
	mConstantBufferFinalPass = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_FinalPass>(TE_IDX_GRESOURCES::CBuffer_HDR_FinalPass);
	mConstantBufferBlurPass = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_BlurPass>(TE_IDX_GRESOURCES::CBuffer_GaussianBlur);

	Camera* _Camera = CameraManager::GetInstance()->GetActiveCamera();
	float fQ = _Camera->GetZFarPlane() / (_Camera->GetZFarPlane() - _Camera->GetZNearPlane());
	float2 _ProjectionValues = float2{ -1 * _Camera->GetZNearPlane() * fQ, -fQ };

	//mRendererCommand_FinalPass.AddUpdateTask([=]() {*mConstantBufferFinalPass->GetData() = ConstantBuffer_Data_FinalPass(0.0025f, 1.5f, 1.0f, _ProjectionValues, float2{mDOFFarStart, 1/mDOFFarRange}); });
	mRendererCommand_FinalPass.AddUpdateTask([=]() {*mConstantBufferFinalPass->GetData() = ConstantBuffer_Data_FinalPass(0.003f, 1.5f, 0.0f, _ProjectionValues, float2{ 1000.0f, 1 / mDOFFarRange }); });

}

void TruthEngine::RenderPass_PostProcessing_HDR::InitPipelines()
{

	RendererStateSet _RendererStates = InitRenderStates(
		TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
		TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
		TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_SPECULAR_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_METALLIC_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION_FALSE,
		TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE,
		TE_RENDERER_STATE_ENABLED_DEPTH_FALSE,
		TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
		TE_RENDERER_STATE_FILL_MODE_SOLID,
		TE_RENDERER_STATE_CULL_MODE_BACK,
		TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);

	//
	//First Pass Pipeline
	//
	Shader* _Shader;
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_FIRSTPASS, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/HDR_DownScaling.hlsl", "", "", "HDRDownScalingFirstPass");
	PipelineCompute::Factory(&mPipelineDownScalingFirstPass, _Shader);

	//
	//Second Pass Pipeline
	//
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_SECONDPASS, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/HDR_DownScaling.hlsl", "", "", "HDRDownScalingSecondPass");
	PipelineCompute::Factory(&mPipelineDownScalingSecondPass, _Shader);

	//
	//Bloom Pass Pipeline
	//
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_BLOOMPASS, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/HDR_DownScaling.hlsl", "", "", "BloomReveal");
	PipelineCompute::Factory(&mPipelineBloomPass, _Shader);


	//
	//Blur Pass Pipeline
	//
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_HORZ, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/GaussianBlur.hlsl", "", "", "HorizFilter");
	PipelineCompute::Factory(&mPipelineBlurPassHorz, _Shader);
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_VERT, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/GaussianBlur.hlsl", "", "", "VerticalFilter");
	PipelineCompute::Factory(&mPipelineBlurPassVert, _Shader);


	//
	//Final Pass Pipeline
	//
	//Reinhard Tone Mapping
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_FINALPASS, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/HDR_PostProcessing.hlsl", "FullScreenQuadVS", "FinalPassPS");

	TE_RESOURCE_FORMAT _RTVFormat[1] = { m_RendererLayer->GetFormatRenderTargetSceneSDR() };

	PipelineGraphics::Factory(&mPipelineFinalPass_ReinhardToneMapping, _RendererStates, _Shader, _countof(_RTVFormat), _RTVFormat, TE_RESOURCE_FORMAT::UNKNOWN, false);

	//ACES Tone Mapping
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_FINALPASS, TE_IDX_MESH_TYPE::MESH_NTT, _RendererStates, "Assets/Shaders/HDR_PostProcessing.hlsl", "FullScreenQuadVS", "FinalPassPS", "", "", "", "", {L"TONE_MAPPING_ACES"});

	PipelineGraphics::Factory(&mPipelineFinalPass_ACESToneMapping, _RendererStates, _Shader, _countof(_RTVFormat), _RTVFormat, TE_RESOURCE_FORMAT::UNKNOWN, false);

}

void TruthEngine::RenderPass_PostProcessing_HDR::ReleaseRendererCommand()
{
	mRendererCommand_DownScaling_FirstPass.Release();
	mRendererCommand_DownScaling_SecondPass.Release();
	mRendererCommand_BloomPass.Release();
	mRendererCommand_BlurPassHorz.Release();
	mRendererCommand_BlurPassVert.Release();
	mRendererCommand_FinalPass.Release();

}

void TruthEngine::RenderPass_PostProcessing_HDR::ReleaseTextures()
{
	mRendererCommand_FinalPass.ReleaseResource(mRWTextureDownScaledHDR);
	mRendererCommand_FinalPass.ReleaseResource(mRWTextureBloom);
	mRendererCommand_FinalPass.ReleaseResource(mRWTextureBluredBloom);
	mRendererCommand_FinalPass.ReleaseResource(mRWTextureBluredBloomHorz);
}

void TruthEngine::RenderPass_PostProcessing_HDR::ReleaseBuffers()
{
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mConstantBufferDownScaling);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mConstantBufferFinalPass);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mBufferRWAverageLumFirstPass);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mBufferRWAverageLumSecondPass0);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mBufferRWAverageLumSecondPass1);
}

void TruthEngine::RenderPass_PostProcessing_HDR::ReleasePipelines()
{
}

void TruthEngine::RenderPass_PostProcessing_HDR::RegisterEventListeners()
{

	auto _Lambda_OnTextureResize = [this](Event& _Event)
	{
		this->OnRendererViewportResize(static_cast<EventRendererViewportResize&>(_Event));
	};

	m_EventListenerList.push_back(TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RendererViewportResize, _Lambda_OnTextureResize));
}

void TruthEngine::RenderPass_PostProcessing_HDR::UnRegisterEventListeners()
{
	TE_INSTANCE_APPLICATION->UnRegisterEventListener(m_EventListenerList.data(), m_EventListenerList.size());
}


void TruthEngine::RenderPass_PostProcessing_HDR::ResizedViewport(uint32_t _Width, uint32_t Height)
{

	mGroupNum = static_cast<uint32_t>(std::ceilf(static_cast<float>(_Width * Height) / (1024.0f * 16.0f)));
	mBufferRWAverageLumFirstPass = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, 4, mGroupNum, false);

	mSceneViewQuarterSize[0] = static_cast<uint32_t>(ceil(static_cast<float>(_Width) / 4.0f));
	mSceneViewQuarterSize[1] = static_cast<uint32_t>(ceil(static_cast<float>(Height) / 4.0f));

	mRendererCommand_DownScaling_FirstPass.AddUpdateTask([=]()
		{
			*mConstantBufferDownScaling->GetData() = ConstantBuffer_Data_DownScaling(mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], mSceneViewQuarterSize[0] * mSceneViewQuarterSize[1], mGroupNum, mAdaptation, mBloomThreshold);
		});

	mRendererCommand_DownScaling_FirstPass.AddUpdateTask([=]()
		{
			*mConstantBufferBlurPass->GetData() = ConstantBuffer_Data_BlurPass(mSceneViewQuarterSize[0], mSceneViewQuarterSize[1]);
		});

	mRWTextureDownScaledHDR = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_DownScaledHDR, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	mRWTextureBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_Bloom, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	mRWTextureBluredBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBlured, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	mRWTextureBluredBloomHorz = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBluredHorz, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
}

void TruthEngine::RenderPass_PostProcessing_HDR::OnRendererViewportResize(const EventRendererViewportResize& _Event)
{
	ResizedViewport(_Event.GetWidth(), _Event.GetHeight());
}
