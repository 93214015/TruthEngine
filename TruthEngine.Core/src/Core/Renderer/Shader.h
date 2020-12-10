#pragma once
#include "ShaderInputLayout.h"

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12 
		{
			class DX12ShaderManager;
		}
	}

	namespace Core
	{
		
		struct ShaderSignatureSR
		{
			uint32_t BaseRegisterSlot;
			uint32_t RegisterSpace;
			uint32_t InputNum;
		};

		struct ShaderSignatureCB
		{
			uint32_t BaseRegisterSlot;
			uint32_t RegisterSpace;
			uint32_t InputNum;
		};

		class Shader
		{
		public:
			Shader(std::string_view name, std::string_view filePath);
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

			inline uint32_t GetRenderTargetNum() const noexcept
			{
				return m_RenderTargetNum;
			}

			inline ShaderSignatureSR GetShaderSignatureSR()
			{
				return m_SignatureSR;
			}

			inline ShaderSignatureCB GetShaderSignatureCB()
			{
				return m_SignatureCB;
			}
			
			
		protected:

			class ShaderConstantBufferSlot
			{
				std::string m_Name;
				uint8_t m_Register;
			};

			class ShaderTextureSlot
			{
				std::string m_Name;
				uint8_t m_Register;
			};

			class ShaderSamplerSlot
			{
				std::string m_Name;
				uint8_t m_Register;
			};

			uint32_t m_ID = 0;
			uint32_t m_RenderTargetNum = 0;

			std::vector<ShaderInputElement> m_InputElements;

			std::string m_Name = "";
			std::string m_FilePath = "";

			ShaderCode m_VS;
			ShaderCode m_HS;
			ShaderCode m_DS;
			ShaderCode m_GS;
			ShaderCode m_PS;
			ShaderCode m_CS;

			std::vector<ShaderConstantBufferSlot> m_ConstantBufferSlots;
			std::vector<ShaderTextureSlot> m_TextureSlots;
			std::vector<ShaderSamplerSlot> m_SamplerSlots;

			ShaderSignatureSR m_SignatureSR;
			ShaderSignatureCB m_SignatureCB;

			friend class ShaderManager;
			friend class API::DirectX12 ::DX12ShaderManager;
			
		};

	}

}