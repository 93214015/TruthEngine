#include "Texture.h"
#pragma once

namespace TruthEngine
{
	namespace API::DirectX12
	{
		class DirectX12BufferManager;
		class DirectX12TextureMaterialManager;
	}

	class TextureMaterial : Texture
	{
	public:
		TextureMaterial(uint32_t id, const char* name, const char* fileName, uint8_t* pdata, uint32_t width, uint32_t height, size_t dataSize, TE_RESOURCE_FORMAT format);
		~TextureMaterial();

		inline uint32_t GetID()const noexcept
		{
			return m_ID;
		}

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

		inline uint32_t GetViewIndex()const noexcept
		{
			return m_ViewIndex;
		}

	protected:

	protected:
		uint32_t m_ID = -1;
		std::string m_Name = "";
		std::string m_FileName = "";

		size_t m_DataSize = 0;

		uint8_t* m_Data = nullptr;

		uint32_t m_ViewIndex = 0;

		//
		// Friend Classes
		//
		friend class TextureMaterialManager;
		friend class API::DirectX12::DirectX12BufferManager;
		friend class API::DirectX12::DirectX12TextureMaterialManager;
	};


	

}