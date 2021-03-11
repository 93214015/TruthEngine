#pragma once

#include "ConstantBuffer.h"
#include "TextureCubeMap.h"
#include "TextureRenderTarget.h"
#include "TextureDepthStencil.h"

namespace TruthEngine
{

	class CommandList;
	class GraphicResource;
	class Texture;
	class TextureMaterial;
	class TextureRenderTarget;
	class TextureDepthStencil;
	class TextureCubeMap;
	class VertexBufferBase;
	class VertexBufferStreamBase;
	class Buffer;
	class BufferUpload;
	class IndexBuffer;
	class SwapChain;


	struct ClearValue_RenderTarget;
	struct ClearValue_DepthStencil;


	struct RenderTargetView
	{
		RenderTargetView() = default;
		RenderTargetView(uint32_t viewIndex, uint32_t resourceIndex, TextureRenderTarget* resource)
			: ViewIndex(viewIndex), ResourceIndex(resourceIndex), Resource(resource)
		{}

		uint32_t ViewIndex = -1;
		uint32_t ResourceIndex = -1;
		TextureRenderTarget* Resource = nullptr;
	};

	struct ShaderResourceView
	{
		ShaderResourceView() = default;
		ShaderResourceView(uint32_t viewIndex, uint32_t resourceIndex, GraphicResource* resource)
			: ViewIndex(viewIndex), ResourceIndex(resourceIndex), Resource(resource)
		{}

		uint32_t ViewIndex = -1;
		uint32_t ResourceIndex = -1;
		GraphicResource* Resource = nullptr;
	};

	struct UnorderedAccessView
	{
		UnorderedAccessView() = default;
		UnorderedAccessView(uint32_t viewIndex, uint32_t resourceIndex, Buffer* resource)
			: ViewIndex(viewIndex), ResourceIndex(resourceIndex), Resource(resource)
		{}

		uint32_t ViewIndex = -1;
		uint32_t ResourceIndex = -1;
		Buffer* Resource = nullptr;
	};

	struct DepthStencilView
	{
		DepthStencilView() = default;
		DepthStencilView(uint32_t viewIndex, uint32_t resourceIndex, TextureDepthStencil* resource)
			: ViewIndex(viewIndex), ResourceIndex(resourceIndex), Resource(resource)
		{}

		uint32_t ViewIndex = -1;
		uint32_t ResourceIndex = -1;
		TextureDepthStencil* Resource = nullptr;
	};

	struct ConstantBufferView
	{
		ConstantBufferView() = default;
		ConstantBufferView(uint32_t viewIndex, uint32_t resourceIndex, Buffer* resource)
			: ViewIndex(viewIndex), ResourceIndex(resourceIndex), Resource(resource)
		{}

		uint32_t ViewIndex = -1;
		uint32_t ResourceIndex = -1;
		Buffer* Resource = nullptr;
	};

	class BufferManager
	{
	public:
		BufferManager();
		~BufferManager() = default;

		static BufferManager* GetInstance() {
			static auto s_BufferManager = Factory();
			return s_BufferManager.get();
		}

		virtual void Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum) = 0;

		virtual void Release() = 0;

		template<class T>
		ConstantBufferUpload<T>* CreateConstantBufferUpload(TE_IDX_GRESOURCES cbIDX)
		{
			auto cbItr = m_Map_ConstantBufferUpload.find(cbIDX);
			if (cbItr != m_Map_ConstantBufferUpload.end())
			{
				return static_cast<ConstantBufferUpload<T>*>(cbItr->second.get());
			}

			auto cb = std::make_shared<ConstantBufferUpload<T>>(cbIDX);

			m_Map_ConstantBufferUpload[cbIDX] = cb;
			m_Map_GraphicResources[cbIDX] = cb.get();

			CreateResource(cb.get());

			return cb.get();
		}

				
		template<class T>
		ConstantBufferDirect<T>* CreateConstantBufferDirect(TE_IDX_GRESOURCES cbIDX)
		{
			auto itr = m_Map_ConstantBufferDirect.find(cbIDX);
			if(itr != m_Map_ConstantBufferDirect.end())
			{
				return static_cast<ConstantBufferDirect<T>*>(itr->second.get());
			}

			auto cb = std::make_shared<ConstantBufferDirect<T>>(cbIDX);

			m_Map_ConstantBufferDirect[cbIDX] = cb;
			m_Map_GraphicResources[cbIDX] = nullptr;

			return cb.get();
		}

		Buffer* CreateBufferStructuredRW(TE_IDX_GRESOURCES _IDX, uint32_t _ElementSizeInByte, uint32_t _ElementNum, bool _IsByteAddressBuffer);

