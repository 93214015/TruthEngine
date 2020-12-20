#pragma once

namespace TruthEngine
{

	namespace API::DirectX12
	{

		struct DirectX12ResourceTable
		{
			DirectX12ResourceTable() = default;
			DirectX12ResourceTable(uint32_t tableIndex, const D3D12_GPU_DESCRIPTOR_HANDLE tableHandle)
				: TableIndex(tableIndex), TableHandle(tableHandle)
			{}

			uint32_t TableIndex;
			D3D12_GPU_DESCRIPTOR_HANDLE TableHandle;
		};


		class DirectX12Manager 
		{
		public:
			std::vector<DirectX12ResourceTable>& GetResourceTable(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX);

			static std::shared_ptr<DirectX12Manager> GetInstance()
			{
				static auto s_Instance = std::make_shared<DirectX12Manager>();
				return s_Instance;
			}
		private:
			std::vector<DirectX12ResourceTable>& CreateResourceTable(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX);

		private:
			std::unordered_map<TE_IDX_RENDERPASS, std::vector<DirectX12ResourceTable>> m_Map_RenderPassResourceTable;

		};
	}
}
