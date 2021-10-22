#pragma once

#include "Core/IO/Serializable.h"

SERIALIZABLE_SEPARATE1(TE, ShaderCode);

namespace TruthEngine
{
	struct ShaderCode
	{
		ShaderCode() = default;
		ShaderCode(size_t bufferSize, void* bufferPointer) : BufferSize(bufferSize), BufferPointer(bufferPointer)
		{}

		ShaderCode(const ShaderCode&) = default;
		ShaderCode& operator=(const ShaderCode&) = default;

		ShaderCode(ShaderCode&& _ShaderCode) noexcept;
		ShaderCode& operator=(ShaderCode&& _ShaderCode) noexcept;

		size_t BufferSize = 0;
		void* BufferPointer = nullptr;

	private:
		void Release();

		friend class Shader;

		SERIALIZE_SEPARATE(TE::ShaderCode);
	};

}
