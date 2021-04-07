#include "pch.h"
#include "ConstantBuffer.h"

namespace TruthEngine {


	ConstantBuffer::ConstantBuffer(TE_IDX_GRESOURCES idx)
		: m_IDX(idx)
	{
	}

	ConstantBufferDirectBase::ConstantBufferDirectBase(TE_IDX_GRESOURCES idx)
		: ConstantBuffer(idx)
	{
	}

	ConstantBufferDirectBase::~ConstantBufferDirectBase()
	{
	}


}