#include "pch.h"
#include "BufferManager.h"

#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"
#include "TextureMaterial.h"

#include "API/DX12/DirectX12BufferManager.h"

namespace TruthEngine {



	std::shared_ptr<TruthEngine::BufferManager> BufferManager::Factory()
	{
		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12 :
			return API::DirectX12::DirectX12BufferManager::GetInstance();
			break;
		default:
			return nullptr;
		}
	}


	TruthEngine::TextureRenderTarget* BufferManager::CreateRenderTarget(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA)
	{
		auto rt = std::make_shared<TextureRenderTarget>(width, height, format, clearValue, useAsShaderResource, enbaleMSAA);

		m_Map_Textures[idx] = rt;

		CreateResource(rt.get());

		return rt.get();
	}


	TruthEngine::TextureDepthStencil* BufferManager::CreateDepthStencil(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource, bool enbaleMSAA)
	{
		auto ds = std::make_shared<TextureDepthStencil>(width, height, format, clearValue, useAsShaderResource, enbaleMSAA);

		m_Map_Textures[idx] = ds;

		CreateResource(ds.get());

		return ds.get();
	}


	TruthEngine::TextureRenderTarget* BufferManager::GetRenderTarget(TE_IDX_TEXTURE idx)
	{
		auto rt = m_Map_Textures.find(idx);

		if (rt == m_Map_Textures.end())
		{
			return nullptr;
		}

		return static_cast<TextureRenderTarget*>(rt->second.get());
	}

	TruthEngine::TextureDepthStencil* BufferManager::GetDepthStencil(TE_IDX_TEXTURE idx)
	{
		auto ds = m_Map_Textures.find(idx);

		if (ds == m_Map_Textures.end())
		{
			return nullptr;
		}

		return static_cast<TextureDepthStencil*>(ds->second.get());
	}

	Texture* BufferManager::GetTexture(TE_IDX_TEXTURE idx)
	{
		auto itr = m_Map_Textures.find(idx);

		if (itr == m_Map_Textures.end())
		{
			return nullptr;
		}

		return itr->second.get();
	}

	ConstantBufferUploadBase* BufferManager::GetConstantBufferUpload(TE_IDX_CONSTANTBUFFER cbIDX)
	{
		auto cbItr = m_Map_ConstantBufferUpload.find(cbIDX);
		if (cbItr != m_Map_ConstantBufferUpload.end())
		{
			return cbItr->second.get();
		}

		return nullptr;
	}

	ConstantBufferDirectBase* BufferManager::GetConstantBufferDirect(TE_IDX_CONSTANTBUFFER cbIDX)
	{
		auto cbItr = m_Map_ConstantBufferDirect.find(cbIDX);
		if (cbItr != m_Map_ConstantBufferDirect.end())
		{
			return cbItr->second.get();
		}

		return nullptr;
	}

}