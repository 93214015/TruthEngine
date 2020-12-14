#include "pch.h"
#include "DX12CommandList.h"
#include "DX12GraphicDevice.h"
#include "DX12PipelineManager.h"
#include "DX12BufferManager.h"
#include "DX12ShaderManager.h"
#include "DX12SwapChain.h"

#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/TextureDepthStencil.h"
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Viewport.h"

namespace TruthEngine::API::DirectX12
{

	DX12CommandList::DX12CommandList(Core::GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, Core::BufferManager* bufferManager, Core::ShaderManager* shaderManager)
		: m_ShaderManager(static_cast<DX12ShaderManager*>(shaderManager)), m_BufferManager(static_cast<DX12BufferManager*>(bufferManager))
	{
		TE_ASSERT_CORE(graphicDevice, "Null Graphic Device is sent to CommandList!");

		auto gDevice = static_cast<DX12GraphicDevice*>(graphicDevice);

		auto result = m_CommandAllocator_Direct.Init(static_cast<D3D12_COMMAND_LIST_TYPE>(type), *gDevice);
		result = m_CommandAllocator_Copy.Init(D3D12_COMMAND_LIST_TYPE_COPY, *gDevice);

		TE_ASSERT_CORE(TE_SUCCEEDED(result), "DX12CommandAllocator initialization failed!");

		result = gDevice->CreateCommandList(m_D3D12CommandList_Direct, static_cast<D3D12_COMMAND_LIST_TYPE>(type));
		result = gDevice->CreateCommandList(m_D3D12CommandList_Copy, D3D12_COMMAND_LIST_TYPE_COPY);

		TE_ASSERT_CORE(TE_SUCCEEDED(result), "DX12CommandList initialization failed!");

		m_QueueClearRT.reserve(8);
		m_QueueClearDS.reserve(1);

		m_eventCopy = CreateEvent(NULL, false, true, NULL);
	}

	void DX12CommandList::Init(Core::GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, Core::BufferManager* bufferManager, Core::ShaderManager* shaderManager)
	{


	}

	void DX12CommandList::Reset()
	{
		_ResetContainers();
		

		m_CommandAllocator_Direct.Reset();
		m_D3D12CommandList_Direct->Reset(m_CommandAllocator_Direct.GetNativeObject(), nullptr);

		_SetDescriptorHeapSRV();
		
	}

	void DX12CommandList::Reset(Core::Pipeline* pipeline)
	{
		_ResetContainers();

		m_CommandAllocator_Direct.Reset();
		m_D3D12CommandList_Direct->Reset(m_CommandAllocator_Direct.GetNativeObject(), TE_INSTANCE_API_DX12_PIPELINEMANAGER->GetPipeline(pipeline).Get());

		_SetDescriptorHeapSRV();
	}

