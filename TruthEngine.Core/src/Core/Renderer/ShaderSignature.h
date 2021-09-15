#pragma once
#include "ShaderInputLayout.h"


namespace TruthEngine
{
	class ShaderSignature
	{
	public:
		enum EShaderVisibility : TE_FLAG_TYPE
		{
			EShaderVisibility_ALL = 0,
			EShaderVisibility_VERTEX = 1,
			EShaderVisibility_HULL = 2,
			EShaderVisibility_DOMAIN = 3,
			EShaderVisibility_GEOMETRY = 4,
			EShaderVisibility_PIXEL = 5,
			EShaderVisibility_AMPLIFICATION = 6,
			EShaderVisibility_MESH = 7
		};

		enum class EShaderRangeType : TE_FLAG_TYPE
		{
			SRV = 0,
			UAV = (SRV + 1),
			CBV = (UAV + 1),
			SAMPLER = (CBV + 1)
		};

		enum class EShaderResourceViewTypes : TE_FLAG_TYPE
		{
			EShaderResourceFlags_SRV_Buffer = 1 << 0,
			EShaderResourceFlags_SRV_Texture = 1 << 1,
			EShaderResourceFlags_CBV = 1 << 2,
			EShaderResourceFlags_DirectConstant = 1 << 3,
			EShaderResourceFlags_UAV_Buffer = 1 << 4,
			EShaderResourceFlags_UAV_Texture = 1 << 5
		};

		enum class EShaderRangeFlags : TE_FLAG_TYPE
		{
			EShaderRangeFlags_NONE = 0,
			EShaderRangeFlags_DESCRIPTORS_VOLATILE = 0x1,
			EShaderRangeFlags_DATA_VOLATILE = 0x2,
			EShaderRangeFlags_DATA_STATIC_WHILE_SET_AT_EXECUTE = 0x4,
			EShaderRangeFlags_DATA_STATIC = 0x8,
			EShaderRangeFlags_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS = 0x10000
		};

		enum class EShaderDirectViewFlags : TE_FLAG_TYPE
		{
			EShaderResourceViewFlag_NONE = 0,
			EShaderResourceViewFlag_DATA_VOLATILE = 0x2,
			EShaderResourceViewFlag_DATA_STATIC_WHILE_SET_AT_EXECUTE = 0x4,
			EShaderResourceViewFlag_DATA_STATIC = 0x8
		};

		enum EShaderSignatureFlags : TE_FLAG_TYPE
		{
			EShaderSignatureFlags_NONE = 0,
			EShaderSignatureFlags_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT = 0x1,
			EShaderSignatureFlags_DENY_VERTEX_SHADER_ROOT_ACCESS = 0x2,
			EShaderSignatureFlags_DENY_HULL_SHADER_ROOT_ACCESS = 0x4,
			EShaderSignatureFlags_DENY_DOMAIN_SHADER_ROOT_ACCESS = 0x8,
			EShaderSignatureFlags_DENY_GEOMETRY_SHADER_ROOT_ACCESS = 0x10,
			EShaderSignatureFlags_DENY_PIXEL_SHADER_ROOT_ACCESS = 0x20,
			EShaderSignatureFlags_ALLOW_STREAM_OUTPUT = 0x40,
			EShaderSignatureFlags_LOCAL_ROOT_SIGNATURE = 0x80,
			EShaderSignatureFlags_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS = 0x100,
			EShaderSignatureFlags_DENY_MESH_SHADER_ROOT_ACCESS = 0x200
		};

		struct ShaderRangeView
		{
			ShaderRangeView(TE_IDX_GRESOURCES _IDX, EShaderRangeType _RangeType);

			TE_IDX_GRESOURCES mIDX;

			EShaderRangeType mShaderRangeType;

			GraphicResource* mGraphicResource;
		};

