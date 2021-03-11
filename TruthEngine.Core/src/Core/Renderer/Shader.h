#pragma once
#include "ShaderInputLayout.h"
#include "BufferManager.h"

namespace TruthEngine
{
	namespace API
	{
		namespace DirectX12
		{
			class DirectX12ShaderManager;
		}
	}

	
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
			ShaderRangeView(TE_IDX_GRESOURCES _IDX, EShaderRangeType _RangeType)
				: mIDX(_IDX), mShaderRangeType(_RangeType)
			{
				mGraphicResource = TE_INSTANCE_BUFFERMANAGER->GetGraphicResource(mIDX);
				TE_ASSERT_CORE(mGraphicResource || _IDX == TE_IDX_GRESOURCES::Texture_MaterialTextures, "ShaderRangeView: Graphic Resource was not found!");
			}

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
				TE_IDX_GRESOURCES _IDX
				, uint16_t _BaseRegisterSlot
				, uint16_t _BaseRegisterSpace
				, EShaderDirectViewFlags _Flags
				, EShaderVisibility _ShaderVisibility
				, EShaderResourceViewTypes _ShaderResourceFlags)
				: mIDX(_IDX), mBaseRegisterSlot(_BaseRegisterSlot), mBaseRegisterSpace(_BaseRegisterSpace), mFlags(_Flags)
				, mShaderVisibility(_ShaderVisibility), mShaderResourceViewType(_ShaderResourceFlags)
			{
				mGraphicResource = TE_INSTANCE_BUFFERMANAGER->GetGraphicResource(mIDX);
				TE_ASSERT_CORE(mGraphicResource, "ShaderRangeView: Graphic Resource was not found!");
			}

			TE_IDX_GRESOURCES mIDX;

			uint16_t mBaseRegisterSlot;
			uint16_t mBaseRegisterSpace;

			EShaderDirectViewFlags mFlags;
			EShaderVisibility mShaderVisibility;
			EShaderResourceViewTypes mShaderResourceViewType;
			GraphicResource* mGraphicResource;
		};

		struct ShaderConstant
		{
			ShaderConstant(TE_IDX_GRESOURCES _IDX, uint16_t _BaseRegisterSlot, uint16_t _BaseRegisterSpace
				, EShaderVisibility _ShaderVisibility)
				: mIDX(_IDX), mBaseRegisterSlot(_BaseRegisterSlot), mBaseRegisterSpace(_BaseRegisterSpace)
				, mShaderVisibility(_ShaderVisibility), m32BitValuesCount(TE_INSTANCE_BUFFERMANAGER->GetConstantBufferDirect(_IDX)->Get32BitNum())
			{}

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

		std::vector<ShaderInputElement> InputElements[static_cast<uint32_t>(TE_IDX_MESH_TYPE::TOTALNUM)];
		EShaderSignatureFlags mShaderSignatureFlags;

	private:

		std::vector<ShaderParameter> mShaderParameters;

		//friend class
		friend class ShaderManager;

		/*struct ShaderConstantBufferViewSlot
		{
			ShaderConstantBufferViewSlot(uint32_t registerSlot, uint32_t registerSpace, TE_IDX_BUFFER constantBufferIDX)
				: mRegister(registerSlot), mRegisterSpace(registerSpace), mConstantBufferIDX(constantBufferIDX)
			{}

			uint32_t mRegister;
			uint32_t mRegisterSpace;
			TE_IDX_BUFFER mConstantBufferIDX;
		};

		struct ShaderShaderResourceViewSlot
		{
			ShaderShaderResourceViewSlot(uint32_t registerSlot, uint32_t registerSpace, TE_RESOURCE_TYPE resourceType, IDX resourceIDX)
				: mRegister(registerSlot), mRegisterSpace(registerSpace), mResourceType(resourceType), mIDX(resourceIDX)
			{}

			uint32_t mRegister;
			uint32_t mRegisterSpace;

			TE_RESOURCE_TYPE mResourceType;

			IDX mIDX;
		};

		struct ShaderUnorderedAccessViewSlot
		{
			ShaderUnorderedAccessViewSlot(uint32_t registerSlot, uint32_t registerSpace, TE_RESOURCE_TYPE resourceType, IDX resourceIDX)
				: mRegister(registerSlot), mRegisterSpace(registerSpace), mResourceType(resourceType), mIDX(resourceIDX)
			{}

			uint32_t mRegister;
			uint32_t mRegisterSpace;
			TE_RESOURCE_TYPE mResourceType;
			IDX mIDX;
		};

		std::vector<std::vector<ShaderConstantBufferViewSlot>> mCBVs;
		std::vector<std::vector<ShaderShaderResourceViewSlot>> mSRVs;*/

	};

	TE_DEFINE_ENUM_FLAG_OPERATORS(ShaderSignature::EShaderVisibility);
	TE_DEFINE_ENUM_FLAG_OPERATORS(ShaderSignature::EShaderSignatureFlags);

	class ShaderRequiredResources
	{
	public:

		void AddResource(const ShaderSignature* _ShaderSignature);

		inline const std::vector<GraphicResource*>& GetSRVResources()const { return mSRV; }
		inline const std::vector<GraphicResource*>& GetCBVResources()const { return mCBV; }
		inline const std::vector<GraphicResource*>& GetUAVResources()const { return mUAV; }

	private:
		std::vector<GraphicResource*> mSRV;
		std::vector<GraphicResource*> mCBV;
		std::vector<GraphicResource*> mUAV;
	};

	class Shader
	{
	public:
		Shader(std::string_view name, TE_IDX_SHADERCLASS shaderClassIDX, TE_IDX_MESH_TYPE meshType, ShaderSignature* shaderSignature, std::string_view filePath);
		virtual ~Shader() = default;

		Shader(Shader&& shader) noexcept = default;
		Shader& operator=(Shader&& shader) = default;

		struct ShaderCode
		{
			ShaderCode() = default;
			ShaderCode(size_t bufferSize, void* bufferPointer) : BufferSize(bufferSize), BufferPointer(bufferPointer)
			{}

			size_t BufferSize = 0;
			void* BufferPointer = nullptr;
		};

		inline ShaderCode GetVS() const noexcept { return m_VS; };
		inline ShaderCode GetDS() const noexcept { return m_DS; };
		inline ShaderCode GetHS() const noexcept { return m_HS; };
		inline ShaderCode GetGS() const noexcept { return m_GS; };
		inline ShaderCode GetPS() const noexcept { return m_PS; };
		inline ShaderCode GetCS() const noexcept { return m_CS; };

		inline const std::vector<ShaderInputElement>* GetInputElements() const noexcept
		{
			return &m_ShaderSignature->InputElements[(uint32_t)m_MeshType];
		}

		inline const ShaderSignature* GetSignature() const noexcept
		{
			return m_ShaderSignature;
		}


		inline TE_IDX_SHADERCLASS GetShaderClassIDX()const noexcept
		{
			return m_ShaderClassIDX;
		}

		inline TE_IDX_MESH_TYPE GetMeshType()const noexcept
		{
			return m_MeshType;
		}


	protected:

		uint32_t m_ID = 0;
		TE_IDX_SHADERCLASS m_ShaderClassIDX = TE_IDX_SHADERCLASS::NONE;
		TE_IDX_MESH_TYPE m_MeshType = TE_IDX_MESH_TYPE::MESH_NTT;

		ShaderSignature* m_ShaderSignature = nullptr;


		std::string m_Name = "";
		std::string m_FilePath = "";

		ShaderCode m_VS;
		ShaderCode m_HS;
		ShaderCode m_DS;
		ShaderCode m_GS;
		ShaderCode m_PS;
		ShaderCode m_CS;


		friend class ShaderManager;
		friend class API::DirectX12::DirectX12ShaderManager;

	};

}