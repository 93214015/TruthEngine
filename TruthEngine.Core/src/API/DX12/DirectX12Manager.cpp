#include "pch.h"
#include "DirectX12Manager.h"

#include "DirectX12ShaderManager.h"
#include "DirectX12BufferManager.h"
#include "DirectX12GraphicDevice.h"
#include "DirectX12TextureMaterialManager.h"	

#include "Core/Renderer/Shader.h"

namespace TruthEngine::API::DirectX12
{




	TE_RESULT DirectX12Manager::AddRootSignature(Core::Shader* shader)
	{
		auto shaderClassIDX = shader->GetShaderClassIDX();
		auto rootSignaturItr = m_ID3D12RootSignatures.find(shader->GetShaderClassIDX());

		if (rootSignaturItr != m_ID3D12RootSignatures.end())
		{
			return TE_SUCCESSFUL;
		}

		auto bufferManager = DirectX12BufferManager::GetInstance().get();
		auto& descHeapSRV = bufferManager->m_DescHeapSRV;

		auto bindedResource = DirectX12ShaderManager::GetInstance()->GetBindedResource(shaderClassIDX);

		uint32_t paramNum = 0;

		for (auto& v : bindedResource->ConstantBuffers)
		{
			if (v.size() > 0)
			{
				paramNum++;
			}
		}
		for (auto& v : bindedResource->Textures)
		{
			if (v.size() > 0)
			{
				paramNum++;
			}
		}


		auto& rootSig = m_DirectX12RootSignatures[shaderClassIDX];
		auto& rootArg = m_DirectX12RootArguments[shaderClassIDX];
		rootSig.Parameters.resize(paramNum);

		paramNum = 0;

		for (uint32_t i = 0; i < bindedResource->ConstantBuffers.size(); ++i)
		{
			auto& v = bindedResource->ConstantBuffers[i];
			if (v.size() > 0)
			{
				auto idx = v[0].ConstantBufferIDX;

				if (static_cast<IDX>(idx) > static_cast<IDX>(TE_IDX_CONSTANTBUFFER::DIRECT_CONSTANTS))
				{
					rootSig.Parameters[paramNum].Type = RootParameterType::DirectConstant;
					rootSig.Parameters[paramNum].DirectConstant.Register = v[0].Register;
					rootSig.Parameters[paramNum].DirectConstant.RegisterSpace = v[0].RegisterSpace;
					rootSig.Parameters[paramNum].DirectConstant.Num32Bits = bufferManager->GetConstantBufferDirect(idx)->Get32BitNum();

					rootArg.DircectConstants[idx] = DirectX12RootArgumentDirectConstant(paramNum);
					paramNum++;

					continue;
				}

				rootSig.Parameters[paramNum].Type = RootParameterType::Table;
				rootSig.Parameters[paramNum].Table.NumDescriptor = static_cast<uint32_t>(v.size());
				rootSig.Parameters[paramNum].Table.Register = v[0].Register;
				rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].RegisterSpace;
				rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::CBV;

				auto cb = bufferManager->GetConstantBufferUpload(v[0].ConstantBufferIDX);
				Core::ConstantBufferView cbv{};
				bufferManager->CreateConstantBufferView(cb, &cbv);
				auto gpuHandle = descHeapSRV.GetGPUHandle(cbv.ViewIndex);

				rootArg.Tables.emplace_back(paramNum, gpuHandle);

				for (uint32_t j = 1; j < v.size(); ++j)
				{
					auto cb = bufferManager->GetConstantBufferUpload(v[j].ConstantBufferIDX);
					Core::ConstantBufferView cbv{};
					bufferManager->CreateConstantBufferView(cb, &cbv);
				}

				paramNum++;
			}
		}

