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
		ShaderInputElement() = default;
		ShaderInputElement(const char* semanticName
			, uint32_t semanticIndex
			, TE_RESOURCE_FORMAT format
			, uint32_t inputSlot
			, uint32_t alignedByteOffset
			, TE_RENDERER_SHADER_INPUT_CLASSIFICATION inputSlotClass
			, uint32_t instanceDataStepRate)
			: SemanticName(semanticName), SemanticIndex(semanticIndex)
			, Format(format), InputSlot(inputSlot), AlignedByteOffset(alignedByteOffset)
			, InputSlotClass(inputSlotClass), InstanceDataStepRate(instanceDataStepRate)
		{};

		std::string SemanticName;
		uint32_t SemanticIndex;
		TE_RESOURCE_FORMAT Format;
		uint32_t InputSlot;
		uint32_t AlignedByteOffset;
		TE_RENDERER_SHADER_INPUT_CLASSIFICATION InputSlotClass;
		uint32_t InstanceDataStepRate;
	};

}
