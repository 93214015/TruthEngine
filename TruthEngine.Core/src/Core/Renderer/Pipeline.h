#pragma once
#include "PipelineState.h"
#include "PipelineBlendDesc.h"
#include "PipelineDepthStencilDesc.h"
#include "ShaderInputLayout.h"
#include "ShaderManager.h"

namespace TruthEngine
{
	namespace API::DirectX12
	{
		class DirectX12PiplineManager;
	}

	class Material;
	class Shader;

	

	class PipelineGraphics
	{
	public:		
		PipelineGraphics() = default;
		virtual ~PipelineGraphics() = default;

		PipelineGraphics(const PipelineGraphics&) = default;
		PipelineGraphics& operator=(const PipelineGraphics&) = default;

		PipelineGraphics(PipelineGraphics&&) noexcept = default;
		PipelineGraphics& operator=(PipelineGraphics&&) noexcept = default;

#pragma region StateGetterSetter

		inline RendererStateSet GetStates() const noexcept
		{
			return m_States;
		}

		inline void SetStates(RendererStateSet states) noexcept
		{
			m_States = states;
		}

		inline void SetState_EnabledShaderHS(TE_RENDERER_STATE_ENABLED_SHADER_HS state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_HS, state);
		}

		inline void SetState_EnabledShaderDS(TE_RENDERER_STATE_ENABLED_SHADER_DS state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_DS, state);
		}

		inline void SetState_EnabledShaderGS(TE_RENDERER_STATE_ENABLED_SHADER_GS state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_GS, state);
		}

		inline void SetState_FrontCounterClockwise(TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE, state);
		}

		inline void SetState_EnabledMapDiffuse(TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, state);
		}

		inline void SetState_EnabledMapNormal(TE_RENDERER_STATE_ENABLED_MAP_NORMAL state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, state);
		}

		inline void SetState_EnabledMapDisplacement(TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT, state);
		}

		inline void SetState_EnabledDepth(TE_RENDERER_STATE_ENABLED_DEPTH state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_DEPTH, state);
		}

		inline void SetState_EnabledStencil(TE_RENDERER_STATE_ENABLED_STENCIL state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_STENCIL, state);
		}

		inline void SetState_FillMode(TE_RENDERER_STATE_FILL_MODE state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_FILL_MODE, state);
		}

		inline void SetState_CullMode(TE_RENDERER_STATE_CULL_MODE state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_CULL_MODE, state);
		}

		inline void SetState_PrimitiveTopology(TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY state)
		{
			SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY, state);
		}



		inline TE_RENDERER_STATE_ENABLED_SHADER_HS GetState_EnabledShaderHS()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_SHADER_HS>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_HS));
		}

		inline TE_RENDERER_STATE_ENABLED_SHADER_DS GetState_EnabledShaderDS()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_SHADER_DS>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_DS));
		}

		inline TE_RENDERER_STATE_ENABLED_SHADER_GS GetState_EnabledShaderGS()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_SHADER_GS>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_GS));
		}

		inline TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE GetState_FrontCounterClockwise()
		{
			return static_cast<TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE));
		}

		inline TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE GetState_EnabledMapDiffuse()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE));
		}

		inline TE_RENDERER_STATE_ENABLED_MAP_NORMAL GetState_EnabledMapNormal()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_MAP_NORMAL>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_NORMAL));
		}

		inline TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT GetState_EnabledMapDisplacement()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT));
		}

		inline TE_RENDERER_STATE_ENABLED_DEPTH GetState_EnabledDepth()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_DEPTH>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_DEPTH));
		}

		inline TE_RENDERER_STATE_ENABLED_STENCIL GetState_EnabledStencil()
		{
			return static_cast<TE_RENDERER_STATE_ENABLED_STENCIL>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_STENCIL));
		}

		inline TE_RENDERER_STATE_FILL_MODE GetState_FillMode()
		{
			return static_cast<TE_RENDERER_STATE_FILL_MODE>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_FILL_MODE));
		}

		inline TE_RENDERER_STATE_CULL_MODE GetState_CullMode()
		{
			return static_cast<TE_RENDERER_STATE_CULL_MODE>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_CULL_MODE));
		}

		inline TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY GetState_PrimitiveTopology()
		{
			return static_cast<TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY>(GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY));
		}

