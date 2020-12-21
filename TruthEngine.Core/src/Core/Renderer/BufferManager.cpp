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


	//void BufferManager::CreateRenderTargetView(TE_IDX_RENDERTARGET idx, RenderTargetView* rtv)
	//{
	//	auto rt = m_Map_RenderTargets.find(idx);

	//	if (rt == m_Map_RenderTargets.end())
	//	{
	//		throw std::exception("CreateRenderTargetView: the renderTarget resource was not found!");
	//	}

	//	return CreateRenderTargetView(rt->second.get(), rtv);
	//}


	//void BufferManager::CreateDepthStencilView(TE_IDX_DEPTHSTENCIL idx, DepthStencilView* dsv)
	//{
	//	auto ds = m_Map_DepthStencils.find(idx);

	//	if (ds == m_Map_DepthStencils.end())
	//	{
	//		throw std::exception("CreateDepthStencilView: the depthStencil resource was not found!");
	//	}

	//	return CreateDepthStencilView(ds->second.get(), dsv);
	//}

	//void BufferManager::CreateConstantBufferView(TE_IDX_CONSTANTBUFFER constantBufferIDX, ConstantBufferView* CBV)
	//{
	//	auto cb = m_Map_ConstantBufferUpload.find(constantBufferIDX);

	//	if (cb == m_Map_ConstantBufferUpload.end())
	//	{
	//		throw std::exception("CreateConstantBufferView: the ConstantBuffer resource was not found!");
	//	}

	//	CreateConstantBufferView(cb->second.get(), CBV);
	//}

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