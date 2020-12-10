#pragma once

#include "Buffer.h"

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12 
		{
			class DX12BufferManager;
		}
	}
}

namespace TruthEngine::Core
{

	class ConstantBufferUploadBase : public BufferUpload
	{
	public:
		ConstantBufferUploadBase(const char* name, size_t sizeInByte)
			: BufferUpload(name, sizeInByte, TE_RESOURCE_USAGE_CONSTANTBUFFER)
		{};

		virtual ~ConstantBufferUploadBase() = default;

		ConstantBufferUploadBase(const ConstantBufferUploadBase&) = default;
		ConstantBufferUploadBase& operator=(const ConstantBufferUploadBase&) = default;

		ConstantBufferUploadBase(ConstantBufferUploadBase&&) noexcept = default;
		ConstantBufferUploadBase& operator=(ConstantBufferUploadBase&&) noexcept = default;

		virtual void UploadData() const = 0;

		virtual void* GetDataPointer() const noexcept = 0;

	protected:
	};

	template<class T>
	class ConstantBufferUpload : public ConstantBufferUploadBase
	{
	public:

		ConstantBufferUpload(const char* name) 
			: ConstantBufferUploadBase(name, sizeof(T))
		{};
		
		virtual ~ConstantBufferUpload() = default;

		T& GetData() noexcept
		{
			return m_DataStructure;
		}

		inline void UploadData() const override
		{
			memcpy(m_MappedData, &m_DataStructure, sizeof(T));
		}

		inline void* GetDataPointer() const noexcept override
		{
			return &m_DataStructure;
		}

	private:
		T m_DataStructure;



		//
		//friend classes
		//
		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DX12BufferManager;
	};

}
