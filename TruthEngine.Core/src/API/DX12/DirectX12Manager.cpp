#include "pch.h"
#include "DirectX12Manager.h"

#include "DirectX12ShaderManager.h"
#include "DirectX12BufferManager.h"
#include "DirectX12GraphicDevice.h"
#include "DirectX12TextureMaterialManager.h"	

#include "Core/Renderer/Shader.h"
#include "Core/Application.h"

namespace TruthEngine::API::DirectX12
{




	TE_RESULT DirectX12Manager::AddRootSignature(TE_IDX_SHADERCLASS shaderClassIDX)
	{
		auto rootSignaturItr = m_ID3D12RootSignatures.find(shaderClassIDX);

		if (rootSignaturItr != m_ID3D12RootSignatures.end())
		{
			return TE_SUCCESSFUL;
		}

		auto bufferManager = DirectX12BufferManager::GetInstance().get();
		auto& descHeapSRV = bufferManager->m_DescHeapSRV;

		auto shaderSignature = DirectX12ShaderManager::GetInstance()->GetShaderSignature(shaderClassIDX);

		uint32_t paramNum = 0;

		for (auto& v : shaderSignature->ConstantBuffers)
		{
			if (v.size() > 0)
			{
				paramNum++;
			}
		}
		for (auto& v : shaderSignature->Textures)
		{
			if (v.size() > 0)
			{
				paramNum++;
			}
		}

		const auto framesOnTheFlyNum = TE_INSTANCE_APPLICATION->GetFramesOnTheFlyNum();
		auto& rootSig = m_DirectX12RootSignatures[shaderClassIDX];
		auto& rootArg = m_DirectX12RootArguments[shaderClassIDX];

		rootSig.Parameters.resize(paramNum);
		rootArg.resize(framesOnTheFlyNum);

		paramNum = 0;

		for (uint32_t i = 0; i < shaderSignature->ConstantBuffers.size(); ++i)
		{
			auto& v = shaderSignature->ConstantBuffers[i];
			if (v.size() > 0)
			{
				auto idx = v[0].ConstantBufferIDX;

				if (static_cast<IDX>(idx) > static_cast<IDX>(TE_IDX_CONSTANTBUFFER::DIRECT_CONSTANTS))
				{
					rootSig.Parameters[paramNum].Type = RootParameterType::DirectConstant;
					rootSig.Parameters[paramNum].DirectConstant.Register = v[0].Register;
					rootSig.Parameters[paramNum].DirectConstant.RegisterSpace = v[0].RegisterSpace;
					rootSig.Parameters[paramNum].DirectConstant.Num32Bits = bufferManager->GetConstantBufferDirect(idx)->Get32BitNum();

					for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
					{
						rootArg[frameIndex].DircectConstants[idx] = DirectX12RootArgumentDirectConstant(paramNum);
					}

					paramNum++;

					continue;
				}

				rootSig.Parameters[paramNum].Type = RootParameterType::Table;
				rootSig.Parameters[paramNum].Table.NumDescriptor = static_cast<uint32_t>(v.size());
				rootSig.Parameters[paramNum].Table.Register = v[0].Register;
				rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].RegisterSpace;
				rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::CBV;

				for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
				{
					auto cb = bufferManager->GetConstantBufferUpload(v[0].ConstantBufferIDX);
					Core::ConstantBufferView cbv{};
					bufferManager->CreateConstantBufferView(cb, &cbv, frameIndex);
					auto gpuHandle = descHeapSRV.GetGPUHandle(cbv.ViewIndex);

					rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);

					for (uint32_t j = 1; j < v.size(); ++j)
					{
						auto cb = bufferManager->GetConstantBufferUpload(v[j].ConstantBufferIDX);
						Core::ConstantBufferView cbv{};
						bufferManager->CreateConstantBufferView(cb, &cbv, frameIndex);
					}
				}
				paramNum++;
			}
		}

		for (uint32_t i = 0; i < shaderSignature->Textures.size(); ++i)
		{
			auto& v = shaderSignature->Textures[i];
			if (v.size() > 0)
			{

				if (v[0].TextureIDX == TE_IDX_TEXTURE::MATERIALTEXTURES)
				{
					rootSig.Parameters[paramNum].Type = RootParameterType::Table;
					rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::SRV;
					rootSig.Parameters[paramNum].Table.NumDescriptor = 500;
					rootSig.Parameters[paramNum].Table.Register = v[0].Register;
					rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].RegisterSpace;

					auto gpuHandle = static_cast<DirectX12TextureMaterialManager*>(Core::TextureMaterialManager::GetInstance())->GetGPUHandle();

					for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
					{
						rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);
					}

					paramNum++;
					continue;
				}


				rootSig.Parameters[paramNum].Type = RootParameterType::Table;
				rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::SRV;
				rootSig.Parameters[paramNum].Table.NumDescriptor = static_cast<uint32_t>(v.size());
				rootSig.Parameters[paramNum].Table.Register = v[0].Register;
				rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].RegisterSpace;

				auto tex = bufferManager->GetTexture(v[0].TextureIDX);
				Core::ShaderResourceView srv;
				bufferManager->CreateShaderResourceView(tex, &srv);
				auto gpuHandle = descHeapSRV.GetGPUHandle(srv.ViewIndex);

				for (uint8_t frameIndex = 0; frameIndex < framesOnTheFlyNum; ++frameIndex)
				{
					rootArg[frameIndex].Tables.emplace_back(paramNum, gpuHandle);
				}

				for (uint32_t j = 1; j < v.size(); ++j)
				{
					auto tex = bufferManager->GetTexture(v[j].TextureIDX);
					Core::ShaderResourceView srv{};
					bufferManager->CreateShaderResourceView(tex, &srv);
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
				/*ranges[paramNum].BaseShaderRegister = param.Table.Register;
				ranges[paramNum].RegisterSpace = param.Table.RegisterSpace;
				ranges[paramNum].OffsetInDescriptorsFromTableStart = 0;
				ranges[paramNum].NumDescriptors = param.Table.NumDescriptor;
				ranges[paramNum].RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(param.Table.Type);*/

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
		////Define Static Samplers
		//
		D3D12_STATIC_SAMPLER_DESC sampler_desc[2];
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
		sampler_desc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc[1].RegisterSpace = 0;
		sampler_desc[1].ShaderRegister = 1;
		sampler_desc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		sampler_desc[1].MaxAnisotropy = 1;
		sampler_desc[1].MinLOD = 0;
		sampler_desc[1].MaxLOD = D3D12_FLOAT32_MAX;
		sampler_desc[1].MipLODBias = 0;


		COMPTR<ID3DBlob> errorBlob;
		COMPTR<ID3DBlob> signatureBlob;

		/*auto signatureDesc = CD3DX12_ROOT_SIGNATURE_DESC(params.size(), params.data(), 2, sampler_desc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS);*/

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

		/*if (FAILED(D3D12SerializeRootSignature(&signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, signatureBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf())))
		{
			OutputDebugString(L"TE_DX12: the serialization of root signature of Renderer3D is failed!");
			OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
			exit(1);
		}*/

		if (FAILED(TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(m_ID3D12RootSignatures[shaderClassIDX].GetAddressOf()))))
		{
			OutputDebugString(L"the Creation of root signature of Renderer3D is failed!");
			exit(1);
		}
	}

}