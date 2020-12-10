#include "pch.h"
#include "DX12BufferManager.h"

#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/TextureDepthStencil.h"
#include "Core/Renderer/ConstantBuffer.h"
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/VertexBufferStream.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/Buffer.h"

#include "DX12GraphicDevice.h"
#include "DX12SwapChain.h"

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


	inline D3D12_RESOURCE_DESC1 GetTextureDesc(uint32_t width, uint32_t height, TE_RESOURCE_USAGE usage, TE_RESOURCE_FORMAT format)
	{
		const auto resourceFlags = GetResourceFlags(usage);
		return CD3DX12_RESOURCE_DESC1::Tex2D(GetFormat(format), static_cast<uint64_t>(width), height, 1, 0, 1, 0, resourceFlags);
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





	TE_RESULT DX12BufferManager::CreateResource(Core::TextureRenderTarget* tRT)
	{
		const auto desc = GetTextureDesc(tRT->m_Width, tRT->m_Height, tRT->m_Usage, tRT->m_Format);

		tRT->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

		auto& resource = m_Resources.emplace_back();

		m_ResouceNameMap[tRT->m_Name.c_str()] = resource.Get();

		D3D12_CLEAR_VALUE v{ GetFormat(tRT->m_Format), { tRT->m_ClearValue.x, tRT->m_ClearValue.y, tRT->m_ClearValue.z, tRT->m_ClearValue.w } };

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(tRT->m_State)
			, &v
			, nullptr, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));


		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DX12BufferManager::CreateResource(Core::TextureDepthStencil* tDS)
	{

		const auto desc = GetTextureDesc(tDS->m_Width, tDS->m_Height, tDS->m_Usage, tDS->m_Format);

		tDS->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

		auto& resource = m_Resources.emplace_back();

		m_ResouceNameMap[tDS->m_Name.c_str()] = resource.Get();

		D3D12_CLEAR_VALUE v{ GetFormat(tDS->m_Format), { tDS->m_ClearValue.depthValue, tDS->m_ClearValue.stencilValue } };

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(tDS->m_State), &v
			, nullptr, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));


		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DX12BufferManager::CreateResource(Core::BufferUpload* cb)
	{
		const auto desc = GetBufferDesc(cb->m_SizeInByte, cb->m_Usage);

		cb->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

		auto& resource = m_Resources.emplace_back();

		m_ResouceNameMap[cb->m_Name.c_str()] = resource.Get();

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(cb->m_State), nullptr
			, nullptr, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));

		CD3DX12_RANGE range(0, 0);

		resource->Map(0, &range, reinterpret_cast<void**>(&cb->m_MappedData));

		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;

	}

	TE_RESULT DX12BufferManager::CreateResource(Core::VertexBufferStreamBase* vb)
	{
		const auto desc = GetBufferDesc(vb->GetVertexBufferSize(), vb->m_Usage);

		vb->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

		auto& resource = m_Resources.emplace_back();

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(vb->m_State), nullptr
			, nullptr, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));


		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	TE_RESULT DX12BufferManager::CreateResource(Core::IndexBuffer* ib)
	{
		const auto desc = GetBufferDesc(ib->GetBufferSize(), TE_RESOURCE_USAGE_INDEXBUFFER);

		ib->m_ResourceIndex = static_cast<uint32_t>(m_Resources.size());

		auto& resource = m_Resources.emplace_back();

		auto hr = TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateCommittedResource2(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)
			, D3D12_HEAP_FLAG_NONE
			, &desc, DX12_GET_STATE(ib->m_State), nullptr
			, nullptr, IID_PPV_ARGS(resource.ReleaseAndGetAddressOf()));


		return SUCCEEDED(hr) ? TE_SUCCESSFUL : TE_RESULT::TE_FAIL;
	}

	Core::RenderTargetView DX12BufferManager::CreateRenderTargetView(Core::TextureRenderTarget* RT)
	{
		const auto rtv = Core::RenderTargetView{ m_DescHeapRTV.GetCurrentIndex(), RT->m_ResourceIndex, nullptr };

		RT->m_ViewIndex = m_DescHeapRTV.AddDescriptor(m_Resources[RT->m_ResourceIndex].Get());

		return rtv;
	}

	Core::RenderTargetView DX12BufferManager::CreateRenderTargetView(Core::SwapChain* swapChain)
	{
		auto sc = static_cast<DX12SwapChain*>(swapChain);

		return Core::RenderTargetView{ sc->InitRTVs(&m_DescHeapRTV), 0, nullptr };
	}

	Core::DepthStencilView DX12BufferManager::CreateDepthStencilView(Core::TextureDepthStencil* DS)
	{

		const auto dsv = Core::DepthStencilView{ m_DescHeapDSV.GetCurrentIndex() };

		D3D12_DEPTH_STENCIL_VIEW_DESC desc;
		desc.Format = GetDSVFormat(DS->m_Format);
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = D3D12_DSV_FLAG_NONE;
		desc.Texture2D.MipSlice = 0;

		DS->m_ViewIndex = m_DescHeapDSV.AddDescriptor(m_Resources[DS->m_ResourceIndex].Get(), &desc);

		return dsv;

	}

	Core::ShaderResourceView DX12BufferManager::CreateShaderResourceView(Core::GraphicResource* graphicResources[], uint32_t resourceNum)
	{

		const auto srv = Core::ShaderResourceView{ m_DescHeapSRV.GetCurrentIndex() };


		for (uint32_t i = 0; i < resourceNum; ++i)
		{
			auto r = graphicResources[i];
			switch (graphicResources[i]->m_Usage)
			{
			case TE_RESOURCE_USAGE_RENDERTARGET:
				m_DescHeapSRV.AddDescriptorSRV(m_Resources[r->m_ResourceIndex].Get(), nullptr);
				break;
			case TE_RESOURCE_USAGE_DEPTHSTENCIL:
			{
				auto d = static_cast<Core::TextureDepthStencil*>(r);
				D3D12_SHADER_RESOURCE_VIEW_DESC desc;
				desc.Format = GetDepthStencilSRVFormat(d->m_Format);
				desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				desc.Texture2D.MipLevels = 1;
				desc.Texture2D.MostDetailedMip = 0;
				m_DescHeapSRV.AddDescriptorSRV(m_Resources[r->m_ResourceIndex].Get(), &desc);
				break;
			}
			default:
				TE_ASSERT_CORE(false, "DX12: Wrong Resource format for creating SRV");
				break;
			}

		}

		return srv;

	}

	Core::ConstantBufferView DX12BufferManager::CreateConstantBufferView(Core::Buffer* CB[], uint32_t resourceNum)
	{

		const auto cbv = Core::ConstantBufferView{ m_DescHeapSRV.GetCurrentIndex() };

		for (uint32_t i = 0; i < resourceNum; ++i)
		{
			auto cb = CB[i];
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc{ m_Resources[cb->m_ResourceIndex]->GetGPUVirtualAddress(), static_cast<uint32_t>(cb->GetSizeInByte()) };

			cb->m_ViewIndex = m_DescHeapSRV.AddDescriptorCBV(&desc);
		}

		return cbv;

	}

	TE_RESULT DX12BufferManager::CreateVertexBuffer(Core::VertexBufferBase* vb)
	{
		uint32_t vertexStreamsNum = vb->GetVertexStreamNum();
		auto vertexStreams = vb->GetVertexBufferStreams();

		//Assign the vertex buffer ID and then increase variable by 1
		vb->m_ID = m_LastVertexBufferID++;

		vb->m_ViewIndex = static_cast<uint32_t>(m_VertexBufferViews.size());

		for (uint32_t i = 0; i < vertexStreamsNum; ++i)
		{
			auto vs = vertexStreams[i];
			TE_ASSERT_CORE(CreateResource(vs), "Creating vertex buffers failed!");

			vs->m_ViewIndex = static_cast<uint32_t>(m_VertexBufferViews.size());
			auto r = m_Resources[vs->m_ResourceIndex];
			auto& view = m_VertexBufferViews.emplace_back();
			view.BufferLocation = r->GetGPUVirtualAddress();
			view.SizeInBytes = static_cast<UINT>(vs->GetVertexBufferSize());
			view.StrideInBytes = static_cast<UINT>(vs->GetVertexSize());
		}


		return TE_SUCCESSFUL;
	}


	TE_RESULT DX12BufferManager::CreateIndexBuffer(Core::IndexBuffer* ib)
	{
		TE_ASSERT_CORE(CreateResource(ib), "Creating index buffer failed!");

		//assing index buffer ID then increase the variable by 1
		ib->m_ID = m_LastIndexBufferID++;

		ib->m_ViewIndex = static_cast<uint32_t>(m_IndexBufferViews.size());

		auto& r = m_Resources[ib->m_ResourceIndex];

		auto& iv = m_IndexBufferViews.emplace_back(D3D12_INDEX_BUFFER_VIEW{ r->GetGPUVirtualAddress() , static_cast<uint32_t>(ib->GetBufferSize()), DXGI_FORMAT_R32_UINT });

		return TE_SUCCESSFUL;
	}

	void DX12BufferManager::Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum)
	{
		m_DescHeapSRV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, shaderResourceViewNum);
		m_DescHeapRTV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, renderTargetViewNum);
		m_DescHeapDSV.Init(TE_INSTANCE_API_DX12_GRAPHICDEVICE, depthBufferViewNum);

		m_Resources.reserve(resourceNum);

		m_Rect_FullScreen.left = 0;
		m_Rect_FullScreen.top = 0;
		m_Rect_FullScreen.right = TE_INSTANCE_APPLICATION->GetClientWidth();
		m_Rect_FullScreen.bottom = TE_INSTANCE_APPLICATION->GetClientHeight();
	}


}