#pragma once

namespace TruthEngine
{

	namespace API::DirectX12
	{

		
		struct DirectX12RootConstant
		{
			uint32_t RootIndex;
			uint32_t Num32BitConstants;
		};

		struct DirectX12RootTable
		{
			DirectX12RootTable() = default;
			DirectX12RootTable(uint32_t tableIndex, const D3D12_GPU_DESCRIPTOR_HANDLE tableHandle)
				: RootIndex(tableIndex), TableHandle(tableHandle)
			{}

			uint32_t RootIndex;
			D3D12_GPU_DESCRIPTOR_HANDLE TableHandle;
		};


		struct DirectX12RootParameter
		{
			std::vector<DirectX12RootTable> RootTables;
			std::unordered_map<TE_IDX_CONSTANTBUFFER, uint32_t> DirectConstants;
		};

		class DirectX12Manager 
		{
		public:
			DirectX12RootParameter* GetRootParameter(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX);

			uint32_t GetRootParameterIndex(TE_IDX_CONSTANTBUFFER constantBufferIDX, TE_IDX_SHADERCLASS shaderClassIDX);

			static std::shared_ptr<DirectX12Manager> GetInstance()
			{
				static auto s_Instance = std::make_shared<DirectX12Manager>();
				return s_Instance;
			}
		private:
			DirectX12RootParameter* CreateRootParameter(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX);

		private:
			std::unordered_map<TE_IDX_SHADERCLASS, DirectX12RootParameter> m_Map_ShaderClassRootParameters;

		};
	}
}