#include "pch.h"
#include "DirectX12BufferManager.h"

#include "DirectXTK12/Inc/ResourceUploadBatch.h"
#include "DirectXTK12/Inc/WICTextureLoader.h"

#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/TextureDepthStencil.h"
#include "Core/Renderer/TextureMaterial.h"
#include "Core/Renderer/ConstantBuffer.h"
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/VertexBufferStream.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/Buffer.h"


#include "DirectX12GraphicDevice.h"
#include "DirectX12SwapChain.h"
#include "DirectX12Manager.h"
#include "DirectX12Helpers.h"

#include "DirectXTK12/Inc/ResourceUploadBatch.h"
#include "DirectXTK12/Inc/WICTextureLoader.h"
#include "DirectXTK12/Inc/DDSTextureLoader.h"
#include "DirectXTex/DirectXTex.h"

#include <boost/filesystem.hpp>

namespace TruthEngine::API::DirectX12
{

	DXGI_FORMAT GetFormat(const TE_RESOURCE_FORMAT format)
	{
		return static_cast<DXGI_FORMAT>(format);
	}


	DXGI_FORMAT GetDSVFormat(const TE_RESOURCE_FORMAT format)
	{
		switch (format)
		{
		case TE_RESOURCE_FORMAT::R32_TYPELESS:
			return DXGI_FORMAT_D32_FLOAT;
		case TE_RESOURCE_FORMAT::R24_UNORM_X8_TYPELESS:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case TE_RESOURCE_FORMAT::R24G8_TYPELESS:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case TE_RESOURCE_FORMAT::R32_FLOAT_X8X24_TYPELESS:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		default:
			TE_ASSERT_CORE(false, "wrong depth stecil format");
			return DXGI_FORMAT_UNKNOWN;
		}
	}


	D3D12_RESOURCE_FLAGS GetResourceFlags(TE_RESOURCE_USAGE usage)
	{
		D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE;

		if (usage & TE_RESOURCE_USAGE_RENDERTARGET)
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (!(usage & TE_RESOURCE_USAGE_SHADERRESOURCE))
		{
			if (usage & TE_RESOURCE_USAGE_DEPTHSTENCIL)
				resourceFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		}

		if (usage & TE_RESOURCE_USAGE_DEPTHSTENCIL)
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		if (usage & TE_RESOURCE_USAGE_UNORDEREDACCESS)
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return resourceFlags;
	}


	D3D12_RESOURCE_DESC1 GetBufferDesc(uint64_t sizeInByte, TE_RESOURCE_USAGE usage)
	{
		sizeInByte = static_cast<uint64_t>(std::ceil(static_cast<double>(sizeInByte) / 256.0) * 256);

		const auto resourceFlags = GetResourceFlags(usage);
		return CD3DX12_RESOURCE_DESC1::Buffer(sizeInByte, resourceFlags);

	}


	D3D12_RESOURCE_DESC1 GetTextureDesc(Texture* _Texture)
	{
		const auto resourceFlags = GetResourceFlags(_Texture->GetUsage());
		auto desc = CD3DX12_RESOURCE_DESC1::Tex2D(GetFormat(_Texture->GetFormat()), static_cast<uint64_t>(_Texture->GetWidth()), _Texture->GetHeight(), _Texture->GetArraySize(), _Texture->GetMipLevels(), 1, 0, resourceFlags);

		if (_Texture->IsMultiSample())
		{
			desc.MipLevels = 1;
			desc.SampleDesc.Count = static_cast<int>(Settings::Graphics::GetMSAA());
			desc.SampleDesc.Quality = 0;
		}

		return desc;
	}


	D3D12_RESOURCE_STATES GetInitialState(TE_RESOURCE_USAGE usage)
	{
		switch (usage)
		{
		case TE_RESOURCE_USAGE_UNKNOWN:
			return D3D12_RESOURCE_STATE_COMMON;
			break;
		case TE_RESOURCE_USAGE_RENDERTARGET:
			return D3D12_RESOURCE_STATE_RENDER_TARGET;
			break;
		case TE_RESOURCE_USAGE_DEPTHSTENCIL:
			return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			break;
		case TE_RESOURCE_USAGE_UNORDEREDACCESS:
			return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			break;
		case TE_RESOURCE_USAGE_SHADERRESOURCE:
			return D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		}
	}

