#include "pch.h"
#include "RenderPass_PostProcessing_HDR.h"

#include "Core/Renderer/ShaderManager.h"

#include "Core/Event/EventApplication.h"
#include <Core/Entity/Camera/Camera.h>
#include <Core/Entity/Camera/CameraManager.h>

TruthEngine::RenderPass_PostProcessing_HDR::RenderPass_PostProcessing_HDR()
	: RenderPass(TE_IDX_RENDERPASS::POSTPROCESSING_HDR)
	, mViewPort(0, 0, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), .0f, 1.0f)
	, mViewRect(static_cast<long>(0), static_cast<long>(0), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()))
	, mAdaptationPercentage(1.0)
{
	mSceneViewQuarterSize[0] = TE_INSTANCE_APPLICATION->GetClientWidth();
	mSceneViewQuarterSize[1] = TE_INSTANCE_APPLICATION->GetClientHeight();
}

TruthEngine::RenderPass_PostProcessing_HDR::~RenderPass_PostProcessing_HDR() = default;

void TruthEngine::RenderPass_PostProcessing_HDR::OnAttach()
{
	mGroupNum = static_cast<uint32_t>(std::ceilf(static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth() * TE_INSTANCE_APPLICATION->GetClientHeight()) / (1024.0f * 16.0f)));

	mRendererCommand_DownScaling_FirstPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_FIRSTPASS);
	mRendererCommand_DownScaling_SecondPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_SECONDPASS);
	mRendererCommand_BloomPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_BLOOMPASS);
	mRendererCommand_BlurPassHorz.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_HORZ);
	mRendererCommand_BlurPassVert.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_GAUSSIANBLUR_VERT);
	mRendererCommand_FinalPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_FINALPASS);

	InitBuffers();
	InitTexture();
	InitPipeline();
	RegisterOnEvents();
}

void TruthEngine::RenderPass_PostProcessing_HDR::OnDetach()
{
	ReleaseResources();
}

void TruthEngine::RenderPass_PostProcessing_HDR::OnImGuiRender()
{
	static auto _CBFinalPassData = mConstantBufferFinalPass->GetData();
	static float _MiddleGrey = _CBFinalPassData->mMiddleGrey;
	static float _WhiteSquare = 1.5f;
	static float _DOFFarRange = 60.0f;


	if (ImGui::Begin("PostProcessing HDR RenderPass"))
	{
		if (ImGui::DragFloat("Middle Grey:", &_MiddleGrey, 0.00001, 0.0f, 1.0f, "%.5f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mMiddleGrey = _MiddleGrey; });
		}
		if (ImGui::DragFloat("Luminance White Square:", &_WhiteSquare, 0.01, 0.0f, 10.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mLumWhiteSqr = _WhiteSquare; });
		}
		if (ImGui::DragFloat("Per Second Adaptation Percentage:", &mAdaptation, 0.01, 0.0f, 1.f, "%.2f"))
		{}
		if (ImGui::DragFloat("Bloom Threshold:", &mBloomThreshold, 0.01, 0.0f, 10.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferDownScaling->GetData()->gBloomThreshold = mBloomThreshold; });
		}
		if (ImGui::DragFloat("Bloom Scale:", &mBloomScale, 0.01, 0.0f, 10.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mBloomScale = mBloomScale; });
		}
		if (ImGui::DragFloat("DOF Far Start:", &mDOFFarStart, 1.0, 1.0f, 100.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mDOFFarValues.x = mDOFFarStart; });
		}
		if (ImGui::DragFloat("DOF Far Range:", &mDOFFarRange, 0.01, 0.0001f, 150.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { mConstantBufferFinalPass->GetData()->mDOFFarValues.y = (1.0f / mDOFFarRange); });
		}
	}
	ImGui::End();
}

