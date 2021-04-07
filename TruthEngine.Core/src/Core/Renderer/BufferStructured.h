#pragma once
#include "Buffer.h"

namespace TruthEngine
{
	template<typename _ElementStruct, size_t _ElementNum>
	class BufferStructured : public BufferUpload
	{
	public:
		BufferStructured()
			: BufferUpload(sizeof(_ElementStruct) * _ElementNum, TE_RESOURCE_USAGE_STRUCTUREDBUFFER)
		{}
		~BufferStructured() = default;

		std::array<_ElementStruct, _ElementNum>& GetData() noexcept
		{
			return mDataStructs;
		}		

	private:

	private:
		std::array<_ElementStruct, _ElementNum> mDataStructs;
	};

}
