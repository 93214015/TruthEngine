#pragma once
#include "PipelineState.h"

namespace TruthEngine
{
	namespace API::DirectX12 
	{
		class DirectX12PiplineManager;
	}

	namespace Core
	{

		class Material;
		class Shader;

		class Pipeline
		{
		public:
			Pipeline(RendererStateSet states, Shader* shader);

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

			inline void SetState_PrimitiveTopology(TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY state)
			{
				SET_RENDERER_STATE(m_States, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY, state);
			}



			inline TE_RENDERER_STATE_ENABLED_SHADER_HS GetState_EnabledShaderHS()
			{
				return static_cast<TE_RENDERER_STATE_ENABLED_SHADER_HS>( GET_RENDERER_STATE(m_States, TE_RENDERER_STATE_ENABLED_SHADER_HS));
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

			TE_RESOURCE_FORMAT m_RTVFormats[8] = {TE_RESOURCE_FORMAT::R8G8B8A8_UNORM};
			TE_RESOURCE_FORMAT m_DSVFormat = TE_RESOURCE_FORMAT::D32_FLOAT;

			friend class PiplineManager;
			friend class API::DirectX12 ::DirectX12PiplineManager;

		};

	}
}
