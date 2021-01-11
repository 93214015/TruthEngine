#include "pch.h"
#include "Shader.h"

namespace TruthEngine
{
	namespace Core
	{
		
		Shader::Shader(std::string_view name, TE_IDX_SHADERCLASS shaderClassIDX, TE_IDX_MESH_TYPE meshType, ShaderSignature* shaderSignature, std::string_view filePath) : m_Name(name), m_FilePath(filePath), m_ShaderClassIDX(shaderClassIDX), m_MeshType(meshType), m_ShaderSignature(shaderSignature)
		{
			
		}

	}
}