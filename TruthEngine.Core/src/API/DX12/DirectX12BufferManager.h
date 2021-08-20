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


	class DirectX12BufferManager final : public BufferManager
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

		TextureCubeMap* CreateTextureCube(TE_IDX_GRESOURCES idx, const char* filePath) override;

		Texture* LoadTextureFromFile(TE_IDX_GRESOURCES _IDX, const char* _FilePath, uint8_t _MipLevels) override;

		void SaveTextureToFile(const Texture& _Texture, const char* _FilePath) override;


		//
		//Create Views Methods
		//
		void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* _outRTV) override;

		void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* _outRTV, uint8_t mipSlice, uint8_t arraySlice) override;

		void CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* _outRTV) override;

		void CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* _outDSV) override;


		void CreateShaderResourceView(GraphicResource* _GraphicResource, ShaderResourceView* _SRV, uint32_t frameIndex) override;


		void CreateUnorderedAccessView(GraphicResource* _GraphicResource, UnorderedAccessView* _outUAV) override;


		void CreateConstantBufferView(Buffer* constantBuffer, ConstantBufferView* CBV, uint32_t frameIndex) override;


		uint64_t GetRequiredSize(const GraphicResource* graphicResource) const override;


		void ReleaseResource(GraphicResource* resource) override;


		ID3D12Resource* GetResource(GraphicResource* graphicResource);


		D3D12_GPU_DESCRIPTOR_HANDLE AddDescriptorSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
		D3D12_GPU_DESCRIPTOR_HANDLE AddDescriptorCBV(const D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
		D3D12_GPU_DESCRIPTOR_HANDLE AddDescriptorUAV(ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);
		D3D12_CPU_DESCRIPTOR_HANDLE AddDescriptorRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr);
		D3D12_CPU_DESCRIPTOR_HANDLE AddDescriptorDSV(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc);

		inline DescriptorHeapSRV& GetDescriptorHeapSRV() noexcept
		{
			return m_DescHeapSRV;
		}

		inline DescriptorHeapDSV& GetDescriptorHeapDSV() noexcept
		{
			return m_DescHeapDSV;
		}

		inline DescriptorHeapRTV& GetDescriptorHeapRTV() noexcept
		{
			return m_DescHeapRTV;
		}



	private:

		void CreateShaderResourceView(Texture* texture, ShaderResourceView* _outSRV) override;

		void CreateShaderResourceView(Buffer* _Buffer, ShaderResourceView* _outSRV, uint32_t frameIndex) override;


		TE_RESULT CreateResource(BufferUpload* buffer) override;

		TE_RESULT CreateResource(Buffer* buffer) override;

		TE_RESULT CreateResource(Texture* _Texture) override;

		TE_RESULT CreateResource(TextureRenderTarget* tRT) override;

		TE_RESULT CreateResource(TextureDepthStencil* tDS) override;

		TE_RESULT CreateResource(VertexBufferStreamBase* vb) override;

		TE_RESULT CreateResource(IndexBuffer* ib) override;

		TE_RESULT CreateResource(TextureMaterial* texture) override;

		void OnReCreateResource(TE_IDX_GRESOURCES _IDX, uint32_t _ResourceIndexOffset) override;
		void OnReCreateResource(GraphicResource* _GResource, uint32_t _ResourceIndexOffset) override;


	private:

		uint32_t m_OffsetSRVMaterialTexture = 50;
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

		std::unordered_map <TE_IDX_GRESOURCES, std::vector<uint32_t>> m_Map_SRVs;
		std::unordered_map <TE_IDX_GRESOURCES, std::vector<uint32_t>> m_Map_UAVs;

		//
		// Friend Classes
		//
		friend class DirectX12CommandList;
		friend class DirectX12Manager;
		friend class DirectX12ShaderManager;
		friend class DirectX12TextureMaterialManager;
	};

}

#define TE_INSTANCE_API_DX12_BUFFERMANAGER TruthEngine::API::DirectX12::DirectX12BufferManager::GetInstance()