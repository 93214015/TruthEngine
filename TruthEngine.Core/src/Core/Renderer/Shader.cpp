#include "pch.h"
#include "Shader.h"

namespace TruthEngine
{

	Shader::Shader(const char* name, TE_IDX_SHADERCLASS shaderClassIDX, ShaderSignature* shaderSignature, std::string_view filePath) 
		: m_Name(name), m_FilePath(filePath), m_ShaderClassIDX(shaderClassIDX), m_ShaderSignature(shaderSignature)
	{
	}


	TruthEngine::ShaderSignature::ShaderParameter& ShaderSignature::AddParameter()
	{
		ShaderSignature::ShaderParameter& _Parameter = mShaderParameters.emplace_back(static_cast<uint32_t>(mShaderParameters.size()));
		return _Parameter;
	}


	void ShaderRequiredResources::AddResource(const ShaderSignature* _ShaderSignature)
	{
		for (const ShaderSignature::ShaderParameter& _ShaderParameter : _ShaderSignature->GetShaderParameters())
		{
			if (const ShaderSignature::ShaderTable* _ShaderTable = std::get_if<ShaderSignature::ShaderTable>(&_ShaderParameter.mParameter); _ShaderTable)
			{
				for (const ShaderSignature::ShaderRange& _ShaderRange : _ShaderTable->mShaderRanges)
				{

					switch (_ShaderRange.mShaderRangeType)
					{
					case ShaderSignature::EShaderRangeType::SRV:
					{
						for (const ShaderSignature::ShaderRangeView& _ShaderRangeView : _ShaderRange.GetShaderViews())
						{
							if (_ShaderRangeView.mGraphicResource)
								mSRV.emplace_back(_ShaderRangeView.mGraphicResource);
						}
						break;
					}
					case ShaderSignature::EShaderRangeType::CBV:
					{
						for (const ShaderSignature::ShaderRangeView& _ShaderRangeView : _ShaderRange.GetShaderViews())
						{
							if (_ShaderRangeView.mGraphicResource)
								mCBV.emplace_back(_ShaderRangeView.mGraphicResource);
						}
						break;
					}
					case ShaderSignature::EShaderRangeType::UAV:
					{
						for (const ShaderSignature::ShaderRangeView& _ShaderRangeView : _ShaderRange.GetShaderViews())
						{
							if (_ShaderRangeView.mGraphicResource)
								mUAV.emplace_back(_ShaderRangeView.mGraphicResource);
						}
						break;
					}
					}
				}
			}
			/*else if (const ShaderSignature::ShaderDirectView* _ShaderResourceView = std::get_if<ShaderSignature::ShaderDirectView>(&_ShaderParameter.mParameter); _ShaderResourceView)
			{
				switch (_ShaderResourceView->mShaderResourceViewType)
				{
				case ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_SRV_Buffer:
				case ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_SRV_Texture:
					mSRV.emplace_back(_ShaderResourceView->mGraphicResource);
					break;
				case  ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_CBV:
					mCBV.emplace_back(_ShaderResourceView->mGraphicResource);
					break;
				case  ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_UAV_Buffer:
				case  ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_UAV_Texture:
					mUAV.emplace_back(_ShaderResourceView->mGraphicResource);
					break;
				default:
					break;
				}
			}*/
		}
	}


}