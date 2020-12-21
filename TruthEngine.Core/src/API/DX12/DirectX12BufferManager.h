#pragma once
#include "Core/Renderer/BufferManager.h"

#include "DirectX12DescriptorHeap.h"


//namespace TruthEngine::Core
//{
//	class VertexBuffer;
//	class GraphicResource;
//	class TextureRenderTarget;
//	class TextureDepthStencil;
//	class ConstantBuffer;
//	class CommandList;
//}

namespace TruthEngine::API::DirectX12 {
	

	class DirectX12BufferManager : public Core::BufferManager
	{

	public:

		static std::shared_ptr<DirectX12BufferManager> GetInstance()
		{
			static std::shared_ptr<DirectX12BufferManager> s_Instance = std::make_shared<DirectX12BufferManager>();
			return s_Instance;
		}

		void Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum) override;

		//TE_RESULT CreateResource(Core::GraphicResource* graphicResource, void* clearValue) override;
		TE_RESULT CreateResource(Core::BufferUpload* buffer) override;

		Core::ConstantBufferUploadBase* GetConstantBufferUpload(TE_IDX_CONSTANTBUFFER cbIDX) override;

		TE_RESULT CreateVertexBuffer(Core::VertexBufferBase* vb) override;

		TE_RESULT CreateIndexBuffer(Core::IndexBuffer* ib) override;

		Core::RenderTargetView CreateRenderTargetView(Core::TextureRenderTarget* RT) override;

		Core::RenderTargetView CreateRenderTargetView(Core::SwapChain* swapChain) override;

		Core::DepthStencilView CreateDepthStencilView(Core::TextureDepthStencil* DS) override;

		Core::ShaderResourceView CreateShaderResourceView(Core::Texture* textures[], uint32_t textureNum) override;

		Core::ShaderResourceView CreateShaderResourceView(Core::Texture* texture) override;

		Core::ConstantBufferView CreateConstantBufferView(const TE_IDX_CONSTANTBUFFER constantBufferIDX) override;

		uint64_t GetRequiredSize(const Core::GraphicResource* graphicResource) const override;

		ID3D12Resource* GetResource(Core::GraphicResource* graphicResource);

	private:
		TE_RESULT CreateResource(Core::TextureRenderTarget* tRT) override;

		TE_RESULT CreateResource(Core::TextureDepthStencil* tDS) override;

		TE_RESULT CreateResource(Core::VertexBufferStreamBase* vb) override;

		TE_RESULT CreateResource(Core::IndexBuffer* ib) override;

		TE_RESULT ReleaseResource(Core::GraphicResource* resource) override;

	private:

		std::vector<COMPTR<ID3D12Resource>> m_Resources;

		DescriptorHeapSRV m_DescHeapSRV;

		DescriptorHeapRTV m_DescHeapRTV;

		DescriptorHeapDSV m_DescHeapDSV;

		D3D12_RECT m_Rect_FullScreen;

		std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferViews;

		std::vector<D3D12_INDEX_BUFFER_VIEW> m_IndexBufferViews;


		//
		// Friend Classes
		//
		friend class DirectX12CommandList;
		friend class DirectX12Manager;
	};

}

#define TE_INSTANCE_API_DX12_BUFFERMANAGER TruthEngine::API::DirectX12::DX12BufferManager::GetInstance()