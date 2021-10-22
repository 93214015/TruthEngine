#include "pch.h"
#include "Core/Renderer/ShaderCode.h"

#include <boost/serialization/binary_object.hpp>

namespace boost
{
	namespace serialization
	{
		template<>
		void save(archive::binary_oarchive& ar, const TE::ShaderCode& shaderCode, const unsigned int version)
		{
			ar << boost::serialization::binary_object(shaderCode.BufferPointer, shaderCode.BufferSize);
		}

		template<>
		void load(archive::binary_iarchive& ar, TE::ShaderCode& shaderCode, const unsigned int version)
		{
			ar >> binary_object(shaderCode.BufferPointer, shaderCode.BufferSize);
		}
	}
}