#pragma endregion

		inline uint32_t GetID() const noexcept
		{
			return m_ID;
		}

		const Shader* GetShader()const noexcept;		

		inline TE_IDX_SHADERCLASS GetShaderClassIDX()const noexcept
		{
			return m_ShaderClassIDX;
		}

		void SetShader(const ShaderHandle& shaderHandle) noexcept;

		inline void SetRendererStates(RendererStateSet states) noexcept
		{
			m_States = states;
		}

		inline RendererStateSet GetRendererStates() const noexcept
		{
			return m_States;
		}

		inline const TE_RESOURCE_FORMAT* GetRenderTargetFormats() const noexcept
		{
			return m_RTVFormats;
		}

		inline TE_RESOURCE_FORMAT GetDSVFormat() const noexcept
		{
			return m_DSVFormat;
		}

		inline uint32_t GetRenderTargetNum() const noexcept
		{
			return m_RenderTargetNum;
		}

		inline float GetDepthBias() const noexcept
		{
			return m_DepthBias;
		}

		inline float GetDepthBiasClamp() const noexcept
		{
			return m_DepthBiasClamp;
		}

		inline float GetSlopeScaledDepthBias() const noexcept
		{
			return m_SlopeScaledDepthBias;
		}

		inline bool GetEnableMSAA() const noexcept
		{
			return m_EnableMSAA;
		}

		const PipelineBlendDesc& GetBlendMode() const noexcept
		{
			return m_PipelineBlendMode;
		}

		const PipelineDepthStencilDesc& GetDepthStencilDesc() const noexcept
		{
			return m_PipelineDepthStencilDesc;
		}

		const std::vector<ShaderInputElement>& GetShaderInputElements() const;

		static void Factory(PipelineGraphics* _outPipeline, RendererStateSet states, const class ShaderHandle& shaderHandle, uint32_t renderTargetNum, const TE_RESOURCE_FORMAT* rtvFormat, TE_RESOURCE_FORMAT dsvFormat, const std::vector<ShaderInputElement>& inputElements, bool enableMSAA, const PipelineBlendDesc& pipelineBlendMode = PipelineBlendDesc{}, const PipelineDepthStencilDesc& _PipelineDepthStencilDesc = PipelineDepthStencilDesc{}, float depthBias = 0.0f, float depthBiasClamp = 0.0f, float slopeScaledDepthBias = 0.0f);

	private:
		PipelineGraphics(uint32_t ID, std::string_view _Name, RendererStateSet states, const class ShaderHandle& shaderHandle, uint32_t renderTargetNum, const TE_RESOURCE_FORMAT* rtvFormat, TE_RESOURCE_FORMAT dsvFormat, const std::vector<ShaderInputElement>& inputElements, bool enableMSAA, const PipelineBlendDesc& pipelineBlendDesc, const PipelineDepthStencilDesc& _PipelineDepthStencilDesc, float depthBias = 0.0f, float depthBiasClamp = 0.0f, float slopeScaledDepthBias = 0.0f);

	private:
		uint32_t m_ID = -1;

		RendererStateSet m_States;

		uint32_t m_RenderTargetNum;

		float m_DepthBias;
		float m_DepthBiasClamp;
		float m_SlopeScaledDepthBias;

		TE_RESOURCE_FORMAT m_RTVFormats[8] = { TE_RESOURCE_FORMAT::UNKNOWN, TE_RESOURCE_FORMAT::UNKNOWN , TE_RESOURCE_FORMAT::UNKNOWN , TE_RESOURCE_FORMAT::UNKNOWN , TE_RESOURCE_FORMAT::UNKNOWN, TE_RESOURCE_FORMAT::UNKNOWN, TE_RESOURCE_FORMAT::UNKNOWN, TE_RESOURCE_FORMAT::UNKNOWN };
		TE_RESOURCE_FORMAT m_DSVFormat = TE_RESOURCE_FORMAT::D32_FLOAT;

		PipelineBlendDesc m_PipelineBlendMode;
		PipelineDepthStencilDesc m_PipelineDepthStencilDesc;

		bool m_EnableMSAA = false;

		ShaderHandle m_ShaderHandle;
		TE_IDX_SHADERCLASS m_ShaderClassIDX = TE_IDX_SHADERCLASS::NONE;

		std::vector<ShaderInputElement> m_ShaderInputElements;

		std::string m_Name = "PSO_";

		friend class PiplineManager;
		friend class API::DirectX12::DirectX12PiplineManager;

	};

	class PipelineCompute
	{
	public:
		PipelineCompute() = default;
		~PipelineCompute() = default;

		PipelineCompute(const PipelineCompute& _Pipeline) = default;
		PipelineCompute& operator=(const PipelineCompute& _Pipeline) = default;

		PipelineCompute(PipelineCompute&& _Pipeline) noexcept = default;
		PipelineCompute& operator=(PipelineCompute&& _Pipeline) noexcept = default;

		inline uint32_t GetID() const noexcept
		{
			return m_ID;
		}

		const Shader* GetShader() const noexcept;

		inline TE_IDX_SHADERCLASS GetShaderClassIDX() const noexcept
		{
			return m_ShaderClassIDX;
		}

		static void Factory(PipelineCompute* _outPipeline, const ShaderHandle& _Shader);

	private:
		PipelineCompute(uint32_t _ID, std::string_view _Name, const ShaderHandle& _ShaderHandle);


	private:
		uint32_t m_ID = -1;

		ShaderHandle m_ShaderHandle;
		TE_IDX_SHADERCLASS m_ShaderClassIDX;

		std::string m_Name = "";


		friend class PiplineManager;
		friend class API::DirectX12::DirectX12PiplineManager;
	};
}
