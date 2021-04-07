#pragma once

#include "ConstantBufferStructs.h"

#include "Buffer.h"

#include "Core/Application.h"

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

namespace TruthEngine
{

	class ConstantBuffer
	{
	public:
		ConstantBuffer(TE_IDX_GRESOURCES idx);


		inline TE_IDX_GRESOURCES GetIDX() const noexcept
		{
			return m_IDX;
		}

	protected:

	protected:
		TE_IDX_GRESOURCES m_IDX;
	};
	
	

	class ConstantBufferUploadBase : public ConstantBuffer, public BufferUpload
	{
	public:
		ConstantBufferUploadBase(TE_IDX_GRESOURCES idx, size_t sizeInByte)
			: ConstantBuffer(idx) , BufferUpload(idx, sizeInByte, TE_RESOURCE_USAGE_CONSTANTBUFFER)
		{};

		virtual ~ConstantBufferUploadBase() = default;

		ConstantBufferUploadBase(const ConstantBufferUploadBase&) = default;
		ConstantBufferUploadBase& operator=(const ConstantBufferUploadBase&) = default;

		ConstantBufferUploadBase(ConstantBufferUploadBase&&) noexcept = default;
		ConstantBufferUploadBase& operator=(ConstantBufferUploadBase&&) noexcept = default;

		virtual void UploadData() const = 0;


	protected:
	};

	template<class T>
	class ConstantBufferUpload : public ConstantBufferUploadBase
	{
	public:

		ConstantBufferUpload(TE_IDX_GRESOURCES idx) 
			: ConstantBufferUploadBase(idx, sizeof(T))
		{};
		
		virtual ~ConstantBufferUpload() = default;

		T* GetData() noexcept
		{
			return reinterpret_cast<T*>(m_MappedData[TE_INSTANCE_APPLICATION->GetCurrentFrameIndex()]);
		}

		inline void UploadData() const override
		{
		}


	private:
		//T m_DataStructure;



		//
		//friend classes
		//
		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DirectX12BufferManager;
	};


	class ConstantBufferDirectBase : public ConstantBuffer
	{
	public:
		ConstantBufferDirectBase(TE_IDX_GRESOURCES idx);
		virtual ~ConstantBufferDirectBase();


		virtual uint32_t Get32BitNum() const noexcept = 0;
		virtual const void* GetDataPtr() const noexcept = 0;
	protected:
		

	protected:

	};

	template<class T>
	class ConstantBufferDirect : public ConstantBufferDirectBase
	{
	public:
		using ConstantBufferDirectBase::ConstantBufferDirectBase;

		inline T* GetData()
		{
			return &m_DataStructure;
		}

		const void* GetDataPtr() const noexcept override
		{
			return static_cast<const void*>(&m_DataStructure);
		}

		constexpr uint32_t Num32bit() const noexcept
		{
			return static_cast<uint32_t>(sizeof(T) / 4);
		}

		uint32_t Get32BitNum() const noexcept override
		{
			return this->Num32bit();
		}

	protected:


	protected:
		T m_DataStructure;
	};


}
