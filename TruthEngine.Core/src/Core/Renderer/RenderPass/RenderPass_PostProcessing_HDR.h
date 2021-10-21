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


	struct RenderTargetView;

	class RenderPass_PostProcessing_HDR final : public RenderPass
	{
	public:
		RenderPass_PostProcessing_HDR(RendererLayer* _RendererLayer);


		void OnImGuiRender() override;


		void OnUpdate(double _DeltaTime) override;


		void BeginScene() override;


		void EndScene() override;


		void Render() override;


	private:

		void InitRendererCommand() override;
		void InitTextures() override;
		void InitBuffers() override;
		void InitPipelines() override;

		void ReleaseRendererCommand() override;
		void ReleaseTextures() override;
		void ReleaseBuffers() override;
		void ReleasePipelines() override;

		void RegisterEventListeners() override;
		void UnRegisterEventListeners() override;

		void ResizedViewport(uint32_t _Width, uint32_t Height);

		void OnRendererViewportResize(const class EventRendererViewportResize& _Event);

	private:

		RendererCommand mRendererCommand_DownScaling_FirstPass;
		RendererCommand mRendererCommand_DownScaling_SecondPass;
		RendererCommand mRendererCommand_BloomPass;
		RendererCommand mRendererCommand_BlurPassHorz;
		RendererCommand mRendererCommand_BlurPassVert;
		RendererCommand mRendererCommand_FinalPass;
		

		struct alignas(16) ConstantBuffer_Data_DownScaling
		{
			ConstantBuffer_Data_DownScaling() = default;
			ConstantBuffer_Data_DownScaling(uint32_t _ScreenQuarterSizeReolutionWidth, uint32_t _ScreenQuarterSizeReolutionHeight, uint32_t _Domain, uint32_t _GroupNum, float _Adaption, float _BloomThreshold)
				: gScreenQuarterSizeResolutionWidth(_ScreenQuarterSizeReolutionWidth), gScreenQuarterSizeResolutionHeight(_ScreenQuarterSizeReolutionHeight)
				, gDomain(_Domain), gGroupNum(_GroupNum), gAdaption(_Adaption), gBloomThreshold(_BloomThreshold)
			{}

			uint32_t gScreenQuarterSizeResolutionWidth = 0; //Resolution of Quarter Size of HDR Source Texture
			uint32_t gScreenQuarterSizeResolutionHeight = 0; //Resolution of Quarter Size of HDR Source Texture
			uint32_t gDomain = 0; // Total Pixel in down-scaled image
			uint32_t gGroupNum = 0; // Groups number dispatched on first pass

			float gAdaption = .0f;
			float gBloomThreshold = .0f;
			float2 Pad;
		};

		struct alignas(16) ConstantBuffer_Data_FinalPass
		{
			ConstantBuffer_Data_FinalPass()
				: mMiddleGrey(0.0025f), mLumWhiteSqr(1.5f), mBloomScale(1.0f)
			{
				mLumWhiteSqr *= mMiddleGrey;
				mLumWhiteSqr *= mLumWhiteSqr;
			}
			ConstantBuffer_Data_FinalPass(float _MiddleGrey, float _LumWhiteSqr, float _BloomScale, float2 _ProjectionValues, float2 _DOFFarValues)
				: mMiddleGrey(_MiddleGrey), mLumWhiteSqr(_LumWhiteSqr), mBloomScale(_BloomScale), mProjectionValues(_ProjectionValues), mDOFFarValues(_DOFFarValues)
			{
				mLumWhiteSqr *= mMiddleGrey;
				mLumWhiteSqr *= mLumWhiteSqr;
			}

			void SetLumWhiteSqr(float _Value)
			{
				mLumWhiteSqr = _Value * mMiddleGrey;
				mLumWhiteSqr *= mLumWhiteSqr;
			}

			// Tone mapping
			float mMiddleGrey;
			float mLumWhiteSqr;
			float mBloomScale;
			float pad;

			float2 mProjectionValues;
			float2 mDOFFarValues;
		};

		struct alignas(16) ConstantBuffer_Data_BlurPass
		{
			ConstantBuffer_Data_BlurPass() = default;
			ConstantBuffer_Data_BlurPass(uint32_t _InputResWidth, uint32_t _InputResHeight)
				: mInputResWidth(_InputResWidth), mInputResHeight(_InputResHeight)
			{}

			uint32_t mInputResWidth;
			uint32_t mInputResHeight;
			uint32_t mPad[2];
		};

		ConstantBufferUpload<ConstantBuffer_Data_DownScaling>* mConstantBufferDownScaling;
		ConstantBufferUpload<ConstantBuffer_Data_FinalPass>* mConstantBufferFinalPass;
		ConstantBufferUpload<ConstantBuffer_Data_BlurPass>* mConstantBufferBlurPass;

		Buffer* mBufferRWAverageLumFirstPass;
		Buffer* mBufferRWAverageLumSecondPass0;
		Buffer* mBufferRWAverageLumSecondPass1;

		Texture* mRWTextureDownScaledHDR;
		Texture* mRWTextureBloom;
		Texture* mRWTextureBluredBloomHorz;
		Texture* mRWTextureBluredBloom;

		PipelineCompute mPipelineDownScalingFirstPass;
		PipelineCompute mPipelineDownScalingSecondPass;
		PipelineCompute mPipelineBloomPass;
		PipelineCompute mPipelineBlurPassHorz;
		PipelineCompute mPipelineBlurPassVert;
		PipelineGraphics mPipelineFinalPass_ReinhardToneMapping;
		PipelineGraphics mPipelineFinalPass_ACESToneMapping;
		PipelineGraphics* mPipelineFinalPass_Selected = &mPipelineFinalPass_ACESToneMapping;


		//ScreenSpace Reflection
		PipelineGraphics m_Pipeline_Reflection;
		PipelineGraphics m_Pipeline_Blend;

		uint32_t mSceneViewQuarterSize[2];
		uint32_t mGroupNum = 0;
		float mAdaptationPercentage; //Init in Constructor 
		float mAdaptation = 0.0f;
		float mBloomScale = 1.0f;
		float mBloomThreshold = 1.0f;
		float mDOFFarStart = 40.0f;
		float mDOFFarRange = 60.0f;


		std::vector<EventListenerHandle> m_EventListenerList;
	};
}
