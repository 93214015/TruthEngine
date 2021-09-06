#include "pch.h"
#include "DirectX12CommandList.h"


#include "DirectX12GraphicDevice.h"
#include "DirectX12Manager.h"
#include "DirectX12PipelineManager.h"
#include "DirectX12BufferManager.h"
#include "DirectX12ShaderManager.h"
#include "DirectX12SwapChain.h"
#include "DirectX12Helpers.h"


#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/TextureDepthStencil.h"
#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/Pipeline.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Shader.h"

namespace TruthEngine::API::DirectX12
{

	static DXGI_FORMAT DXGI_GetDepthStencilSRVFormat(const TE_RESOURCE_FORMAT format)
	{
		return static_cast<DXGI_FORMAT>(GetDepthStencilSRVFormat(format));
	}


	struct RootParameterVisitor
	{
		ID3D12GraphicsCommandList* m_D3D12CommandList;

		void operator()(const DirectX12RootArgumentTable& rootTable)
		{
			m_D3D12CommandList->SetGraphicsRootDescriptorTable(rootTable.ParameterIndex, rootTable.GPUDescriptorHandle);
		}
	};


	DirectX12CommandList::DirectX12CommandList(GraphicDevice* graphicDevice, TE_RENDERER_COMMANDLIST_TYPE type, BufferManager* bufferManager, ShaderManager* shaderManager, TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint8_t frameIndex)
		: CommandList(renderPassIDX, shaderClassIDX, frameIndex)
		, m_ShaderManager(static_cast<DirectX12ShaderManager*>(shaderManager)), m_BufferManager(static_cast<DirectX12BufferManager*>(bufferManager))
	{
		TE_ASSERT_CORE(graphicDevice, "Null Graphic Device is sent to CommandList!");

		auto gDevice = static_cast<DirectX12GraphicDevice*>(graphicDevice);

		auto result = mCommandAllocator.Init(static_cast<D3D12_COMMAND_LIST_TYPE>(type), *gDevice);

		TE_ASSERT_CORE(TE_SUCCEEDED(result), "DX12CommandAllocator initialization failed!");

		result = gDevice->CreateCommandList(mD3D12CommandList, static_cast<D3D12_COMMAND_LIST_TYPE>(type));

		TE_ASSERT_CORE(TE_SUCCEEDED(result), "DX12CommandList initialization failed!");

		m_QueueClearRT.reserve(8);
		m_QueueClearDS.reserve(8);


	}

	void DirectX12CommandList::ResetCompute()
	{
		_ResetContainers();

		mCommandAllocator.Reset();
		mD3D12CommandList->Reset(mCommandAllocator.GetNativeObject(), nullptr);

		_SetDescriptorHeapSRV();
		if (m_ShaderClassIDX != TE_IDX_SHADERCLASS::NONE)
		{
			_SetRootSignatureCompute(false);
			_SetRootArgumentsCompute(false);
		}
	}

	void DirectX12CommandList::ResetCompute(PipelineCompute* pipeline)
	{
		_ResetContainers();

		mCommandAllocator.Reset();
		mD3D12CommandList->Reset(mCommandAllocator.GetNativeObject(), nullptr);

		bool _NewShaderClass = false;

		if (m_ShaderClassIDX != pipeline->GetShaderClassIDX())
		{
			_NewShaderClass = true;
			m_ShaderClassIDX = pipeline->GetShaderClassIDX();
		}

		_SetDescriptorHeapSRV();
		_SetRootSignatureCompute(_NewShaderClass);
		_SetRootArgumentsCompute(_NewShaderClass);


		SetPipelineCompute(pipeline);
	}

	void DirectX12CommandList::ResetGraphics()
	{
		_ResetContainers();

		mCommandAllocator.Reset();
		mD3D12CommandList->Reset(mCommandAllocator.GetNativeObject(), nullptr);

		_SetDescriptorHeapSRV();
		if (m_ShaderClassIDX != TE_IDX_SHADERCLASS::NONE)
		{
			_SetRootSignatureGraphics(false);
			_SetRootArgumentsGraphics(false);
		}
	}

