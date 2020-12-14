#pragma once
#include "Core/Renderer/BufferManager.h"

#include "DescriptorHeap.h"


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
	

	class DX12BufferManager : public Core::BufferManager
	{

	public:

		static std::shared_ptr<DX12BufferManager> GetInstance()
		{
			static std::shared_ptr<DX12BufferManager> s_Instance = std::make_shared<DX12BufferManager>();
			return s_Instance;
		}

		void Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum) override;

		//TE_RESULT CreateResource(Core::GraphicResource* graphicResource, void* clearValue) override;
		TE_RESULT CreateResource(Core::TextureRenderTarget* tRT) override;
		TE_RESULT CreateResource(Core::TextureDepthStencil* tDS) override;
		TE_RESULT CreateResource(Core::BufferUpload* buffer) override;

		TE_RESULT CreateVertexBuffer(Core::VertexBufferBase* vb) override;
		TE_RESULT CreateIndexBuffer(Core::IndexBuffer* ib) override;


		Core::RenderTargetView CreateRenderTargetView(Core::TextureRenderTarget* RT) override;
		Core::RenderTargetView CreateRenderTargetView(Core::SwapChain* swapChain) override;


		Core::DepthStencilView CreateDepthStencilView(Core::TextureDepthStencil* DS) override;


		Core::ShaderResourceView CreateShaderResourceView(Core::Texture* textures[], uint32_t textureNum) override;
		Core::ShaderResourceView CreateShaderResourceView(Core::Texture* texture) override;


		Core::ConstantBufferView CreateConstantBufferView(Core::Buffer* CB) override;


		size_t GetAllocatedSizeOnGPU(Core::GraphicResource* graphicResource);

	private:
		TE_RESULT CreateResource(Core::VertexBufferStreamBase* vb) override;
		TE_RESULT CreateResource(Core::IndexBuffer* ib) override;

		TE_RESULT ReleaseResource(Core::GraphicResource* resource) override;

	private:

		std::vector<COMPTR<ID3D12Resource>> m_Resources;
		std::unordered_map<const char*, ID3D12Resource*> m_ResouceNameMap;

		DescriptorHeapSRV m_DescHeapSRV;
		DescriptorHeapRTV m_DescHeapRTV;
		DescriptorHeapDSV m_DescHeapDSV;

		D3D12_RECT m_Rect_FullScreen;

		std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferViews;
		std::vector<D3D12_INDEX_BUFFER_VIEW> m_IndexBufferViews;



		friend class DX12CommandList;
	};

}

#define TE_INSTANCE_API_DX12_BUFFERMANAGER TruthEngine::API::DirectX12::DX12BufferManager::GetInstance()