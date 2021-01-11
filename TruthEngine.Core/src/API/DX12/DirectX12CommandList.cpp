#include "pch.h"
#include "DirectX12CommandList.h"


#include "DirectX12GraphicDevice.h"
#include "DirectX12Manager.h"
#include "DirectX12PipelineManager.h"
#include "DirectX12BufferManager.h"
#include "DirectX12ShaderManager.h"
#include "DirectX12SwapChain.h"


#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/TextureDepthStencil.h"
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Shader.h"

namespace TruthEngine::API::DirectX12
{

	struct RootParameterVisitor
	{
		ID3D12GraphicsCommandList* m_D3D12CommandList;

		void operator()(const DirectX12RootArgumentTable& rootTable)
		{
			m_D3D12CommandList->SetGraphicsRootDescriptorTable(rootTable.ParameterIndex, rootTable.GPUDescriptorHandle);
		}
	};


	DirectX12CommandList::DirectX12CommandList(Core::GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, Core::BufferManager* bufferManager, Core::ShaderManager* shaderManager, TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX)
		: CommandList(renderPassIDX, shaderClassIDX)
		, m_ShaderManager(static_cast<DirectX12ShaderManager*>(shaderManager)), m_BufferManager(static_cast<DirectX12BufferManager*>(bufferManager))
	{
		TE_ASSERT_CORE(graphicDevice, "Null Graphic Device is sent to CommandList!");

		auto gDevice = static_cast<DirectX12GraphicDevice*>(graphicDevice);

		auto result = m_CommandAllocator.Init(static_cast<D3D12_COMMAND_LIST_TYPE>(type), *gDevice);

		TE_ASSERT_CORE(TE_SUCCEEDED(result), "DX12CommandAllocator initialization failed!");

		result = gDevice->CreateCommandList(m_D3D12CommandList, static_cast<D3D12_COMMAND_LIST_TYPE>(type));

		TE_ASSERT_CORE(TE_SUCCEEDED(result), "DX12CommandList initialization failed!");

		m_QueueClearRT.reserve(8);
		m_QueueClearDS.reserve(1);

		
	}

	void DirectX12CommandList::Reset()
	{
		_ResetContainers();


		m_CommandAllocator.Reset();
		m_D3D12CommandList->Reset(m_CommandAllocator.GetNativeObject(), nullptr);

		_SetDescriptorHeapSRV();

	}

	void DirectX12CommandList::Reset(Core::Pipeline* pipeline)
	{
		_ResetContainers();

		m_CommandAllocator.Reset();
		m_D3D12CommandList->Reset(m_CommandAllocator.GetNativeObject(), TE_INSTANCE_API_DX12_PIPELINEMANAGER->GetPipeline(pipeline).Get());

		_SetDescriptorHeapSRV();
	}

