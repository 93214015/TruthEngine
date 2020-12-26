#include "pch.h"
#include "DirectX12Manager.h"

#include "DirectX12ShaderManager.h"
#include "DirectX12BufferManager.h"

namespace TruthEngine::API::DirectX12
{
	DirectX12RootParameter* DirectX12Manager::GetRootParameter(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX)
	{
		if (renderPassIDX == TE_IDX_RENDERPASS::NONE || shaderClassIDX == TE_IDX_SHADERCLASS::NONE)
			return nullptr;

		auto itr = m_Map_ShaderClassRootParameters.find(shaderClassIDX);
		if (itr != m_Map_ShaderClassRootParameters.end())
		{
			return &itr->second;
		}
		else //Resource table has not been created yet! 
		{
			return CreateRootParameter(renderPassIDX, shaderClassIDX);
		}
	}

	uint32_t DirectX12Manager::GetRootParameterIndex(TE_IDX_CONSTANTBUFFER constantBufferIDX, TE_IDX_SHADERCLASS shaderClassIDX)
	{
		auto& rootParameter = m_Map_ShaderClassRootParameters[shaderClassIDX];

		auto itr = rootParameter.DirectConstants.find(constantBufferIDX);

		if (itr == rootParameter.DirectConstants.end())
		{
			return -1;
		}

		return itr->second;


		/*auto shaderManager = DirectX12ShaderManager::GetInstance().get();
		auto bindedResource = shaderManager->GetBindedResource(shaderIDX);


		for (uint32_t i = 0; i < bindedResource->ConstantBuffers.size(); ++i)
		{
			auto& v = bindedResource->ConstantBuffers[i];
			for (uint32_t j = 0; j < v.size(); ++j)
			{
				if (v[j] == cbIDX)
				{
					return i;
				}
			}
		}

		return -1;*/
	}

	DirectX12RootParameter* DirectX12Manager::CreateRootParameter(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX)
	{
		auto shaderManager = DirectX12ShaderManager::GetInstance().get();
		auto bufferManager = DirectX12BufferManager::GetInstance().get();

		auto bindedResource = shaderManager->GetBindedResource(shaderClassIDX);

		auto& rootParameter = m_Map_ShaderClassRootParameters[shaderClassIDX];

		for (uint32_t i = 0; i < bindedResource->ConstantBuffers.size(); ++i)
		{
			auto& v = bindedResource->ConstantBuffers[i];
			if (v.size() > 0)
			{

				if (static_cast<uint32_t>(v[0]) > static_cast<uint32_t>(TE_IDX_CONSTANTBUFFER::DIRECT_CONSTANTS))
				{
					rootParameter.DirectConstants[v[0]] = i;
					continue;
				}

				Core::ConstantBufferView cbv{};
				auto cb = bufferManager->GetConstantBufferUpload(v[0]);
				bufferManager->CreateConstantBufferView(cb, &cbv);
				auto gpuHandle = bufferManager->m_DescHeapSRV.GetGPUHandle(cbv.ViewIndex);

				rootParameter.RootTables.emplace_back(i, gpuHandle);

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

		return &rootParameter;
	}

}