	void DX12CommandList::SetPipeline(Core::Pipeline* pipeline)
	{
		m_D3D12CommandList_Direct->SetGraphicsRootSignature(m_ShaderManager->GetRootSignature(pipeline->GetShader()));
		m_D3D12CommandList_Direct->SetPipelineState(TE_INSTANCE_API_DX12_PIPELINEMANAGER->GetPipeline(pipeline).Get());
		m_D3D12CommandList_Direct->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(pipeline->GetState_PrimitiveTopology()));

	}

	void DX12CommandList::SetRenderTarget(Core::SwapChain* swapChain, const Core::RenderTargetView RTV)
	{
		auto sc = static_cast<DX12SwapChain*>(swapChain);

		if (sc->GetBackBufferState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
		{
			_ChangeResourceState(sc->GetBackBufferResource()
				, D3D12_RESOURCE_STATE_PRESENT
				, D3D12_RESOURCE_STATE_RENDER_TARGET);

			sc->SetBackBufferState(D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		m_RTVHandles[m_RTVHandleNum] = m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex + sc->GetCurrentFrameIndex());

		m_RTVHandleNum++;

	}

	void DX12CommandList::SetRenderTarget(const Core::RenderTargetView RTV)
	{
		ChangeResourceState(RTV.Resource, TE_RESOURCE_STATES::RENDER_TARGET);

		m_RTVHandles[m_RTVHandleNum] = m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex);

		m_RTVHandleNum++;
	}

	void DX12CommandList::SetDepthStencil(const Core::DepthStencilView DSV)
	{
		ChangeResourceState(DSV.Resource, TE_RESOURCE_STATES::DEPTH_WRITE);

		m_DSVHandle = m_BufferManager->m_DescHeapDSV.GetCPUHandle(DSV.ViewIndex);

		m_DSVHandleNum = 1;
	}

	void DX12CommandList::SetShaderResource(const Core::ShaderResourceView srv, const uint32_t registerIndex)
	{
		m_SRVHandles_Texture[registerIndex] = m_BufferManager->m_DescHeapSRV.GetGPUHandle(srv.ViewIndex);
	}

	void DX12CommandList::SetConstantBuffer(const Core::ConstantBufferView cbv, const uint32_t registerIndex)
	{
		m_SRVHandles_CB[registerIndex] = m_BufferManager->m_DescHeapSRV.GetGPUHandle(cbv.ViewIndex);
	}

	void DX12CommandList::UpdateConstantBuffer(Core::ConstantBufferUploadBase* cb)
	{
		// TODO: implemetn update constant buffer
		return;
	}

	void DX12CommandList::ChangeResourceState(Core::GraphicResource* resource, TE_RESOURCE_STATES newState)
	{
		if (resource->GetState() == newState)
			return;

		_ChangeResourceState(m_BufferManager->m_Resources[resource->GetResourceIndex()].Get(), DX12_GET_STATE(resource->GetState()), DX12_GET_STATE(newState));

		resource->SetState(newState);
	}

	void DX12CommandList::SetVertexBuffer(Core::VertexBufferBase* vertexBuffer)
	{
		const auto vbID = vertexBuffer->GetID();
		if (vbID != m_AssignedVertexBufferID)
		{
			m_D3D12CommandList_Direct->IASetVertexBuffers(0, vertexBuffer->GetVertexStreamNum(), &m_BufferManager->m_VertexBufferViews[vertexBuffer->GetViewIndex()]);
			m_AssignedVertexBufferID = vbID;
		}

	}

	void DX12CommandList::SetIndexBuffer(Core::IndexBuffer* indexBuffer)
	{
		const auto ibID = indexBuffer->GetID();
		if (m_AssignedIndexBufferID != ibID)
		{
			m_D3D12CommandList_Direct->IASetIndexBuffer(&m_BufferManager->m_IndexBufferViews[indexBuffer->GetViewIndex()]);
			m_AssignedIndexBufferID = ibID;
		}
	}

	void DX12CommandList::DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset)
	{
		Commit();
		m_D3D12CommandList_Direct->DrawIndexedInstanced(indexNum, 1, indexOffset, vertexOffset, 0);
	}

	void DX12CommandList::Draw(uint32_t vertexNum, uint32_t vertexOffset)
	{
		Commit();
		m_D3D12CommandList_Direct->DrawInstanced(vertexNum, 1, vertexOffset, 0);
	}

	void DX12CommandList::Release()
	{
		m_D3D12CommandList_Direct->Release();
		m_CommandAllocator_Direct.GetNativeObject()->Release();

		m_SRVHandles_CB.clear();
		m_SRVHandles_Texture.clear();

		m_ResourceBarriers.clear();


	}

	void DX12CommandList::ClearRenderTarget(const Core::RenderTargetView RTV)
	{
		m_QueueClearRT.emplace_back(m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex), RTV.Resource->GetClearValues());
	}

	void DX12CommandList::ClearRenderTarget(const Core::SwapChain* swapChain, const Core::RenderTargetView RTV)
	{
		m_QueueClearRT.emplace_back(m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex + swapChain->GetCurrentFrameIndex()), swapChain->GetClearValues());
	}

	void DX12CommandList::ClearDepthStencil(const Core::DepthStencilView DSV)
	{
		m_QueueClearDS.emplace_back(m_BufferManager->m_DescHeapDSV.GetCPUHandle(DSV.ViewIndex), DSV.Resource->GetClearValues());
	}

	void DX12CommandList::Submit()
	{
		TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT->ExecuteCommandList(this);
	}


	void DX12CommandList::Commit()
	{



		if (m_ResourceBarriers.size() > 0)
		{
			m_D3D12CommandList_Direct->ResourceBarrier(m_ResourceBarriers.size(), m_ResourceBarriers.data());
			m_ResourceBarriers.clear();
		}

		if (m_RTVHandleNum > 0)
		{
			m_D3D12CommandList_Direct->OMSetRenderTargets(1, m_RTVHandles, false, m_DSVHandleNum > 0 ? &m_DSVHandle : NULL);
			m_RTVHandleNum = 0;
		}




		for (auto& c : m_QueueClearRT)
		{
			m_D3D12CommandList_Direct->ClearRenderTargetView(c.RTV, &c.ClearValue.x, 1, &m_BufferManager->m_Rect_FullScreen);
		}
		m_QueueClearRT.clear();


		for (auto& c : m_QueueClearDS)
		{
			m_D3D12CommandList_Direct->ClearDepthStencilView(c.DSV, D3D12_CLEAR_FLAG_DEPTH, c.ClearValue.depthValue, c.ClearValue.stencilValue, 1, &m_BufferManager->m_Rect_FullScreen);
		}
		m_QueueClearDS.clear();



		// TODO: need to be implemented
		/*if (!m_SRVHandles_Texture.empty())
		{
			std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> srvHandles;
			srvHandles.reserve(m_SRVHandles_Texture.size());

			uint32_t regSlot = m_SRVHandles_Texture.begin()->first;

			for (auto& srv : m_SRVHandles_Texture)
			{
				srvHandles.emplace_back(srv.second);
			}

			m_CommandList->SetGraphicsRootDescriptorTable(0, srvHandles[0]);
		}
		m_SRVHandles_Texture.clear();


		if (!m_SRVHandles_CB.empty())
		{
			std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> cbvHandles;
			cbvHandles.reserve(m_SRVHandles_CB.size());

			uint32_t regSlot = m_SRVHandles_CB.begin()->first;

			for (auto& srv : m_SRVHandles_CB)
			{
				cbvHandles.emplace_back(srv.second);
			}

			m_CommandList->SetGraphicsRootDescriptorTable(1, cbvHandles[0]);
		}
		m_SRVHandles_CB.clear();*/

	}

	void DX12CommandList::Present()
	{
		auto& sc = TE_INSTANCE_API_DX12_SWAPCHAIN;
		auto currentState = sc.GetBackBufferState();
		if (currentState != D3D12_RESOURCE_STATE_PRESENT)
		{
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(sc.GetBackBufferResource(), currentState, D3D12_RESOURCE_STATE_PRESENT);
			m_D3D12CommandList_Direct->ResourceBarrier(1, &barrier);
			sc.SetBackBufferState(D3D12_RESOURCE_STATE_PRESENT);

			Submit();
		}
		sc.Present();
	}

	void DX12CommandList::SetViewport(Core::Viewport* viewport, Core::ViewRect* rect)
	{
		m_D3D12CommandList_Direct->RSSetViewports(1, reinterpret_cast<D3D12_VIEWPORT*>(viewport));
		m_D3D12CommandList_Direct->RSSetScissorRects(1, reinterpret_cast<D3D12_RECT*>(rect));
	}

	bool DX12CommandList::IsRunning()
	{
		return m_CommandAllocator_Direct.IsRunning();
	}

	void DX12CommandList::_ResetContainers()
	{
		m_RTVHandleNum = 0;
		m_DSVHandleNum = 0;

		m_AssignedVertexBufferID = -1;
		m_AssignedIndexBufferID = -1;
		m_SRVHandles_CB.clear();
		m_SRVHandles_Texture.clear();

		m_ResourceBarriers.clear();

		m_QueueClearDS.clear();
		m_QueueClearRT.clear();

	}

	void DX12CommandList::_SetDescriptorHeapSRV()
	{
		auto descHeapSRV = m_BufferManager->m_DescHeapSRV.GetDescriptorHeap();
		m_D3D12CommandList_Direct->SetDescriptorHeaps(1, &descHeapSRV);
	}

	void DX12CommandList::UploadData(Core::Buffer* buffer, void* data, size_t sizeInByte)
	{

	}

	void DX12CommandList::_SubmitCopyCommands()
	{

		if (m_QueueCopyDefaultResource.size() == 0)
		{
			return;
		}


		m_CommandAllocator_Copy.Reset();
		m_D3D12CommandList_Copy->Reset(m_CommandAllocator_Copy.GetNativeObject(), nullptr);

		auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_CopyQueueRequiredSize);
		
		auto device = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetDevice();

		ID3D12Resource* intermediateResource;

		device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&intermediateResource));

		uint64_t offset = 0;
		for (auto& c : m_QueueCopyDefaultResource)
		{
			D3D12_SUBRESOURCE_DATA data;
			data.pData = c.Data;
			data.RowPitch = static_cast<LONG_PTR>(c.size);
			data.SlicePitch = 0;

			UpdateSubresources<1>(m_D3D12CommandList_Copy.Get(), c.resource, intermediateResource, offset, 0, 1, &data);

			offset += c.size;
		}

		auto queue = TE_INSTANCE_API_DX12_COMMANDQUEUECOPY;
		queue->ExecuteCommandList(this);

		concurrency::task t = concurrency::create_task([intermediateResource, this]() 
			{ 
				WaitForSingleObject(m_eventCopy, INFINITE);
				intermediateResource->Release();
			});
	}

}