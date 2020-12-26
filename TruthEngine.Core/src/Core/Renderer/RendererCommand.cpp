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

namespace TruthEngine::Core
{

	RendererCommand::RendererCommand() = default;

	void RendererCommand::Init(TE_IDX_RENDERPASS renderPassIDX, TE_IDX_SHADERCLASS shaderClassIDX, uint32_t ParallelCommandsNum, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<ShaderManager> shaderManager)
	{
		m_BufferManager = bufferManager ? bufferManager : TE_INSTANCE_BUFFERMANAGER;
		m_ShaderManager = shaderManager ? shaderManager : TE_INSTANCE_SHADERMANAGER;

		//m_CommandLists = std::vector<std::shared_ptr<CommandList>>(ParallelCommandsNum, CommandList::Factory(&TE_INSTANCE_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, m_BufferManager, shaderManager));

		for (uint32_t i = 0; i < ParallelCommandsNum; ++i)
		{
			m_CommandLists.push_back(CommandList::Factory(TE_INSTANCE_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, m_BufferManager, shaderManager, renderPassIDX, shaderClassIDX));
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

	void RendererCommand::SetPipeline(Pipeline* pipeline, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetPipeline(pipeline);
	}

	void RendererCommand::SetRenderTarget(const RenderTargetView RTV, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetRenderTarget(RTV);
	}

	void RendererCommand::SetRenderTarget(SwapChain* swapChain, const RenderTargetView RTV, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetRenderTarget(swapChain, RTV);
	}

	void RendererCommand::SetDepthStencil(const DepthStencilView DSV, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetDepthStencil(DSV);
	}

	void RendererCommand::SetShaderResource(const ShaderResourceView SRV, uint32_t registerIndex, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetShaderResource(SRV, registerIndex);
	}

	void RendererCommand::SetConstantBuffer(const ConstantBufferView CBV, uint32_t registerIndex, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetConstantBuffer(CBV, registerIndex);
	}

	void RendererCommand::UploadData(ConstantBufferUploadBase* cb, uint32_t cmdListIndex)
	{
	}

	void RendererCommand::UploadData(Buffer* buffer, void* data, size_t sizeInByte, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->UploadData(buffer, data, sizeInByte);
	}

	void RendererCommand::UploadData(VertexBufferBase* vertexBuffer, uint32_t cmdListIndex)
	{
		auto cmdList = m_CommandLists[cmdListIndex];

		for (auto vbs : vertexBuffer->GetVertexBufferStreams())
		{
			cmdList->UploadData(vbs, vbs->GetDataPtr(), vbs->GetBufferSize());
		}
	}

	void RendererCommand::UploadData(IndexBuffer* indexBuffer, uint32_t cmdListIndex)
	{
		m_CommandLists[cmdListIndex]->UploadData(indexBuffer, indexBuffer->GetDataPtr(), indexBuffer->GetBufferSize());
	}

	void RendererCommand::SetVertexBuffer(VertexBufferBase* vertexBuffer, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetVertexBuffer(vertexBuffer);
	}

	void RendererCommand::SetIndexBuffer(IndexBuffer* indexBuffer, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetIndexBuffer(indexBuffer);
	}

	void RendererCommand::DrawIndexed(Mesh* mesh, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->SetVertexBuffer(mesh->GetVertexBuffer());
		m_CommandLists[cmdListIndex]->SetIndexBuffer(mesh->GetIndexBuffer());

		m_CommandLists[cmdListIndex]->DrawIndexed(mesh->GetIndexNum(), mesh->GetIndexOffset(), mesh->GetVertexOffset());
	}

	void RendererCommand::Draw(Mesh* mesh, uint32_t cmdListIndex /*= 0*/)
	{

	}

	void RendererCommand::ClearRenderTarget(const RenderTargetView RTV, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->ClearRenderTarget(RTV);
	}

	void RendererCommand::ClearRenderTarget(const SwapChain* swapChain, const RenderTargetView RTV, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->ClearRenderTarget(swapChain, RTV);
	}

	void RendererCommand::ClearDepthStencil(const DepthStencilView DSV, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->ClearDepthStencil(DSV);
	}

	void RendererCommand::Resize(TextureRenderTarget* texture, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV)
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

	void RendererCommand::Resize(TE_IDX_RENDERTARGET idx, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV)
	{
		auto rt = m_BufferManager->GetRenderTarget(idx);
		Resize(rt, width, height, RTV, SRV);
	}

	void RendererCommand::Resize(TextureDepthStencil* texture, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV)
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

	void RendererCommand::Resize(TE_IDX_DEPTHSTENCIL idx, uint32_t width, uint32_t height, DepthStencilView* DSV, ShaderResourceView* SRV)
	{
		auto ds = m_BufferManager->GetDepthStencil(idx);
		Resize(ds, width, height, DSV, SRV);
	}

	void RendererCommand::Resize(SwapChain* swapChain, uint32_t width, uint32_t height, RenderTargetView* RTV, ShaderResourceView* SRV)
	{
		WaitForGPU();

		swapChain->Resize(width, height, TE_INSTANCE_APPLICATION->GetFramesInFlightNum());

		if (RTV != nullptr)
		{
			m_BufferManager->CreateRenderTargetView(swapChain, RTV);
		}
		if (SRV != nullptr)
		{
			//m_BufferManager->CreateShaderResourceView()
		}
	}


	void RendererCommand::EndAndPresent(uint32_t cmdListIndex /*= 0*/)
	{
		m_LastCommadListIndex = cmdListIndex;

		m_CommandLists[cmdListIndex]->Present();
	}

	void RendererCommand::End(uint32_t cmdListIndex /*= 0*/)
	{
		m_LastCommadListIndex = cmdListIndex;

		m_CommandLists[cmdListIndex]->Commit();
		m_CommandLists[cmdListIndex]->Submit();
	}

	void RendererCommand::Begin(uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->Reset();
	}

	void RendererCommand::Begin(Pipeline* pipeline, uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->Reset(pipeline);
	}

	void RendererCommand::SetViewPort(Viewport* viewport, ViewRect* rect, uint32_t cmdListIndex /* =0 */)
	{
		m_CommandLists[cmdListIndex]->SetViewport(viewport, rect);
	}


	TruthEngine::Core::TextureRenderTarget* RendererCommand::CreateRenderTarget(TE_IDX_RENDERTARGET idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTarget(idx, width, height, format, clearValue, useAsShaderResource);
	}


	TruthEngine::Core::TextureDepthStencil* RendererCommand::CreateDepthStencil(TE_IDX_DEPTHSTENCIL idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateDepthStencil(idx, width, height, format, clearValue, useAsShaderResource);
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

	void RendererCommand::ReleaseResource(TE_IDX_RENDERTARGET idx)
	{
		WaitForGPU();

		auto rt = m_BufferManager->GetRenderTarget(idx);

		m_BufferManager->ReleaseResource(rt);
	}

	void RendererCommand::ReleaseResource(TE_IDX_DEPTHSTENCIL idx)
	{
		WaitForGPU();
		
		auto ds = m_BufferManager->GetDepthStencil(idx);

		m_BufferManager->ReleaseResource(ds);
	}

	void RendererCommand::CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* DSV)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateDepthStencilView(DS, DSV);
	}

	void RendererCommand::CreateDepthStencilView(TE_IDX_DEPTHSTENCIL idx, DepthStencilView* DSV)
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

	void RendererCommand::CreateRenderTargetView(TE_IDX_RENDERTARGET idx, RenderTargetView* RTV)
	{
		auto rt = TE_INSTANCE_BUFFERMANAGER->GetRenderTarget(idx);
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTargetView(rt, RTV);
	}

	void RendererCommand::CreateShaderResourceView(Texture* textures[], uint32_t textureNum, ShaderResourceView* SRV)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateShaderResourceView(textures, textureNum, SRV);
	}

	void RendererCommand::CreateShaderResourceView(Texture* texture, ShaderResourceView* SRV)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateShaderResourceView(texture, SRV);
	}

	void RendererCommand::CreateConstantBufferView(TE_IDX_CONSTANTBUFFER idx, ConstantBufferView* CBV)
	{
		auto cb = TE_INSTANCE_BUFFERMANAGER->GetConstantBufferUpload(idx);
		return TE_INSTANCE_BUFFERMANAGER->CreateConstantBufferView(cb, CBV);
	}

	TE_RESULT RendererCommand::CreateVertexBuffer(VertexBufferBase* vb)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateVertexBuffer(vb);
	}

	TE_RESULT RendererCommand::CreateIndexBuffer(IndexBuffer* ib)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateIndexBuffer(ib);
	}

	bool RendererCommand::IsRunning(uint32_t cmdListIndex)
	{
		return m_CommandLists[cmdListIndex]->IsRunning();
	}

	void RendererCommand::WaitForGPU()
	{
		TE_INSTANCE_GRAPHICDEVICE->WaitForGPU();
	}

}