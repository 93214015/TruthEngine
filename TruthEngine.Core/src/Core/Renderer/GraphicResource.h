#pragma once


//////////////////////////////////////////////////////////////////////////
// forward declarions
//////////////////////////////////////////////////////////////////////////
namespace TruthEngine
{
	namespace Core {
	}

	namespace API::DirectX12 
	{
		class DX12BufferManager;
	}
}



namespace TruthEngine::Core
{
	class GraphicResource {

	public:

		GraphicResource(const char* name
			, TE_RESOURCE_USAGE usage
			, TE_RESOURCE_TYPE type
			, TE_RESOURCE_STATES initState
		);

		virtual ~GraphicResource() = default;


		inline TE_RESOURCE_TYPE GetType() const noexcept { return m_Type; }
		inline uint32_t GetResourceIndex() const noexcept { return m_ResourceIndex; }
		inline uint32_t GetViewIndex() const noexcept { return m_ViewIndex; }
		inline TE_RESOURCE_STATES GetState() const noexcept { return m_State; }
		inline void SetState(TE_RESOURCE_STATES state) noexcept { m_State = state; }


	protected:
		std::string m_Name;
		uint32_t m_ResourceIndex = 0, m_ViewIndex = 0;
		TE_RESOURCE_USAGE m_Usage = TE_RESOURCE_USAGE_UNKNOWN;
		TE_RESOURCE_TYPE m_Type = TE_RESOURCE_TYPE::UNKNOWN;
		TE_RESOURCE_STATES m_State = TE_RESOURCE_STATES::COMMON;


		//
		// Friend Classes
		//
		friend class BufferManager;
		friend class TruthEngine::API::DirectX12::DX12BufferManager;
	};
}
