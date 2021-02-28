#pragma once
#include "Buffer.h"

namespace TruthEngine
{
	namespace API::DirectX12 
	{
		class DirectX12BufferManager;
	}

		class IndexBuffer : public Buffer
		{
		public:
			IndexBuffer() : Buffer(0, TE_RESOURCE_USAGE_INDEXBUFFER, TE_RESOURCE_STATES::COPY_DEST)
			{}

			inline size_t GetBufferSize() const noexcept
			{
				return m_Indecies.size() * sizeof(uint32_t);
			}

			inline void AddIndex(uint32_t index) noexcept
			{
				m_Indecies.push_back(index);
			}

			inline size_t GetIndexOffset() const noexcept
			{
				return m_Indecies.size();
			}

			inline const std::vector<uint32_t>& GetIndecies() const
			{
				return m_Indecies;
			}

			inline const uint32_t GetID() const noexcept
			{
				return m_ID;
			}

			inline const void* GetDataPtr() const noexcept
			{
				return m_Indecies.data();
			}

			inline uint32_t GetViewIndex() const noexcept
			{
				return m_ViewIndex;
			}

			inline void ReserveSpace(uint32_t size) noexcept
			{
				m_Indecies.reserve(size);
			}

			inline void AddSpace(uint32_t size) noexcept
			{
				auto currentSpace = m_Indecies.size();
				m_Indecies.reserve(currentSpace + size);
			}

		protected:
			uint32_t m_ID;
			uint32_t m_ViewIndex;
			std::vector<uint32_t> m_Indecies;


			//friend classes
			friend class BufferManager;
			friend class API::DirectX12 ::DirectX12BufferManager;
		};
}
