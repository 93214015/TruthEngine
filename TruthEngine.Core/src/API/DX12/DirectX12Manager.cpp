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

	/*TE_RESULT DirectX12Manager::AddRootSignature(TE_IDX_SHADERCLASS shaderClassIDX)
	{
		auto rootSignaturItr = m_ID3D12RootSignatures.find(shaderClassIDX);

		if (rootSignaturItr != m_ID3D12RootSignatures.end())
		{
			return TE_SUCCESSFUL;
		}

		auto _BufferManager = DirectX12BufferManager::GetInstance().get();
		auto& descHeapSRV = _BufferManager->m_DescHeapSRV;

		auto shaderSignature = DirectX12ShaderManager::GetInstance()->GetShaderSignature(shaderClassIDX);

		uint32_t paramNum = 0;

		for (auto& v : shaderSignature->mShaderViews)
		{
			if (v.size() > 0)
			{
				paramNum++;
			}
		}

		/ *for (auto& v : shaderSignature->mCBVs)
		{
			if (v.size() > 0)
			{
				paramNum++;
			}
		}
		for (auto& v : shaderSignature->mSRVs)
		{
			if (v.size() > 0)
			{
				paramNum++;
			}
		}* /

		const auto framesOnTheFlyNum = TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum();
		auto& rootSig = m_DirectX12RootSignatures[shaderClassIDX];
		auto& rootArg = m_DirectX12RootArguments[shaderClassIDX];

		rootSig.Parameters.resize(paramNum);
		rootArg.resize(framesOnTheFlyNum);

		paramNum = 0;

		uint8_t _SRVIndex = 0;
		uint8_t _CBVIndex = 0;
		uint8_t _UAVIndex = 0;


		for (uint32_t i = 0; i < shaderSignature->mShaderViews.size(); ++i)
		{

			auto& _ShaderViews = shaderSignature->mShaderViews[i];

			if (_ShaderViews.size() > 0)
			{

				for (auto& _ShaderView : _ShaderViews)
				{

					if (_ShaderView.mShaderResourceFlag == TE_SHADER_RESOURCE_FLAG_DIRECTCONSTANT)
					{
						TE_IDX_BUFFER _IDX = static_cast<TE_IDX_BUFFER>(_ShaderView.mIDX);
						rootSig.Parameters[paramNum].Type = RootParameterType::DirectConstant;
						rootSig.Parameters[paramNum].DirectConstant.Register = _ShaderView.mRegisterSlot;
						rootSig.Parameters[paramNum].DirectConstant.RegisterSpace = _ShaderView.mRegisterSpace;
						rootSig.Parameters[paramNum].DirectConstant.Num32Bits = _BufferManager->GetConstantBufferDirect(_IDX)->Get32BitNum();

						for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
						{
							rootArg[frameIndex].DircectConstants[_IDX] = DirectX12RootArgumentDirectConstant(paramNum);
						}

						paramNum++;

						continue;
					}


					rootSig.Parameters[paramNum].Type = RootParameterType::Table;
					rootSig.Parameters[paramNum].Table.NumDescriptor = static_cast<uint32_t>(_ShaderViews.size());
					rootSig.Parameters[paramNum].Table.Register = _ShaderView.mRegisterSlot;
					rootSig.Parameters[paramNum].Table.RegisterSpace = _ShaderView.mRegisterSpace;
					rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::CBV;


					switch (_ShaderView.mShaderResourceFlag)
					{

					case TE_SHADER_RESOURCE_FLAG_SRV:
					{

						break;
					}
					case TE_SHADER_RESOURCE_FLAG_CBV:
					{

						TE_IDX_BUFFER _IDX = static_cast<TE_IDX_BUFFER>(_ShaderView.mIDX);

						if (_ShaderView.mIDX > static_cast<IDX>(TE_IDX_BUFFER::DIRECT_CONSTANTS))
						{
							rootSig.Parameters[paramNum].Type = RootParameterType::DirectConstant;
							rootSig.Parameters[paramNum].DirectConstant.Register = _ShaderView.mRegisterSlot;
							rootSig.Parameters[paramNum].DirectConstant.RegisterSpace = _ShaderView.mRegisterSpace;
							rootSig.Parameters[paramNum].DirectConstant.Num32Bits = _BufferManager->GetConstantBufferDirect(_IDX)->Get32BitNum();

							for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
							{
								rootArg[frameIndex].DircectConstants[_IDX] = DirectX12RootArgumentDirectConstant(paramNum);
							}

							paramNum++;

							continue;
						}

						rootSig.Parameters[paramNum].Type = RootParameterType::Table;
						rootSig.Parameters[paramNum].Table.NumDescriptor = static_cast<uint32_t>(_ShaderViews.size());
						rootSig.Parameters[paramNum].Table.Register = _ShaderView.mRegisterSlot;
						rootSig.Parameters[paramNum].Table.RegisterSpace = _ShaderView.mRegisterSpace;
						rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::CBV;

						for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
						{
							auto cb = _BufferManager->GetConstantBufferUpload(_IDX);
							ConstantBufferView cbv{};
							_BufferManager->CreateConstantBufferView(cb, &cbv, frameIndex);
							auto gpuHandle = descHeapSRV.GetGPUHandle(cbv.ViewIndex);

							rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);

							for (uint32_t j = 1; j < _ShaderViews.size(); ++j)
							{
								auto cb = _BufferManager->GetConstantBufferUpload(_ShaderViews[j].mIDX);
								ConstantBufferView cbv{};
								_BufferManager->CreateConstantBufferView(cb, &cbv, frameIndex);
							}
						}
						paramNum++;

						break;
					}
					case TE_SHADER_RESOURCE_FLAG_UAV:
					{



						break;
					}
					default:
						TE_LOG_CORE_ERROR("Shader View Resource Usage Was Invalid!");
						break;
					}

				}

			}

		}

		/ *for (uint32_t i = 0; i < shaderSignature->mCBVs.size(); ++i)
		{
			auto& v = shaderSignature->mCBVs[i];
			if (v.size() > 0)
			{
				auto idx = v[0].mConstantBufferIDX;

				if (static_cast<IDX>(idx) > static_cast<IDX>(TE_IDX_BUFFER::DIRECT_CONSTANTS))
				{
					rootSig.Parameters[paramNum].Type = RootParameterType::DirectConstant;
					rootSig.Parameters[paramNum].DirectConstant.Register = v[0].mRegister;
					rootSig.Parameters[paramNum].DirectConstant.RegisterSpace = v[0].mRegisterSpace;
					rootSig.Parameters[paramNum].DirectConstant.Num32Bits = _BufferManager->GetConstantBufferDirect(idx)->Get32BitNum();

					for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
					{
						rootArg[frameIndex].DircectConstants[idx] = DirectX12RootArgumentDirectConstant(paramNum);
					}

					paramNum++;

					continue;
				}

				rootSig.Parameters[paramNum].Type = RootParameterType::Table;
				rootSig.Parameters[paramNum].Table.NumDescriptor = static_cast<uint32_t>(v.size());
				rootSig.Parameters[paramNum].Table.Register = v[0].mRegister;
				rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].mRegisterSpace;
				rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::CBV;

				for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
				{
					auto cb = _BufferManager->GetConstantBufferUpload(v[0].mConstantBufferIDX);
					ConstantBufferView cbv{};
					_BufferManager->CreateConstantBufferView(cb, &cbv, frameIndex);
					auto gpuHandle = descHeapSRV.GetGPUHandle(cbv.ViewIndex);

					rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);

					for (uint32_t j = 1; j < v.size(); ++j)
					{
						auto cb = _BufferManager->GetConstantBufferUpload(v[j].mConstantBufferIDX);
						ConstantBufferView cbv{};
						_BufferManager->CreateConstantBufferView(cb, &cbv, frameIndex);
					}
				}
				paramNum++;
			}
		}* /

		for (uint32_t i = 0; i < shaderSignature->mSRVs.size(); ++i)
		{
			auto& v = shaderSignature->mSRVs[i];
			if (v.size() > 0)
			{

				rootSig.Parameters[paramNum].Type = RootParameterType::Table;
				rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::SRV;
				rootSig.Parameters[paramNum].Table.NumDescriptor = static_cast<uint32_t>(v.size());
				rootSig.Parameters[paramNum].Table.Register = v[0].mRegister;
				rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].mRegisterSpace;

				if (v[0].mResourceType == TE_RESOURCE_TYPE::TEXTURE2D)
				{
					if (IsIDXTextureMaterialTexture(static_cast<TE_IDX_TEXTURE>(v[0].mIDX)))
					{

						rootSig.Parameters[paramNum].Table.NumDescriptor = 500;

						/ *rootSig.Parameters[paramNum].Type = RootParameterType::Table;
						rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::SRV;
						rootSig.Parameters[paramNum].Table.Register = v[0].mRegister;
						rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].mRegisterSpace;* /

						auto gpuHandle = static_cast<DirectX12TextureMaterialManager*>(TextureMaterialManager::GetInstance())->GetGPUHandle();

						for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
						{
							rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);
						}

						paramNum++;
						continue;
					}

					auto tex = _BufferManager->GetTexture(static_cast<TE_IDX_TEXTURE>(v[0].mIDX));
					ShaderResourceView srv;
					_BufferManager->CreateShaderResourceView(tex, &srv);
					auto gpuHandle = descHeapSRV.GetGPUHandle(srv.ViewIndex);

					for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
					{
						rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);
					}
				}

				if (v[0].mResourceType == TE_RESOURCE_TYPE::BUFFER)
				{
					auto _Buffer = _BufferManager->GetBuffer(static_cast<TE_IDX_BUFFER>(v[0].mIDX));
					ShaderResourceView srv;
					_BufferManager->CreateShaderResourceView(_Buffer, &srv);
					auto gpuHandle = descHeapSRV.GetGPUHandle(srv.ViewIndex);

					for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
					{
						rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);
					}
				}

				for (uint32_t j = 1; j < v.size(); ++j)
				{
					switch (v[j].mResourceType)
					{
					case TE_RESOURCE_TYPE::BUFFER:
						auto _Buffer = _BufferManager->GetBuffer(static_cast<TE_IDX_BUFFER>(v[j].mIDX));
						ShaderResourceView srv;
						_BufferManager->CreateShaderResourceView(_Buffer, &srv);
						break;
					case TE_RESOURCE_TYPE::TEXTURE2D:
						auto _Texture = _BufferManager->GetTexture(static_cast<TE_IDX_TEXTURE>(v[j].mIDX));
						ShaderResourceView srv{};
						_BufferManager->CreateShaderResourceView(_Texture, &srv);
						break;
					default:
						break;
					}
				}

				paramNum++;
			}
		}

		std::vector<CD3DX12_ROOT_PARAMETER1> params(rootSig.Parameters.size());
		std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges(paramNum);

		paramNum = 0;

		for (auto& param : rootSig.Parameters)
		{

			switch (param.Type)
			{
			case RootParameterType::Table:
			{
				/ *ranges[paramNum].BaseShaderRegister = param.Table.Register;
				ranges[paramNum].RegisterSpace = param.Table.RegisterSpace;
				ranges[paramNum].OffsetInDescriptorsFromTableStart = 0;
				ranges[paramNum].NumDescriptors = param.Table.NumDescriptor;
				ranges[paramNum].RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(param.Table.Type);* /

				ranges[paramNum].Init(static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(param.Table.Type), param.Table.NumDescriptor, param.Table.Register, param.Table.RegisterSpace, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

				params[paramNum].InitAsDescriptorTable(1, &ranges[paramNum], D3D12_SHADER_VISIBILITY_ALL);
				paramNum++;
				break;
			}
			case RootParameterType::DirectConstant:
			{
				params[paramNum].InitAsConstants(param.DirectConstant.Num32Bits, param.DirectConstant.Register, param.DirectConstant.RegisterSpace, D3D12_SHADER_VISIBILITY_ALL);
				paramNum++;
				break;
			}
			}
		}

		//
		//Define Static Samplers
		//
		D3D12_STATIC_SAMPLER_DESC sampler_desc[4];

		sampler_desc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler_desc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc[0].RegisterSpace = 0;
		sampler_desc[0].ShaderRegister = 0;
		sampler_desc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		sampler_desc[0].MaxAnisotropy = 1;
		sampler_desc[0].MinLOD = 0;
		sampler_desc[0].MaxLOD = D3D12_FLOAT32_MAX;
		sampler_desc[0].MipLODBias = 0;

		sampler_desc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		sampler_desc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler_desc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc[1].RegisterSpace = 0;
		sampler_desc[1].ShaderRegister = 1;
		sampler_desc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		sampler_desc[1].MaxAnisotropy = 1;
		sampler_desc[1].MinLOD = 0;
		sampler_desc[1].MaxLOD = D3D12_FLOAT32_MAX;
		sampler_desc[1].MipLODBias = 0;

		sampler_desc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		sampler_desc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler_desc[2].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc[2].RegisterSpace = 0;
		sampler_desc[2].ShaderRegister = 2;
		sampler_desc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		sampler_desc[2].MaxAnisotropy = 1;
		sampler_desc[2].MinLOD = 0;
		sampler_desc[2].MaxLOD = D3D12_FLOAT32_MAX;
		sampler_desc[2].MipLODBias = 0;

		sampler_desc[3].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[3].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[3].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler_desc[3].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		sampler_desc[3].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
		sampler_desc[3].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc[3].RegisterSpace = 0;
		sampler_desc[3].ShaderRegister = 3;
		sampler_desc[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		sampler_desc[3].MaxAnisotropy = 1;
		sampler_desc[3].MinLOD = 0;
		sampler_desc[3].MaxLOD = D3D12_FLOAT32_MAX;
		sampler_desc[3].MipLODBias = 0;


		COMPTR<ID3DBlob> errorBlob;
		COMPTR<ID3DBlob> signatureBlob;

		/ *auto signatureDesc = CD3DX12_ROOT_SIGNATURE_DESC(params.size(), params.data(), 2, sampler_desc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS);* /

		D3D12_ROOT_SIGNATURE_FLAGS signatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC signatureDesc;
		signatureDesc.Init_1_1(params.size(), params.data(), _countof(sampler_desc), sampler_desc, signatureFlags);

		if (FAILED(D3DX12SerializeVersionedRootSignature(&signatureDesc, TE_INSTANCE_API_DX12_GRAPHICDEVICE.GetFeature_RootSignature().HighestVersion, signatureBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf())))
		{
			OutputDebugString(L"TE_DX12: the serialization of root signature of Renderer3D is failed!");
			OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
			exit(1);
		}

		/ *if (FAILED(D3D12SerializeRootSignature(&signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, signatureBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf())))
		{
			OutputDebugString(L"TE_DX12: the serialization of root signature of Renderer3D is failed!");
			OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
			exit(1);
		}* /

		if (FAILED(TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(m_ID3D12RootSignatures[shaderClassIDX].GetAddressOf()))))
		{
			OutputDebugString(L"the Creation of root signature of Renderer3D is failed!");
			exit(1);
		}
	}*/


}