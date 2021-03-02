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

#include "DirectXTK12/Inc/ResourceUploadBatch.h"
#include "DirectXTK12/Inc/WICTextureLoader.h"
#include "DirectXTK12/Inc/DDSTextureLoader.h"

#include <boost/filesystem.hpp>

namespace TruthEngine::API::DirectX12
{

	inline DXGI_FORMAT GetFormat(const TE_RESOURCE_FORMAT format)
	{
		return static_cast<DXGI_FORMAT>(format);
	}


	inline DXGI_FORMAT GetDSVFormat(const TE_RESOURCE_FORMAT format)
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

	inline DXGI_FORMAT GetDepthStencilSRVFormat(const TE_RESOURCE_FORMAT format)
	{
		switch (format)
		{
		case TE_RESOURCE_FORMAT::R32_TYPELESS:
			return DXGI_FORMAT_R32_FLOAT;
		case TE_RESOURCE_FORMAT::R24_UNORM_X8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case TE_RESOURCE_FORMAT::R24G8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case TE_RESOURCE_FORMAT::R32_FLOAT_X8X24_TYPELESS:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		default:
			TE_ASSERT_CORE(false, "wrong depth stecil format");
			throw;
			return DXGI_FORMAT_UNKNOWN;
		}

	}


	inline D3D12_RESOURCE_FLAGS GetResourceFlags(TE_RESOURCE_USAGE usage)
	{
		D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE;

		if (usage & TE_RESOURCE_USAGE_RENDERTARGET)
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (!(usage & TE_RESOURCE_USAGE_SHADERRESOURCE))
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		}

