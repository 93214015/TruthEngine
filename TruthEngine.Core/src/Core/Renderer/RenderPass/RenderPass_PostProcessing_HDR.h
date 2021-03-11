#pragma once
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Viewport.h"

namespace TruthEngine
{

	template<class T>
	class ConstantBufferUpload;

	class Buffer;
	class PipelineGraphics;
	class PipelineCompute;
	class EventTextureResize;


	struct RenderTargetView;

	class RenderPass_PostProcessing_HDR: public RenderPass
	{
	public:
		RenderPass_PostProcessing_HDR();
		~RenderPass_PostProcessing_HDR();


		void OnAttach() override;


		void OnDetach() override;


		void OnImGuiRender() override;


		void BeginScene() override;


		void EndScene() override;


		void Render() override;


	private:

		void InitBuffers();
		void InitTexture();
		void InitPipeline();
		void ReleaseResources();

		void OnRenderTargetResize(const EventTextureResize& _Event);
		void RegisterOnEvents();

	private:

		RendererCommand mRendererCommand_DownScaling_FirstPass;
		RendererCommand mRendererCommand_DownScaling_SecondPass;
		RendererCommand mRendererCommand_FinalPass;

		struct ConstantBuffer_Data_DownScaling
		{
			ConstantBuffer_Data_DownScaling() = default;
			ConstantBuffer_Data_DownScaling(uint32_t _ScreenQuarterSizeReolutionWidth, uint32_t _ScreenQuarterSizeReolutionHeight, uint32_t _Domain, uint32_t _GroupNum)
				: gScreenQuarterSizeResolutionWidth(_ScreenQuarterSizeReolutionWidth), gScreenQuarterSizeResolutionHeight(_ScreenQuarterSizeReolutionHeight)
				, gDomain(_Domain), gGroupNum(_GroupNum)
			{}

			uint32_t gScreenQuarterSizeResolutionWidth = 0; //Resolution of Quarter Size of HDR Source Texture
			uint32_t gScreenQuarterSizeResolutionHeight = 0; //Resolution of Quarter Size of HDR Source Texture
			uint32_t gDomain = 0; // Total Pixel in down-scaled image
			uint32_t gGroupNum = 0; // Groups number dispatched on first pass
		};

		struct ConstantBuffer_Data_FinalPass
		{
			ConstantBuffer_Data_FinalPass()
				: mMiddleGrey(0.0025f), mLumWhiteSqr(1.5f)
			{
				mLumWhiteSqr *= mMiddleGrey;
				mLumWhiteSqr *= mLumWhiteSqr;
			}
			ConstantBuffer_Data_FinalPass(float _MiddleGrey, float _LumWhiteSqr)
				: mMiddleGrey(_MiddleGrey), mLumWhiteSqr(_LumWhiteSqr)
			{
				mLumWhiteSqr *= mMiddleGrey;
				mLumWhiteSqr *= mLumWhiteSqr;
			}
			// Tone mapping
			float mMiddleGrey;
			float mLumWhiteSqr;

			float2 pad;
		};

		ConstantBufferUpload<ConstantBuffer_Data_DownScaling>* mConstantBufferDownScaling;
		ConstantBufferUpload<ConstantBuffer_Data_FinalPass>* mConstantBufferFinalPass;

		Buffer* mBufferRWAverageLumFirstPass;
		Buffer* mBufferRWAverageLumSecondPass;

		RenderTargetView mRenderTargetView_SceneBuffer;

		PipelineCompute* mPipelineDownScalingFirstPass = nullptr;
		PipelineCompute* mPipelineDownScalingSecondPass = nullptr;
		PipelineGraphics* mPipelineFinalPass = nullptr;

		Viewport mViewPort;
		ViewRect mViewRect;

		uint32_t mGroupNum = 0;
	};
}