void TruthEngine::RenderPass_PostProcessing_HDR::BeginScene()
{

	mRendererCommand_DownScaling_FirstPass.BeginCompute(mPipelineDownScalingFirstPass);


	mRendererCommand_DownScaling_SecondPass.BeginCompute(mPipelineDownScalingSecondPass);


	mRendererCommand_BloomPass.BeginCompute(mPipelineBloomPass);


	mRendererCommand_BlurPassHorz.BeginCompute(mPipelineBlurPassHorz);


	mRendererCommand_BlurPassVert.BeginCompute(mPipelineBlurPassVert);


	mRendererCommand_FinalPass.BeginGraphics(mPipelineFinalPass);
	mRendererCommand_FinalPass.SetRenderTarget(mRenderTargetView_SceneBuffer);
	mRendererCommand_FinalPass.SetViewPort(&mViewPort, &mViewRect);

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
	double _dt = TE_INSTANCE_APPLICATION->FrameTime() * 0.001;
	mAdaptation = mAdaptationPercentage < 0.0001f ? 1.0f : _dt * mAdaptationPercentage;
	mAdaptation = mAdaptation < 0.9999f ? mAdaptation : 0.9999f;
	mRendererCommand_DownScaling_SecondPass.AddUpdateTask([this]() {mConstantBufferDownScaling->GetData()->gAdaption = mAdaptation; });


	//mRendererCommand_DownScaling_FirstPass.SetPipelineCompute(mPipelineDownScalingFirstPass);
	mRendererCommand_DownScaling_FirstPass.Dispatch(mGroupNum, 1, 1);


	static uint32_t _BufferIndex = 0;
	GraphicResource* _CurrentBuffer = _BufferIndex == 0 ? mBufferRWAverageLumSecondPass0 : mBufferRWAverageLumSecondPass1;
	GraphicResource* _PrevBuffer = _BufferIndex == 0 ? mBufferRWAverageLumSecondPass1 : mBufferRWAverageLumSecondPass0;
	mRendererCommand_DownScaling_SecondPass.SetDirectUnorderedAccessViewCompute(_CurrentBuffer, 0);
	mRendererCommand_DownScaling_SecondPass.SetDirectShaderResourceViewCompute(_PrevBuffer, 0);
	//mRendererCommand_DownScaling_SecondPass.SetPipelineCompute(mPipelineDownScalingSecondPass);
	mRendererCommand_DownScaling_SecondPass.Dispatch(1, 1, 1);


	mRendererCommand_BloomPass.SetDirectShaderResourceViewCompute(_CurrentBuffer, 1);
	//mRendererCommand_BloomPass.SetPipelineCompute(mPipelineBloomPass);
	mRendererCommand_BloomPass.Dispatch(mGroupNum, 1, 1);

	uint32_t horz = static_cast<uint32_t>(ceil(mSceneViewQuarterSize[0] / (128.0f - 12.0f)));
	uint32_t vert = static_cast<uint32_t>(ceil(mSceneViewQuarterSize[1] / (128.0f - 12.0f)));
	//Horz Pipeline is set in BeginScene() Stage
	mRendererCommand_BlurPassHorz.Dispatch( horz , mSceneViewQuarterSize[1] , 1 );
	mRendererCommand_BlurPassVert.Dispatch( mSceneViewQuarterSize[0], vert, 1);


	mRendererCommand_FinalPass.SetDirectShaderResourceViewGraphics(_CurrentBuffer, 4);
	//mRendererCommand_FinalPass.SetPipelineGraphics(mPipelineFinalPass);
	mRendererCommand_FinalPass.Draw(4, 0);

	_BufferIndex = (_BufferIndex + 1) % 2;
}


void TruthEngine::RenderPass_PostProcessing_HDR::InitBuffers()
{
	TE_ASSERT_CORE(mGroupNum != 0, "RenderPass_PostProcessingHDR : The GroupSize is 0");

	mBufferRWAverageLumFirstPass = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, 4, mGroupNum, false);
	mBufferRWAverageLumSecondPass0 = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass0, 4, 1, false);
	mBufferRWAverageLumSecondPass1 = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass1, 4, 1, false);

	mConstantBufferDownScaling = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_DownScaling>(TE_IDX_GRESOURCES::CBuffer_HDR_DownScaling);
	mConstantBufferFinalPass = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_FinalPass>(TE_IDX_GRESOURCES::CBuffer_HDR_FinalPass);
	mConstantBufferBlurPass = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_BlurPass>(TE_IDX_GRESOURCES::CBuffer_GaussianBlur);

	Camera* _Camera = CameraManager::GetInstance()->GetActiveCamera();
	float fQ = _Camera->GetZFarPlane() / (_Camera->GetZFarPlane() - _Camera->GetZNearPlane());
	float2 _ProjectionValues = float2{ -1 * _Camera->GetZNearPlane() * fQ, -fQ };

	mRendererCommand_FinalPass.AddUpdateTask([=]() {*mConstantBufferFinalPass->GetData() = ConstantBuffer_Data_FinalPass(0.0025f, 1.5f, 1.0f, _ProjectionValues, float2{mDOFFarStart, 1/mDOFFarRange}); });
}

