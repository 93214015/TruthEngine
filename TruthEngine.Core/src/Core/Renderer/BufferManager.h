#pragma once

#include "ConstantBuffer.h"

namespace TruthEngine
{

	class CommandList;
	class GraphicResource;
	class Texture;
	class TextureMaterial;
	class TextureRenderTarget;
	class TextureDepthStencil;
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
		ShaderResourceView(uint32_t viewIndex, uint32_t resourceIndex, Texture* resource)
			: ViewIndex(viewIndex), ResourceIndex(resourceIndex), Resource(resource)
		{}

		uint32_t ViewIndex = -1;
		uint32_t ResourceIndex = -1;
		Texture* Resource = nullptr;
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
		BufferManager() = default;
		~BufferManager() = default;

		static BufferManager* GetInstance() {
			static auto s_BufferManager = Factory();
			return s_BufferManager.get();
		}

		virtual void Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum) = 0;

		virtual void Release() = 0;

		template<class T>
		ConstantBufferUpload<T>* CreateConstantBufferUpload(TE_IDX_CONSTANTBUFFER cbIDX)
		{
			auto cbItr = m_Map_ConstantBufferUpload.find(cbIDX);
			if (cbItr != m_Map_ConstantBufferUpload.end())
			{
				return static_cast<ConstantBufferUpload<T>*>(cbItr->second.get());
			}

			auto cb = std::make_shared<ConstantBufferUpload<T>>(cbIDX);

			m_Map_ConstantBufferUpload[cbIDX] = cb;

			CreateResource(cb.get());

			return cb.get();
		}

		template<class T>
		ConstantBufferUploadArray<T>* CreateConstantBufferUploadArray(TE_IDX_CONSTANTBUFFER idx, uint32_t arraySize)
		{
			auto itr = m_Map_ConstantBufferUpload.find(idx);
			if (itr != m_Map_ConstantBufferUpload.end())
			{
				return static_cast<ConstantBufferUploadArray<T>*>(itr->second.get());
			}

			auto cb = std::make_shared<ConstantBufferUploadArray<T>>(idx, arraySize);
			m_Map_ConstantBufferUpload[idx] = cb;

			CreateResource(cb.get());

			return (cb.get());
		}
				
		template<class T>
		ConstantBufferDirect<T>* CreateConstantBufferDirect(TE_IDX_CONSTANTBUFFER cbIDX)
		{
			auto itr = m_Map_ConstantBufferDirect.find(cbIDX);
			if(itr != m_Map_ConstantBufferDirect.end())
			{
				return static_cast<ConstantBufferDirect<T>*>(itr->second.get());
			}

			auto cb = std::make_shared<ConstantBufferDirect<T>>(cbIDX);

			m_Map_ConstantBufferDirect[cbIDX] = cb;

			return cb.get();
		}

		TextureRenderTarget* CreateRenderTarget(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_RenderTarget& clearValue, bool useAsShaderResource, bool enbaleMSAA);

		TextureDepthStencil* CreateDepthStencil(TE_IDX_TEXTURE idx, uint32_t width, uint32_t height, TE_RESOURCE_FORMAT format, const ClearValue_DepthStencil& clearValue, bool useAsShaderResource, bool enbaleMSAA);

		TextureRenderTarget* GetRenderTarget(TE_IDX_TEXTURE idx);

		TextureDepthStencil* GetDepthStencil(TE_IDX_TEXTURE idx);

		Texture* GetTexture(TE_IDX_TEXTURE idx);

		ConstantBufferUploadBase* GetConstantBufferUpload(TE_IDX_CONSTANTBUFFER cbIDX);

		ConstantBufferDirectBase* GetConstantBufferDirect(TE_IDX_CONSTANTBUFFER cbIDX);

		virtual void CreateRenderTargetView(TextureRenderTarget* RT, RenderTargetView* rtv) = 0;

		virtual void CreateRenderTargetView(SwapChain* swapChain, RenderTargetView* rtv) = 0;

		virtual void CreateDepthStencilView(TextureDepthStencil* DS, DepthStencilView* dsv) = 0;

		virtual void CreateShaderResourceView(Texture* textures[], uint32_t textureNum, ShaderResourceView* srv) = 0;

		virtual void CreateShaderResourceView(Texture* texture, ShaderResourceView* srv) = 0;

		virtual void CreateConstantBufferView(ConstantBufferUploadBase* constantBuffer, ConstantBufferView* cbv, uint8_t frameIndex) = 0;

		virtual TE_RESULT CreateVertexBuffer(VertexBufferBase* vb) = 0;

		virtual TE_RESULT CreateIndexBuffer(IndexBuffer* ib) = 0;

		virtual uint64_t GetRequiredSize(const GraphicResource* graphicResource) const = 0;

		virtual void ReleaseResource(GraphicResource* graphicResource) = 0;


	protected:
		virtual TE_RESULT CreateResource(BufferUpload* buffer) = 0;
		virtual TE_RESULT CreateResource(TextureRenderTarget* tRT) = 0;
		virtual TE_RESULT CreateResource(TextureDepthStencil* tDS) = 0;
		virtual TE_RESULT CreateResource(VertexBufferStreamBase* vb) = 0;
		virtual TE_RESULT CreateResource(IndexBuffer* ib) = 0;
		virtual TE_RESULT CreateResource(TextureMaterial* texture) = 0;

	protected:
		uint32_t m_LastVertexBufferID;
		uint32_t m_LastIndexBufferID;

		std::unordered_map<TE_IDX_CONSTANTBUFFER, std::shared_ptr<ConstantBufferUploadBase>> m_Map_ConstantBufferUpload;
		std::unordered_map<TE_IDX_CONSTANTBUFFER, std::shared_ptr<ConstantBufferDirectBase>> m_Map_ConstantBufferDirect;
		std::unordered_map<TE_IDX_TEXTURE, std::shared_ptr<Texture>> m_Map_Textures;

		static std::shared_ptr<BufferManager> Factory();

		//friend class
		friend class RendererCommand;
		friend class TextureMaterialManager;
	};
}

#define TE_INSTANCE_BUFFERMANAGER TruthEngine::BufferManager::GetInstance()
