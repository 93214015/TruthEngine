#include "pch.h"
#include "DirectX12Manager.h"

#include "DirectX12ShaderManager.h"
#include "DirectX12BufferManager.h"

namespace TruthEngine::API::DirectX12
{
	std::vector<DirectX12ResourceTable>& DirectX12Manager::GetResourceTable(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX)
	{
		auto itr = m_Map_RenderPassResourceTable.find(renderPassIDX);
		if (itr != m_Map_RenderPassResourceTable.end())
		{
			return itr->second;
		}
		else //Resource table has not been created yet! 
		{
			return CreateResourceTable(renderPassIDX, shaderClassIDX);
		}
	}

	std::vector<DirectX12ResourceTable>& DirectX12Manager::CreateResourceTable(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX)
	{
		auto shaderManager = DirectX12ShaderManager::GetInstance().get();
		auto bufferManager = DirectX12BufferManager::GetInstance().get();

		auto bindedResource = shaderManager->GetBindedResource(shaderClassIDX);

		auto& resourceTables = m_Map_RenderPassResourceTable[renderPassIDX];


		for (uint32_t i = 0; i < bindedResource->ConstantBuffers.size(); ++i)
		{
			auto& v = bindedResource->ConstantBuffers[i];
			if (v.size() > 0)
			{
				Core::ConstantBufferView cbv{};
				auto cb = bufferManager->GetConstantBufferUpload(v[0]);
				bufferManager->CreateConstantBufferView(cb, &cbv);
				auto gpuHandle = bufferManager->m_DescHeapSRV.GetGPUHandle(cbv.ViewIndex);

				resourceTables.emplace_back(i, gpuHandle);

				for (uint32_t j = 1; j < v.size(); ++j)
				{
					Core::ConstantBufferView dummycbv{};
					cb = bufferManager->GetConstantBufferUpload(v[j]);
					bufferManager->CreateConstantBufferView(cb, &dummycbv);
				}
			}
		}

		for (uint32_t i = 0; i < bindedResource->Textures.size(); ++i)
		{
			for (uint32_t j = 0; j < bindedResource->Textures[i].size(); ++j)
			{
				throw std::exception("DirectX12Manager CreateResourceTable for textures has not been implemented!");
			}
		}

		return resourceTables;
	}

}