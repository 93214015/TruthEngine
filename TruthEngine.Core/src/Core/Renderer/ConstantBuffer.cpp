#include "pch.h"
#include "ConstantBuffer.h"

namespace TruthEngine::Core {


	ConstantBuffer::ConstantBuffer(TE_IDX_CONSTANTBUFFER idx)
		: m_IDX(idx)
	{
	}

	ConstantBufferDirectBase::ConstantBufferDirectBase(TE_IDX_CONSTANTBUFFER idx)
		: ConstantBuffer(idx)
	{
	}

	ConstantBufferDirectBase::~ConstantBufferDirectBase()
	{
	}


}