		for (uint32_t i = 0; i < bindedResource->Textures.size(); ++i)
		{
			auto& v = bindedResource->Textures[i];
			if (v.size() > 0)
			{
				if (v[0].TextureIDX == TE_IDX_TEXTURE::MATERIALTEXTURE_DIFFUSE)
				{
					rootSig.Parameters[paramNum].Type = RootParameterType::Table;
					rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::SRV;
					rootSig.Parameters[paramNum].Table.NumDescriptor = 50;
					rootSig.Parameters[paramNum].Table.Register = v[0].Register;
					rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].RegisterSpace;

					auto gpuHandle = static_cast<DirectX12TextureMaterialManager*>(Core::TextureMaterialManager::GetInstance())->GetGPUHandle_Diffuse();

					rootArg.Tables.emplace_back(paramNum, gpuHandle);

					paramNum++;
					continue;
				}
				else if (v[0].TextureIDX == TE_IDX_TEXTURE::MATERIALTEXTURE_NORMAL)
				{
					rootSig.Parameters[paramNum].Type = RootParameterType::Table;
					rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::SRV;
					rootSig.Parameters[paramNum].Table.NumDescriptor = 50;
					rootSig.Parameters[paramNum].Table.Register = v[0].Register;
					rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].RegisterSpace;

					auto gpuHandle = static_cast<DirectX12TextureMaterialManager*>(Core::TextureMaterialManager::GetInstance())->GetGPUHandle_Normal();

					rootArg.Tables.emplace_back(paramNum, gpuHandle);

					paramNum++;
					continue;
				}
				else if (v[0].TextureIDX == TE_IDX_TEXTURE::MATERIALTEXTURE_DISPLACEMENT)
				{
					rootSig.Parameters[paramNum].Type = RootParameterType::Table;
					rootSig.Parameters[paramNum].Table.Type = RootParameterDescriptorType::SRV;
					rootSig.Parameters[paramNum].Table.NumDescriptor = 50;
					rootSig.Parameters[paramNum].Table.Register = v[0].Register;
					rootSig.Parameters[paramNum].Table.RegisterSpace = v[0].RegisterSpace;

					auto gpuHandle = static_cast<DirectX12TextureMaterialManager*>(Core::TextureMaterialManager::GetInstance())->GetGPUHandle_Displacement();

					rootArg.Tables.emplace_back(paramNum, gpuHandle);

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

				rootArg.Tables.emplace_back(paramNum, gpuHandle);

				for (uint32_t j = 1; j < v.size(); ++j)
				{
					auto tex = bufferManager->GetTexture(v[j].TextureIDX);
					Core::ShaderResourceView srv{};
					bufferManager->CreateShaderResourceView(tex, &srv);
				}

				paramNum++;
			}
		}

		std::vector<CD3DX12_ROOT_PARAMETER> params(rootSig.Parameters.size());
		std::vector< D3D12_DESCRIPTOR_RANGE> ranges(paramNum);

		paramNum = 0;

		for (auto& param : rootSig.Parameters)
		{

			switch (param.Type)
			{
			case RootParameterType::Table:
			{
				ranges[paramNum].BaseShaderRegister = param.Table.Register;
				ranges[paramNum].RegisterSpace = param.Table.RegisterSpace;
				ranges[paramNum].OffsetInDescriptorsFromTableStart = 0;
				ranges[paramNum].NumDescriptors = param.Table.NumDescriptor;
				ranges[paramNum].RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(param.Table.Type);
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
		D3D12_STATIC_SAMPLER_DESC sampler_desc = {};
		sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.RegisterSpace = 0;
		sampler_desc.ShaderRegister = 0;
		sampler_desc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		COMPTR<ID3DBlob> errorBlob;
		COMPTR<ID3DBlob> signatureBlob;

		auto signatureDesc = CD3DX12_ROOT_SIGNATURE_DESC(params.size(), params.data(), 1, &sampler_desc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS);

		if (FAILED(D3D12SerializeRootSignature(&signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, signatureBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf())))
		{
			OutputDebugString(L"TE_DX12: the serialization of root signature of Renderer3D is failed!");
			OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
			exit(1);
		}

		if (FAILED(TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(m_ID3D12RootSignatures[TE_IDX_SHADERCLASS::FORWARDRENDERING].GetAddressOf()))))
		{
			OutputDebugString(L"the Creation of root signature of Renderer3D is failed!");
			exit(1);
		}
	}

}