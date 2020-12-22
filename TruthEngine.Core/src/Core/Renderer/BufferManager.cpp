#include "pch.h"
#include "BufferManager.h"

#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"

#include "API/DX12/DirectX12BufferManager.h"

namespace TruthEngine::Core {



	std::shared_ptr<TruthEngine::Core::BufferManager> BufferManager::Factory()
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


	TruthEngine::Core::TextureRenderTarget* BufferManager::CreateRenderTarget(TE_IDX_RENDERTARGET idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource)
	{
		auto rt = std::make_shared<TextureRenderTarget>(width, height, format, clearValue, useAsShaderResource);

		m_Map_RenderTargets[idx] = rt;

		CreateResource(rt.get());

		return rt.get();
	}


	TruthEngine::Core::TextureDepthStencil* BufferManager::CreateDepthStencil(TE_IDX_DEPTHSTENCIL idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource)
	{
		auto ds = std::make_shared<TextureDepthStencil>(width, height, format, clearValue, useAsShaderResource);

		m_Map_DepthStencils[idx] = ds;

		CreateResource(ds.get());

		return ds.get();
	}

	TruthEngine::Core::TextureRenderTarget* BufferManager::GetRenderTarget(TE_IDX_RENDERTARGET idx)
	{
		auto rt = m_Map_RenderTargets.find(idx);

		if (rt == m_Map_RenderTargets.end())
		{
			return nullptr;
		}

		return rt->second.get();
	}

	TruthEngine::Core::TextureDepthStencil* BufferManager::GetDepthStencil(TE_IDX_DEPTHSTENCIL idx)
	{
		auto ds = m_Map_DepthStencils.find(idx);

		if (ds == m_Map_DepthStencils.end())
		{
			return nullptr;
		}

		return ds->second.get();
	}

}