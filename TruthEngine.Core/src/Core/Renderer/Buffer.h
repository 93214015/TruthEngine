#pragma once
#include "GraphicResource.h"

namespace TruthEngine
{
	namespace API::DirectX12 
	{
		class DirectX12BufferManager;
	}
}

namespace TruthEngine::Core
{


	class Buffer : public GraphicResource
	{
	public:
		Buffer(const char* name
			, size_t sizeInByte
			, TE_RESOURCE_USAGE usage
			, TE_RESOURCE_STATES initState)
			: GraphicResource(name, usage, TE_RESOURCE_TYPE::BUFFER, initState)
			, m_SizeInByte(static_cast<uint64_t>(sizeInByte))
		{}
		virtual ~Buffer() = default;


		inline uint64_t GetSizeInByte() const noexcept
		{
			return m_SizeInByte;
		}

		inline void SetSizeInByte(uint64_t size) noexcept
		{
			m_SizeInByte = size;
		}

	protected:
		uint64_t m_SizeInByte = 0;


	};



	class BufferUpload : public Buffer
	{
	public:

		BufferUpload(const char* name
			, size_t sizeInByte
			, TE_RESOURCE_USAGE usage)
			: Buffer(name, sizeInByte, usage, TE_RESOURCE_STATES::GENERIC_READ0)
		{}

		virtual ~BufferUpload() = default;

		BufferUpload(const BufferUpload&) = default;
		BufferUpload& operator=(const BufferUpload&) = default;

		BufferUpload(BufferUpload&&) noexcept = default;
		BufferUpload& operator=(BufferUpload&&) noexcept = default;

		inline void SetData(void* data) noexcept
		{
			m_DataSource = data;
		}

	protected:
		uint8_t* m_MappedData = nullptr;
		void* m_DataSource;

		//friend classes
		friend class BufferManager;
		friend class API::DirectX12 ::DirectX12BufferManager;
	};



	class BufferUploadIntermediate : public BufferUpload
	{
	public:
		BufferUploadIntermediate(const char* name, size_t sizeInByte)
			: BufferUpload(name, sizeInByte, TE_RESOURCE_USAGE_INTERMEDIATEUPLOADBBUFFER)
		{}

		virtual ~BufferUploadIntermediate() = default;

		BufferUploadIntermediate(const BufferUploadIntermediate&) = default;
		BufferUploadIntermediate& operator=(const BufferUploadIntermediate&) = default;

		BufferUploadIntermediate(BufferUploadIntermediate&&) noexcept = default;
		BufferUploadIntermediate& operator=(BufferUploadIntermediate&&) noexcept = default;

	
	protected:

	};



	class BufferDefault : public Buffer 
	{
	public:
		using Buffer::Buffer;

		virtual ~BufferDefault() = default;

		BufferDefault(const BufferDefault&) = default;
		BufferDefault& operator=(const BufferDefault&) = default;

		BufferDefault(BufferDefault&&) noexcept = default;
		BufferDefault& operator=(BufferDefault&&) noexcept = default;
	};
}