void TruthEngine::RenderPass_PostProcessing_HDR::InitTexture()
{
	mRendererCommand_DownScaling_FirstPass.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &mRenderTargetView_SceneBuffer);

	{
		uint32_t _Width = ceil( static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()) / 4.0f);
		uint32_t _Height = ceil( static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()) / 4.0f);
		mRWTextureDownScaledHDR = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_DownScaledHDR, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
		mRWTextureBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_Bloom, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
		mRWTextureBluredBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBlured, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
		mRWTextureBluredBloomHorz = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBluredHorz, _Width, _Height, TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	}
}

void TruthEngine::RenderPass_PostProcessing_HDR::InitPipeline()
{

	RendererStateSet _RendererStates = InitRenderStates(
		TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
		TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
		TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE,
		TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE,
		TE_RENDERER_STATE_ENABLED_DEPTH_FALSE,
		TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
		TE_RENDERER_STATE_FILL_MODE_SOLID,
		TE_RENDERER_STATE_CULL_MODE_BACK,
		TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		TE_RENDERER_STATE_COMPARISSON_FUNC_LESS_EQUAL
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
	ShaderManager::GetInstance()->AddShader(&_Shader, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_FINALPASS, TE_IDX_MESH_TYPE::MESH_SIMPLE, _RendererStates, "Assets/Shaders/HDR_PostProcessing.hlsl", "FullScreenQuadVS", "FinalPassPS");

	TE_RESOURCE_FORMAT _RTVFormat[1] = { TE_RESOURCE_FORMAT::R8G8B8A8_UNORM };

	PipelineGraphics::Factory(&mPipelineFinalPass, _RendererStates, _Shader, _countof(_RTVFormat), _RTVFormat, TE_RESOURCE_FORMAT::UNKNOWN, false);

}

void TruthEngine::RenderPass_PostProcessing_HDR::ReleaseResources()
{
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mConstantBufferDownScaling);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mConstantBufferFinalPass);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mBufferRWAverageLumFirstPass);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mBufferRWAverageLumSecondPass0);
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mBufferRWAverageLumSecondPass1);
}

void TruthEngine::RenderPass_PostProcessing_HDR::OnRenderTargetResize(const EventTextureResize & _Event)
{
	if (_Event.GetIDX() != TE_IDX_GRESOURCES::Texture_RT_SceneBuffer)
		return;

	uint32_t width = _Event.GetWidth();
	uint32_t height = _Event.GetHeight();

	mViewPort.Resize(width, height);
	mViewRect = ViewRect{ 0, 0, static_cast<long>(width), static_cast<long>(height) };

	mGroupNum = static_cast<uint32_t>(std::ceilf(static_cast<float>(width * height) / (1024.0f * 16.0f)));
	mBufferRWAverageLumFirstPass = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, 4, mGroupNum, false);

	mSceneViewQuarterSize[0] = static_cast<uint32_t>(ceil( static_cast<float>(width) / 4.0f));
	mSceneViewQuarterSize[1] = static_cast<uint32_t>(ceil(static_cast<float>(height) / 4.0f));

	mRendererCommand_DownScaling_FirstPass.AddUpdateTask([=]()
		{
			*mConstantBufferDownScaling->GetData() = ConstantBuffer_Data_DownScaling(mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], mSceneViewQuarterSize[0] * mSceneViewQuarterSize[1], mGroupNum, mAdaptation, mBloomThreshold);
		});

	mRendererCommand_DownScaling_FirstPass.AddUpdateTask([=]()
		{
			*mConstantBufferBlurPass->GetData() = ConstantBuffer_Data_BlurPass(mSceneViewQuarterSize[0], mSceneViewQuarterSize[1]);
		});

	mRendererCommand_DownScaling_FirstPass.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &mRenderTargetView_SceneBuffer);

	mRWTextureDownScaledHDR = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_DownScaledHDR, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	mRWTextureBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_Bloom, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	mRWTextureBluredBloom = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBlured, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);
	mRWTextureBluredBloomHorz = mRendererCommand_DownScaling_FirstPass.CreateTextureRW(TE_IDX_GRESOURCES::Texture_RW_BloomBluredHorz, mSceneViewQuarterSize[0], mSceneViewQuarterSize[1], TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT, true, false);

}

void TruthEngine::RenderPass_PostProcessing_HDR::RegisterOnEvents()
{
	auto _Lambda_OnTextureResize = [this](Event& _Event)
	{
		this->OnRenderTargetResize(static_cast<EventTextureResize&>(_Event));
	};
	TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RenderTargetResize, _Lambda_OnTextureResize);
}
