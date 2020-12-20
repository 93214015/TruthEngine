#pragma once


namespace TruthEngine::Core
{

	class CommandList;
	class GraphicResource;
	class Texture;
	class TextureRenderTarget;
	class TextureDepthStencil;
	class VertexBufferBase;
	class VertexBufferStreamBase;
	class ConstantBufferUploadBase;
	class Buffer;
	class BufferUpload;
	class IndexBuffer;
	class SwapChain;

	template<class T>
	class ConstantBufferUpload;

	struct RenderTargetView
	{
		uint32_t ViewIndex;
		uint32_t ResourceIndex;
		TextureRenderTarget* Resource;
	};

	struct ShaderResourceView
	{
		uint32_t ViewIndex;
		uint32_t ResourceIndex;
		Texture* Resource;
	};

	struct DepthStencilView
	{
		uint32_t ViewIndex;
		uint32_t ResourceIndex;
		TextureDepthStencil* Resource;
	};

	struct ConstantBufferView
	{
		uint32_t ViewIndex;
		uint32_t ResourceIndex;
		Buffer* Resource;
	};

	class BufferManager
	{
	public:
		BufferManager() = default;
		~BufferManager() = default;

		static std::shared_ptr<BufferManager> GetInstance() {
			static auto s_BufferManager = Factory();
			return s_BufferManager;
		}

		virtual void Init(uint32_t resourceNum, uint32_t shaderResourceViewNum, uint32_t renderTargetViewNum, uint32_t depthBufferViewNum) = 0;

		virtual TE_RESULT CreateResource(TextureRenderTarget* tRT) = 0;
		virtual TE_RESULT CreateResource(TextureDepthStencil* tDS) = 0;
		virtual TE_RESULT CreateResource(BufferUpload* buffer) = 0;

		template<class T>
		ConstantBufferUpload<T>* CreateConstantBufferUpload(TE_IDX_CONSTANTBUFFER cbIDX)
		{
			auto cbItr = m_Map_ConstantBufferUpload.find(cbIDX);
			if (cbItr != m_Map_ConstantBufferUpload.end())
			{
				return static_cast<ConstantBufferUpload<T>*>(cbItr->second.get());
			}

			auto cb = std::make_shared<ConstantBufferUpload<T>>("");

			m_Map_ConstantBufferUpload[cbIDX] = cb;

			CreateResource(cb.get());

			return cb.get();
		}

		virtual ConstantBufferUploadBase* GetConstantBufferUpload(TE_IDX_CONSTANTBUFFER cbIDX) = 0;

		virtual RenderTargetView CreateRenderTargetView(TextureRenderTarget* RT) = 0;
		virtual RenderTargetView CreateRenderTargetView(SwapChain* swapChain) = 0;
		virtual DepthStencilView CreateDepthStencilView(TextureDepthStencil* DS) = 0;
		virtual ShaderResourceView CreateShaderResourceView(Texture* textures[], uint32_t textureNum) = 0;
		virtual ShaderResourceView CreateShaderResourceView(Texture* texture) = 0;
		//virtual ConstantBufferView CreateConstantBufferView(Buffer* CB) = 0;
		virtual ConstantBufferView CreateConstantBufferView(const TE_IDX_CONSTANTBUFFER constantBufferIDX) = 0;

		virtual TE_RESULT CreateVertexBuffer(VertexBufferBase* vb) = 0;
		virtual TE_RESULT CreateIndexBuffer(IndexBuffer* ib) = 0;

		virtual uint64_t GetRequiredSize(const GraphicResource* graphicResource) const = 0;


	protected:
		virtual TE_RESULT CreateResource(VertexBufferStreamBase* vb) = 0;
		virtual TE_RESULT CreateResource(IndexBuffer* ib) = 0;

		virtual TE_RESULT ReleaseResource(GraphicResource* resource) = 0;

	protected:
		uint32_t m_LastVertexBufferID;
		uint32_t m_LastIndexBufferID;

		std::unordered_map<TE_IDX_CONSTANTBUFFER, std::shared_ptr<ConstantBufferUploadBase>> m_Map_ConstantBufferUpload;

		static std::shared_ptr<BufferManager> Factory();
	};

}

#define TE_INSTANCE_BUFFERMANAGER TruthEngine::Core::BufferManager::GetInstance()