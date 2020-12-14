#pragma once


enum TE_IDX
{
	TE_IDX_TEXTURE_SHADOWMAP,
	TE_IDX_TEXTURE_BACKBUFFER,
	TE_IDX_TEXTURE_GBUFFER_COLOR,
	TE_IDX_TEXTURE_GBUFFER_NORMAL,
	TE_IDX_TEXTURE_GBUFFER_SPECULAR,
	TE_IDX_TEXTURE_DEPTHMAP_RENDER3D
};

#define TE_IDN_TEXTURE_SHADOWMAP "IDN_SHADOWMAP"
#define TE_IDN_TEXTURE_BACKBUFFER "IDN_BACKBUFFER"
#define TE_IDN_TEXTURE_GBUFFER_COLOR "IDN_GBUFFER_COLOR"
#define TE_IDN_TEXTURE_GBUFFER_NORMAL "IDN_GBUFFER_NORMAL"
#define TE_IDN_TEXTURE_GBUFFER_SPECULAR "IDN_GBUFFER_SPECULAR"
#define TE_IDN_TEXTURE_DEPTHMAP_RENDER3D "IDN_DEPTHMAP_RENDER3D"

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

		virtual RenderTargetView CreateRenderTargetView(TextureRenderTarget* RT) = 0;
		virtual RenderTargetView CreateRenderTargetView(SwapChain* swapChain) = 0;
		virtual DepthStencilView CreateDepthStencilView(TextureDepthStencil* DS) = 0;
		virtual ShaderResourceView CreateShaderResourceView(Texture* textures[], uint32_t textureNum) = 0;
		virtual ShaderResourceView CreateShaderResourceView(Texture* texture) = 0;
		virtual ConstantBufferView CreateConstantBufferView(Buffer* CB) = 0;

		virtual TE_RESULT CreateVertexBuffer(VertexBufferBase* vb) = 0;
		virtual TE_RESULT CreateIndexBuffer(IndexBuffer* ib) = 0;

		//virtual TE_RESULT CreateConstantBuffer(ConstantBufferBase* cb) = 0;

		/*virtual void BindRenderTarget(CommandList* cmdList) = 0;
		virtual void BindDepthStencil(CommandList* cmdList) = 0;
		virtual void BindShaderResource(CommandList* cmdList) = 0;*/

	protected:
		virtual TE_RESULT CreateResource(VertexBufferStreamBase* vb) = 0;
		virtual TE_RESULT CreateResource(IndexBuffer* ib) = 0;

		virtual TE_RESULT ReleaseResource(GraphicResource* resource) = 0;

	protected:
		uint32_t m_LastVertexBufferID;
		uint32_t m_LastIndexBufferID;

		static std::shared_ptr<BufferManager> Factory();
	};

}

#define TE_INSTANCE_BUFFERMANAGER TruthEngine::Core::BufferManager::GetInstance()