		struct ShaderRange
		{
			ShaderRange(uint16_t _BaseRegisterSlot, uint16_t _BaseRegisterSpace, EShaderRangeType _RangeType, EShaderRangeFlags _Flags, const std::vector<ShaderRangeView>& _ShaderViews, uint32_t _ShaderViewNum = -1)
				: mBaseRegisterSlot(_BaseRegisterSlot), mBaseRegisterSpace(_BaseRegisterSpace), mShaderRangeType(_RangeType), mFlags(_Flags), mShaderViews(_ShaderViews), mShaderViewNum(_ShaderViewNum)
			{}

			uint16_t mBaseRegisterSlot;
			uint16_t mBaseRegisterSpace;

			EShaderRangeType mShaderRangeType;
			EShaderRangeFlags mFlags;

			const std::vector<ShaderRangeView>& GetShaderViews()const
			{
				return mShaderViews;
			}

			uint32_t GetShaderViewNum()const
			{
				return mShaderViewNum != -1 ? mShaderViewNum : static_cast<uint32_t>(mShaderViews.size());
			}


		private:
			std::vector<ShaderRangeView> mShaderViews;
			uint32_t mShaderViewNum;
		};

		struct ShaderTable
		{
			ShaderTable(EShaderVisibility _ShaderVisibility, const std::vector<ShaderRange>& _ShaderViewRanges)
				: mShaderVisibility(_ShaderVisibility), mShaderRanges(_ShaderViewRanges)
			{}

			size_t GetViewsCount() const
			{
				size_t _Result = 0;

				for (auto& v : mShaderRanges)
				{
					_Result += v.GetShaderViewNum();
				}

				return _Result;
			}

			std::vector<ShaderRange> mShaderRanges;
			EShaderVisibility mShaderVisibility;
		};

		struct ShaderDirectView
		{

			ShaderDirectView(
				uint16_t _BaseRegisterSlot
				, uint16_t _BaseRegisterSpace
				, EShaderDirectViewFlags _Flags
				, EShaderVisibility _ShaderVisibility
				, EShaderResourceViewTypes _ShaderResourceFlags)
				: mBaseRegisterSlot(_BaseRegisterSlot), mBaseRegisterSpace(_BaseRegisterSpace), mFlags(_Flags)
				, mShaderVisibility(_ShaderVisibility), mShaderResourceViewType(_ShaderResourceFlags)
			{
			}

			uint16_t mBaseRegisterSlot;
			uint16_t mBaseRegisterSpace;

			EShaderDirectViewFlags mFlags;
			EShaderVisibility mShaderVisibility;
			EShaderResourceViewTypes mShaderResourceViewType;
		};

		struct ShaderConstant
		{
			ShaderConstant(TE_IDX_GRESOURCES _IDX
				, uint16_t _BaseRegisterSlot
				, uint16_t _BaseRegisterSpace
				, EShaderVisibility _ShaderVisibility);

			TE_IDX_GRESOURCES mIDX;

			uint16_t mBaseRegisterSlot;
			uint16_t mBaseRegisterSpace;

			EShaderVisibility mShaderVisibility;

			uint32_t m32BitValuesCount;

		};

		struct ShaderParameter
		{
			ShaderParameter(uint32_t _ParameterIndex)
				: mParameterIndex(_ParameterIndex)
			{}

			uint32_t mParameterIndex;

			std::variant<std::monostate, ShaderTable, ShaderDirectView, ShaderConstant> mParameter = std::monostate();
		};

		ShaderParameter& AddParameter();
		const std::vector<ShaderParameter>& GetShaderParameters() const
		{
			return mShaderParameters;
		}
		inline size_t GetParameterNum()const
		{
			return mShaderParameters.size();
		}

		//std::vector<ShaderInputElement> InputElements[static_cast<uint32_t>(TE_IDX_MESH_TYPE::TOTALNUM)];
		EShaderSignatureFlags mShaderSignatureFlags;

	private:

		std::vector<ShaderParameter> mShaderParameters;

		//friend class
		friend class ShaderManager;

	};

	TE_DEFINE_ENUM_FLAG_OPERATORS(ShaderSignature::EShaderVisibility);
	TE_DEFINE_ENUM_FLAG_OPERATORS(ShaderSignature::EShaderSignatureFlags);
}