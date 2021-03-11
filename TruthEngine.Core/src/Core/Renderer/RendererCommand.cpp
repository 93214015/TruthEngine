#include "pch.h"
#include "RendererCommand.h"

#include "Core/Application.h"

#include "GraphicDevice.h"
#include "BufferManager.h"
#include "ShaderManager.h"
#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "CommandList.h"
#include "Viewport.h"
#include "SwapChain.h"

#include "Core/Entity/Model/Mesh.h"

#include "API/DX12/DirectX12CommandList.h"

namespace TruthEngine
{

	RendererCommand::RendererCommand() = default;

	void RendererCommand::Init(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, BufferManager* bufferManager, ShaderManager* shaderManager)
	{
		m_BufferManager = bufferManager ? bufferManager : TE_INSTANCE_BUFFERMANAGER;
		m_ShaderManager = shaderManager ? shaderManager : TE_INSTANCE_SHADERMANAGER;

		//m_CommandLists = std::vector<std::shared_ptr<CommandList>>(ParallelCommandsNum, CommandList::Factory(&TE_INSTANCE_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, m_BufferManager, shaderManager));

		auto frameOnTheFlyNum = TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum();

		m_CommandLists.reserve(frameOnTheFlyNum);

		for (uint32_t i = 0; i < frameOnTheFlyNum; ++i)
		{
			m_CommandLists.push_back(CommandList::Factory(TE_INSTANCE_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, m_BufferManager, shaderManager, renderPassIDX, shaderClassIDX, i));
		}

	}

	void RendererCommand::Release()
	{
		for (auto cm : m_CommandLists)
		{
			cm->Release();
		}

		m_CommandLists.clear();
	}

	void RendererCommand::SetPipelineGraphics(PipelineGraphics* pipeline)
	{
		m_CurrentCommandList->SetPipelineGraphics(pipeline);
	}

	void RendererCommand::SetPipelineCompute(PipelineCompute* pipeline)
	{
		m_CurrentCommandList->SetPipelineCompute(pipeline);
	}

	void RendererCommand::SetRenderTarget(const RenderTargetView RTV)
	{
		m_CurrentCommandList->SetRenderTarget(RTV);
	}

