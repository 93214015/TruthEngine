#pragma once

#define TE_INSTANCE_API_DX12_PIPELINEMANAGER DirectX12PiplineManager::GetInstance()

namespace TruthEngine
{
		class PipelineGraphics;
		class PipelineCompute;
		class Shader;
		class EventRendererNewGraphicsPipeline;
		class EventRendererNewComputePipeline;

	namespace API::DirectX12 
	{

		class DirectX12PiplineManager
		{
		public:
			DirectX12PiplineManager();


			inline static std::shared_ptr<DirectX12PiplineManager> GetInstance()
			{
				static std::shared_ptr<DirectX12PiplineManager> s_DX12PipelineManager = std::make_shared<DirectX12PiplineManager>();
				return s_DX12PipelineManager;
			}

			D3D12_GRAPHICS_PIPELINE_STATE_DESC GetDesc(const PipelineGraphics* _Pipeline) const;
			D3D12_COMPUTE_PIPELINE_STATE_DESC GetDesc(const PipelineCompute* _Pipeline) const;

			COMPTR<ID3D12PipelineState> GetGraphicsPipeline(PipelineGraphics* pipeline);
			COMPTR<ID3D12PipelineState> GetComputePipeline(PipelineCompute* pipeline);

		private:
			//D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicPipelineDesc(const Pipeline* pipeline);
			TE_RESULT AddGraphicsPipeline(PipelineGraphics* pipeline, COMPTR<ID3D12PipelineState>& PSO, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
			TE_RESULT AddComputePipeline(PipelineCompute* pipeline, COMPTR<ID3D12PipelineState>& PSO, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc);

			TE_RESULT OnEventNewPipelineGraphics(const EventRendererNewGraphicsPipeline& _Event);
			TE_RESULT OnEventNewPipelineCompute(const EventRendererNewComputePipeline& _Event);

		private:
			COMPTR<ID3D12PipelineLibrary1> m_PiplineLibrary;

			std::unordered_map<uint32_t, COMPTR<ID3D12PipelineState>> m_MapPipelineState;
			
		};

	}
}

