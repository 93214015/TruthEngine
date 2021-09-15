#include "pch.h"
#include "ShaderSignature.h"

#include "BufferManager.h"

namespace TruthEngine
{
	ShaderSignature::ShaderRangeView::ShaderRangeView(TE_IDX_GRESOURCES _IDX, EShaderRangeType _RangeType)
		: mIDX(_IDX), mShaderRangeType(_RangeType)
	{
		mGraphicResource = TE_INSTANCE_BUFFERMANAGER->GetGraphicResource(mIDX);
		TE_ASSERT_CORE(mGraphicResource || _IDX == TE_IDX_GRESOURCES::Texture_MaterialTextures, "ShaderRangeView: Graphic Resource was not found!");
	}
	ShaderSignature::ShaderConstant::ShaderConstant(TE_IDX_GRESOURCES _IDX, uint16_t _BaseRegisterSlot, uint16_t _BaseRegisterSpace, EShaderVisibility _ShaderVisibility)
		: mIDX(_IDX), mBaseRegisterSlot(_BaseRegisterSlot), mBaseRegisterSpace(_BaseRegisterSpace)
		, mShaderVisibility(_ShaderVisibility), m32BitValuesCount(TE_INSTANCE_BUFFERMANAGER->GetConstantBufferDirect(_IDX)->Get32BitNum())
	{}
}