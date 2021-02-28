#pragma once

#define TE_INSTANCE_API_DX12_PIPELINEMANAGER DirectX12PiplineManager::GetInstance()

namespace TruthEngine
{
		class Pipeline;
		class Shader;

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

			COMPTR<ID3D12PipelineState> GetPipeline(Pipeline* pipeline);

		private:
			//D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicPipelineDesc(const Pipeline* pipeline);
			TE_RESULT AddPipeline(Pipeline* pipeline, COMPTR<ID3D12PipelineState>& PSO, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

		private:
			COMPTR<ID3D12PipelineLibrary1> m_PiplineLibrary;
			uint32_t m_PipelineNum = 0;

			
		};

	}
}

