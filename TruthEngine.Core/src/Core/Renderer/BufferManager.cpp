#include "pch.h"
#include "BufferManager.h"

#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"
#include "TextureCubeMap.h"
#include "TextureMaterial.h"

#include "API/DX12/DirectX12BufferManager.h"

namespace TruthEngine {



	std::shared_ptr<TruthEngine::BufferManager> BufferManager::Factory()
	{
		switch (Settings::GetRendererAPI())
		{
		case TE_RENDERER_API::DirectX12:
			return API::DirectX12::DirectX12BufferManager::GetInstance();
			break;
		default:
			return nullptr;
		}
	}


	BufferManager::BufferManager()
	{
		m_TexturesRenderTarget.reserve(50);
		m_TexturesDepthStencil.reserve(20);
		m_TexturesCubeMap.reserve(20);
		m_Buffers.reserve(100);
		m_Textures.reserve(2000);
	}

	Buffer* BufferManager::CreateBufferStructuredRW(TE_IDX_GRESOURCES _IDX, uint32_t _ElementSizeInByte, uint32_t _ElementNum, bool _IsByteAddressBuffer)
	{
		auto _Itr = m_Map_GraphicResources.find(_IDX);
		Buffer* _Buffer = nullptr;

		if (_Itr == m_Map_GraphicResources.end())
		{

			_Buffer = &m_Buffers.emplace_back(_IDX, (_ElementSizeInByte * _ElementNum), TE_RESOURCE_USAGE_SHADERRESOURCE | TE_RESOURCE_USAGE_STRUCTUREDBUFFER | TE_RESOURCE_USAGE_UNORDEREDACCESS, TE_RESOURCE_STATES::UNORDERED_ACCESS, _ElementNum, _ElementSizeInByte, _IsByteAddressBuffer, false);

			m_Map_Buffers[_IDX] = _Buffer;
			m_Map_GraphicResources[_IDX] = _Buffer;
		}
		else
		{
			_Buffer = static_cast<Buffer*>(_Itr->second);
			_Buffer->m_SizeInByte = (_ElementSizeInByte * _ElementNum);
			_Buffer->m_ElementNum = _ElementNum;
			_Buffer->m_ElementSizeInByte = _ElementSizeInByte;
		}

		CreateResource(_Buffer);

		return _Buffer;
	}

	Texture* BufferManager::CreateTextureRW(TE_IDX_GRESOURCES _IDX, uint32_t _Width, uint32_t _Height, TE_RESOURCE_FORMAT _FORMAT, bool _UseAsShaderResource, bool _EnableMSAA)
	{
		auto& _Itr = m_Map_GraphicResources.find(_IDX);

		Texture* _Texture;

		TE_RESOURCE_USAGE _Usage = TE_RESOURCE_USAGE_UNORDEREDACCESS;
		_Usage |= _UseAsShaderResource ? TE_RESOURCE_USAGE_SHADERRESOURCE : static_cast<TE_RESOURCE_USAGE>(0);

		if (_Itr != m_Map_GraphicResources.end())
		{
			_Texture = static_cast<Texture*>(_Itr->second);
			_Texture->m_Width = _Width;
			_Texture->m_Height = _Height;
			_Texture->m_Format = _FORMAT;
		}
		else
		{
			_Texture = &m_Textures.emplace_back(_IDX, _Width, _Height, _FORMAT, _Usage, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::UNORDERED_ACCESS, _EnableMSAA);

			m_Map_Textures[_IDX] = _Texture;
			m_Map_GraphicResources[_IDX] = _Texture;
		}

		CreateResource(_Texture);

		return _Texture;
	}

	TruthEngine::TextureRenderTarget* BufferManager::CreateRenderTarget(TE_IDX_GRESOURCES _IDX, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA)
	{
		auto _Itr = m_Map_GraphicResources.find(_IDX);

		TextureRenderTarget* rt = nullptr;

		if (_Itr == m_Map_GraphicResources.end())
		{
			rt = &m_TexturesRenderTarget.emplace_back(_IDX, width, height, format, clearValue, useAsShaderResource, enbaleMSAA);

			m_Map_Textures[_IDX] = rt;
			m_Map_GraphicResources[_IDX] = rt;
		}
		else
		{
			rt = static_cast<TextureRenderTarget*>(_Itr->second);
			rt->m_Width = width;
			rt->m_Height = height;
			rt->m_Format = format;
			rt->m_EnableMSAA = enbaleMSAA;
			rt->m_ClearValue = clearValue;
			//*rt = TextureRenderTarget(_IDX, width, height, format, clearValue, useAsShaderResource, enbaleMSAA);
		}

		CreateResource(rt);

		return rt;
	}


