#include "pch.h"

#include "Core/Renderer/Shader.h"

using namespace TruthEngine;

namespace boost
{
	namespace serialization
	{
		template<>
		void save(boost::archive::binary_oarchive& ar, const Shader& shader, const unsigned int version)
		{
			ar << shader.m_ID;
			ar << shader.m_FilePath;
			ar << shader.m_VS;
			ar << shader.m_HS;
			ar << shader.m_DS;
			ar << shader.m_GS;
			ar << shader.m_PS;
			ar << shader.m_CS;
		}
	}
}