#include "pch.h"
#include "TextureMaterial.h"

#include "Core/Renderer/BufferManager.h"

namespace TruthEngine::Core
{
	TextureMaterial::TextureMaterial()
		: GraphicResource(TE_RESOURCE_USAGE::TE_RESOURCE_USAGE_SHADERRESOURCE, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST)
	{}

	TextureMaterial::TextureMaterial(const char* name, uint8_t* pdata, uint32_t width, uint32_t height, size_t dataSize, TE_RESOURCE_FORMAT format)
		: GraphicResource(TE_RESOURCE_USAGE::TE_RESOURCE_USAGE_SHADERRESOURCE, TE_RESOURCE_TYPE::TEXTURE2D, TE_RESOURCE_STATES::COPY_DEST)
		, m_Name(name), m_Data(pdata), m_Width(width), m_Height(height), m_DataSize(dataSize), m_Format(format)
	{
	}

	TextureMaterial::~TextureMaterial()
	{
		delete[] m_Data;
	}

	void TextureMaterialManager::CreateTextureMaterial(const char* name, uint8_t* data, uint32_t width, uint32_t height, uint32_t dataSize, TE_RESOURCE_FORMAT format)
	{

		auto& tex = m_Textures.emplace_back(name, data, width, height, dataSize, format);

		m_Map_Textures[name] = &tex;

		TE_INSTANCE_BUFFERMANAGER->CreateResource(&tex);
	}
}