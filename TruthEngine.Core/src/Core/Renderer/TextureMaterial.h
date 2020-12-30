#pragma once
#include "GraphicResource.h"

namespace TruthEngine::Core
{
	
	class TextureMaterial : GraphicResource
	{
	public:
		TextureMaterial();
		TextureMaterial(const char* name, uint8_t* pdata, uint32_t width, uint32_t height, size_t dataSize , TE_RESOURCE_FORMAT format);
		~TextureMaterial();

		inline uint8_t* GetData() const noexcept
		{
			return m_Data;
		}

		inline uint32_t GetWidth()const noexcept
		{
			return m_Width;
		}

		inline uint32_t GetHeight()const noexcept
		{
			return m_Height;
		}

		inline TE_RESOURCE_FORMAT GetFormat()const noexcept
		{
			return m_Format;
		}

		inline size_t GetDataSize()const noexcept
		{
			return m_DataSize;
		}

	protected:

	protected:
		std::string m_Name = "";

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		TE_RESOURCE_FORMAT m_Format = TE_RESOURCE_FORMAT::UNKNOWN;
		size_t m_DataSize = 0;

		uint8_t* m_Data = nullptr;
	};

	class TextureMaterialManager
	{
	public:

		inline void AddSpace(size_t size)
		{
			m_Textures.reserve(m_Textures.size() + size);
		}
		inline size_t GetOffset() const noexcept
		{
			return m_Textures.size();
		}
		void CreateTextureMaterial(const char* name, uint8_t* data, uint32_t width, uint32_t height, uint32_t dataSize, TE_RESOURCE_FORMAT format);

		static TextureMaterialManager* GetInstance()
		{
			static TextureMaterialManager s_Instance;
			return &s_Instance;
		}

	protected:

	protected:
		std::vector<TextureMaterial> m_Textures;
		std::unordered_map<const char*, TextureMaterial*> m_Map_Textures;
	};

}