	void DirectX12CommandList::ResetGraphics(PipelineGraphics* pipeline)
	{
		_ResetContainers();

		mCommandAllocator.Reset();
		mD3D12CommandList->Reset(mCommandAllocator.GetNativeObject(), TE_INSTANCE_API_DX12_PIPELINEMANAGER->GetGraphicsPipeline(pipeline).Get());

		bool _NewShaderClass = false;

		if (m_ShaderClassIDX != pipeline->GetShaderClassIDX())
		{
			_NewShaderClass = true;
			m_ShaderClassIDX = pipeline->GetShaderClassIDX();
		}

		_SetDescriptorHeapSRV();
		_SetRootSignatureGraphics(_NewShaderClass);
		_SetRootArgumentsGraphics(_NewShaderClass);

		SetPipelineGraphics(pipeline);
	}

	void DirectX12CommandList::SetPipelineGraphics(PipelineGraphics* pipeline)
	{
		if (pipeline->GetShaderClassIDX() != m_ShaderClassIDX)
		{
			m_ShaderClassIDX = pipeline->GetShaderClassIDX();
			_SetRootSignatureGraphics(true);
			_SetRootArgumentsGraphics(true);
		}
		mD3D12CommandList->SetPipelineState(TE_INSTANCE_API_DX12_PIPELINEMANAGER->GetGraphicsPipeline(pipeline).Get());
		mD3D12CommandList->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(pipeline->GetState_PrimitiveTopology()));
	}

	void DirectX12CommandList::SetPipelineCompute(PipelineCompute* pipeline)
	{
		if (pipeline->GetShaderClassIDX() != m_ShaderClassIDX)
		{
			m_ShaderClassIDX = pipeline->GetShaderClassIDX();
			_SetRootSignatureCompute(true);
			_SetRootArgumentsCompute(true);
		}
		mD3D12CommandList->SetPipelineState(TE_INSTANCE_API_DX12_PIPELINEMANAGER->GetComputePipeline(pipeline).Get());
	}

	void DirectX12CommandList::SetRenderTarget(SwapChain* swapChain, const RenderTargetView& RTV)
	{
		auto sc = static_cast<DirectX12SwapChain*>(swapChain);

		_ChangeResourceState(sc, TE_RESOURCE_STATES::RENDER_TARGET);

		auto frameIndex = sc->GetCurrentFrameIndex();

		m_RTVHandles[m_RTVHandleNum] = m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex + frameIndex);

		m_RTVHandleNum++;

	}

	void DirectX12CommandList::SetRenderTarget(const RenderTargetView& RTV)
	{
		_ChangeResourceState(RTV.Resource, TE_RESOURCE_STATES::RENDER_TARGET);

		m_RTVHandles[m_RTVHandleNum] = m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex);

		m_RTVHandleNum++;
	}

	void DirectX12CommandList::SetDepthStencil(const DepthStencilView& DSV)
	{
		_ChangeResourceState(DSV.Resource, TE_RESOURCE_STATES::DEPTH_WRITE);

		m_DSVHandle = m_BufferManager->m_DescHeapDSV.GetCPUHandle(DSV.ViewIndex);

		m_DSVHandleNum = 1;
	}

	void DirectX12CommandList::SetDepthStencil(const DepthStencilView& DSV, uint32_t _StencilRefValue)
	{
		SetDepthStencil(DSV);
		mD3D12CommandList->OMSetStencilRef(_StencilRefValue);
	}

	void DirectX12CommandList::ResolveMultiSampledTexture(Texture* _SourceTexture, Texture* _DestTexture)
	{
		_ChangeResourceState(_SourceTexture, TE_RESOURCE_STATES::RESOLVE_SOURCE);
		_ChangeResourceState(_DestTexture, TE_RESOURCE_STATES::RESOLVE_DEST);

		ID3D12Resource* _SourceResource = m_BufferManager->GetResource(_SourceTexture);
		ID3D12Resource* _DestResource =  m_BufferManager->GetResource(_DestTexture);

		DXGI_FORMAT _Format = static_cast<DXGI_FORMAT>(_SourceTexture->GetFormat());

		if (_SourceTexture->GetUsage() == TE_RESOURCE_USAGE_DEPTHSTENCIL)
		{
			_Format = DXGI_GetDepthStencilSRVFormat(_SourceTexture->GetFormat());
		}

		m_QueueResolveResource.emplace_back(_SourceResource, _DestResource, _Format);
	}

	/*void DirectX12CommandList::SetShaderResource(const ShaderResourceView srv, const uint32_t registerIndex)
	{
		m_SRVHandles_Texture[registerIndex] = m_BufferManager->m_DescHeapSRV.GetGPUHandle(srv.ViewIndex);
	}

	void DirectX12CommandList::SetConstantBuffer(const ConstantBufferView cbv, const uint32_t registerIndex)
	{
		m_SRVHandles_CB[registerIndex] = m_BufferManager->m_DescHeapSRV.GetGPUHandle(cbv.ViewIndex);
	}*/

	void DirectX12CommandList::UpdateConstantBuffer(ConstantBufferUploadBase* cb)
	{
		throw std::exception("DX12CommandList UpdateConstantBuffer has not been implemented");
		return;
	}

	void DirectX12CommandList::_SetRootArgumentsGraphics(bool _NewShaderClass)
	{

		if (m_ShaderRequiredResources == nullptr || _NewShaderClass)
		{
			m_ShaderRequiredResources = TE_INSTANCE_SHADERMANAGER->GetShaderRequiredResources(m_ShaderClassIDX);
		}

		for (GraphicResource* _CBVResource : m_ShaderRequiredResources->GetCBVResources())
		{
			if (_CBVResource->GetState() != TE_RESOURCE_STATES::GENERIC_READ0)
				_ChangeResourceState(_CBVResource, TE_RESOURCE_STATES::VERTEX_AND_CONSTANT_BUFFER);
		}

		for (GraphicResource* _SRVResource : m_ShaderRequiredResources->GetSRVResources())
		{
			_ChangeResourceState(_SRVResource, TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE);
		}

		for (GraphicResource* _UAVResource : m_ShaderRequiredResources->GetUAVResources())
		{
			_ChangeResourceState(_UAVResource, TE_RESOURCE_STATES::UNORDERED_ACCESS);
		}


		if (mRootArguments == nullptr || _NewShaderClass)
		{
			mRootArguments = DirectX12Manager::GetInstance()->GetRootArguments(m_ShaderClassIDX, m_FrameIndex);
		}

		for (auto& _Table : mRootArguments->Tables)
		{
			mD3D12CommandList->SetGraphicsRootDescriptorTable(_Table.ParameterIndex, _Table.GPUDescriptorHandle);
		}

	}

	void DirectX12CommandList::_SetRootSignatureGraphics(bool _NewShaderClass)
	{
		if (mRootSignature == nullptr || _NewShaderClass)
		{
			mRootSignature = DirectX12Manager::GetInstance()->GetD3D12RootSignature(m_ShaderClassIDX);
		}
		mD3D12CommandList->SetGraphicsRootSignature(mRootSignature);
	}

	void DirectX12CommandList::_SetRootArgumentsCompute(bool _NewShaderClass)
	{
		if (m_ShaderRequiredResources == nullptr || _NewShaderClass)
		{
			m_ShaderRequiredResources = TE_INSTANCE_SHADERMANAGER->GetShaderRequiredResources(m_ShaderClassIDX);
		}


		for (GraphicResource* _CBVResource : m_ShaderRequiredResources->GetCBVResources())
		{
			if (_CBVResource->GetState() != TE_RESOURCE_STATES::GENERIC_READ0)
				_ChangeResourceState(_CBVResource, TE_RESOURCE_STATES::VERTEX_AND_CONSTANT_BUFFER);
		}

		for (GraphicResource* _SRVResource : m_ShaderRequiredResources->GetSRVResources())
		{
			_ChangeResourceState(_SRVResource, TE_RESOURCE_STATES::NON_PIXEL_SHADER_RESOURCE);
		}

		for (GraphicResource* _UAVResource : m_ShaderRequiredResources->GetUAVResources())
		{
			_ChangeResourceState(_UAVResource, TE_RESOURCE_STATES::UNORDERED_ACCESS);
		}


		if (mRootArguments == nullptr || _NewShaderClass)
		{
			mRootArguments = DirectX12Manager::GetInstance()->GetRootArguments(m_ShaderClassIDX, m_FrameIndex);
		}

		for (auto& _Table : mRootArguments->Tables)
		{
			mD3D12CommandList->SetComputeRootDescriptorTable(_Table.ParameterIndex, _Table.GPUDescriptorHandle);
		}
	}

	void DirectX12CommandList::_SetRootSignatureCompute(bool _NewShaderClass)
	{
		if (mRootSignature == nullptr || _NewShaderClass)
		{
			mRootSignature = DirectX12Manager::GetInstance()->GetD3D12RootSignature(m_ShaderClassIDX);
		}
		mD3D12CommandList->SetComputeRootSignature(mRootSignature);
	}

	void DirectX12CommandList::_ChangeResourceState(GraphicResource* resource, TE_RESOURCE_STATES newState)
	{
		auto _OldState = resource->GetState();

		if (_OldState == newState)
			return;

		uint32_t _ResourceIndex = resource->GetResourceIndex();

		if ((resource->GetUsage() & TE_RESOURCE_USAGE_CONSTANTBUFFER) && (_OldState != TE_RESOURCE_STATES::GENERIC_READ0))
		{
			_ResourceIndex += m_FrameIndex;
		}

		/*if (_OldState == TE_RESOURCE_STATES::UNORDERED_ACCESS)
		{
			_QueueBarrierUAV(m_BufferManager->m_Resources[_ResourceIndex].Get());
		}*/

		_QueueBarrierTransition(m_BufferManager->m_Resources[_ResourceIndex].Get(), DX12_GET_STATE(_OldState), DX12_GET_STATE(newState));

		resource->SetState(newState);
	}

	void DirectX12CommandList::_ChangeResourceState(DirectX12SwapChain* swapChain, TE_RESOURCE_STATES newState)
	{

		auto _CurrentState = swapChain->GetBackBufferState();

		if (_CurrentState != static_cast<D3D12_RESOURCE_STATES>(newState))
		{
			_QueueBarrierTransition(swapChain->GetBackBufferResource()
				, _CurrentState
				, static_cast<D3D12_RESOURCE_STATES>(newState));

			swapChain->SetBackBufferState(static_cast<D3D12_RESOURCE_STATES>(newState));
		}
	}

	void DirectX12CommandList::SetVertexBuffer(VertexBufferBase* vertexBuffer)
	{
		const auto vbID = vertexBuffer->GetID();
		if (vbID != m_AssignedVertexBufferID)
		{
			auto vbss = vertexBuffer->GetVertexBufferStreams();
			for (auto vbs : vbss)
			{
				_ChangeResourceState(vbs, TE_RESOURCE_STATES::VERTEX_AND_CONSTANT_BUFFER);
			}

			mD3D12CommandList->IASetVertexBuffers(0, vertexBuffer->GetVertexStreamNum(), &m_BufferManager->m_VertexBufferViews[vertexBuffer->GetViewIndex()]);
			m_AssignedVertexBufferID = vbID;
		}

	}

	void DirectX12CommandList::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
		const auto ibID = indexBuffer->GetID();
		if (m_AssignedIndexBufferID != ibID)
		{
			_ChangeResourceState(indexBuffer, TE_RESOURCE_STATES::INDEX_BUFFER);

			mD3D12CommandList->IASetIndexBuffer(&m_BufferManager->m_IndexBufferViews[indexBuffer->GetViewIndex()]);
			m_AssignedIndexBufferID = ibID;
		}
	}

	void DirectX12CommandList::DrawIndexed(uint32_t indexNum, uint32_t indexOffset, uint32_t vertexOffset)
	{
		mD3D12CommandList->DrawIndexedInstanced(indexNum, 1, indexOffset, vertexOffset, 0);
	}

	void DirectX12CommandList::Draw(uint32_t vertexNum, uint32_t vertexOffset)
	{
		mD3D12CommandList->DrawInstanced(vertexNum, 1, vertexOffset, 0);
	}

	void DirectX12CommandList::Dispatch(uint32_t GroupNumX, uint32_t GroupNumY, uint32_t GroupNumZ)
	{
		mD3D12CommandList->Dispatch(GroupNumX, GroupNumY, GroupNumZ);
	}

	void DirectX12CommandList::Release()
	{
		mD3D12CommandList->Release();
		mCommandAllocator.Release();

		_ResetContainers();
	}

	void DirectX12CommandList::ClearRenderTarget(const RenderTargetView& RTV)
	{
		_ChangeResourceState(RTV.Resource, TE_RESOURCE_STATES::RENDER_TARGET);

		m_QueueClearRT.emplace_back(m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex), RTV.Resource->GetClearValues(), D3D12_RECT{ static_cast<long>(0.0), static_cast <long>(0.0), static_cast<long>(RTV.Resource->GetWidth()), static_cast<long>(RTV.Resource->GetHeight()) });
	}

	void DirectX12CommandList::ClearRenderTarget(SwapChain* swapChain, const RenderTargetView& RTV)
	{
		_ChangeResourceState(static_cast<DirectX12SwapChain*>(swapChain), TE_RESOURCE_STATES::RENDER_TARGET);

		m_QueueClearRT.emplace_back(m_BufferManager->m_DescHeapRTV.GetCPUHandle(RTV.ViewIndex + swapChain->GetCurrentFrameIndex()), swapChain->GetClearValues(), D3D12_RECT{ static_cast<long>(0.0), static_cast<long>(0.0), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()) });
	}

	void DirectX12CommandList::ClearDepthStencil(const DepthStencilView& DSV)
	{
		_ChangeResourceState(DSV.Resource, TE_RESOURCE_STATES::DEPTH_WRITE);

		m_QueueClearDS.emplace_back(m_BufferManager->m_DescHeapDSV.GetCPUHandle(DSV.ViewIndex), DSV.Resource->GetClearValues(), D3D12_RECT{ static_cast<long>(0.0), static_cast<long>(0.0), static_cast<long>(DSV.Resource->GetWidth()), static_cast<long>(DSV.Resource->GetHeight()) });
	}

	void DirectX12CommandList::Submit()
	{
		TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT->ExecuteCommandList(this);
	}


	void DirectX12CommandList::Commit()
	{

		if (m_ResourceBarriers.size() > 0)
		{
			mD3D12CommandList->ResourceBarrier(m_ResourceBarriers.size(), m_ResourceBarriers.data());
			m_ResourceBarriers.clear();
		}

		_UploadDefaultBuffers();

		if (m_RTVHandleNum > 0 || m_DSVHandleNum > 0)
		{
			mD3D12CommandList->OMSetRenderTargets(m_RTVHandleNum, m_RTVHandleNum > 0 ? m_RTVHandles : nullptr, false, m_DSVHandleNum > 0 ? &m_DSVHandle : NULL);
			m_RTVHandleNum = 0;
			m_DSVHandleNum = 0;
		}


		for (auto& c : m_QueueClearRT)
		{
			mD3D12CommandList->ClearRenderTargetView(c.RTV, &c.ClearValue.x, 1, &c.mRect);
		}
		m_QueueClearRT.clear();


		for (auto& c : m_QueueClearDS)
		{
			mD3D12CommandList->ClearDepthStencilView(c.DSV, static_cast<D3D12_CLEAR_FLAGS>(c.ClearValue.flags), c.ClearValue.depthValue, c.ClearValue.stencilValue, 1, &c.mRect);
		}
		m_QueueClearDS.clear();

		for (auto& _BindResource : mBindPendingGraphicsSRVs)
		{
			mD3D12CommandList->SetGraphicsRootShaderResourceView(_BindResource.mRootParamterIndex, _BindResource.mResource->GetGPUVirtualAddress());
		}
		mBindPendingGraphicsSRVs.clear();

		for (auto& _BindResource : mBindPendingGraphicsCBVs)
		{
			mD3D12CommandList->SetGraphicsRootConstantBufferView(_BindResource.mRootParamterIndex, _BindResource.mResource->GetGPUVirtualAddress());
		}
		mBindPendingGraphicsCBVs.clear();

		for (auto& _BindResource : mBindPendingGraphicsUAVs)
		{
			mD3D12CommandList->SetGraphicsRootUnorderedAccessView(_BindResource.mRootParamterIndex, _BindResource.mResource->GetGPUVirtualAddress());
		}
		mBindPendingGraphicsUAVs.clear();

		for (auto& _BindResource : mBindPendingComputeSRVs)
		{
			mD3D12CommandList->SetComputeRootShaderResourceView(_BindResource.mRootParamterIndex, _BindResource.mResource->GetGPUVirtualAddress());
		}
		mBindPendingComputeSRVs.clear();

		for (auto& _BindResource : mBindPendingComputeCBVs)
		{
			mD3D12CommandList->SetComputeRootConstantBufferView(_BindResource.mRootParamterIndex, _BindResource.mResource->GetGPUVirtualAddress());
		}
		mBindPendingComputeCBVs.clear();

		for (auto& _BindResource : mBindPendingComputeUAVs)
		{
			mD3D12CommandList->SetComputeRootUnorderedAccessView(_BindResource.mRootParamterIndex, _BindResource.mResource->GetGPUVirtualAddress());
		}
		mBindPendingComputeUAVs.clear();

		for (auto& _ResolvePendings : m_QueueResolveResource)
		{
			mD3D12CommandList->ResolveSubresource(_ResolvePendings.mDestinationResource, 0, _ResolvePendings.mSourceResource, 0, static_cast<DXGI_FORMAT>(_ResolvePendings.mFormat));
		}
		m_QueueResolveResource.clear();

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
			mD3D12CommandList->ResourceBarrier(1, &barrier);
			sc.SetBackBufferState(D3D12_RESOURCE_STATE_PRESENT);

			Submit();
		}

		sc.Present();
	}

	void DirectX12CommandList::SetViewport(const Viewport* viewport, const ViewRect* rect)
	{
		mD3D12CommandList->RSSetViewports(1, reinterpret_cast<const D3D12_VIEWPORT*>(viewport));
		mD3D12CommandList->RSSetScissorRects(1, reinterpret_cast<const D3D12_RECT*>(rect));
	}

	bool DirectX12CommandList::IsRunning()
	{
		return mCommandAllocator.IsRunning();
	}

	void DirectX12CommandList::WaitToFinish()
	{
		mCommandAllocator.WaitToFinish();
	}

	void DirectX12CommandList::_ResetContainers()
	{
		m_RTVHandleNum = 0;
		m_DSVHandleNum = 0;

		m_AssignedVertexBufferID = -1;
		m_AssignedIndexBufferID = -1;
		/*m_SRVHandles_CB.clear();
		m_SRVHandles_Texture.clear();*/

		m_ResourceBarriers.clear();

		m_QueueClearDS.clear();
		m_QueueClearRT.clear();

	}

	void DirectX12CommandList::_SetDescriptorHeapSRV()
	{

		auto descHeapSRV = m_BufferManager->m_DescHeapSRV.GetDescriptorHeap();
		mD3D12CommandList->SetDescriptorHeaps(1, &descHeapSRV);

	}

	void DirectX12CommandList::UploadData(Buffer* buffer, const void* data, size_t sizeInByte)
	{
		_ChangeResourceState(buffer, TE_RESOURCE_STATES::COPY_DEST);

		m_QueueCopyDefaultResource.emplace_back(m_BufferManager->m_Resources[buffer->GetResourceIndex()].Get(), data, sizeInByte);

		m_CopyQueueRequiredSize += buffer->GetRequiredSize();

	}

	void DirectX12CommandList::SetDirectConstantGraphics(ConstantBufferDirectBase* cb)
	{
		auto& directConstant = mRootArguments->DircectConstants.find(cb->GetIDX())->second;
		mD3D12CommandList->SetGraphicsRoot32BitConstants(directConstant.ParameterIndex, cb->Get32BitNum(), cb->GetDataPtr(), 0);
	}

	void DirectX12CommandList::SetDirectConstantCompute(ConstantBufferDirectBase* cb)
	{
		auto& directConstant = mRootArguments->DircectConstants.find(cb->GetIDX())->second;
		mD3D12CommandList->SetComputeRoot32BitConstants(directConstant.ParameterIndex, cb->Get32BitNum(), cb->GetDataPtr(), 0);
	}

	void DirectX12CommandList::SetDirectViewSRVGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot)
	{
		_ChangeResourceState(_GraphicResource, TE_RESOURCE_STATES::PIXEL_SHADER_RESOURCE);

		auto _ParamterIndex = mRootArguments->DescriptorSRV.find(_ShaderRegisterSlot)->second.mParameterIndex;

		mBindPendingGraphicsSRVs.emplace_back(TE_INSTANCE_API_DX12_BUFFERMANAGER->GetResource(_GraphicResource), _ParamterIndex);
	}

	void DirectX12CommandList::SetDirectViewCBVGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot)
	{
		_ChangeResourceState(_GraphicResource, TE_RESOURCE_STATES::VERTEX_AND_CONSTANT_BUFFER);

		auto _ParamterIndex = mRootArguments->DescriptorCBV.find(_ShaderRegisterSlot)->second.mParameterIndex;

		mBindPendingGraphicsCBVs.emplace_back(TE_INSTANCE_API_DX12_BUFFERMANAGER->GetResource(_GraphicResource), _ParamterIndex);
	}

	void DirectX12CommandList::SetDirectViewUAVGraphics(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot)
	{
		_ChangeResourceState(_GraphicResource, TE_RESOURCE_STATES::UNORDERED_ACCESS);

		auto _ParamterIndex = mRootArguments->DescriptorUAV.find(_ShaderRegisterSlot)->second.mParameterIndex;

		mBindPendingGraphicsUAVs.emplace_back(TE_INSTANCE_API_DX12_BUFFERMANAGER->GetResource(_GraphicResource), _ParamterIndex);
	}

	void DirectX12CommandList::SetDirectViewSRVCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot)
	{
		_ChangeResourceState(_GraphicResource, TE_RESOURCE_STATES::NON_PIXEL_SHADER_RESOURCE);

		auto _ParamterIndex = mRootArguments->DescriptorSRV.find(_ShaderRegisterSlot)->second.mParameterIndex;

		mBindPendingComputeSRVs.emplace_back(TE_INSTANCE_API_DX12_BUFFERMANAGER->GetResource(_GraphicResource), _ParamterIndex);
	}

	void DirectX12CommandList::SetDirectViewCBVCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot)
	{
		_ChangeResourceState(_GraphicResource, TE_RESOURCE_STATES::VERTEX_AND_CONSTANT_BUFFER);

		auto _ParamterIndex = mRootArguments->DescriptorCBV.find(_ShaderRegisterSlot)->second.mParameterIndex;

		mBindPendingComputeCBVs.emplace_back(TE_INSTANCE_API_DX12_BUFFERMANAGER->GetResource(_GraphicResource), _ParamterIndex);
	}

	void DirectX12CommandList::SetDirectViewUAVCompute(GraphicResource* _GraphicResource, uint32_t _ShaderRegisterSlot)
	{
		_ChangeResourceState(_GraphicResource, TE_RESOURCE_STATES::UNORDERED_ACCESS);

		auto _ParamterIndex = mRootArguments->DescriptorUAV.find(_ShaderRegisterSlot)->second.mParameterIndex;

		mBindPendingComputeUAVs.emplace_back(TE_INSTANCE_API_DX12_BUFFERMANAGER->GetResource(_GraphicResource), _ParamterIndex);
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

			auto uplaodedSize = UpdateSubresources<1>(mD3D12CommandList.Get(), c.D3D12Resource, m_IntermediateResource.Get(), offset, 0, 1, &data);
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