#pragma once


//////////////////////////////////////////////////////////////////////////
// forward declarions
//////////////////////////////////////////////////////////////////////////
namespace TruthEngine
{
	namespace API::DirectX12 
	{
		class DirectX12BufferManager;
	}
}



namespace TruthEngine
{
	class GraphicResource {

	public:

		GraphicResource(
			TE_IDX_GRESOURCES _IDX
			,TE_RESOURCE_USAGE usage
			, TE_RESOURCE_TYPE type
			, TE_RESOURCE_STATES initState
		);

		virtual ~GraphicResource() = default;


		inline TE_RESOURCE_TYPE GetType() const noexcept { return m_Type; }
		inline uint32_t GetResourceIndex() const noexcept { return m_ResourceIndex; }
		/*inline uint32_t GetViewIndex() const noexcept { return m_ViewIndex; }*/
		inline TE_RESOURCE_STATES GetState() const noexcept { return m_State; }
		inline TE_RESOURCE_USAGE GetUsage() const noexcept { return m_Usage; }
		inline void SetState(TE_RESOURCE_STATES state) noexcept { m_State = state; }
		inline TE_IDX_GRESOURCES GetIDX()const noexcept { return m_IDX; }

		uint64_t GetRequiredSize() const;

	protected:
		uint32_t m_ResourceIndex = -1; /* , m_ViewIndex = -1;*/
		TE_RESOURCE_USAGE m_Usage = TE_RESOURCE_USAGE_UNKNOWN;
		TE_RESOURCE_TYPE m_Type = TE_RESOURCE_TYPE::UNKNOWN;
		TE_RESOURCE_STATES m_State = TE_RESOURCE_STATES::COMMON;
		TE_IDX_GRESOURCES m_IDX = TE_IDX_GRESOURCES::NONE;


		//
		// Friend Classes
		//
		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DirectX12BufferManager;
	};
}
