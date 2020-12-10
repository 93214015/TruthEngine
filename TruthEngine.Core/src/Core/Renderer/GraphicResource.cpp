#include "pch.h"
#include "GraphicResource.h"

namespace TruthEngine::Core {
	GraphicResource::GraphicResource(const char* name, TE_RESOURCE_USAGE usage, TE_RESOURCE_TYPE type, TE_RESOURCE_STATES initState)
		: m_Name(name), m_Usage(usage), m_Type(type), m_State(initState)
	{

	}
}