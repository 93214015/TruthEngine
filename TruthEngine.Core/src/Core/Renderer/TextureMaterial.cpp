#include "pch.h"
#include "TextureMaterial.h"

#include "Core/Renderer/BufferManager.h"

namespace TruthEngine::Core
{

	TextureMaterial::TextureMaterial(uint32_t id, const char* name, const char* fileName, uint8_t* pdata, uint32_t width, uint32_t height, size_t dataSize, TE_RESOURCE_FORMAT format)
		: Texture(width, height, format, TE_RESOURCE_USAGE::TE_RESOURCE_USAGE_SHADERRESOURCE, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST)
		, m_ID(id), m_Name(name), m_Data(pdata), m_DataSize(dataSize), m_FileName(fileName)
	{
	}


	TextureMaterial::~TextureMaterial()
	{
		delete[] m_Data;
	}

	
}