	void DirectX12CommandList::SetPipeline(Core::Pipeline* pipeline)
	{
		m_D3D12CommandList->SetGraphicsRootSignature(DirectX12Manager::GetInstance()->GetD3D12RootSignature(m_ShaderClassIDX));
		m_D3D12CommandList->SetPipelineState(TE_INSTANCE_API_DX12_PIPELINEMANAGER->GetPipeline(pipeline).Get());
		m_D3D12CommandList->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(pipeline->GetState_PrimitiveTopology()));
		
	}

	void DirectX12CommandList::SetRenderTarget(Core::SwapChain* swapChain, const Core::RenderTargetView RTV)
	{
		auto sc = static_cast<DirectX12SwapChain*>(swapChain);

		if (sc->GetBackBufferState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
		{
			_QueueBarrier(sc->GetBackBufferResource()
				, D3D12_RESOURCE_STATE_PRESENT
				, D3D12_RESOURCE_STATE_RENDER_TARGET);

			sc->SetBackBufferState(D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		m_RTVHandles[m_RTVHandleNum] = m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex + sc->GetCurrentFrameIndex());

		m_RTVHandleNum++;

	}

	void DirectX12CommandList::SetRenderTarget(const Core::RenderTargetView RTV)
	{
		_ChangeResourceState(RTV.Resource, TE_RESOURCE_STATES::RENDER_TARGET);

		m_RTVHandles[m_RTVHandleNum] = m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex);

		m_RTVHandleNum++;
	}

	void DirectX12CommandList::SetDepthStencil(const Core::DepthStencilView DSV)
	{
		_ChangeResourceState(DSV.Resource, TE_RESOURCE_STATES::DEPTH_WRITE);

		m_DSVHandle = m_BufferManager->m_DescHeapDSV.GetCPUHandle(DSV.ViewIndex);

		m_DSVHandleNum = 1;
	}

	void DirectX12CommandList::SetShaderResource(const Core::ShaderResourceView srv, const uint32_t registerIndex)
	{
		m_SRVHandles_Texture[registerIndex] = m_BufferManager->m_DescHeapSRV.GetGPUHandle(srv.ViewIndex);
	}

	void DirectX12CommandList::SetConstantBuffer(const Core::ConstantBufferView cbv, const uint32_t registerIndex)
	{
		m_SRVHandles_CB[registerIndex] = m_BufferManager->m_DescHeapSRV.GetGPUHandle(cbv.ViewIndex);
	}

	void DirectX12CommandList::UpdateConstantBuffer(Core::ConstantBufferUploadBase* cb)
	{
		throw std::exception("DX12CommandList UpdateConstantBuffer has not been implemented");
		return;
	}

	void DirectX12CommandList::_BindResource()
	{	

		for (auto& t0 : m_ShaderSignature->Textures)
		{
			for (auto& t : t0)
			{
				if (auto gresource = m_BufferManager->GetTexture(t.TextureIDX); gresource != nullptr)
				{
					_ChangeResourceState(gresource, TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE);
				}
			}
		}

		if (m_RenderPassIDX != TE_IDX_RENDERPASS::NONE)
		{
			if (m_RootArguments == nullptr)
			{
				m_RootArguments = DirectX12Manager::GetInstance()->GetRootArguments(m_ShaderClassIDX);
			}

			for (auto& t : m_RootArguments->Tables)
			{
				m_D3D12CommandList->SetGraphicsRootDescriptorTable(t.ParameterIndex, t.GPUDescriptorHandle);
			}
		}
	}

	void DirectX12CommandList::_ChangeResourceState(Core::GraphicResource* resource, TE_RESOURCE_STATES newState)
	{
		if (resource->GetState() == newState)
			return;

		_QueueBarrier(m_BufferManager->m_Resources[resource->GetResourceIndex()].Get(), DX12_GET_STATE(resource->GetState()), DX12_GET_STATE(newState));

		resource->SetState(newState);
	}

	void DirectX12CommandList::SetVertexBuffer(Core::VertexBufferBase* vertexBuffer)
	{
		const auto vbID = vertexBuffer->GetID();
		if (vbID != m_AssignedVertexBufferID)
		{
			auto vbss = vertexBuffer->GetVertexBufferStreams();
			for (auto vbs : vbss)
			{
				_ChangeResourceState(vbs, TE_RESOURCE_STATES::VERTEX_AND_CONSTANT_BUFFER);
			}

			m_D3D12CommandList->IASetVertexBuffers(0, vertexBuffer->GetVertexStreamNum(), &m_BufferManager->m_VertexBufferViews[vertexBuffer->GetViewIndex()]);
			m_AssignedVertexBufferID = vbID;
		}

	}

	void DirectX12CommandList::SetIndexBuffer(Core::IndexBuffer* indexBuffer)
	{
		const auto ibID = indexBuffer->GetID();
		if (m_AssignedIndexBufferID != ibID)
		{
			_ChangeResourceState(indexBuffer, TE_RESOURCE_STATES::INDEX_BUFFER);

			m_D3D12CommandList->IASetIndexBuffer(&m_BufferManager->m_IndexBufferViews[indexBuffer->GetViewIndex()]);
			m_AssignedIndexBufferID = ibID;
		}
	}

	void DirectX12CommandList::DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset)
	{
		Commit();
		m_D3D12CommandList->DrawIndexedInstanced(indexNum, 1, indexOffset, vertexOffset, 0);
	}

	void DirectX12CommandList::Draw(uint32_t vertexNum, uint32_t vertexOffset)
	{
		Commit();
		m_D3D12CommandList->DrawInstanced(vertexNum, 1, vertexOffset, 0);
	}

	void DirectX12CommandList::Release()
	{
		m_D3D12CommandList->Release();
		m_CommandAllocator.GetNativeObject()->Release();

		m_SRVHandles_CB.clear();
		m_SRVHandles_Texture.clear();

		m_ResourceBarriers.clear();


	}

	void DirectX12CommandList::ClearRenderTarget(const Core::RenderTargetView RTV)
	{
		m_QueueClearRT.emplace_back(m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex), RTV.Resource->GetClearValues());
	}

	void DirectX12CommandList::ClearRenderTarget(const Core::SwapChain* swapChain, const Core::RenderTargetView RTV)
	{
		m_QueueClearRT.emplace_back(m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex + swapChain->GetCurrentFrameIndex()), swapChain->GetClearValues());
	}

	void DirectX12CommandList::ClearDepthStencil(const Core::DepthStencilView DSV)
	{
		m_QueueClearDS.emplace_back(m_BufferManager->m_DescHeapDSV.GetCPUHandle(DSV.ViewIndex), DSV.Resource->GetClearValues());
	}

	void DirectX12CommandList::Submit()
	{
		TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT->ExecuteCommandList(this);
	}


	void DirectX12CommandList::Commit()
	{



		if (m_ResourceBarriers.size() > 0)
		{
			m_D3D12CommandList->ResourceBarrier(m_ResourceBarriers.size(), m_ResourceBarriers.data());
			m_ResourceBarriers.clear();
		}

		_UploadDefaultBuffers();

		if (m_RTVHandleNum > 0)
		{
			m_D3D12CommandList->OMSetRenderTargets(1, m_RTVHandles, false, m_DSVHandleNum > 0 ? &m_DSVHandle : NULL);
			m_RTVHandleNum = 0;
		}


		for (auto& c : m_QueueClearRT)
		{
			m_D3D12CommandList->ClearRenderTargetView(c.RTV, &c.ClearValue.x, 1, &m_BufferManager->m_Rect_FullScreen);
		}
		m_QueueClearRT.clear();


		for (auto& c : m_QueueClearDS)
		{
			m_D3D12CommandList->ClearDepthStencilView(c.DSV, D3D12_CLEAR_FLAG_DEPTH, c.ClearValue.depthValue, c.ClearValue.stencilValue, 1, &m_BufferManager->m_Rect_FullScreen);
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

	void DirectX12CommandList::Present()
	{
		auto& sc = TE_INSTANCE_API_DX12_SWAPCHAIN;
		auto currentState = sc.GetBackBufferState();
		if (currentState != D3D12_RESOURCE_STATE_PRESENT)
		{
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(sc.GetBackBufferResource(), currentState, D3D12_RESOURCE_STATE_PRESENT);
			m_D3D12CommandList->ResourceBarrier(1, &barrier);
			sc.SetBackBufferState(D3D12_RESOURCE_STATE_PRESENT);

			Submit();
		}
		sc.Present();
	}

	void DirectX12CommandList::SetViewport(Core::Viewport* viewport, Core::ViewRect* rect)
	{
		m_D3D12CommandList->RSSetViewports(1, reinterpret_cast<D3D12_VIEWPORT*>(viewport));
		m_D3D12CommandList->RSSetScissorRects(1, reinterpret_cast<D3D12_RECT*>(rect));
	}

	bool DirectX12CommandList::IsRunning()
	{
		return m_CommandAllocator.IsRunning();
	}

	void DirectX12CommandList::WaitToFinish()
	{
		m_CommandAllocator.WaitToFinish();
	}

	void DirectX12CommandList::_ResetContainers()
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

	void DirectX12CommandList::_SetDescriptorHeapSRV()
	{
		if (m_ShaderClassIDX != TE_IDX_SHADERCLASS::NONE)
		{
			m_D3D12CommandList->SetGraphicsRootSignature(DirectX12Manager::GetInstance()->GetD3D12RootSignature(m_ShaderClassIDX));
		}

		auto descHeapSRV = m_BufferManager->m_DescHeapSRV.GetDescriptorHeap();
		m_D3D12CommandList->SetDescriptorHeaps(1, &descHeapSRV);

		if (m_ShaderClassIDX != TE_IDX_SHADERCLASS::NONE)
		{
			_BindResource();
		}
	}

	void DirectX12CommandList::UploadData(Core::Buffer* buffer, const void* data, size_t sizeInByte)
	{
		_ChangeResourceState(buffer, TE_RESOURCE_STATES::COPY_DEST);

		m_QueueCopyDefaultResource.emplace_back(m_BufferManager->m_Resources[buffer->GetResourceIndex()].Get(), data, sizeInByte);

		m_CopyQueueRequiredSize += buffer->GetRequiredSize();
	}

	void DirectX12CommandList::UploadData(Core::ConstantBufferDirectBase* cb)
	{
		auto& directConstant = m_RootArguments->DircectConstants.find(cb->GetIDX())->second;
		m_D3D12CommandList->SetGraphicsRoot32BitConstants(directConstant.ParameterIndex, cb->Get32BitNum(), cb->GetDataPtr(), 0);
	}

	void DirectX12CommandList::_UploadDefaultBuffers()
	{
		if (m_QueueCopyDefaultResource.size() == 0)
		{
			return;
		}

		auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_CopyQueueRequiredSize);

		auto device = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetDevice();

		device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_IntermediateResource));

		uint64_t offset = 0;
		for (auto& c : m_QueueCopyDefaultResource)
		{
			D3D12_SUBRESOURCE_DATA data;
			data.pData = c.Data;
			data.RowPitch = static_cast<LONG_PTR>(c.Size);
			data.SlicePitch = 0;

			auto uplaodedSize = UpdateSubresources<1>(m_D3D12CommandList.Get(), c.D3D12Resource, m_IntermediateResource.Get(), offset, 0, 1, &data);
			if (uplaodedSize == 0)
			{
				TE_LOG_CORE_WARN("UploadToDefault Buffer was not completed");
			}

			offset += c.Size;
		}

		m_QueueCopyDefaultResource.clear();
		m_CopyQueueRequiredSize = 0;

	}


	// 	void DX12CommandList::_SubmitCopyCommands()
