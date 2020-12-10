#pragma once
#include "Buffer.h"
#include "VertexDataStructures.h"

namespace TruthEngine::Core
{

	class GraphicDevice;

	class VertexBufferStreamBase : public Buffer
	{
	public:
		VertexBufferStreamBase() : Buffer("", 0, TE_RESOURCE_USAGE_VERTEXBUFFER, TE_RESOURCE_STATES::COPY_DEST)
		{};
		virtual ~VertexBufferStreamBase() = default;

		virtual void ReserveSpace(const size_t size) noexcept = 0;

		virtual size_t GetVertexBufferSize() const noexcept = 0;
		virtual size_t GetVertexNum() const noexcept = 0;
		virtual size_t GetVertexSize() const noexcept = 0;

	};

	template<class T>
	class VertexBufferStream : public VertexBufferStreamBase
	{
	public:
		inline void ReserveSpace(const size_t size) noexcept override
		{
			m_Vertecies.reserve(size);
		}
		
		inline size_t GetVertexBufferSize() const noexcept override
		{
			return m_Vertecies.size() * sizeof(T);
		}

		inline size_t GetVertexNum() const noexcept
		{
			return m_Vertecies.size();
		}

		inline size_t GetVertexSize() const noexcept
		{
			return sizeof(T);
		}

		inline void AddVertex(const T& vertex) noexcept
		{
			m_Vertecies.push_back(vertex);
		}

		inline T& AddVertex() noexcept
		{
			return m_Vertecies.emplace_back();
		}

		inline const std::vector<T>& GetData() const noexcept
		{
			return m_Vertecies;
		}

	protected:
		std::vector<T> m_Vertecies;

	};

}