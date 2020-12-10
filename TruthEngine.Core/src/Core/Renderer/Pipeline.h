#pragma once
#include "PipelineState.h"

namespace TruthEngine
{
	namespace API::DirectX12 
	{
		class DX12PiplineManager;
	}

	namespace Core
	{

		class Material;
		class Shader;

		class Pipeline
		{
		public:
			Pipeline() = default;

			virtual ~Pipeline() = default;

			Pipeline(const Pipeline&) = default;
			Pipeline& operator=(const Pipeline&) = default;

			Pipeline(Pipeline&&) noexcept = default;
			Pipeline& operator=(Pipeline&&) noexcept = default;

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

			inline void SetState_PrimitiveTopology (TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY state)
			{
				SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY, state);
			}


			inline TE_RENDERER_STATE_ENABLED_SHADER_HS GetState_EnabledShaderHS()
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_HS);
			}

			inline void GetState_EnabledShaderDS()
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_DS);
			}

			inline void GetState_EnabledShaderGS(TE_RENDERER_STATE_ENABLED_SHADER_GS state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_GS);
			}

			inline void GetState_FrontCounterClockwise()
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE);
			}

			inline void GetState_EnabledMapDiffuse(TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE);
			}

			inline void GetState_EnabledMapNormal(TE_RENDERER_STATE_ENABLED_MAP_NORMAL state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_NORMAL);
			}

			inline void GetState_EnabledMapDisplacement(TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT);
			}

			inline void GetState_EnabledDepth(TE_RENDERER_STATE_ENABLED_DEPTH state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_DEPTH);
			}

			inline void GetState_EnabledStencil(TE_RENDERER_STATE_ENABLED_STENCIL state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_STENCIL);
			}

			inline void GetState_FillMode(TE_RENDERER_STATE_FILL_MODE state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_FILL_MODE);
			}

			inline void GetState_CullMode(TE_RENDERER_STATE_CULL_MODE state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_CULL_MODE);
			}

			inline void GetState_PrimitiveTopology(TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY state)
			{
				GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY);
			}

#pragma endregion

			inline const Shader* GetShader()const noexcept
			{
				return m_Shader;
			}

			inline void SetShader(Shader* shader) noexcept
			{
				m_Shader = shader;
			}

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

		private:


		private:
			uint32_t m_ID;
			std::string m_Name = "PSO_****";

			RendererStateSet m_States;
			Shader* m_Shader;

			TE_RESOURCE_FORMAT m_RTVFormats[8];
			TE_RESOURCE_FORMAT m_DSVFormat;

			friend class PiplineManager;
			friend class API::DirectX12 ::DX12PiplineManager;

		};

	}
}
