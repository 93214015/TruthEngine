#include "pch.h"
#include "TextureCubeMap.h"

TruthEngine::TextureCubeMap::TextureCubeMap(TE_IDX_GRESOURCES _IDX, uint32_t id, const char* name, const char* fileName, uint8_t* pdata, uint32_t width, uint32_t height, size_t dataSize, TE_RESOURCE_FORMAT format)
	: Texture(_IDX, width, height, 6, format, TE_RESOURCE_USAGE::TE_RESOURCE_USAGE_SHADERRESOURCE, TE_RESOURCE_TYPE::TEXTURECUBE, TE_RESOURCE_STATES::COPY_DEST, false)
	, m_ID(id), m_Name(name), m_Data(pdata), m_DataSize(dataSize), m_FileName(fileName)
{

}
