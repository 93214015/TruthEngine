#include "pch.h"
#include "Buffer.h"

#include "Core/Application.h"

TruthEngine::Buffer::Buffer(size_t sizeInByte, TE_RESOURCE_USAGE usage, TE_RESOURCE_STATES initState)
	: GraphicResource(usage, TE_RESOURCE_TYPE::BUFFER, initState)
	, m_SizeInByte(static_cast<uint64_t>(sizeInByte))
{
	m_MappedData.resize(TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum());
}

const void* TruthEngine::Buffer::GetDataPtr(uint8_t frameIndex) const noexcept
{
	return m_MappedData[frameIndex];
}
