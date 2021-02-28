#pragma once
#include "GraphicResource.h"



namespace TruthEngine
{
	namespace API::DirectX12
	{
		class DirectX12BufferManager;
	}
}

namespace TruthEngine
{


	class Buffer : public GraphicResource
	{
	public:
		Buffer(size_t sizeInByte, TE_RESOURCE_USAGE usage, TE_RESOURCE_STATES initState);
			/*Buffer(size_t sizeInByte, TE_RESOURCE_USAGE usage, TE_RESOURCE_STATES initState)
			: GraphicResource(usage, TE_RESOURCE_TYPE::BUFFER, initState)
			, m_SizeInByte(static_cast<uint64_t>(sizeInByte))*/
		

		virtual ~Buffer() = default;


		inline uint64_t GetSizeInByte() const noexcept
		{
			return m_SizeInByte;
		}

		inline void SetSizeInByte(uint64_t size) noexcept
		{
			m_SizeInByte = size;
		}

		const void* GetDataPtr(uint8_t frameIndex) const noexcept;
	

	protected:
		std::vector<uint8_t*> m_MappedData;
		uint64_t m_SizeInByte = 0;

	};



	class BufferUpload : public Buffer
	{
	public:

		BufferUpload(size_t sizeInByte , TE_RESOURCE_USAGE usage)
			: Buffer(sizeInByte, usage, TE_RESOURCE_STATES::GENERIC_READ0)
		{}

		virtual ~BufferUpload() = default;

		BufferUpload(const BufferUpload&) = default;
		BufferUpload& operator=(const BufferUpload&) = default;

		BufferUpload(BufferUpload&&) noexcept = default;
		BufferUpload& operator=(BufferUpload&&) noexcept = default;



	protected:

		//friend classes
		friend class BufferManager;
		friend class API::DirectX12::DirectX12BufferManager;
	};



	class BufferUploadIntermediate : public BufferUpload
	{
	public:
		BufferUploadIntermediate(size_t sizeInByte)
			: BufferUpload(sizeInByte, TE_RESOURCE_USAGE_INTERMEDIATEUPLOADBBUFFER)
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