	size_t GetPixelByteSize(TE_RESOURCE_FORMAT _Format)
	{
		switch (_Format)
		{
		case TE_RESOURCE_FORMAT::R32G32B32A32_TYPELESS:
		case TE_RESOURCE_FORMAT::R32G32B32A32_FLOAT:
		case TE_RESOURCE_FORMAT::R32G32B32A32_UINT:
		case TE_RESOURCE_FORMAT::R32G32B32A32_SINT:
			return 16;
			break;
		case TE_RESOURCE_FORMAT::R32G32B32_TYPELESS:
		case TE_RESOURCE_FORMAT::R32G32B32_FLOAT:
		case TE_RESOURCE_FORMAT::R32G32B32_UINT:
		case TE_RESOURCE_FORMAT::R32G32B32_SINT:
			return 12;
			break;
		case TE_RESOURCE_FORMAT::R16G16B16A16_TYPELESS:
		case TE_RESOURCE_FORMAT::R16G16B16A16_FLOAT:
		case TE_RESOURCE_FORMAT::R16G16B16A16_UNORM:
		case TE_RESOURCE_FORMAT::R16G16B16A16_UINT:
		case TE_RESOURCE_FORMAT::R16G16B16A16_SNORM:
		case TE_RESOURCE_FORMAT::R16G16B16A16_SINT:
		case TE_RESOURCE_FORMAT::R32G32_TYPELESS:
		case TE_RESOURCE_FORMAT::R32G32_FLOAT:
		case TE_RESOURCE_FORMAT::R32G32_UINT:
		case TE_RESOURCE_FORMAT::R32G32_SINT:
		case TE_RESOURCE_FORMAT::R32G8X24_TYPELESS:
		case TE_RESOURCE_FORMAT::D32_FLOAT_S8X24_UINT:
		case TE_RESOURCE_FORMAT::R32_FLOAT_X8X24_TYPELESS:
		case TE_RESOURCE_FORMAT::X32_TYPELESS_G8X24_UINT:
			return 8;
			break;
		case TE_RESOURCE_FORMAT::R10G10B10A2_TYPELESS:
		case TE_RESOURCE_FORMAT::R10G10B10A2_UNORM:
		case TE_RESOURCE_FORMAT::R10G10B10A2_UINT:
		case TE_RESOURCE_FORMAT::R11G11B10_FLOAT:
		case TE_RESOURCE_FORMAT::R8G8B8A8_TYPELESS:
		case TE_RESOURCE_FORMAT::R8G8B8A8_UNORM:
		case TE_RESOURCE_FORMAT::R8G8B8A8_UNORM_SRGB:
		case TE_RESOURCE_FORMAT::R8G8B8A8_UINT:
		case TE_RESOURCE_FORMAT::R8G8B8A8_SNORM:
		case TE_RESOURCE_FORMAT::R8G8B8A8_SINT:
		case TE_RESOURCE_FORMAT::R16G16_TYPELESS:
		case TE_RESOURCE_FORMAT::R16G16_FLOAT:
		case TE_RESOURCE_FORMAT::R16G16_UNORM:
		case TE_RESOURCE_FORMAT::R16G16_UINT:
		case TE_RESOURCE_FORMAT::R16G16_SNORM:
		case TE_RESOURCE_FORMAT::R16G16_SINT:
		case TE_RESOURCE_FORMAT::R32_TYPELESS:
		case TE_RESOURCE_FORMAT::D32_FLOAT:
		case TE_RESOURCE_FORMAT::R32_FLOAT:
		case TE_RESOURCE_FORMAT::R32_UINT:
		case TE_RESOURCE_FORMAT::R32_SINT:
		case TE_RESOURCE_FORMAT::R24G8_TYPELESS:
		case TE_RESOURCE_FORMAT::D24_UNORM_S8_UINT:
		case TE_RESOURCE_FORMAT::R24_UNORM_X8_TYPELESS:
		case TE_RESOURCE_FORMAT::X24_TYPELESS_G8_UINT:
			return 4;
			break;
		case TE_RESOURCE_FORMAT::R8G8_TYPELESS:
		case TE_RESOURCE_FORMAT::R8G8_UNORM:
		case TE_RESOURCE_FORMAT::R8G8_UINT:
		case TE_RESOURCE_FORMAT::R8G8_SNORM:
		case TE_RESOURCE_FORMAT::R8G8_SINT:
		case TE_RESOURCE_FORMAT::R16_TYPELESS:
		case TE_RESOURCE_FORMAT::R16_FLOAT:
		case TE_RESOURCE_FORMAT::D16_UNORM:
		case TE_RESOURCE_FORMAT::R16_UNORM:
		case TE_RESOURCE_FORMAT::R16_UINT:
		case TE_RESOURCE_FORMAT::R16_SNORM:
		case TE_RESOURCE_FORMAT::R16_SINT:
			return 2;
			break;
		case TE_RESOURCE_FORMAT::R8_TYPELESS:
		case TE_RESOURCE_FORMAT::R8_UNORM:
		case TE_RESOURCE_FORMAT::R8_UINT:
		case TE_RESOURCE_FORMAT::R8_SNORM:
		case TE_RESOURCE_FORMAT::R8_SINT:
		case TE_RESOURCE_FORMAT::A8_UNORM:
			return 1;
			break;
		default:
			return -1;
			break;
		}
	}



	TE_RESULT DirectX12BufferManager::CreateResource(TextureRenderTarget* tRT)
	{

		//		COMPTR<ID3D12Resource>& resource = tRT->m_ResourceIndex == -1 ? m_Resources.emplace_back() : m_Resources[tRT->m_ResourceIndex];

		COMPTR<ID3D12Resource>* resource;

		bool _ReCreation = false;
		if (tRT->m_ResourceIndex == -1)
		{
			tRT->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			resource = std::addressof(m_Resources.emplace_back());
		}
		else
		{
			_ReCreation = true;
			resource = std::addressof(m_Resources[tRT->m_ResourceIndex]);
		}

		D3D12_CLEAR_VALUE v{ GetFormat(tRT->m_Format), { tRT->m_ClearValue.x, tRT->m_ClearValue.y, tRT->m_ClearValue.z, tRT->m_ClearValue.w } };

		const auto desc = GetTextureDesc(tRT);
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(tRT->m_State)
			, &v
			, nullptr, IID_PPV_ARGS(resource->ReleaseAndGetAddressOf()));

		if (_ReCreation)
		{
			OnReCreateResource(tRT, 0);
		}

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DirectX12BufferManager::CreateResource(TextureDepthStencil* tDS)
	{

		bool _ReCreation = false;
		COMPTR<ID3D12Resource>* resource;
		if (tDS->m_ResourceIndex == -1)
		{
			tDS->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			resource = std::addressof(m_Resources.emplace_back());
		}
		else
		{
			_ReCreation = true;
			resource = std::addressof(m_Resources[tDS->m_ResourceIndex]);
		}

		D3D12_CLEAR_VALUE v{ GetDSVFormat(tDS->m_Format), { tDS->m_ClearValue.depthValue, tDS->m_ClearValue.stencilValue } };

		const auto desc = GetTextureDesc(tDS);
		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(tDS->m_State)
			, &v
			, nullptr
			, IID_PPV_ARGS(resource->ReleaseAndGetAddressOf()));