		if (usage & TE_RESOURCE_USAGE_DEPTHSTENCIL)
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		if (usage & TE_RESOURCE_USAGE_UAV)
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return resourceFlags;
	}


	inline D3D12_RESOURCE_DESC1 GetBufferDesc(uint64_t sizeInByte, TE_RESOURCE_USAGE usage)
	{
		sizeInByte = static_cast<uint64_t>(std::ceil(static_cast<double>(sizeInByte) / 256.0) * 256);

		const auto resourceFlags = GetResourceFlags(usage);
		return CD3DX12_RESOURCE_DESC1::Buffer(sizeInByte, resourceFlags);
	}


	inline D3D12_RESOURCE_DESC1 GetTextureDesc(Texture* _Texture)
	{
		const auto resourceFlags = GetResourceFlags(_Texture->GetUsage());
		auto desc = CD3DX12_RESOURCE_DESC1::Tex2D(GetFormat(_Texture->GetFormat()), static_cast<uint64_t>(_Texture->GetWidth()), _Texture->GetHeight(), 1, 1, 1, 0, resourceFlags);

		if (_Texture->IsMultiSample())
		{
			desc.MipLevels = 1;
			desc.SampleDesc.Count = static_cast<int>(Settings::MSAA);
			desc.SampleDesc.Quality = 0;
		}

		return desc;
	}


	inline D3D12_RESOURCE_STATES GetInitialState(TE_RESOURCE_USAGE usage)
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
		case TE_RESOURCE_USAGE_UAV:
			return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			break;
		case TE_RESOURCE_USAGE_SHADERRESOURCE:
			return D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		}
	}



	TE_RESULT DirectX12BufferManager::CreateResource(TextureRenderTarget* tRT)
	{
		const auto desc = GetTextureDesc(tRT);

		//		COMPTR<ID3D12Resource>& resource = tRT->m_ResourceIndex == -1 ? m_Resources.emplace_back() : m_Resources[tRT->m_ResourceIndex];

		COMPTR<ID3D12Resource>* resource;

		if (tRT->m_ResourceIndex == -1)
		{
			tRT->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			resource = std::addressof(m_Resources.emplace_back());
		}
		else
		{
			resource = std::addressof(m_Resources[tRT->m_ResourceIndex]);
		}

		D3D12_CLEAR_VALUE v{ GetFormat(tRT->m_Format), { tRT->m_ClearValue.x, tRT->m_ClearValue.y, tRT->m_ClearValue.z, tRT->m_ClearValue.w } };

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(tRT->m_State)
			, &v
			, nullptr, IID_PPV_ARGS(resource->ReleaseAndGetAddressOf()));

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DirectX12BufferManager::CreateResource(TextureDepthStencil* tDS)
	{
		const auto desc = GetTextureDesc(tDS);

		COMPTR<ID3D12Resource>* resource;
		if (tDS->m_ResourceIndex == -1)
		{
			tDS->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			resource = &m_Resources.emplace_back();
		}
		else
		{
			resource = &m_Resources[tDS->m_ResourceIndex];
		}

		D3D12_CLEAR_VALUE v{ GetDSVFormat(tDS->m_Format), { tDS->m_ClearValue.depthValue, tDS->m_ClearValue.stencilValue } };

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(tDS->m_State), &v
			, nullptr, IID_PPV_ARGS(resource->ReleaseAndGetAddressOf()));


		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DirectX12BufferManager::CreateResource(BufferUpload* buffer)
	{

		auto framesOnTheFly = TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum();

		COMPTR<ID3D12Resource>* resource;

		HRESULT hr;

		if (buffer->m_ResourceIndex == -1)
		{
			buffer->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			for (uint8_t i = 0; i < framesOnTheFly; ++i)
				m_Resources.emplace_back();

			resource = &m_Resources[buffer->m_ResourceIndex];
		}
		else
		{
			resource = &m_Resources[buffer->m_ResourceIndex];
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
		}

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;

	}

	TE_RESULT DirectX12BufferManager::CreateResource(VertexBufferStreamBase* vb)
	{
		const auto desc = GetBufferDesc(vb->GetBufferSize(), vb->m_Usage);

		COMPTR<ID3D12Resource>* resource;
		if (vb->m_ResourceIndex == -1)
		{
			vb->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

			resource = &m_Resources.emplace_back();
		}
		else
		{
			resource = &m_Resources[vb->m_ResourceIndex];
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

			resource = &m_Resources.emplace_back();
		}
		else
		{
			resource = &m_Resources[ib->m_ResourceIndex];
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


	void DirectX12BufferManager::CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV)
	{
		if (RTV->ViewIndex == -1)
		{
			*RTV = RenderTargetView{ m_DescHeapRTV.GetCurrentIndex(), RT->m_ResourceIndex, RT };
			m_DescHeapRTV.AddDescriptor(m_Resources[RT->m_ResourceIndex].Get());

		}
		else
		{
			m_DescHeapRTV.ReplaceDescriptor(m_Resources[RT->m_ResourceIndex].Get(), RTV->ViewIndex);
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
			*DSV = DepthStencilView{ m_DescHeapDSV.GetCurrentIndex(), DS->m_ResourceIndex, DS };

			m_DescHeapDSV.AddDescriptor(m_Resources[DS->m_ResourceIndex].Get(), &desc);
		}
		else
		{
			m_DescHeapDSV.ReplaceDescriptor(m_Resources[DS->m_ResourceIndex].Get(), &desc, DSV->ViewIndex);
		}

	}

	void DirectX12BufferManager::CreateShaderResourceView(Texture* textures[], uint32_t textureNum, ShaderResourceView* SRV)
	{
		throw;
	}

	void DirectX12BufferManager::CreateShaderResourceView(Texture* texture, ShaderResourceView* SRV)
	{

		D3D12_SHADER_RESOURCE_VIEW_DESC _SRVDesc{};
		auto _D3DResource = m_Resources[texture->m_ResourceIndex].Get();
		auto _ResourceDesc = _D3DResource->GetDesc();
		_SRVDesc.Format = _ResourceDesc.Format;
		_SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (SRV->ViewIndex == -1)
		{
			*SRV = ShaderResourceView{ m_DescHeapSRV.GetCurrentIndex(), texture->m_ResourceIndex, texture };


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
			default:
				break;
			}

			switch (texture->m_Usage)
			{
			case (TE_RESOURCE_USAGE_SHADERRESOURCE):
			case ((uint32_t)TE_RESOURCE_USAGE_RENDERTARGET | (uint32_t)TE_RESOURCE_USAGE_SHADERRESOURCE):
				m_DescHeapSRV.AddDescriptorSRV(m_Resources[texture->m_ResourceIndex].Get(), &_SRVDesc);
				break;
			case ((uint32_t)TE_RESOURCE_USAGE_DEPTHSTENCIL | (uint32_t)TE_RESOURCE_USAGE_SHADERRESOURCE):
			{
				_SRVDesc.Format = GetDepthStencilSRVFormat(static_cast<TE_RESOURCE_FORMAT>(_SRVDesc.Format));

				m_DescHeapSRV.AddDescriptorSRV(m_Resources[texture->m_ResourceIndex].Get(), &_SRVDesc);
				break;
			}
			default:
				TE_ASSERT_CORE(false, "DX12: Wrong Resource format for creating SRV");
				break;
			}
		}
		else
		{
			switch (texture->m_Usage)
			{
			case  ((uint32_t)TE_RESOURCE_USAGE_RENDERTARGET | (uint32_t)TE_RESOURCE_USAGE_SHADERRESOURCE):
				m_DescHeapSRV.ReplaceDescriptorSRV(m_Resources[texture->m_ResourceIndex].Get(), &_SRVDesc, SRV->ViewIndex);
				break;
			case ((uint32_t)TE_RESOURCE_USAGE_DEPTHSTENCIL | (uint32_t)TE_RESOURCE_USAGE_SHADERRESOURCE):
			{
				
				_SRVDesc.Format = GetDepthStencilSRVFormat(static_cast<TE_RESOURCE_FORMAT>(_SRVDesc.Format));

				m_DescHeapSRV.ReplaceDescriptorSRV(m_Resources[texture->m_ResourceIndex].Get(), &_SRVDesc, SRV->ViewIndex);
				break;
			}
			default:
				TE_ASSERT_CORE(false, "DX12: Wrong Resource format for creating SRV");
				break;
			}
		}
	}

	void DirectX12BufferManager::CreateConstantBufferView(ConstantBufferUploadBase* constantBuffer, ConstantBufferView* CBV, uint8_t frameIndex)
	{
		uint32_t resourceIndex = constantBuffer->m_ResourceIndex + frameIndex;

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc{ m_Resources[resourceIndex]->GetGPUVirtualAddress(), static_cast<uint32_t>(constantBuffer->GetRequiredSize()) };

		if (CBV->ViewIndex == -1)
		{
			*CBV = ConstantBufferView{ m_DescHeapSRV.GetCurrentIndex(), resourceIndex, constantBuffer };

			m_DescHeapSRV.AddDescriptorCBV(&desc);
		}
		else
		{
			m_DescHeapSRV.ReplaceDescriptorCBV(&desc, CBV->ViewIndex);
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

		//Assign the vertex buffer ID and then increase variable by 1
		vb->m_ID = m_LastVertexBufferID++;

		vb->m_ViewIndex = static_cast<uint32_t>(m_VertexBufferViews.size());

		for (uint32_t i = 0; i < vertexStreamsNum; ++i)
		{
			auto vs = vertexStreams[i];
			CreateResource(vs);

			vs->m_ViewIndex = static_cast<uint32_t>(m_VertexBufferViews.size());
			auto r = m_Resources[vs->m_ResourceIndex];
			auto& view = m_VertexBufferViews.emplace_back();
			view.BufferLocation = r->GetGPUVirtualAddress();
			view.SizeInBytes = static_cast<UINT>(vs->GetBufferSize());
			view.StrideInBytes = static_cast<UINT>(vs->GetVertexSize());
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
		m_TexturesRenderTarget.reserve(20);
		m_TexturesDepthStencil.reserve(20);
		m_TexturesCubeMap.reserve(10);


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

		m_UnboundedConstantBuffers.clear();

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

		m_Resources[resourceIndex]->Release();
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

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12BufferManager::AddDescriptorRTV(ID3D12Resource* resource)
	{
		auto index = m_DescHeapRTV.AddDescriptor(resource);
		return m_DescHeapRTV.GetCPUHandle(index);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12BufferManager::AddDescriptorDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		auto index = m_DescHeapDSV.AddDescriptor(resource, desc);
		return m_DescHeapDSV.GetCPUHandle(index);
	}

	TruthEngine::TextureCubeMap* DirectX12BufferManager::CreateTextureCube(TE_IDX_TEXTURE idx, const char* filePath)
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

		TextureCubeMap& tex = m_TexturesCubeMap.emplace_back(index, fileName.c_str(), filePath, nullptr, desc.Width, desc.Height, 0, static_cast<TE_RESOURCE_FORMAT>(desc.Format));
		tex.m_ResourceIndex = resourceIndex;

		m_Map_Textures[idx] = &tex;

		return &tex;
	}

}