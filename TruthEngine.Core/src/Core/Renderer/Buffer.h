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
		Buffer(TE_IDX_GRESOURCES _IDX, size_t sizeInByte, TE_RESOURCE_USAGE usage, TE_RESOURCE_STATES initState, uint32_t elementNum, uint32_t elementSizeInByte, bool isByteAddress, bool isUploadHeapBuffer);
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

		inline uint32_t GetElementNum() const noexcept
		{
			return m_ElementNum;
		}

		inline uint32_t GetElementSizeInByte() const noexcept
		{
			return m_ElementSizeInByte;
		}

		inline bool IsByteAddress() const noexcept
		{
			return m_IsByteAddress;
		}

		const void* GetDataPtr(uint8_t frameIndex) const noexcept;
	

	protected:
		size_t m_SizeInByte = 0;
		uint32_t m_ElementNum = 0;
		uint32_t m_ElementSizeInByte = 0;
		bool m_IsByteAddress = false;
		bool m_IsUploadHeapBuffer = false;

		std::vector<uint8_t*> m_MappedData;



		//friend classes
		friend class BufferManager;
		friend class API::DirectX12::DirectX12BufferManager;
	};



	class BufferUpload : public Buffer
	{
	public:

		BufferUpload(TE_IDX_GRESOURCES _IDX, size_t sizeInByte , TE_RESOURCE_USAGE usage)
			: Buffer(_IDX, sizeInByte, usage, TE_RESOURCE_STATES::GENERIC_READ0, 0, 0, false, true)
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
