#pragma once
#include "VertexBufferStream.h"

namespace TruthEngine {
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

	class VertexBufferBase
	{
	public:
		virtual uint32_t GetVertexStreamNum() const = 0;
		virtual std::vector<VertexBufferStreamBase*> GetVertexBufferStreams() = 0;

		inline uint32_t GetViewIndex()const noexcept { return m_ViewIndex; }

		inline const uint32_t GetID() const noexcept
		{
			return m_ID;
		}

	protected:
		uint32_t m_ID = 0;

		uint32_t m_ViewIndex = 0;


		//friend classes
		friend class BufferManager;
		friend class API::DirectX12 ::DirectX12BufferManager;
	};


	template<typename ...Ts>
	class VertexBuffer : public VertexBufferBase
	{

	public:
		VertexBuffer() = default;
		VertexBuffer(uint32_t vertexNum, uint32_t indexNum)
		{
			ReserveSpace(vertexNum, indexNum);
		}

		virtual ~VertexBuffer() = default;


		void ReserveSpace(uint32_t vertexNum, uint32_t indexNum)
		{
			m_Indecies.reserve(indexNum);

			std::apply([vertexNum](auto&&... vertexStream) {((vertexStream.ReserveSpace(vertexNum)), ...); }, m_VertexStreams)
		}

		inline void AddIndex(const uint32_t index) noexcept
		{
			m_Indecies.push_back(index);
		}

		inline size_t GetVertexOffset() const noexcept
		{
			return std::get<0>(m_VertexStreams).GetVertexNum();
		}

		inline size_t GetIndexOffset() const noexcept
		{
			return m_Indecies.size();
		}

		void AddVertex(const Ts& ...vertex)
		{
			((std::get<VertexBufferStream<Ts>>(m_VertexStreams).AddVertex(vertex)), ...);
		}

		inline uint32_t GetVertexStreamNum() const override
		{
			return m_VertexStreamNum;
		}

		inline std::vector<VertexBufferStreamBase*> GetVertexBufferStreams() override
		{
			std::vector<VertexBufferStreamBase*> vs(sizeof...(Ts));
			uint32_t i = 0;
			((vs[i++] = &std::get<VertexBufferStream<Ts>>(m_VertexStreams)), ...);
			return vs;
		}

		template<class T>
		const std::vector<T>& GetVertexData() const
		{
			return std::get<VertexBufferStream<T>>(m_VertexStreams).GetData();
		}


	protected:
		uint32_t m_VertexBufferResourceIndex = 0;
		uint32_t m_IndexBufferResourceIndex = 0;

	protected:
		static const uint32_t m_VertexStreamNum = static_cast<uint32_t>(sizeof...(Ts));

		std::vector<uint32_t> m_Indecies;

		std::tuple<VertexBufferStream<Ts>...> m_VertexStreams;




		//
		// friend Classes
		//
		friend class BufferManager;
		friend class API::DirectX12 ::DirectX12BufferManager;

	};

}
