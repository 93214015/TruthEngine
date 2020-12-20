#include "pch.h"
#include "Shader.h"

namespace TruthEngine
{
	namespace Core
	{
		
		Shader::Shader(TE_IDX_SHADERCLASS shaderClassIDX, std::string_view name, std::string_view filePath) : m_Name(name), m_FilePath(filePath), m_ShaderClassIDX(shaderClassIDX)
		{
			
		}

	}
}