	TruthEngine::TextureDepthStencil* BufferManager::CreateDepthStencil(TE_IDX_GRESOURCES _IDX, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource, bool enbaleMSAA)
	{

		auto _Itr = m_Map_GraphicResources.find(_IDX);
		TextureDepthStencil* ds = nullptr;

		if (_Itr == m_Map_GraphicResources.end())
		{
			ds = &m_TexturesDepthStencil.emplace_back(_IDX, width, height, format, clearValue, useAsShaderResource, enbaleMSAA);

			m_Map_Textures[_IDX] = ds;
			m_Map_GraphicResources[_IDX] = ds;
		}
		else
		{
			ds = static_cast<TextureDepthStencil*>(_Itr->second);
			ds->m_Width = width;
			ds->m_Height = height;
			ds->m_ClearValue = clearValue;
			ds->m_EnableMSAA = enbaleMSAA;
			ds->m_Format = format;
		}

		CreateResource(ds);

		return ds;
	}


	TruthEngine::GraphicResource* BufferManager::GetGraphicResource(TE_IDX_GRESOURCES _IDX) const
	{
		auto& _Itr = m_Map_GraphicResources.find(_IDX);

		if (_Itr == m_Map_GraphicResources.end())
		{
			return nullptr;
		}

		return _Itr->second;
	}


	TruthEngine::TextureRenderTarget* BufferManager::GetRenderTarget(TE_IDX_GRESOURCES idx)
	{
		auto rt = m_Map_Textures.find(idx);

		if (rt == m_Map_Textures.end())
		{
			return nullptr;
		}

		return static_cast<TextureRenderTarget*>(rt->second);
	}

	TruthEngine::TextureDepthStencil* BufferManager::GetDepthStencil(TE_IDX_GRESOURCES idx)
	{
		auto ds = m_Map_Textures.find(idx);

		if (ds == m_Map_Textures.end())
		{
			return nullptr;
		}

		return static_cast<TextureDepthStencil*>(ds->second);
	}

	Buffer* BufferManager::GetBuffer(TE_IDX_GRESOURCES _IDX)
	{
		auto _Itr = m_Map_Buffers.find(_IDX);
		if (_Itr != m_Map_Buffers.end())
		{
			return _Itr->second;
		}
		else
		{
			return nullptr;
		}
	}

	TruthEngine::TextureCubeMap* BufferManager::GetCubeMap(uint32_t index)
	{
		return &m_TexturesCubeMap[index];
	}

	TruthEngine::TextureCubeMap* BufferManager::GetSkyCubeMap()
	{
		return &m_TexturesCubeMap[m_SkyCubeMapIndex];
	}

	Texture* BufferManager::GetTexture(TE_IDX_GRESOURCES idx)
	{
		auto itr = m_Map_Textures.find(idx);

		if (itr == m_Map_Textures.end())
		{
			return nullptr;
		}

		return itr->second;
	}

	ConstantBufferUploadBase* BufferManager::GetConstantBufferUpload(TE_IDX_GRESOURCES cbIDX)
	{
		auto cbItr = m_Map_ConstantBufferUpload.find(cbIDX);
		if (cbItr != m_Map_ConstantBufferUpload.end())
		{
			return cbItr->second.get();
		}

		return nullptr;
	}

	ConstantBufferDirectBase* BufferManager::GetConstantBufferDirect(TE_IDX_GRESOURCES cbIDX)
	{
		auto cbItr = m_Map_ConstantBufferDirect.find(cbIDX);
		if (cbItr != m_Map_ConstantBufferDirect.end())
		{
			return cbItr->second.get();
		}

		return nullptr;
	}

}