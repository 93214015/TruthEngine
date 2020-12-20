#pragma once
#include "ShaderInputLayout.h"

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12
		{
			class DirectX12ShaderManager;
		}
	}

	namespace Core
	{

		class Shader
		{
		public:
			Shader(TE_IDX_SHADERCLASS shaderClassIDX, std::string_view name, std::string_view filePath);
			virtual ~Shader() = default;

			Shader(Shader&& shader) noexcept = default;
			Shader& operator=(Shader&& shader) = default;

			struct ShaderCode
			{
				ShaderCode() = default;
				ShaderCode(size_t bufferSize, void* bufferPointer) : BufferSize(bufferSize), BufferPointer(bufferPointer)
				{}

				size_t BufferSize = 0;
				void* BufferPointer = nullptr;
			};

			inline ShaderCode GetVS() const noexcept { return m_VS; };
			inline ShaderCode GetDS() const noexcept { return m_DS; };
			inline ShaderCode GetHS() const noexcept { return m_HS; };
			inline ShaderCode GetGS() const noexcept { return m_GS; };
			inline ShaderCode GetPS() const noexcept { return m_PS; };
			inline ShaderCode GetCS() const noexcept { return m_CS; };

			inline const std::vector<ShaderInputElement>& GetInputElements() const noexcept
			{
				return m_InputElements;
			}

			inline void AddInputElement(const ShaderInputElement& inputElement)
			{
				m_InputElements.push_back(inputElement);
			}

			inline uint32_t GetRenderTargetNum() const noexcept
			{
				return m_RenderTargetNum;
			}


			inline TE_IDX_SHADERCLASS GetShaderClassIDX()const noexcept
			{
				return m_ShaderClassIDX;
			}


		protected:

			uint32_t m_ID = 0;
			TE_IDX_SHADERCLASS m_ShaderClassIDX = TE_IDX_SHADERCLASS::NONE;
			uint32_t m_RenderTargetNum = 1;

			std::vector<ShaderInputElement> m_InputElements;

			std::string m_Name = "";
			std::string m_FilePath = "";

			ShaderCode m_VS;
			ShaderCode m_HS;
			ShaderCode m_DS;
			ShaderCode m_GS;
			ShaderCode m_PS;
			ShaderCode m_CS;


			friend class ShaderManager;
			friend class API::DirectX12::DirectX12ShaderManager;

		};

	}

}