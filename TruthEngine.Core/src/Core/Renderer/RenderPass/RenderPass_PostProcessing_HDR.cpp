#include "pch.h"
#include "RenderPass_PostProcessing_HDR.h"

#include "Core/Renderer/ShaderManager.h"

#include "Core/Event/EventApplication.h"

TruthEngine::RenderPass_PostProcessing_HDR::RenderPass_PostProcessing_HDR()
	: RenderPass(TE_IDX_RENDERPASS::POSTPROCESSING_HDR)
	, mViewPort(0, 0, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), .0f, 1.0f)
	, mViewRect(static_cast<long>(0), static_cast<long>(0), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()))
{}

TruthEngine::RenderPass_PostProcessing_HDR::~RenderPass_PostProcessing_HDR() = default;

void TruthEngine::RenderPass_PostProcessing_HDR::OnAttach()
{
	mGroupNum = static_cast<uint32_t>(std::ceilf(static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth() * TE_INSTANCE_APPLICATION->GetClientHeight()) / (1024.0f * 16.0f)));

	mRendererCommand_DownScaling_FirstPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_FIRSTPASS);
	mRendererCommand_DownScaling_SecondPass.Init(TE_IDX_RENDERPASS::POSTPROCESSING_HDR, TE_IDX_SHADERCLASS::POSTPROCESSING_HDR_DOWNSACLING_SECONDPASS);
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


	if (ImGui::Begin("PostProcessing HDR RenderPass"))
	{
		if (ImGui::DragFloat("Middle Grey:", &_MiddleGrey, 0.00001, 0.0f, 1.0f, "%.5f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { *mConstantBufferFinalPass->GetData() = ConstantBuffer_Data_FinalPass(_MiddleGrey, _WhiteSquare); });
		}
		if (ImGui::DragFloat("Luminance White Square:", &_WhiteSquare, 0.01, 0.0f, 10.f, "%2.2f"))
		{
			mRendererCommand_FinalPass.AddUpdateTask([this]() { *mConstantBufferFinalPass->GetData() = ConstantBuffer_Data_FinalPass(_MiddleGrey, _WhiteSquare); });
		}
	}
	ImGui::End();
}

void TruthEngine::RenderPass_PostProcessing_HDR::BeginScene()
{
	mRendererCommand_DownScaling_FirstPass.BeginCompute();

	mRendererCommand_DownScaling_SecondPass.BeginCompute();

	mRendererCommand_FinalPass.BeginGraphics();
	mRendererCommand_FinalPass.SetRenderTarget(mRenderTargetView_SceneBuffer);
	mRendererCommand_FinalPass.SetViewPort(&mViewPort, &mViewRect);
}

void TruthEngine::RenderPass_PostProcessing_HDR::EndScene()
{
	mRendererCommand_DownScaling_FirstPass.End();

	mRendererCommand_DownScaling_SecondPass.End();

	mRendererCommand_FinalPass.End();
}

#include "API/DX12/DirectX12GraphicDevice.h"
#include "API/DX12/DirectX12BufferManager.h"

void TruthEngine::RenderPass_PostProcessing_HDR::Render()
{
	mRendererCommand_DownScaling_FirstPass.SetPipelineCompute(mPipelineDownScalingFirstPass);
	mRendererCommand_DownScaling_FirstPass.Dispatch(mGroupNum, 1, 1);

	mRendererCommand_DownScaling_SecondPass.SetPipelineCompute(mPipelineDownScalingSecondPass);
	mRendererCommand_DownScaling_SecondPass.Dispatch(1, 1, 1);

	mRendererCommand_FinalPass.SetPipelineGraphics(mPipelineFinalPass);
	mRendererCommand_FinalPass.Draw(4, 0);

}


void TruthEngine::RenderPass_PostProcessing_HDR::InitBuffers()
{
	TE_ASSERT_CORE(mGroupNum != 0, "RenderPass_PostProcessingHDR : The GroupSize is 0");

	mBufferRWAverageLumFirstPass = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumFirstPass, 4, mGroupNum, false);
	mBufferRWAverageLumSecondPass = mRendererCommand_DownScaling_FirstPass.CreateBufferStructuredRW(TE_IDX_GRESOURCES::Buffer_HDRAverageLumSecondPass, 4, 1, false);

	mConstantBufferDownScaling = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_DownScaling>(TE_IDX_GRESOURCES::CBuffer_HDR_DownScaling);
	mConstantBufferFinalPass = mRendererCommand_DownScaling_FirstPass.CreateConstantBufferUpload<ConstantBuffer_Data_FinalPass>(TE_IDX_GRESOURCES::CBuffer_HDR_FinalPass);

	mRendererCommand_FinalPass.AddUpdateTask([=]() {*mConstantBufferFinalPass->GetData() = ConstantBuffer_Data_FinalPass(0.0025f, 1.5f); });

}

void TruthEngine::RenderPass_PostProcessing_HDR::InitTexture()
{
	mRendererCommand_DownScaling_FirstPass.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &mRenderTargetView_SceneBuffer);
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
	mRendererCommand_DownScaling_FirstPass.ReleaseResource(mBufferRWAverageLumSecondPass);
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

	uint32_t _QWidth = width / 4;
	uint32_t _QHeight = height / 4;

	mRendererCommand_DownScaling_FirstPass.AddUpdateTask([=]()
		{
			*mConstantBufferDownScaling->GetData() = ConstantBuffer_Data_DownScaling(_QWidth, _QHeight, _QWidth * _QHeight, mGroupNum);
		});

	mRendererCommand_DownScaling_FirstPass.CreateRenderTargetView(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer, &mRenderTargetView_SceneBuffer);
}

void TruthEngine::RenderPass_PostProcessing_HDR::RegisterOnEvents()
{
	auto _Lambda_OnTextureResize = [this](Event& _Event)
	{
		this->OnRenderTargetResize(static_cast<EventTextureResize&>(_Event));
	};
	TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::RenderTargetResize, _Lambda_OnTextureResize);
}
