#include "pch.h"
#include "GraphicResource.h"
#include "BufferManager.h"

namespace TruthEngine::Core {
	GraphicResource::GraphicResource(TE_RESOURCE_USAGE usage, TE_RESOURCE_TYPE type, TE_RESOURCE_STATES initState)
		: m_Usage(usage), m_Type(type), m_State(initState)
	{

	}

	uint64_t GraphicResource::GetRequiredSize() const
	{
		return TE_INSTANCE_BUFFERMANAGER->GetRequiredSize(this);
	}
}