#pragma once
#include "BufferManager.h"
#include "ShaderSignature.h"

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12
		{
			class DirectX12ShaderManager;
		}
	}

	
	

	class ShaderRequiredResources
	{
	public:

		void AddResource(const ShaderSignature* _ShaderSignature);

		inline const std::vector<GraphicResource*>& GetSRVResources()const { return mSRV; }
		inline const std::vector<GraphicResource*>& GetCBVResources()const { return mCBV; }
		inline const std::vector<GraphicResource*>& GetUAVResources()const { return mUAV; }

	private:
		std::vector<GraphicResource*> mSRV;
		std::vector<GraphicResource*> mCBV;
		std::vector<GraphicResource*> mUAV;
	};

	class Shader
	{
	public:
		Shader(const char* name, TE_IDX_SHADERCLASS shaderClassIDX, ShaderSignature* shaderSignature, std::string_view filePath);
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

		/*inline const std::vector<ShaderInputElement>* GetInputElements() const noexcept
		{
			return &m_ShaderSignature->InputElements[(uint32_t)m_MeshType];
		}*/

		inline const ShaderSignature* GetSignature() const noexcept
		{
			return m_ShaderSignature;
		}


		inline TE_IDX_SHADERCLASS GetShaderClassIDX()const noexcept
		{
			return m_ShaderClassIDX;
		}



	protected:

		uint32_t m_ID = 0;
		TE_IDX_SHADERCLASS m_ShaderClassIDX = TE_IDX_SHADERCLASS::NONE;

		ShaderSignature* m_ShaderSignature = nullptr;


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