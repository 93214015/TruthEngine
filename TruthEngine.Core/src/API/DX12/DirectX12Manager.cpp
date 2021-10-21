#include "pch.h"
#include "DirectX12Manager.h"

#include "DirectX12ShaderManager.h"
#include "DirectX12BufferManager.h"
#include "DirectX12GraphicDevice.h"
#include "DirectX12TextureMaterialManager.h"	

#include "Core/Renderer/Shader.h"
#include "Core/Application.h"
#include "Core/Renderer/ShaderManager.h"

namespace TruthEngine::API::DirectX12
{

	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...)->overload<Ts...>;

	inline D3D12_SHADER_VISIBILITY GetD3D12ShaderVisibility(const ShaderSignature::EShaderVisibility _ShaderVisibility)
	{
		return static_cast<D3D12_SHADER_VISIBILITY>(_ShaderVisibility);
	}

	inline D3D12_DESCRIPTOR_RANGE_TYPE GetD3D12RangeType(const ShaderSignature::EShaderRangeType _RangeType)
	{
		return static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(_RangeType);
	}

	inline D3D12_DESCRIPTOR_RANGE_FLAGS GetD3D12RangeFlag(const ShaderSignature::EShaderRangeFlags _RangeFlag)
	{
		return static_cast<D3D12_DESCRIPTOR_RANGE_FLAGS>(_RangeFlag);
	}

	inline D3D12_ROOT_DESCRIPTOR_FLAGS GetD3D12DescriptorFlag(const ShaderSignature::EShaderDirectViewFlags& _Flag)
	{
		return static_cast<D3D12_ROOT_DESCRIPTOR_FLAGS>(_Flag);
	}

	inline D3D12_ROOT_SIGNATURE_FLAGS GetD3D12SignatureFlag(const ShaderSignature::EShaderSignatureFlags _Flags)
	{
		return static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(_Flags);
	}

	std::tuple<CD3DX12_GPU_DESCRIPTOR_HANDLE, uint32_t> _CreateResourceView(const ShaderSignature::ShaderRangeView& _ShaderRangeView, const uint32_t _FrameIndex)
	{
		if (_ShaderRangeView.mIDX == TE_IDX_GRESOURCES::Texture_MaterialTextures)
		{
			return { DirectX12TextureMaterialManager::GetInstance()->GetGPUHandle(), DirectX12TextureMaterialManager::GetInstance()->GetIndexOffset() };
		}

		CD3DX12_GPU_DESCRIPTOR_HANDLE _GPUDescHandle;
		uint32_t _ViewIndex;

		switch (_ShaderRangeView.mShaderRangeType)
		{
		case ShaderSignature::EShaderRangeType::SRV:
		{
			ShaderResourceView _SRV{};
			TE_INSTANCE_API_DX12_BUFFERMANAGER->CreateShaderResourceView(_ShaderRangeView.mGraphicResource, &_SRV, _FrameIndex);
			_ViewIndex = _SRV.ViewIndex;
			_GPUDescHandle = TE_INSTANCE_API_DX12_BUFFERMANAGER->GetDescriptorHeapSRV().GetGPUHandle(_SRV.ViewIndex);
			break;
		}
		case ShaderSignature::EShaderRangeType::CBV:
		{
			ConstantBufferView cbv{};
			TE_INSTANCE_API_DX12_BUFFERMANAGER->CreateConstantBufferView(static_cast<Buffer*>(_ShaderRangeView.mGraphicResource), &cbv, _FrameIndex);
			_ViewIndex = cbv.ViewIndex;
			_GPUDescHandle = TE_INSTANCE_API_DX12_BUFFERMANAGER->GetDescriptorHeapSRV().GetGPUHandle(cbv.ViewIndex);
			break;
		}
		case ShaderSignature::EShaderRangeType::UAV:
		{
			UnorderedAccessView _UAV{};
			TE_INSTANCE_API_DX12_BUFFERMANAGER->CreateUnorderedAccessView(static_cast<Buffer*>(_ShaderRangeView.mGraphicResource), &_UAV);
			_ViewIndex = _UAV.ViewIndex;
			_GPUDescHandle = TE_INSTANCE_API_DX12_BUFFERMANAGER->GetDescriptorHeapSRV().GetGPUHandle(_UAV.ViewIndex);
			break;
		}
		default:
			TE_LOG_CORE_ERROR("Shader View Resource Usage Was Invalid!");
			break;
		}

		return { _GPUDescHandle, _ViewIndex };
	}

	TE_RESULT DirectX12Manager::AddRootSignature(TE_IDX_SHADERCLASS shaderClassIDX)
	{
		auto rootSignaturItr = m_ID3D12RootSignatures.find(shaderClassIDX);

		if (rootSignaturItr != m_ID3D12RootSignatures.end())
		{
			return TE_SUCCESSFUL;
		}

		auto _BufferManager = DirectX12BufferManager::GetInstance().get();
		auto& _DescHeapSRV = _BufferManager->m_DescHeapSRV;

		auto _ShaderSignature = DirectX12ShaderManager::GetInstance()->GetShaderSignature(shaderClassIDX);

		const auto _FramesOnTheFlyNum = TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum();
		/*auto& _RootSig = m_DirectX12RootSignatures[shaderClassIDX];*/
		auto& _RootArgsPerFrame = m_DirectX12RootArguments[shaderClassIDX];
		_RootArgsPerFrame.resize(_FramesOnTheFlyNum);

		std::vector<CD3DX12_ROOT_PARAMETER1> _D3D12RootParameters(_ShaderSignature->GetParameterNum());

		uint32_t _ParamNum = 0;
		std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE1>> _D3DRangesHolder;

		auto _Lambda_ProcessTable = [&_D3D12RootParameters, &_ParamNum, &_RootArgsPerFrame, &_DescHeapSRV, &_D3DRangesHolder, _FramesOnTheFlyNum](const ShaderSignature::ShaderTable& _ShaderTable)
		{
			//Create Table RootParameter

			std::vector<CD3DX12_DESCRIPTOR_RANGE1>& _D3DRanges = _D3DRangesHolder.emplace_back();
			_D3DRanges.reserve(_ShaderTable.mShaderRanges.size());

			for (const auto& _Range : _ShaderTable.mShaderRanges)
			{
				_D3DRanges.emplace_back().Init
				(
					GetD3D12RangeType(_Range.mShaderRangeType)
					, _Range.GetShaderViewNum()
					, _Range.mBaseRegisterSlot
					, _Range.mBaseRegisterSpace
					, GetD3D12RangeFlag(_Range.mFlags)
				);
			}

			_D3D12RootParameters[_ParamNum].InitAsDescriptorTable(_D3DRanges.size(), _D3DRanges.data(), GetD3D12ShaderVisibility(_ShaderTable.mShaderVisibility));

			//Create Root Arguments Table for each On the fly frame 

			for (uint8_t frameIndex = 0; frameIndex < _FramesOnTheFlyNum; ++frameIndex)
			{
				auto [_GPUDescHandle, _HeapStartOffset] = _CreateResourceView(_ShaderTable.mShaderRanges[0].GetShaderViews()[0], frameIndex);

				for (uint32_t i = 1; i < _ShaderTable.mShaderRanges[0].GetShaderViews().size(); ++i)
				{
					_CreateResourceView(_ShaderTable.mShaderRanges[0].GetShaderViews()[i], frameIndex);
				}

				for (uint32_t _ShaderRangeIndex = 1; _ShaderRangeIndex < _ShaderTable.mShaderRanges.size(); ++_ShaderRangeIndex)
				{
					const ShaderSignature::ShaderRange& _Range = _ShaderTable.mShaderRanges[_ShaderRangeIndex];
					for (const ShaderSignature::ShaderRangeView& _ShaderRangeView : _Range.GetShaderViews())
					{
						_CreateResourceView(_ShaderRangeView, frameIndex);
					}
				}

				_RootArgsPerFrame[frameIndex].Tables.emplace_back(_ParamNum, _HeapStartOffset, _GPUDescHandle);
			}

			_ParamNum++;
		};

		auto _Lambda_ProcessDirectView = [&_D3D12RootParameters, &_ParamNum, &_RootArgsPerFrame, _FramesOnTheFlyNum](const ShaderSignature::ShaderDirectView& _ShaderResourceView)
		{
			switch (_ShaderResourceView.mShaderResourceViewType)
			{
			case ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_CBV:
			{
				_D3D12RootParameters[_ParamNum].InitAsConstantBufferView(
					_ShaderResourceView.mBaseRegisterSlot
					, _ShaderResourceView.mBaseRegisterSpace
					, GetD3D12DescriptorFlag(_ShaderResourceView.mFlags)
					, GetD3D12ShaderVisibility(_ShaderResourceView.mShaderVisibility)
				);

				for (uint8_t frameIndex = 0; frameIndex < _FramesOnTheFlyNum; ++frameIndex)
				{
					_RootArgsPerFrame[frameIndex].DescriptorCBV[_ShaderResourceView.mBaseRegisterSlot] = DirectX12RootArgumentDescriptor(_ParamNum);
				}

				break;
			}
			case ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_SRV_Buffer:
			case ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_SRV_Texture:
			{
				_D3D12RootParameters[_ParamNum].InitAsShaderResourceView(
					_ShaderResourceView.mBaseRegisterSlot
					, _ShaderResourceView.mBaseRegisterSpace
					, GetD3D12DescriptorFlag(_ShaderResourceView.mFlags)
					, GetD3D12ShaderVisibility(_ShaderResourceView.mShaderVisibility)
				);

				for (uint8_t frameIndex = 0; frameIndex < _FramesOnTheFlyNum; ++frameIndex)
				{
					_RootArgsPerFrame[frameIndex].DescriptorSRV[_ShaderResourceView.mBaseRegisterSlot] = DirectX12RootArgumentDescriptor(_ParamNum);
				}

				break;
			}
			case ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_UAV_Buffer:
			case ShaderSignature::EShaderResourceViewTypes::EShaderResourceFlags_UAV_Texture:
			{
				_D3D12RootParameters[_ParamNum].InitAsUnorderedAccessView(
					_ShaderResourceView.mBaseRegisterSlot
					, _ShaderResourceView.mBaseRegisterSpace
					, GetD3D12DescriptorFlag(_ShaderResourceView.mFlags)
					, GetD3D12ShaderVisibility(_ShaderResourceView.mShaderVisibility)
				);

				for (uint8_t frameIndex = 0; frameIndex < _FramesOnTheFlyNum; ++frameIndex)
				{
					_RootArgsPerFrame[frameIndex].DescriptorUAV[_ShaderResourceView.mBaseRegisterSlot] = DirectX12RootArgumentDescriptor(_ParamNum);
				}

				break;
			}
			default:
				throw;
				break;
			}

			_ParamNum++;
		};

		auto _Lambda_ProcessDirectConstants = [&_D3D12RootParameters, &_ParamNum, &_RootArgsPerFrame, _FramesOnTheFlyNum](const ShaderSignature::ShaderConstant& _ShaderConstant)
		{
			_D3D12RootParameters[_ParamNum].InitAsConstants(_ShaderConstant.m32BitValuesCount, _ShaderConstant.mBaseRegisterSlot, _ShaderConstant.mBaseRegisterSpace, GetD3D12ShaderVisibility(_ShaderConstant.mShaderVisibility));

			for (uint8_t frameIndex = 0; frameIndex < _FramesOnTheFlyNum; ++frameIndex)
			{
				_RootArgsPerFrame[frameIndex].DircectConstants[_ShaderConstant.mIDX] = DirectX12RootArgumentDirectConstant(_ParamNum);
			}

			_ParamNum++;
		};

		for (const ShaderSignature::ShaderParameter& _ShaderParameter : _ShaderSignature->GetShaderParameters())
		{
			std::visit
			(
				overload
				{
					[](const std::monostate&) { throw; },
					_Lambda_ProcessDirectView,
					_Lambda_ProcessTable,
					_Lambda_ProcessDirectConstants
				},
				_ShaderParameter.mParameter
			);
		}

		//
		//Define Static Samplers
		//
		D3D12_STATIC_SAMPLER_DESC sampler_desc[8];

		size_t _SamplerIndex = 0;


		// Linear - Wrap

		{

			auto& _SameplerDesc = sampler_desc[_SamplerIndex];

			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			_SameplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MaxAnisotropy = 1;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;

		}

		// Linear - clamp

		{

			auto& _SameplerDesc = sampler_desc[_SamplerIndex];

			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			_SameplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MaxAnisotropy = 1;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;

		}

		// Point - Border Black

		{

			auto& _SameplerDesc = sampler_desc[_SamplerIndex];

			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			_SameplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MaxAnisotropy = 1;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;
		}

		// Point - Border White

		{

			auto& _SameplerDesc = sampler_desc[_SamplerIndex];

			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			_SameplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MaxAnisotropy = 1;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;

		}

		// Point - Wrap


		{
			auto& _SameplerDesc = sampler_desc[_SamplerIndex];

			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			_SameplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MaxAnisotropy = 1;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;

		}

		//Anisotropic 16X

		{
			auto& _SameplerDesc = sampler_desc[_SamplerIndex];

			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			_SameplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			_SameplerDesc.Filter = D3D12_FILTER_COMPARISON_ANISOTROPIC;
			_SameplerDesc.MaxAnisotropy = 16u;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;
		}

		// Comparoison Sampler - Greater

		{
			auto& _SameplerDesc = sampler_desc[_SamplerIndex];


			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
			_SameplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MaxAnisotropy = 1;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;

		}

		// Comparoison Sampler - Less

		{
			auto& _SameplerDesc = sampler_desc[_SamplerIndex];

			_SameplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			_SameplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
			_SameplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
			_SameplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			_SameplerDesc.RegisterSpace = 0;
			_SameplerDesc.ShaderRegister = _SamplerIndex;
			_SameplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			_SameplerDesc.MaxAnisotropy = 1;
			_SameplerDesc.MinLOD = 0;
			_SameplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			_SameplerDesc.MipLODBias = 0;

			_SamplerIndex++;
		}
			


		COMPTR<ID3DBlob> errorBlob;
		COMPTR<ID3DBlob> signatureBlob;


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC signatureDesc;
		signatureDesc.Init_1_1(_D3D12RootParameters.size(), _D3D12RootParameters.data(), _countof(sampler_desc), sampler_desc, GetD3D12SignatureFlag(_ShaderSignature->mShaderSignatureFlags));

		if (FAILED(D3DX12SerializeVersionedRootSignature(&signatureDesc, TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFeature_RootSignature().HighestVersion, signatureBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf())))
		{
			OutputDebugString(L"TE_DX12: the serialization of root signature of Renderer3D is failed!");
			OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
			exit(1);
		}

		if (FAILED(TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(m_ID3D12RootSignatures[shaderClassIDX].GetAddressOf()))))
		{
			OutputDebugString(L"the Creation of root signature of Renderer3D is failed!");
			exit(1);
		}
	}


}