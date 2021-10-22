#include "pch.h"
#include "ShaderCode.h"

namespace TruthEngine
{
	ShaderCode::ShaderCode(ShaderCode&& _ShaderCode) noexcept
	{
		BufferSize = _ShaderCode.BufferSize;
		BufferPointer = _ShaderCode.BufferPointer;

		_ShaderCode.BufferSize = 0;
		_ShaderCode.BufferPointer = nullptr;
	}

	ShaderCode& ShaderCode::operator=(ShaderCode&& _ShaderCode) noexcept
	{
		BufferSize = _ShaderCode.BufferSize;
		BufferPointer = _ShaderCode.BufferPointer;

		_ShaderCode.BufferSize = 0;
		_ShaderCode.BufferPointer = nullptr;

		return *this;
	}

	void ShaderCode::Release()
	{
		BufferSize = 0;
		delete BufferPointer;
		BufferPointer = nullptr;
	}
}