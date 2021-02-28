#pragma once
#include "Core/Renderer/BufferManager.h"

#include "DirectX12DescriptorHeap.h"


//namespace TruthEngine
//{
//	class VertexBuffer;
//	class GraphicResource;
//	class TextureRenderTarget;
//	class TextureDepthStencil;
//	class ConstantBuffer;
//	class CommandList;
//}

namespace TruthEngine::API::DirectX12 {


	class DirectX12BufferManager : public BufferManager
	{

	public:



		static std::shared_ptr<DirectX12BufferManager> GetInstance()
		{
			static std::shared_ptr<DirectX12BufferManager> s_Instance = std::make_shared<DirectX12BufferManager>();
			return s_Instance;
		}

		void Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum) override;

		void Release() override;

		TE_RESULT CreateVertexBuffer(VertexBufferBase* vb) override;

		TE_RESULT CreateIndexBuffer(IndexBuffer* ib) override;

		void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* RTV) override;

		void CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* RTV) override;

		void CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* DSV) override;

		void CreateShaderResourceView(Texture* textures[], uint32_t textureNum, ShaderResourceView* SRV) override;

		void CreateShaderResourceView(Texture* texture, ShaderResourceView* SRV) override;

		void CreateConstantBufferView(ConstantBufferUploadBase* constantBuffer, ConstantBufferView* CBV, uint8_t frameIndex) override;

		uint64_t GetRequiredSize(const GraphicResource* graphicResource) const override;

		void ReleaseResource(GraphicResource* resource) override;

		ID3D12Resource* GetResource(GraphicResource* graphicResource);

		D3D12_GPU_DESCRIPTOR_HANDLE AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
		D3D12_GPU_DESCRIPTOR_HANDLE AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
		D3D12_GPU_DESCRIPTOR_HANDLE AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);
		D3D12_CPU_DESCRIPTOR_HANDLE AddDescriptorRTV(ID3D12Resource* resource);
		D3D12_CPU_DESCRIPTOR_HANDLE AddDescriptorDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc);

		inline DescriptorHeapSRV& GetDescriptorHeapSRV()
		{
			return m_DescHeapSRV;
		}

	private:
		TE_RESULT CreateResource(BufferUpload* buffer) override;

		TE_RESULT CreateResource(TextureRenderTarget* tRT) override;

		TE_RESULT CreateResource(TextureDepthStencil* tDS) override;

		TE_RESULT CreateResource(VertexBufferStreamBase* vb) override;

		TE_RESULT CreateResource(IndexBuffer* ib) override;

		TE_RESULT CreateResource(TextureMaterial* texture) override;


	private:

		uint32_t m_OffsetSRVMaterialTexture_Diffuse = 50;
		uint32_t m_OffsetSRVMaterialTexture_Normal = 400;
		uint32_t m_OffsetSRVMaterialTexture_Displacement = 700;
		uint32_t m_IndexSRVMaterialTexture_Diffuse = 0;
		uint32_t m_IndexSRVMaterialTexture_Normal = 0;
		uint32_t m_IndexSRVMaterialTexture_Displacement = 0;

		std::vector<COMPTR<ID3D12Resource>> m_Resources;

		DescriptorHeapSRV m_DescHeapSRV;

		DescriptorHeapRTV m_DescHeapRTV;

		DescriptorHeapDSV m_DescHeapDSV;

		D3D12_RECT m_Rect_FullScreen;

		std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VertexBufferViews;

		std::vector<D3D12_INDEX_BUFFER_VIEW> m_IndexBufferViews;

		std::unordered_map<TE_IDX_CONSTANTBUFFER, D3D12_GPU_DESCRIPTOR_HANDLE> m_UnboundedConstantBuffers;


		//
		// Friend Classes
		//
		friend class DirectX12CommandList;
		friend class DirectX12Manager;
		friend class DirectX12ShaderManager;
		friend class DirectX12TextureMaterialManager;
	};

}

#define TE_INSTANCE_API_DX12_BUFFERMANAGER TruthEngine::API::DirectX12::DX12BufferManager::GetInstance()