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
			std::variant<DirectX12RootTable, DirectX12RootConstant> parameter;
		};

		class DirectX12Manager 
		{
		public:
			std::vector<DirectX12RootParameter>& GetRootParamters(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX);

			static std::shared_ptr<DirectX12Manager> GetInstance()
			{
				static auto s_Instance = std::make_shared<DirectX12Manager>();
				return s_Instance;
			}
		private:
			std::vector<DirectX12RootTable>& CreateResourceTable(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX);

		private:
			std::unordered_map<TE_IDX_RENDERPASS, std::vector<DirectX12RootTable>> m_Map_RenderPassResourceTable;

		};
	}
}