		if (_ReCreation)
		{
			OnReCreateResource(tDS, 0);
		}

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DirectX12BufferManager::CreateResource(BufferUpload* buffer)
	{

		auto framesOnTheFly = TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum();

		COMPTR<ID3D12Resource>* resource;

		HRESULT hr;

		bool _ReCreation = false;
		if (buffer->m_ResourceIndex == -1)
		{
			buffer->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			for (uint8_t i = 0; i < framesOnTheFly; ++i)
				m_Resources.emplace_back();

			resource = std::addressof(m_Resources[buffer->m_ResourceIndex]);
		}
		else
		{
			_ReCreation = true;
			resource = std::addressof(m_Resources[buffer->m_ResourceIndex]);
		}

		const auto desc = GetBufferDesc(buffer->m_SizeInByte, buffer->m_Usage);

		for (uint8_t i = 0; i < framesOnTheFly; ++i)
		{
			hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
				, D3D12_HEAP_FLAG_NONE
				, &desc, DX12_GET_STATE(buffer->m_State), nullptr
				, nullptr, IID_PPV_ARGS(resource[i].ReleaseAndGetAddressOf()));

			CD3DX12_RANGE range(0, 0);

			resource[i]->Map(0, &range, reinterpret_cast<void**>(&buffer->m_MappedData[i]));

			if (_ReCreation)
			{
				OnReCreateResource(buffer, i);
			}
		}

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;

	}

	TE_RESULT DirectX12BufferManager::CreateResource(VertexBufferStreamBase* vb)
	{
		if (vb->GetBufferSize() == 0)
			return TE_RESULT::TE_RESULT_GRAHICRESOURCE_CREATION_ZEROSIZE;

		const auto desc = GetBufferDesc(vb->GetBufferSize(), vb->m_Usage);

		COMPTR<ID3D12Resource>* resource;
		if (vb->m_ResourceIndex == -1)
		{
			vb->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			resource = std::addressof(m_Resources.emplace_back());
		}
		else
		{
			resource = std::addressof(m_Resources[vb->m_ResourceIndex]);
		}

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(vb->m_State), nullptr
			, nullptr, IID_PPV_ARGS(resource->ReleaseAndGetAddressOf()));


		/*size_t offset = vb->GetSizeInByte();

		ResourceBufferUpload* uploadBuffer = new ResourceBufferUpload();
		uploadBuffer->Initialization(device, static_cast<UINT>(offset));

		offset = 0;

		auto l = [&offset, uploadBuffer, cmdList](VertexBufferStreamBase& vbsb)
		{
			vbsb.UploadDataToGPU(cmdList, uploadBuffer, offset);
			offset += vbsb.GetRequiredSize();
		};*/

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DirectX12BufferManager::CreateResource(IndexBuffer* ib)
	{
		const auto desc = GetBufferDesc(ib->GetBufferSize(), TE_RESOURCE_USAGE_INDEXBUFFER);

		COMPTR<ID3D12Resource>* resource;
		if (ib->m_ResourceIndex == -1)
		{
			ib->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			resource = std::addressof(m_Resources.emplace_back());
		}
		else
		{
			resource = std::addressof(m_Resources[ib->m_ResourceIndex]);
		}

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(ib->m_State), nullptr
			, nullptr, IID_PPV_ARGS(resource->ReleaseAndGetAddressOf()));


		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DirectX12BufferManager::CreateResource(TextureMaterial* texture)
	{
		auto d3d12device = static_cast<DirectX12GraphicDevice*>(TE_INSTANCE_GRAPHICDEVICE)->GetDevice();
		DirectX::ResourceUploadBatch uploadBatch(d3d12device);

		uploadBatch.Begin(D3D12_COMMAND_LIST_TYPE_COPY);
		texture->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());
		auto resource = std::addressof(m_Resources.emplace_back());
		DirectX::CreateWICTextureFromMemoryEx(d3d12device, uploadBatch, texture->GetData(), texture->GetDataSize(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::WIC_LOADER_DEFAULT, resource->ReleaseAndGetAddressOf());
		uploadBatch.End(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject());

		return TE_SUCCESSFUL;
	}

	void DirectX12BufferManager::OnReCreateResource(TE_IDX_GRESOURCES _IDX, uint32_t _ResourceIndexOffset)
	{
		GraphicResource* _GResource = m_Map_GraphicResources.find(_IDX)->second;
		OnReCreateResource(_GResource, _ResourceIndexOffset);
	}

	void DirectX12BufferManager::OnReCreateResource(GraphicResource* _GResource, uint32_t _ResourceIndexOffset)
	{
		if (_GResource->m_Usage & TE_RESOURCE_USAGE_SHADERRESOURCE)
		{
			auto _Itr = m_Map_SRVs.find(_GResource->m_IDX);
			if (_Itr != m_Map_SRVs.end())
			{
				//for (uint32_t _ViewHeapOffset : _Itr->second)
				for (uint32_t i = 0; i < _Itr->second.size(); ++i)
				{
					ShaderResourceView _SRV{ _Itr->second[i], _GResource->m_ResourceIndex, _GResource };
					CreateShaderResourceView(_GResource, &_SRV, _ResourceIndexOffset);
				}
			}
		}
		if (_GResource->m_Usage & TE_RESOURCE_USAGE_UNORDEREDACCESS)
		{
			auto& _Itr = m_Map_UAVs.find(_GResource->m_IDX);
			if (_Itr != m_Map_UAVs.end())
			{
				for (uint32_t _ViewHeapOffset : _Itr->second)
				{
					UnorderedAccessView _UAV{ _ViewHeapOffset, _GResource->m_ResourceIndex, static_cast<Buffer*>(_GResource) };
					CreateUnorderedAccessView(_GResource, &_UAV);
				}
			}
		}
	}

	TE_RESULT DirectX12BufferManager::CreateResource(Buffer* _Buffer)
	{

		auto _FramesOnTheFly = TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum();

		_FramesOnTheFly = ((_Buffer->m_Usage & TE_RESOURCE_USAGE_CONSTANTBUFFER) || !(_Buffer->m_Usage & TE_RESOURCE_USAGE_UNORDEREDACCESS)) ? _FramesOnTheFly : 1;

		COMPTR<ID3D12Resource>* _D3DResource;


		bool _ReCreation = false;
		if (_Buffer->m_ResourceIndex == -1)
		{
			_Buffer->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			_D3DResource = std::addressof(m_Resources.emplace_back());

			for (uint8_t i = 1; i < _FramesOnTheFly; ++i)
				m_Resources.emplace_back();

		}
		else
		{
			_ReCreation = true;
			_D3DResource = std::addressof(m_Resources[_Buffer->m_ResourceIndex]);
		}

		HRESULT hr;

		D3D12_RESOURCE_DESC1 _BufferDesc = GetBufferDesc(_Buffer->GetSizeInByte(), _Buffer->m_Usage);

		D3D12_HEAP_TYPE _HeapType = _Buffer->m_IsUploadHeapBuffer ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;

		for (uint8_t i = 0; i < _FramesOnTheFly; ++i)
		{

			hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
				&CD3DX12_HEAP_PROPERTIES(_HeapType)
				, D3D12_HEAP_FLAG_NONE
				, &_BufferDesc
				, DX12_GET_STATE(_Buffer->m_State)
				, nullptr
				, nullptr
				, IID_PPV_ARGS(_D3DResource->ReleaseAndGetAddressOf())
			);

			CD3DX12_RANGE range(0, 0);

			if (_Buffer->m_IsUploadHeapBuffer)
				_D3DResource[i]->Map(0, &range, reinterpret_cast<void**>(&_Buffer->m_MappedData[i]));

			if (_ReCreation)
			{
				OnReCreateResource(_Buffer, i);
			}
		}

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DirectX12BufferManager::CreateResource(Texture* _Texture)
	{

		COMPTR<ID3D12Resource>* _D3DResource;


		bool _ReCreation = false;
		if (_Texture->m_ResourceIndex == -1)
		{
			_Texture->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			_D3DResource = std::addressof(m_Resources.emplace_back());
		}
		else
		{
			_ReCreation = true;
			_D3DResource = std::addressof(m_Resources[_Texture->m_ResourceIndex]);
		}


		D3D12_RESOURCE_DESC1 _TextureDesc = GetTextureDesc(_Texture);

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &_TextureDesc, DX12_GET_STATE(_Texture->m_State)
			, nullptr
			, nullptr
			, IID_PPV_ARGS(_D3DResource->ReleaseAndGetAddressOf()));

		if (_ReCreation)
		{
			OnReCreateResource(_Texture, 0);
		}

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;

	}


	void DirectX12BufferManager::CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV)
	{
		if (RTV->ViewIndex == -1)
		{
			uint32_t _ViewIndex = m_DescHeapRTV.AddDescriptor(m_Resources[RT->m_ResourceIndex].Get());
			*RTV = RenderTargetView{ _ViewIndex, RT->m_ResourceIndex, RT };

		}
		else
		{
			m_DescHeapRTV.ReplaceDescriptor(m_Resources[RT->m_ResourceIndex].Get(), RTV->ViewIndex);
			*RTV = RenderTargetView{ RTV->ViewIndex, RT->m_ResourceIndex, RT };
		}
	}

	struct TED3D12RenderTargetViewDesc
	{
		TED3D12RenderTargetViewDesc(Texture* _Texture, uint8_t _MipSlice, uint8_t _ArraySlice, uint8_t _PlaneSlice)
		{
			D3D12Desc.Format = DX12_GET_FORMAT(_Texture->GetFormat());
			switch (_Texture->GetType())
			{
			case TE_RESOURCE_TYPE::BUFFER:
				throw;
				break;
			case TE_RESOURCE_TYPE::TEXTURE1D:
				D3D12Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
				D3D12Desc.Texture1D.MipSlice = _MipSlice;
				break;
			case TE_RESOURCE_TYPE::TEXTURE2D:
				D3D12Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				D3D12Desc.Texture2D.MipSlice = _MipSlice;
				D3D12Desc.Texture2D.PlaneSlice = _PlaneSlice;
				break;
			case TE_RESOURCE_TYPE::TEXTURE3D:
				throw;
				break;
			case TE_RESOURCE_TYPE::TEXTURE1DARRAY:
				D3D12Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
				D3D12Desc.Texture1DArray.ArraySize = _Texture->GetArraySize();
				D3D12Desc.Texture1DArray.FirstArraySlice = _ArraySlice;
				D3D12Desc.Texture1DArray.MipSlice = _MipSlice;
				break;
			case TE_RESOURCE_TYPE::TEXTURECUBE:
			case TE_RESOURCE_TYPE::TEXTURE2DARRAY:
			case TE_RESOURCE_TYPE::TEXTURECUBEARRAY:
				D3D12Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				D3D12Desc.Texture2DArray.ArraySize = _Texture->GetArraySize();
				D3D12Desc.Texture2DArray.FirstArraySlice = _ArraySlice;
				D3D12Desc.Texture2DArray.MipSlice = _MipSlice;
				D3D12Desc.Texture2DArray.PlaneSlice = _PlaneSlice;
				break;
			default:
				break;
			}
		}

		D3D12_RENDER_TARGET_VIEW_DESC D3D12Desc;
	};

	void DirectX12BufferManager::CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* _outRTV, uint8_t mipSlice, uint8_t arraySlice)
	{
		TED3D12RenderTargetViewDesc _TERTVDesc{ RT, mipSlice, arraySlice, 0 };

		if (_outRTV->ViewIndex == -1)
		{
			uint32_t _ViewIndex = m_DescHeapRTV.AddDescriptor(m_Resources[RT->m_ResourceIndex].Get(), &_TERTVDesc.D3D12Desc);
			*_outRTV = RenderTargetView{ _ViewIndex, RT->m_ResourceIndex, RT };

		}
		else
		{
			m_DescHeapRTV.ReplaceDescriptor(m_Resources[RT->m_ResourceIndex].Get(), _outRTV->ViewIndex, &_TERTVDesc.D3D12Desc);
			*_outRTV = RenderTargetView{ _outRTV->ViewIndex, RT->m_ResourceIndex, RT };
		}
	}

	void DirectX12BufferManager::CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* RTV)
	{
		auto sc = static_cast<DirectX12SwapChain*>(swapChain);

		sc->InitRTVs(&m_DescHeapRTV, RTV);
	}

	void DirectX12BufferManager::CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* DSV)
	{

		D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
		desc.Format = GetDSVFormat(DS->m_Format);
		desc.ViewDimension = DS->m_EnableMSAA ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_DSV_FLAG_NONE;
		desc.Texture2D.MipSlice = 0;

		if (DSV->ViewIndex == -1)
		{
			uint32_t _ViewIndex = m_DescHeapDSV.AddDescriptor(m_Resources[DS->m_ResourceIndex].Get(), &desc);

			*DSV = DepthStencilView{ _ViewIndex, DS->m_ResourceIndex, DS };
		}
		else
		{
			m_DescHeapDSV.ReplaceDescriptor(m_Resources[DS->m_ResourceIndex].Get(), &desc, DSV->ViewIndex);

			*DSV = DepthStencilView{ DSV->ViewIndex, DS->m_ResourceIndex, DS };
		}

	}

	void DirectX12BufferManager::CreateShaderResourceView(Texture* texture, ShaderResourceView* SRV)
	{

		D3D12_SHADER_RESOURCE_VIEW_DESC _SRVDesc{};
		auto _D3DResource = m_Resources[texture->m_ResourceIndex].Get();
		auto _ResourceDesc = _D3DResource->GetDesc();
		_SRVDesc.Format = _ResourceDesc.Format;
		_SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (texture->m_Type)
		{
		case TE_RESOURCE_TYPE::TEXTURE1D:
		{
			_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			_SRVDesc.Texture1D.MipLevels = _ResourceDesc.MipLevels;
			_SRVDesc.Texture1D.MostDetailedMip = 0;
			_SRVDesc.Texture1D.ResourceMinLODClamp = 0.0f;
			break;
		}
		case TE_RESOURCE_TYPE::TEXTURE2D:
		{
			_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			_SRVDesc.Texture2D.MipLevels = _ResourceDesc.MipLevels;
			_SRVDesc.Texture2D.MostDetailedMip = 0;
			_SRVDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			_SRVDesc.Texture2D.PlaneSlice = 0;
			break;
		}
		case TE_RESOURCE_TYPE::TEXTURE3D:
			_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			_SRVDesc.Texture3D.MipLevels = _ResourceDesc.MipLevels;
			_SRVDesc.Texture3D.MostDetailedMip = 0;
			_SRVDesc.Texture3D.ResourceMinLODClamp = 0.0f;
			break;
		case TE_RESOURCE_TYPE::TEXTURECUBE:
			_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			_SRVDesc.TextureCube.MipLevels = _ResourceDesc.MipLevels;
			_SRVDesc.TextureCube.MostDetailedMip = 0;
			_SRVDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			break;
		case TE_RESOURCE_TYPE::TEXTURE1DARRAY:
		{
			_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			_SRVDesc.Texture1DArray.ArraySize = _ResourceDesc.DepthOrArraySize;
			_SRVDesc.Texture1DArray.MipLevels = _ResourceDesc.MipLevels;
			_SRVDesc.Texture1DArray.FirstArraySlice = 0;
			_SRVDesc.Texture1DArray.MostDetailedMip = 0;
			_SRVDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
			break;
		}
		case TE_RESOURCE_TYPE::TEXTURE2DARRAY:
		{
			_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			_SRVDesc.Texture2DArray.ArraySize = _ResourceDesc.DepthOrArraySize;
			_SRVDesc.Texture2DArray.MipLevels = _ResourceDesc.MipLevels;
			_SRVDesc.Texture2DArray.FirstArraySlice = 0;
			_SRVDesc.Texture2DArray.MostDetailedMip = 0;
			_SRVDesc.Texture2DArray.PlaneSlice = 0;
			_SRVDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			break;
		}
		case TE_RESOURCE_TYPE::TEXTURECUBEARRAY:
			_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			_SRVDesc.TextureCubeArray.MipLevels = _ResourceDesc.MipLevels;
			_SRVDesc.TextureCubeArray.MostDetailedMip = 0;
			_SRVDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			_SRVDesc.TextureCubeArray.First2DArrayFace = 0;
			_SRVDesc.TextureCubeArray.NumCubes = _ResourceDesc.DepthOrArraySize;
			break;
		default:
			break;
		}

		if (texture->m_Usage & TE_RESOURCE_USAGE_DEPTHSTENCIL)
		{
			_SRVDesc.Format = static_cast<DXGI_FORMAT>(GetDepthStencilSRVFormat(static_cast<TE_RESOURCE_FORMAT>(_SRVDesc.Format)));
		}

		if (SRV->ViewIndex == -1)
		{
			uint32_t _ViewIndex = m_DescHeapSRV.AddDescriptorSRV(m_Resources[texture->m_ResourceIndex].Get(), &_SRVDesc);

			*SRV = ShaderResourceView{ _ViewIndex, texture->m_ResourceIndex, texture };
		}
		else
		{
			m_DescHeapSRV.ReplaceDescriptorSRV(m_Resources[texture->m_ResourceIndex].Get(), &_SRVDesc, SRV->ViewIndex);

			*SRV = ShaderResourceView{ SRV->ViewIndex, texture->m_ResourceIndex, texture };
		}
	}

	void DirectX12BufferManager::CreateShaderResourceView(Buffer* _Buffer, ShaderResourceView* _SRV, uint32_t frameIndex)
	{
		uint32_t _ResourceIndex = _Buffer->m_ResourceIndex + (frameIndex * (uint32_t)!static_cast<bool>(_Buffer->m_Usage & TE_RESOURCE_USAGE_UNORDEREDACCESS));

		D3D12_SHADER_RESOURCE_VIEW_DESC _SRVDesc{};
		ID3D12Resource* _D3DResource = m_Resources[_ResourceIndex].Get();
		D3D12_RESOURCE_DESC _ResourceDesc = _D3DResource->GetDesc();
		_SRVDesc.Format = _ResourceDesc.Format;
		_SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		_SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		_SRVDesc.Buffer.FirstElement = 0;
		_SRVDesc.Buffer.Flags = _Buffer->m_IsByteAddress ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE;
		_SRVDesc.Buffer.NumElements = _Buffer->m_ElementNum;
		_SRVDesc.Buffer.StructureByteStride = _Buffer->m_ElementSizeInByte;

		if (_SRV->ViewIndex == -1)
		{
			uint32_t _ViewIndex = m_DescHeapSRV.AddDescriptorSRV(_D3DResource, &_SRVDesc);

			*_SRV = ShaderResourceView{ _ViewIndex, _ResourceIndex, _Buffer };
		}
		else
		{
			m_DescHeapSRV.ReplaceDescriptorSRV(_D3DResource, &_SRVDesc, _SRV->ViewIndex);

			*_SRV = ShaderResourceView{ _SRV->ViewIndex, _ResourceIndex, _Buffer };
		}
	}

	void DirectX12BufferManager::CreateShaderResourceView(GraphicResource* _GraphicResource, ShaderResourceView* _SRV, uint32_t frameIndex)
	{
		TE_ASSERT_CORE(_GraphicResource->m_Usage & TE_RESOURCE_USAGE_SHADERRESOURCE, "The Graphic Resource sent to create a ShaderResourceView doesn't have a ShaderResourceView Flag!");

		const bool _ReplaceView = _SRV->ViewIndex != -1;

		switch (_GraphicResource->m_Type)
		{
		case TE_RESOURCE_TYPE::TEXTURE1D:
		case TE_RESOURCE_TYPE::TEXTURE2D:
		case TE_RESOURCE_TYPE::TEXTURE3D:
		case TE_RESOURCE_TYPE::TEXTURECUBE:
		case TE_RESOURCE_TYPE::TEXTURE1DARRAY:
		case TE_RESOURCE_TYPE::TEXTURE2DARRAY:
		case TE_RESOURCE_TYPE::TEXTURECUBEARRAY:
			CreateShaderResourceView(static_cast<Texture*>(_GraphicResource), _SRV);
			break;
		case TE_RESOURCE_TYPE::BUFFER:
			CreateShaderResourceView(static_cast<Buffer*>(_GraphicResource), _SRV, frameIndex);
			break;
		default:
			throw std::runtime_error("BufferManager.CreateShaderResourceView: The GraphicResource doesn't have a valid RESOURCE_TYPE");
			break;
		}

		if (!_ReplaceView)
			m_Map_SRVs[_GraphicResource->m_IDX].emplace_back(_SRV->ViewIndex);

	}

	void DirectX12BufferManager::CreateConstantBufferView(Buffer* constantBuffer, ConstantBufferView* CBV, uint32_t frameIndex)
	{
		uint32_t resourceIndex = constantBuffer->m_ResourceIndex + frameIndex;

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc{ m_Resources[resourceIndex]->GetGPUVirtualAddress(), static_cast<uint32_t>(constantBuffer->GetRequiredSize()) };

		if (CBV->ViewIndex == -1)
		{
			uint32_t _ViewIndex = m_DescHeapSRV.AddDescriptorCBV(&desc);

			*CBV = ConstantBufferView{ _ViewIndex, resourceIndex, constantBuffer };
		}
		else
		{
			m_DescHeapSRV.ReplaceDescriptorCBV(&desc, CBV->ViewIndex);

			*CBV = ConstantBufferView{ CBV->ViewIndex, resourceIndex, constantBuffer };
		}
	}

	uint64_t DirectX12BufferManager::GetRequiredSize(const GraphicResource* graphicResource) const
	{
		auto resource = m_Resources[graphicResource->m_ResourceIndex].Get();

		ID3D12Device* device;
		resource->GetDevice(IID_PPV_ARGS(&device));

		auto desc = resource->GetDesc();

		uint64_t requiredSize;
		device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &requiredSize);

		device->Release();

		return requiredSize;
	}

	TE_RESULT DirectX12BufferManager::CreateVertexBuffer(VertexBufferBase* vb)
	{
		uint32_t vertexStreamsNum = vb->GetVertexStreamNum();
		auto vertexStreams = vb->GetVertexBufferStreams();

		//Assign the vertex buffer ID and then increase variable by 1 ( if is not assigned before : != -1)
		if (vb->m_ID == -1)
			vb->m_ID = m_LastVertexBufferID++;

		if (vb->m_ViewIndex == -1)
			vb->m_ViewIndex = static_cast<uint32_t>(m_VertexBufferViews.size());

		for (uint32_t i = 0; i < vertexStreamsNum; ++i)
		{
			auto vs = vertexStreams[i];
			CreateResource(vs);

			D3D12_VERTEX_BUFFER_VIEW* _View = nullptr;
			if (vs->m_ViewIndex == -1)
			{
				vs->m_ViewIndex = static_cast<uint32_t>(m_VertexBufferViews.size());
				_View = &m_VertexBufferViews.emplace_back();
			}
			else
			{
				_View = &m_VertexBufferViews[vs->m_ViewIndex];
			}

			auto r = m_Resources[vs->m_ResourceIndex];
			_View->BufferLocation = r->GetGPUVirtualAddress();
			_View->SizeInBytes = static_cast<UINT>(vs->GetBufferSize());
			_View->StrideInBytes = static_cast<UINT>(vs->GetVertexSize());
		}

		return TE_SUCCESSFUL;
	}

	TE_RESULT DirectX12BufferManager::CreateIndexBuffer(IndexBuffer* ib)
	{
		CreateResource(ib);

		//assing index buffer ID then increase the variable by 1
		ib->m_ID = m_LastIndexBufferID++;

		ib->m_ViewIndex = static_cast<uint32_t>(m_IndexBufferViews.size());

		auto& r = m_Resources[ib->m_ResourceIndex];

		auto& iv = m_IndexBufferViews.emplace_back(D3D12_INDEX_BUFFER_VIEW{ r->GetGPUVirtualAddress() , static_cast<uint32_t>(ib->GetBufferSize()), DXGI_FORMAT_R32_UINT });

		return TE_SUCCESSFUL;
	}

	void DirectX12BufferManager::Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum)
	{
		m_TexturesRenderTarget.reserve(50);
		m_TexturesDepthStencil.reserve(50);
		m_TexturesCubeMap.reserve(20);


		m_DescHeapSRV = DescriptorHeapSRV(1);

		m_DescHeapSRV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, shaderResourceViewNum);
		m_DescHeapRTV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, renderTargetViewNum);
		m_DescHeapDSV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, depthBufferViewNum);

		m_Resources.reserve(resourceNum);

		m_Rect_FullScreen.left = 0;
		m_Rect_FullScreen.top = 0;
		m_Rect_FullScreen.right = TE_INSTANCE_APPLICATION->GetClientWidth();
		m_Rect_FullScreen.bottom = TE_INSTANCE_APPLICATION->GetClientHeight();
	}

	void DirectX12BufferManager::Release()
	{
		m_Resources.clear();

		m_DescHeapSRV.Release();
		m_DescHeapDSV.Release();
		m_DescHeapRTV.Release();


		m_IndexBufferViews.clear();
		m_VertexBufferViews.clear();
	}

	void DirectX12BufferManager::ReleaseResource(GraphicResource* resource)
	{
		auto resourceIndex = resource->m_ResourceIndex;

		if (resourceIndex == -1)
		{
			return;
		}

		m_Resources[resourceIndex].Reset();
	}


	ID3D12Resource* DirectX12BufferManager::GetResource(GraphicResource* graphicResource)
	{
		return m_Resources[graphicResource->m_ResourceIndex].Get();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectX12BufferManager::AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
	{
		auto index = m_DescHeapSRV.AddDescriptorSRV(resource, desc);
		return m_DescHeapSRV.GetGPUHandle(index);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectX12BufferManager::AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
	{
		auto index = m_DescHeapSRV.AddDescriptorCBV(cbvDesc);
		return m_DescHeapSRV.GetGPUHandle(index);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectX12BufferManager::AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
	{
		auto index = m_DescHeapSRV.AddDescriptorUAV(resource, counterResource, desc);
		return m_DescHeapSRV.GetGPUHandle(index);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12BufferManager::AddDescriptorRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc)
	{
		auto index = m_DescHeapRTV.AddDescriptor(resource, desc);
		return m_DescHeapRTV.GetCPUHandle(index);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12BufferManager::AddDescriptorDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		auto index = m_DescHeapDSV.AddDescriptor(resource, desc);
		return m_DescHeapDSV.GetCPUHandle(index);
	}

	TruthEngine::TextureCubeMap* DirectX12BufferManager::CreateTextureCube(TE_IDX_GRESOURCES idx, const char* filePath)
	{
		auto index = static_cast<uint32_t>(m_TexturesCubeMap.size());

		boost::filesystem::path textureFilePath(filePath);
		std::string fileName = textureFilePath.filename().string();

		auto d3d12device = static_cast<DirectX12GraphicDevice*>(TE_INSTANCE_GRAPHICDEVICE)->GetDevice();
		DirectX::ResourceUploadBatch uploadBatch(d3d12device);

		uploadBatch.Begin(D3D12_COMMAND_LIST_TYPE_COPY);
		auto resourceIndex = static_cast<uint32_t>(m_Resources.size());
		auto resource = std::addressof(m_Resources.emplace_back());
		bool isCubeMap = false;
		DirectX::CreateDDSTextureFromFileEx(d3d12device, uploadBatch, to_wstring(std::string_view(filePath)).c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, resource->ReleaseAndGetAddressOf(), nullptr, &isCubeMap);
		uploadBatch.End(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject());

		auto desc = (*resource)->GetDesc();

		TextureCubeMap& tex = m_TexturesCubeMap.emplace_back(idx, index, fileName.c_str(), filePath, nullptr, desc.Width, desc.Height, 0, static_cast<TE_RESOURCE_FORMAT>(desc.Format));
		tex.m_ResourceIndex = resourceIndex;

		m_Map_Textures[idx] = &tex;
		m_Map_GraphicResources[idx] = &tex;

		return &tex;
	}

	Texture* DirectX12BufferManager::CreateTexture(TE_IDX_GRESOURCES _IDX, uint32_t _Width, uint32_t _Height, uint8_t _ArraySize, uint8_t _MipLevels, TE_RESOURCE_FORMAT _Format, TE_RESOURCE_TYPE _ResourceType, TE_RESOURCE_STATES _State, const void* _InitData)
	{
		Texture* _OutTexture = nullptr;

		auto _Itr = m_Map_Textures.find(_IDX);
		if (_Itr == m_Map_Textures.end())
		{
			_OutTexture = &m_Textures.emplace_back(_IDX, _Width, _Height, _ArraySize, _MipLevels, _Format, TE_RESOURCE_USAGE_SHADERRESOURCE, _ResourceType, TE_RESOURCE_STATES::COPY_DEST, false);
		}
		else
		{
			_OutTexture = _Itr->second;

			_OutTexture->m_Width = _Width;
			_OutTexture->m_Height = _Height;
			_OutTexture->m_ArraySize = _ArraySize;
			_OutTexture->m_MipLevels = _MipLevels;
			_OutTexture->m_Format = _Format;
			_OutTexture->m_Usage = TE_RESOURCE_USAGE_SHADERRESOURCE;
			_OutTexture->m_Type = _ResourceType;
			_OutTexture->m_State = _InitData != nullptr ? TE_RESOURCE_STATES::COPY_DEST : (TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE | TE_RESOURCE_STATES::NON_PIXEL_SHADER_RESOURCE);
			_OutTexture->m_EnableMSAA = false;
		}

		CreateResource(_OutTexture);

		if (_InitData != nullptr)
		{
			auto d3d12device = static_cast<DirectX12GraphicDevice*>(TE_INSTANCE_GRAPHICDEVICE)->GetDevice();
			DirectX::ResourceUploadBatch uploadBatch(d3d12device);

			D3D12_SUBRESOURCE_DATA _D3D12Data;
			_D3D12Data.pData = _InitData;
			size_t _PixelByteSize = GetPixelByteSize(_Format);
			_D3D12Data.RowPitch = _PixelByteSize * _Width;
			_D3D12Data.SlicePitch = _PixelByteSize * _Width * _Height;

			ID3D12Resource* _ID3D12Resource = m_Resources[_OutTexture->GetResourceIndex()].Get();

			uploadBatch.Begin(D3D12_COMMAND_LIST_TYPE_COPY);
			uploadBatch.Upload(_ID3D12Resource, 0, &_D3D12Data, 1);
			uploadBatch.Transition(_ID3D12Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			uploadBatch.End(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject());
		}

		m_Map_Textures[_IDX] = _OutTexture;
		m_Map_GraphicResources[_IDX] = _OutTexture;

		return _OutTexture;
	}

	Texture* DirectX12BufferManager::LoadTextureFromFile(TE_IDX_GRESOURCES _IDX, const char* _FilePath, uint8_t _MipLevels)
	{
		boost::filesystem::path textureFilePath(_FilePath);
		std::string fileName = textureFilePath.filename().string();
		std::wstring filePathW = textureFilePath.wstring();
		std::wstring fileExtension = textureFilePath.extension().wstring();

		auto d3d12device = static_cast<DirectX12GraphicDevice*>(TE_INSTANCE_GRAPHICDEVICE)->GetDevice();
		DirectX::ResourceUploadBatch uploadBatch(d3d12device);

		//auto resourceIndex = static_cast<uint32_t>(m_Resources.size());
		//auto resource = std::addressof(m_Resources.emplace_back());

		Texture* _OutTexture = nullptr;

		uploadBatch.Begin(D3D12_COMMAND_LIST_TYPE_COPY);
		if (fileExtension == L".hdr")
		{
			DirectX::ScratchImage _ScratchImage;
			DirectX::LoadFromHDRFile(filePathW.c_str(), nullptr, _ScratchImage);

			auto _Image = _ScratchImage.GetImage(0, 0, 0);
			_OutTexture = &m_Textures.emplace_back( _IDX, static_cast<uint32_t>(_Image->width), static_cast<uint32_t>(_Image->height), 1, _MipLevels, static_cast<TE_RESOURCE_FORMAT>(_Image->format), TE_RESOURCE_USAGE_SHADERRESOURCE, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST, false );

			CreateResource(_OutTexture);

			D3D12_SUBRESOURCE_DATA _InitData;
			_InitData.pData = _Image->pixels;
			_InitData.RowPitch = _Image->rowPitch;
			_InitData.SlicePitch = _Image->slicePitch;

			uploadBatch.Upload(m_Resources[_OutTexture->GetResourceIndex()].Get(), 0, &_InitData, 1);
			uploadBatch.Transition(m_Resources[_OutTexture->GetResourceIndex()].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
		else
		{
			auto resourceIndex = static_cast<uint32_t>(m_Resources.size());
			auto resource = std::addressof(m_Resources.emplace_back());


			if (fileExtension == L".dds")
			{
				DirectX::CreateDDSTextureFromFileEx(d3d12device, uploadBatch, filePathW.c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, resource->ReleaseAndGetAddressOf());
			}
			else
			{
				DirectX::CreateWICTextureFromFileEx(d3d12device, uploadBatch, filePathW.c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::WIC_LOADER_DEFAULT, resource->ReleaseAndGetAddressOf());
			}

			auto desc = (*resource)->GetDesc();

			TE_RESOURCE_TYPE _RType = TE_RESOURCE_TYPE::UNKNOWN;

			if (desc.DepthOrArraySize == 1)
			{
				_RType = TE_RESOURCE_TYPE::TEXTURE2D;
			}
			else if (desc.DepthOrArraySize == 6)
			{
				_RType = TE_RESOURCE_TYPE::TEXTURECUBE;
			}
			else
			{
				_RType = TE_RESOURCE_TYPE::TEXTURE2DARRAY;
			}

			_OutTexture = &m_Textures.emplace_back( _IDX, static_cast<uint32_t>(desc.Width), static_cast<uint32_t>(desc.Height), static_cast<uint8_t>(desc.DepthOrArraySize), _MipLevels, static_cast<TE_RESOURCE_FORMAT>(desc.Format), TE_RESOURCE_USAGE_SHADERRESOURCE, _RType, TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE | TE_RESOURCE_STATES::NON_PIXEL_SHADER_RESOURCE, false );
			_OutTexture->m_ResourceIndex = resourceIndex;
		}
		uploadBatch.End(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject());

		m_Map_Textures[_IDX] = _OutTexture;
		m_Map_GraphicResources[_IDX] = _OutTexture;

		return _OutTexture;
	}

	void DirectX12BufferManager::SaveTextureToFile(const Texture& _Texture, const char* _FilePath)
	{
		boost::filesystem::path _BoostFilePath(_FilePath);
		std::wstring _FilePathW = _BoostFilePath.wstring();
		std::wstring _FileName = _BoostFilePath.filename().wstring();
		std::wstring _Extension = _BoostFilePath.extension().wstring();

		std::transform(_Extension.cbegin(), _Extension.cend(), _Extension.begin(), ::tolower);

		D3D12_RESOURCE_STATES _State = DX12_GET_STATE(_Texture.GetState());
		DirectX::ScratchImage _ScratchImage;
		HRESULT _HResult = DirectX::CaptureTexture(TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT->GetNativeObject(), m_Resources[_Texture.GetResourceIndex()].Get(), _Texture.GetType() == TE_RESOURCE_TYPE::TEXTURECUBE, _ScratchImage, _State, _State);
		TE_ASSERT_CORE(SUCCEEDED(_HResult), "DirectX12 Capture Texture is failed");

		if (_Extension == L".dds")
		{
			//_HResult = DirectX::SaveDDSTextureToFile(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject(), m_Resources[_Texture.GetResourceIndex()].Get(), _FilePathW.c_str(), _State, _State);
			_HResult = DirectX::SaveToDDSFile(_ScratchImage.GetImages(), _ScratchImage.GetImageCount(), _ScratchImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, _FilePathW.c_str());
		}
		else
		{
			_HResult = DirectX::SaveToWICFile(_ScratchImage.GetImages(), _ScratchImage.GetImageCount(), DirectX::WIC_FLAGS_NONE, GUID_ContainerFormatJpeg, _FilePathW.c_str());
		}

		TE_ASSERT_CORE(SUCCEEDED(_HResult), "The Saving Texture To File Is Failed");

	}

	void DirectX12BufferManager::CreateUnorderedAccessView(GraphicResource* _GraphicResource, UnorderedAccessView* _outUAV)
	{


		D3D12_UNORDERED_ACCESS_VIEW_DESC _UAVDesc;

		switch (_GraphicResource->m_Type)
		{
		case TE_RESOURCE_TYPE::BUFFER:
		{
			Buffer* _Buffer = static_cast<Buffer*>(_GraphicResource);
			_UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			_UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
			_UAVDesc.Buffer.FirstElement = 0;
			_UAVDesc.Buffer.CounterOffsetInBytes = 0;
			_UAVDesc.Buffer.NumElements = _Buffer->m_ElementNum;
			_UAVDesc.Buffer.StructureByteStride = _Buffer->m_ElementSizeInByte;
			_UAVDesc.Buffer.Flags = _Buffer->m_IsByteAddress ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE;
			break;
		}
		case TE_RESOURCE_TYPE::TEXTURE2D:
		{
			Texture* _Texture = static_cast<Texture*>(_GraphicResource);
			_UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			_UAVDesc.Format = GetFormat(_Texture->GetFormat());
			_UAVDesc.Texture2D.MipSlice = 0;
			_UAVDesc.Texture2D.PlaneSlice = 0;
			break;
		}
		case TE_RESOURCE_TYPE::TEXTURE1D:
		{
			Texture* _Texture = static_cast<Texture*>(_GraphicResource);
			_UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
			_UAVDesc.Format = GetFormat(_Texture->GetFormat());
			_UAVDesc.Texture1D.MipSlice = 0;
			break;
		}
		case TE_RESOURCE_TYPE::TEXTURE3D:
		{
			Texture* _Texture = static_cast<Texture*>(_GraphicResource);
			_UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			_UAVDesc.Format = GetFormat(_Texture->GetFormat());
			_UAVDesc.Texture3D.FirstWSlice = 0;
			_UAVDesc.Texture3D.MipSlice = 0;
			_UAVDesc.Texture3D.WSize = 1;
			break;
		}
		default:
			break;
		}


		ID3D12Resource* _D3DResource = m_Resources[_GraphicResource->m_ResourceIndex].Get();

		bool _ReplaceView = false;

		if (_outUAV->ViewIndex == -1)
		{
			uint32_t _ViewIndex = m_DescHeapSRV.AddDescriptorUAV(_D3DResource, nullptr, &_UAVDesc);

			*_outUAV = UnorderedAccessView{ _ViewIndex, _GraphicResource->m_ResourceIndex, _GraphicResource };
		}
		else
		{
			_ReplaceView = true;

			m_DescHeapSRV.ReplaceDescriptorUAV(_D3DResource, nullptr, &_UAVDesc, _outUAV->ViewIndex);

			*_outUAV = UnorderedAccessView{ _outUAV->ViewIndex, _GraphicResource->m_ResourceIndex, _GraphicResource };
		}

		if (!_ReplaceView)
			m_Map_UAVs[_GraphicResource->m_IDX].emplace_back(_outUAV->ViewIndex);
	}

}