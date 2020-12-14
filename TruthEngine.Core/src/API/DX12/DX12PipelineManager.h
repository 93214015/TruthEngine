#pragma once

#define TE_INSTANCE_API_DX12_PIPELINEMANAGER DX12PiplineManager::Get()

namespace TruthEngine
{


	namespace Core
	{
		class Pipeline;
		class Shader;
	}

	namespace API::DirectX12 
	{

		class DX12PiplineManager
		{
		public:
			DX12PiplineManager();


			inline static std::shared_ptr<DX12PiplineManager> Get()
			{
				static std::shared_ptr<DX12PiplineManager> s_DX12PipelineManager = std::make_shared<DX12PiplineManager>();
				return s_DX12PipelineManager;
			}

			COMPTR<ID3D12PipelineState> GetPipeline(Core::Pipeline* pipeline);

		private:
			//D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicPipelineDesc(const Core::Pipeline* pipeline);
			TE_RESULT AddPipeline(Core::Pipeline* pipeline, COMPTR<ID3D12PipelineState>& PSO, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

		private:
			COMPTR<ID3D12PipelineLibrary1> m_PiplineLibrary;
			uint32_t m_PipelineNum = 0;

			
		};

	}
}

