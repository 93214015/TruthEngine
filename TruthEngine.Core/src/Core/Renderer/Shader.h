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

		struct ShaderSignature
		{
			struct ShaderConstantBufferSlot
			{
				ShaderConstantBufferSlot(uint32_t registerSlot, uint32_t registerSpace, TE_IDX_CONSTANTBUFFER constantBufferIDX)
					: Register(registerSlot), RegisterSpace(registerSpace), ConstantBufferIDX(constantBufferIDX)
				{}

				uint32_t Register;
				uint32_t RegisterSpace;
				TE_IDX_CONSTANTBUFFER ConstantBufferIDX;
			};

			struct ShaderTextureSlot
			{
				ShaderTextureSlot(uint32_t registerSlot, uint32_t registerSpace, TE_IDX_TEXTURE textureIDX)
					: Register(registerSlot), RegisterSpace(registerSpace), TextureIDX(textureIDX)
				{}

				uint32_t Register;
				uint32_t RegisterSpace;
				TE_IDX_TEXTURE TextureIDX;
			};

			std::vector<std::vector<ShaderConstantBufferSlot>> ConstantBuffers;
			std::vector<std::vector<ShaderTextureSlot>> Textures;

			std::vector<ShaderInputElement> InputElements[static_cast<uint32_t>(TE_IDX_MESH_TYPE::TOTALNUM)];
		};


		class Shader
		{
		public:
			Shader(std::string_view name, TE_IDX_SHADERCLASS shaderClassIDX, TE_IDX_MESH_TYPE meshType, ShaderSignature* shaderSignature, std::string_view filePath);
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

			inline const std::vector<ShaderInputElement>* GetInputElements() const noexcept
			{
				return &m_ShaderSignature->InputElements[(uint32_t)m_MeshType];
			}

			inline const ShaderSignature* GetSignature() const noexcept
			{
				return m_ShaderSignature;
			}


			inline uint32_t GetRenderTargetNum() const noexcept
			{
				return m_RenderTargetNum;
			}


			inline TE_IDX_SHADERCLASS GetShaderClassIDX()const noexcept
			{
				return m_ShaderClassIDX;
			}

			inline TE_IDX_MESH_TYPE GetMeshType()const noexcept
			{
				return m_MeshType;
			}


		protected:

			uint32_t m_ID = 0;
			TE_IDX_SHADERCLASS m_ShaderClassIDX = TE_IDX_SHADERCLASS::NONE;
			TE_IDX_MESH_TYPE m_MeshType = TE_IDX_MESH_TYPE::MESH_NTT;
			uint32_t m_RenderTargetNum = 1;

			ShaderSignature* m_ShaderSignature;
			

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