	void RendererCommand::SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV)
	{
		m_CurrentCommandList->SetRenderTarget(swapChain, RTV);
	}

	void RendererCommand::SetDepthStencil(const DepthStencilView DSV)
	{
		m_CurrentCommandList->SetDepthStencil(DSV);
	}

	void RendererCommand::SetDirectConstantGraphics(ConstantBufferDirectBase* cb)
	{
		m_CurrentCommandList->SetDirectConstantGraphics(cb);
	}

	void RendererCommand::SetDirectConstantCompute(ConstantBufferDirectBase* cb)
	{
		m_CurrentCommandList->SetDirectConstantCompute(cb);
	}

	/*void RendererCommand::SetShaderResource(const ShaderResourceView SRV, uint32_t registerIndex)
	{
		m_CurrentCommandList->SetShaderResource(SRV, registerIndex);
	}

	void RendererCommand::SetConstantBuffer(const ConstantBufferView CBV, uint32_t registerIndex)
	{
		m_CurrentCommandList->SetConstantBuffer(CBV, registerIndex);
	}*/

	void RendererCommand::UploadData(ConstantBufferUploadBase* cb)
	{
		
	}

	void RendererCommand::UploadData(Buffer* buffer, void* data, size_t sizeInByte)
	{
		m_CurrentCommandList->UploadData(buffer, data, sizeInByte);
	}

	void RendererCommand::UploadData(VertexBufferBase* vertexBuffer)
	{
		auto cmdList = m_CurrentCommandList;

		for (auto vbs : vertexBuffer->GetVertexBufferStreams())
		{
			cmdList->UploadData(vbs, vbs->GetDataPtr(), vbs->GetBufferSize());
		}
	}

	void RendererCommand::UploadData(IndexBuffer* indexBuffer)
	{
		m_CurrentCommandList->UploadData(indexBuffer, indexBuffer->GetDataPtr(), indexBuffer->GetBufferSize());
	}

	void RendererCommand::SetVertexBuffer(VertexBufferBase* vertexBuffer)
	{
		m_CurrentCommandList->SetVertexBuffer(vertexBuffer);
	}

	void RendererCommand::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
		m_CurrentCommandList->SetIndexBuffer(indexBuffer);
	}

	void RendererCommand::DrawIndexed(const Mesh* mesh)
	{
		m_CurrentCommandList->SetVertexBuffer(mesh->GetVertexBuffer());
		m_CurrentCommandList->SetIndexBuffer(mesh->GetIndexBuffer());

		m_CurrentCommandList->DrawIndexed(mesh->GetIndexNum(), mesh->GetIndexOffset(), mesh->GetVertexOffset());
	}

	void RendererCommand::Draw(Mesh* mesh)
	{
	}

	void RendererCommand::Draw(uint32_t _VertexNum, uint32_t _VertexOffset)
	{
		m_CurrentCommandList->Draw(_VertexNum, _VertexOffset);
	}

	void RendererCommand::Dispatch(uint32_t GroupNumX, uint32_t GroupNumY, uint32_t GroupNumZ)
	{
		m_CurrentCommandList->Dispatch(GroupNumX, GroupNumY, GroupNumZ);
	}

	void RendererCommand::ClearRenderTarget(const RenderTargetView RTV)
	{
		m_CurrentCommandList->ClearRenderTarget(RTV);
	}

	void RendererCommand::ClearRenderTarget(const SwapChain* swapChain, const RenderTargetView RTV)
	{
		m_CurrentCommandList->ClearRenderTarget(swapChain, RTV);
	}

	void RendererCommand::ClearDepthStencil(const DepthStencilView DSV)
	{
		m_CurrentCommandList->ClearDepthStencil(DSV);
	}

	void RendererCommand::ResizeRenderTarget(TextureRenderTarget* texture, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV)
	{
		WaitForGPU();

		texture->Resize(width, height);
		m_BufferManager->CreateResource(texture);

		if (RTV != nullptr)
		{
			m_BufferManager->CreateRenderTargetView(texture, RTV);
		}
		if (SRV != nullptr)
		{
			m_BufferManager->CreateShaderResourceView(texture, SRV);
		}
	}

	void RendererCommand::ResizeRenderTarget(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV)
	{
		auto rt = m_BufferManager->GetRenderTarget(idx);
		ResizeRenderTarget(rt, width, height, RTV, SRV);
	}

	void RendererCommand::ResizeDepthStencil(TextureDepthStencil* texture, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV)
	{
		WaitForGPU();

		texture->Resize(width, height);
		m_BufferManager->CreateResource(texture);

		if (DSV != nullptr)
		{
			m_BufferManager->CreateDepthStencilView(texture, DSV);
		}
		if (SRV != nullptr)
		{
			m_BufferManager->CreateShaderResourceView(texture, SRV);
		}
	}

	void RendererCommand::ResizeDepthStencil(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV)
	{
		auto ds = m_BufferManager->GetDepthStencil(idx);
		ResizeDepthStencil(ds, width, height, DSV, SRV);
	}

	void RendererCommand::ResizeSwapChain(SwapChain* swapChain, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV)
	{
		WaitForGPU();

		swapChain->Resize(width, height, TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum());

		if (RTV != nullptr)
		{
			m_BufferManager->CreateRenderTargetView(swapChain, RTV);
		}
		if (SRV != nullptr)
		{
			//m_BufferManager->CreateShaderResourceView()
		}
	}


	void RendererCommand::EndAndPresent()
	{
		m_CurrentCommandList->Present();
	}

	void RendererCommand::End()
	{
		m_CurrentCommandList->Commit();
		m_CurrentCommandList->Submit();
	}

	void RendererCommand::BeginCompute()
	{
		m_CurrentCommandList = m_CommandLists[TE_INSTANCE_APPLICATION->GetCurrentFrameIndex()].get();

		m_CurrentCommandList->ResetCompute();

		m_CurrentCommandList->ExecuteUpdateTasks();
	}

	void RendererCommand::BeginGraphics()
	{
		m_CurrentCommandList = m_CommandLists[TE_INSTANCE_APPLICATION->GetCurrentFrameIndex()].get();

		m_CurrentCommandList->ResetGraphics();

		m_CurrentCommandList->ExecuteUpdateTasks();

	}

	void RendererCommand::BeginGraphics(PipelineGraphics* pipeline)
	{
		m_CurrentCommandList = m_CommandLists[TE_INSTANCE_APPLICATION->GetCurrentFrameIndex()].get();

		m_CurrentCommandList->ResetGraphics(pipeline);

		m_CurrentCommandList->ExecuteUpdateTasks();
	}

	void RendererCommand::SetViewPort(Viewport* viewport, ViewRect* rect /* =0 */)
	{
		m_CurrentCommandList->SetViewport(viewport, rect);
	}


	TruthEngine::TextureRenderTarget* RendererCommand::CreateRenderTarget(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTarget(idx, width, height, format, clearValue, useAsShaderResource, enbaleMSAA);
	}


	TruthEngine::TextureDepthStencil* RendererCommand::CreateDepthStencil(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource, bool enbaleMSAA)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateDepthStencil(idx, width, height, format, clearValue, useAsShaderResource, enbaleMSAA);
	}


	TruthEngine::TextureCubeMap* RendererCommand::CreateTextureCubeMap(TE_IDX_GRESOURCES idx, const char* filePath)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateTextureCube(idx, filePath);
	}

	Buffer* RendererCommand::CreateBufferStructuredRW(TE_IDX_GRESOURCES _IDX, uint32_t _ElementSizeInByte, uint32_t _ElementNum, bool _IsByteAddressBuffer)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateBufferStructuredRW(_IDX, _ElementSizeInByte, _ElementNum, _IsByteAddressBuffer);
	}

	TE_RESULT RendererCommand::CreateResource(BufferUpload* cb)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateResource(cb);
	}

	void RendererCommand::ReleaseResource(GraphicResource* graphicResource)
	{
		WaitForGPU();

		m_BufferManager->ReleaseResource(graphicResource);
	}

	void RendererCommand::ReleaseResource(TE_IDX_GRESOURCES idx)
	{
		WaitForGPU();

		auto rt = m_BufferManager->GetTexture(idx);

		m_BufferManager->ReleaseResource(rt);
	}


	void RendererCommand::CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* DSV)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateDepthStencilView(DS, DSV);
	}

	void RendererCommand::CreateDepthStencilView(TE_IDX_GRESOURCES idx, DepthStencilView* DSV)
	{
		auto ds = TE_INSTANCE_BUFFERMANAGER->GetDepthStencil(idx);
		return TE_INSTANCE_BUFFERMANAGER->CreateDepthStencilView(ds, DSV);
	}

	void RendererCommand::CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTargetView(RT, RTV);
	}

	void RendererCommand::CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* RTV)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTargetView(swapChain, RTV);
	}

	void RendererCommand::CreateRenderTargetView(TE_IDX_GRESOURCES idx, RenderTargetView* RTV)
	{
		auto rt = TE_INSTANCE_BUFFERMANAGER->GetRenderTarget(idx);
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTargetView(rt, RTV);
	}

	void RendererCommand::CreateShaderResourceView(Texture* texture, ShaderResourceView* SRV)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateShaderResourceView(texture, SRV);
	}

	void RendererCommand::CreateConstantBufferView(TE_IDX_GRESOURCES idx, ConstantBufferView* CBV)
	{
		auto cb = TE_INSTANCE_BUFFERMANAGER->GetConstantBufferUpload(idx);
		return TE_INSTANCE_BUFFERMANAGER->CreateConstantBufferView(cb, CBV, TE_INSTANCE_APPLICATION->GetCurrentFrameIndex());
	}

	TE_RESULT RendererCommand::CreateVertexBuffer(VertexBufferBase* vb)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateVertexBuffer(vb);
	}

	TE_RESULT RendererCommand::CreateIndexBuffer(IndexBuffer* ib)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateIndexBuffer(ib);
	}

	bool RendererCommand::IsRunning()
	{
		return m_CurrentCommandList->IsRunning();
	}

	void RendererCommand::WaitForGPU()
	{
		TE_INSTANCE_GRAPHICDEVICE->WaitForGPU();
	}

	void RendererCommand::AddUpdateTask(const std::function<void()>& task)
	{
		for (auto commandList : m_CommandLists)
			commandList->AddUpdateTask(task);
	}

}