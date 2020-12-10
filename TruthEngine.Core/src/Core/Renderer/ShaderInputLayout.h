#pragma once

enum class TE_RENDERER_SHADER_INPUT_CLASSIFICATION
{
	PER_VERTEX = 0,
	PER_INSTANCE = 1
};

namespace TruthEngine::Core
{


	struct ShaderInputElement
	{
		std::string SemanticName;
		uint32_t SemanticIndex;
		TE_RESOURCE_FORMAT Format;
		uint32_t InputSlot;
		uint32_t AlignedByteOffset;
		TE_RENDERER_SHADER_INPUT_CLASSIFICATION InputSlotClass;
		uint32_t InstanceDataStepRate;
	};

}
