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

namespace TruthEngine
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

		inline const std::vector<VertexData::Pos>& GetPosData() const noexcept
		{
			return mVertexBufferStreamPos.GetData();
		}

		inline size_t GetVertexNum() const noexcept
		{
			return mVertexBufferStreamPos.GetData().size();
		}

		inline std::vector<VertexData::Pos>& GetPosData() noexcept
		{
			return mVertexBufferStreamPos.GetData();
		}

	protected:
		uint32_t m_ID = -1;

		uint32_t m_ViewIndex = -1;

		VertexBufferStream<VertexData::Pos> mVertexBufferStreamPos;


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


		void ReserveSpace(uint32_t vertexNum)
		{
			mVertexBufferStreamPos.ReserveSpace(vertexNum);

			std::apply([vertexNum](auto&&... vertexStream) {((vertexStream.ReserveSpace(vertexNum)), ...); }, m_VertexStreams);
		}

		void AddSpace(uint32_t vertexNum)
		{
			mVertexBufferStreamPos.AddSpace(vertexNum);

			std::apply([vertexNum](auto&&... vertexStream) {((vertexStream.AddSpace(vertexNum)), ...); }, m_VertexStreams);
		}


		inline size_t GetVertexOffset() const noexcept
		{
			return mVertexBufferStreamPos.GetVertexNum();
		}


		void AddVertex(const VertexData::Pos& _Position, const Ts& ...vertex)
		{
			mVertexBufferStreamPos.AddVertex(_Position);
			((std::get<VertexBufferStream<Ts>>(m_VertexStreams).AddVertex(vertex)), ...);
		}

		constexpr uint32_t GetVertexStreamNum() const override
		{
			return m_VertexStreamNum;
		}

		inline std::vector<VertexBufferStreamBase*> GetVertexBufferStreams() override
		{
			std::vector<VertexBufferStreamBase*> vs(m_VertexStreamNum);
			uint32_t i = 0;
			vs[i++] = &mVertexBufferStreamPos;
			((vs[i++] = &std::get<VertexBufferStream<Ts>>(m_VertexStreams)), ...);
			return vs;
		}

		template<class T>
		const std::vector<T>& GetVertexData() const
		{
			return std::get<VertexBufferStream<T>>(m_VertexStreams).GetData();
		}

		template<class T>
		std::vector<T>& GetVertexData()
		{
			return std::get<VertexBufferStream<T>>(m_VertexStreams).GetData();
		}


	protected:
		uint32_t m_VertexBufferResourceIndex = 0;
		uint32_t m_IndexBufferResourceIndex = 0;

	protected:
		static constexpr uint32_t m_VertexStreamNum = (static_cast<uint32_t>(sizeof...(Ts)) + 1);

		std::tuple<VertexBufferStream<Ts>...> m_VertexStreams;


		//
		// friend Classes
		//
		friend class BufferManager;
		friend class API::DirectX12 ::DirectX12BufferManager;

	};

}