// 	{
// 
// 		if (m_QueueCopyDefaultResource.size() == 0)
// 		{
// 			return;
// 		}
// 
// 
// 		for (auto& barrier : m_ResourceBarriers_Copy)
// 		{
// 			m_D3D12CommandList_Copy->ResourceBarrier(m_ResourceBarriers_Copy.size(), m_ResourceBarriers_Copy.data());
// 			m_ResourceBarriers_Copy.clear();
// 		}
// 
// 		m_CommandAllocator_Copy.Reset();
// 		m_D3D12CommandList_Copy->Reset(m_CommandAllocator_Copy.GetNativeObject(), nullptr);
// 
// 		auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_CopyQueueRequiredSize);
// 
// 		auto device = TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetDevice();
// 
// 		ID3D12Resource* intermediateResource;
// 
// 		device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&intermediateResource));
// 
// 		uint64_t offset = 0;
// 		for (auto& c : m_QueueCopyDefaultResource)
// 		{
// 			D3D12_SUBRESOURCE_DATA data;
// 			data.pData = c.Data;
// 			data.RowPitch = static_cast<LONG_PTR>(c.Size);
// 			data.SlicePitch = 0;
// 
// 			UpdateSubresources<1>(m_D3D12CommandList_Copy.Get(), c.D3D12Resource, intermediateResource, offset, 0, 1, &data);
// 
// 			offset += c.Size;
// 		}
// 
// 		auto queue = TE_INSTANCE_API_DX12_COMMANDQUEUECOPY;
// 		queue->ExecuteCommandList(this);
// 
// 		concurrency::task t = concurrency::create_task([intermediateResource, this]()
// 			{
// 				WaitForSingleObject(m_eventCopy, INFINITE);
// 				intermediateResource->Release();
// 			});
// 	}

}