#include "pch.h"
#include "Buffer.h"

#include "Core/Application.h"

TruthEngine::Buffer::Buffer(TE_IDX_GRESOURCES _IDX, size_t sizeInByte, TE_RESOURCE_USAGE usage, TE_RESOURCE_STATES initState, uint32_t elementNum, uint32_t elementSizeInByte, bool isByteAddress, bool isUploadHeapBuffer)
	: GraphicResource(_IDX, usage, TE_RESOURCE_TYPE::BUFFER, initState)
	, m_SizeInByte(static_cast<uint64_t>(sizeInByte)), m_ElementNum(elementNum), m_ElementSizeInByte(elementSizeInByte), m_IsByteAddress(isByteAddress), m_IsUploadHeapBuffer(isUploadHeapBuffer)
{
	m_MappedData.resize(TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum());
}

const void* TruthEngine::Buffer::GetDataPtr(uint8_t frameIndex) const noexcept
{
	return m_MappedData[frameIndex];
}
