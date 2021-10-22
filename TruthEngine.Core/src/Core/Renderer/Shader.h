#pragma once
#include "ShaderRequiredResource.h"
#include "ShaderCode.h"
#include "BufferManager.h"
#include "ShaderSignature.h"

#include "Core/IO/Serializable.h"

SERIALIZABLE_SEPARATE1(TruthEngine, Shader);

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12
		{
			class DirectX12ShaderManager;
		}
	}

	using TE_ShaderClass_UniqueIdentifier = uint32_t;

	using TE_ShaderID = uint64_t;


	class Shader
	{
	public:
		Shader(TE_ShaderID shaderID, ShaderSignature* shaderSignature, std::string_view filePath);
		virtual ~Shader();

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader(Shader&& shader) noexcept = default;
		Shader& operator=(Shader&& shader) = default;

		
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

		const ShaderSignature* GetSignature() const noexcept
		{
			return m_ShaderSignature;
		}


		TE_IDX_SHADERCLASS GetShaderClassIDX()const noexcept
		{
			return static_cast<TE_IDX_SHADERCLASS>(m_ID >> 32);
		}

		TE_ShaderClass_UniqueIdentifier GetShaderClassUniqueIdentifier() const noexcept
		{
			return static_cast<TE_ShaderClass_UniqueIdentifier>(m_ID & 0x0000FFFF);
		}
		



	protected:

		//	ID is made of two part: the first 32bit is ShaderClassIDX and the second 32 bit
		//	is ShaderClass UniqueIdentifier 
		TE_ShaderID m_ID = 0;

		ShaderSignature* m_ShaderSignature = nullptr;


		//std::string m_Name = "";
		std::string m_FilePath = "";

		ShaderCode m_VS;
		ShaderCode m_HS;
		ShaderCode m_DS;
		ShaderCode m_GS;
		ShaderCode m_PS;
		ShaderCode m_CS;


		friend class ShaderManager;
		friend class API::DirectX12::DirectX12ShaderManager;

		SERIALIZE_SEPARATE(TE::Shader);

	};

}