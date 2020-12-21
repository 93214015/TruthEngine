#pragma once

#include "ConstantBufferStructs.h"

#include "Buffer.h"

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12 
		{
			class DirectX12BufferManager;
		}
	}
}

namespace TruthEngine::Core
{

	class ConstantBufferUploadBase : public BufferUpload
	{
	public:
		ConstantBufferUploadBase(size_t sizeInByte)
			: BufferUpload(sizeInByte, TE_RESOURCE_USAGE_CONSTANTBUFFER)
		{};

		virtual ~ConstantBufferUploadBase() = default;

		ConstantBufferUploadBase(const ConstantBufferUploadBase&) = default;
		ConstantBufferUploadBase& operator=(const ConstantBufferUploadBase&) = default;

		ConstantBufferUploadBase(ConstantBufferUploadBase&&) noexcept = default;
		ConstantBufferUploadBase& operator=(ConstantBufferUploadBase&&) noexcept = default;

		virtual void UploadData() const = 0;

		virtual const void* GetDataPointer() const noexcept = 0;

	protected:
	};

	template<class T>
	class ConstantBufferUpload : public ConstantBufferUploadBase
	{
	public:

		ConstantBufferUpload() 
			: ConstantBufferUploadBase(sizeof(T))
		{};
		
		virtual ~ConstantBufferUpload() = default;

		T* GetData() noexcept
		{
			return reinterpret_cast<T*>(m_MappedData);
		}

		inline void UploadData() const override
		{
			//memcpy(m_MappedData, &m_DataStructure, sizeof(T));
		}

		inline const void* GetDataPointer() const noexcept override
		{
			return static_cast<void*>(m_MappedData);
		}

	private:
		uint32_t m_ID;
		//T m_DataStructure;



		//
		//friend classes
		//
		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DirectX12BufferManager;
	};

	

}
