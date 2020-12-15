#include "pch.h"
#include "RendererCommand.h"

#include "GraphicDevice.h"
#include "BufferManager.h"
#include "ShaderManager.h"
#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "CommandList.h"
#include "Viewport.h"

#include "Core/Entity/Model/Mesh.h"

#include "API/DX12/DX12CommandList.h"

namespace TruthEngine::Core
{

	RendererCommand::RendererCommand() = default;

	void RendererCommand::Init(uint32_t ParallelCommandsNum, std::shared_ptr<BufferManager> bufferManager, std::shared_ptr<ShaderManager> shaderManager)
	{
		m_BufferManager = bufferManager ? bufferManager : TE_INSTANCE_BUFFERMANAGER;
		m_ShaderManager = shaderManager ? shaderManager : TE_INSTANCE_SHADERMANAGER;

		//m_CommandLists = std::vector<std::shared_ptr<CommandList>>(ParallelCommandsNum, CommandList::Factory(&TE_INSTANCE_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, m_BufferManager, shaderManager));

		for (uint32_t i = 0; i < ParallelCommandsNum; ++i)
		{
			m_CommandLists.push_back(CommandList::Factory(&TE_INSTANCE_GRAPHICDEVICE, TE_RENDERER_COMMANDLIST_TYPE::DIRECT, m_BufferManager, shaderManager));
		}

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
		m_CommandLists[cmdListIndex]->SetDepthStencil(DSV);
	}

	void RendererCommand::EndAndPresent(uint32_t cmdListIndex /*= 0*/)
	{
		m_CommandLists[cmdListIndex]->Present();
	}

	void RendererCommand::End(uint32_t cmdListIndex /*= 0*/)
	{
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

	TE_RESULT RendererCommand::CreateResource(TextureRenderTarget* tRT)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateResource(tRT);
	}

	TE_RESULT RendererCommand::CreateResource(TextureDepthStencil* tDS)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateResource(tDS);
	}

	TE_RESULT RendererCommand::CreateResource(BufferUpload* cb)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateResource(cb);
	}

	TruthEngine::Core::DepthStencilView RendererCommand::CreateDepthStencilView(TextureDepthStencil* DS)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateDepthStencilView(DS);
	}

	TruthEngine::Core::RenderTargetView RendererCommand::CreateRenderTargetView(TextureRenderTarget* RT)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTargetView(RT);
	}

	TruthEngine::Core::RenderTargetView RendererCommand::CreateRenderTargetView(SwapChain* swapChain)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateRenderTargetView(swapChain);
	}

	TruthEngine::Core::ShaderResourceView RendererCommand::CreateShaderResourceView(Texture* textures[], uint32_t textureNum)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateShaderResourceView(textures, textureNum);
	}

	TruthEngine::Core::ShaderResourceView RendererCommand::CreateShaderResourceView(Texture* texture)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateShaderResourceView(texture);
	}

	TruthEngine::Core::ConstantBufferView RendererCommand::CreateConstantBufferView(Buffer* CB)
	{
		return TE_INSTANCE_BUFFERMANAGER->CreateConstantBufferView(CB);
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

}