		TextureRenderTarget* CreateRenderTarget(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA);

		TextureDepthStencil* CreateDepthStencil(TE_IDX_GRESOURCES idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource, bool enbaleMSAA);

		GraphicResource* GetGraphicResource(TE_IDX_GRESOURCES _IDX) const;

		TextureRenderTarget* GetRenderTarget(TE_IDX_GRESOURCES idx);

		TextureDepthStencil* GetDepthStencil(TE_IDX_GRESOURCES idx);

		Buffer* GetBuffer(TE_IDX_GRESOURCES _IDX);

		TextureCubeMap* GetCubeMap(uint32_t index);

		TextureCubeMap* GetSkyCubeMap();

		Texture* GetTexture(TE_IDX_GRESOURCES idx);

		ConstantBufferUploadBase* GetConstantBufferUpload(TE_IDX_GRESOURCES cbIDX);

		ConstantBufferDirectBase* GetConstantBufferDirect(TE_IDX_GRESOURCES cbIDX);

		virtual TextureCubeMap* CreateTextureCube(TE_IDX_GRESOURCES idx, const char* filePath) = 0;

		virtual void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* rtv) = 0;

		virtual void CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* rtv) = 0;

		virtual void CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* dsv) = 0;

		/*virtual void CreateShaderResourceView(Texture* textures[], uint32_t textureNum, ShaderResourceView* srv) = 0;*/

		virtual void CreateShaderResourceView(GraphicResource* _GraphicResource, ShaderResourceView* _SRV, uint32_t frameIndex) = 0;

		virtual void CreateUnorderedAccessView(Buffer* _BufferRW, UnorderedAccessView* _UAV) = 0;

		virtual void CreateConstantBufferView(Buffer* constantBuffer, ConstantBufferView* cbv, uint32_t frameIndex) = 0;

		virtual TE_RESULT CreateVertexBuffer(VertexBufferBase* vb) = 0;

		virtual TE_RESULT CreateIndexBuffer(IndexBuffer* ib) = 0;

		virtual uint64_t GetRequiredSize(const GraphicResource* graphicResource) const = 0;

		virtual void ReleaseResource(GraphicResource* graphicResource) = 0;


	protected:

		virtual void CreateShaderResourceView(Texture* texture, ShaderResourceView* srv) = 0;
		virtual void CreateShaderResourceView(Buffer* _Buffer, ShaderResourceView* SRV, uint32_t frameIndex) = 0;

		virtual TE_RESULT CreateResource(BufferUpload* buffer) = 0;
		virtual TE_RESULT CreateResource(Buffer* buffer) = 0;
		virtual TE_RESULT CreateResource(TextureRenderTarget* tRT) = 0;
		virtual TE_RESULT CreateResource(TextureDepthStencil* tDS) = 0;
		virtual TE_RESULT CreateResource(VertexBufferStreamBase* vb) = 0;
		virtual TE_RESULT CreateResource(IndexBuffer* ib) = 0;
		virtual TE_RESULT CreateResource(TextureMaterial* texture) = 0;

		virtual void OnReCreateResource(TE_IDX_GRESOURCES _IDX, uint32_t _ResourceIndexOffset) = 0;
		virtual void OnReCreateResource(GraphicResource* _GResource, uint32_t _ResourceIndexOffset) = 0;

	protected:
		uint32_t m_LastVertexBufferID;
		uint32_t m_LastIndexBufferID;

		uint32_t m_SkyCubeMapIndex = 0;

		std::unordered_map<TE_IDX_GRESOURCES, GraphicResource*> m_Map_GraphicResources;
		std::unordered_map<TE_IDX_GRESOURCES, std::shared_ptr<ConstantBufferUploadBase>> m_Map_ConstantBufferUpload;
		std::unordered_map<TE_IDX_GRESOURCES, std::shared_ptr<ConstantBufferDirectBase>> m_Map_ConstantBufferDirect;
		std::unordered_map<TE_IDX_GRESOURCES, Buffer*> m_Map_Buffers;
		std::unordered_map<TE_IDX_GRESOURCES, Texture*> m_Map_Textures;

		std::vector<TextureRenderTarget> m_TexturesRenderTarget;
		std::vector<TextureDepthStencil> m_TexturesDepthStencil;
		std::vector<TextureCubeMap> m_TexturesCubeMap;
		std::vector<Buffer> m_Buffers;

		static std::shared_ptr<BufferManager> Factory();

		//friend class
		friend class RendererCommand;
		friend class TextureMaterialManager;
	};
}

#define TE_INSTANCE_BUFFERMANAGER TruthEngine::BufferManager::GetInstance()
