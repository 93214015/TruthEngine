#include "pch.h"
#include "DX12ShaderManager.h"
#include "DX12GraphicDevice.h"



#include "DirectXShaderCompiler/inc/dxcapi.h"
#include "DirectXShaderCompiler/inc/d3d12shader.h"

namespace TruthEngine
{

	namespace API
	{
		namespace DirectX12 
		{

			TE_RESULT DX12ShaderManager::AddShader(Core::Shader** outShader, std::string_view name, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry /*= ""*/, std::string_view dsEntry /*= ""*/, std::string_view hsEntry /*= ""*/, std::string_view gsEntry /*= ""*/)
			{
				auto item = m_ShadersNameMap.find(name);

				if (item != m_ShadersNameMap.end())
				{
					*outShader = item->second.get();
					return TE_RESULT_RENDERER_SHADER_HAS_EXIST;
				}

				auto shader = std::make_shared<Core::Shader>(name, filePath);
				m_ShadersNameMap[name] = shader;
				*outShader = shader.get();


				if (csEntry != "")
				{
					shader->m_CS = CompileShader(name, filePath, csEntry, "cs");
				}
				if (vsEntry != "")
				{
					shader->m_VS = CompileShader(name, filePath, vsEntry, "vs");
				}
				if (psEntry != "")
				{
					shader->m_PS = CompileShader(name, filePath, psEntry, "ps");
				}
				if (gsEntry != "")
				{
					shader->m_GS = CompileShader(name, filePath, gsEntry, "gs");
				}
				if (dsEntry != "")
				{
					shader->m_DS = CompileShader(name, filePath, dsEntry, "ds");
				}
				if (hsEntry != "")
				{
					shader->m_HS = CompileShader(name, filePath, hsEntry, "hs");
				}

				shader->m_ID = m_ShaderID++;


				return AddRootSignature(shader.get());

			}

			TE_RESULT DX12ShaderManager::AddShader(Core::Shader** outShader, RendererStateSet states, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry, std::string_view dsEntry, std::string_view hsEntry, std::string_view gsEntry)
			{
				states &= m_StateMask;

				auto item = m_ShadersStateMap.find(states);

				if (item != m_ShadersStateMap.end())
				{
					*outShader = item->second.get();
					return TE_RESULT_RENDERER_SHADER_HAS_EXIST;
				}

				std::string name = "shader" + std::to_string(m_ShadersStateMap.size());

				auto shader = std::make_shared<Core::Shader>(name, filePath);
				m_ShadersStateMap[states] = shader;
				*outShader = shader.get();

				if (csEntry != "")
				{
					shader->m_CS = CompileShader(name, filePath, csEntry, "cs");
				}
				if (vsEntry != "")
				{
					shader->m_VS = CompileShader(name, filePath, vsEntry, "vs");
				}
				if (psEntry != "")
				{
					shader->m_PS = CompileShader(name, filePath, psEntry, "ps");
				}
				if (gsEntry != "")
				{
					shader->m_GS = CompileShader(name, filePath, gsEntry, "gs");
				}
				if (dsEntry != "")
				{
					shader->m_DS = CompileShader(name, filePath, dsEntry, "ds");
				}
				if (hsEntry != "")
				{
					shader->m_HS = CompileShader(name, filePath, hsEntry, "hs");
				}

				shader->m_ID = m_ShaderID++;

				return AddRootSignature(shader.get());

			}

			// 			TruthEngine::Core::Shader::ShaderCode DX12ShaderManager::CompileShader(std::string_view filePath, std::string_view entry, std::string_view shaderStage)
			// 			{
			// 				std::string target = shaderStage.data() + compileTargetVersion;
			// 
			// 				ID3DBlob* codeBlob;
			// 				COMPTR<ID3DBlob> errorBlob;
			// 
			// 				auto hr = D3DCompileFromFile(to_wstring(filePath).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.data(), target.c_str(), 0, 0, &codeBlob, errorBlob.GetAddressOf());
			// 				if (FAILED(hr))
			// 				{
			// 					TE_LOG_CORE_ERROR("The Shader Compilation was failed;\n file: {0}\n error: {1}", filePath.data(), static_cast<const char*>(errorBlob->GetBufferPointer()));
			// 					exit(-1);
			// 				}
			// 
			// 
			// 				return Core::Shader::ShaderCode(codeBlob->GetBufferSize(), codeBlob->GetBufferPointer());
			// 			}

			Core::Shader::ShaderCode DX12ShaderManager::CompileShader(std::string_view shaderName, std::string_view filePath, std::string_view entry, std::string_view shaderStage)
			{

				// 
				// Create compiler and utils.
				//
				COMPTR<IDxcUtils> utils;
				COMPTR<IDxcCompiler3> compiler;
				DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
				DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

				//
				// Create default include handler. (You can create your own...)
				//
				COMPTR<IDxcIncludeHandler> includeHandler;
				utils->CreateDefaultIncludeHandler(&includeHandler);

				std::wstring filePathW = to_wstring(filePath);
				std::wstring target = to_wstring(shaderStage) + compileTargetVersion;
				std::wstring name = to_wstring(shaderName);

				//
				// COMMAND LINE: dxc myshader.hlsl -E main -T ps_6_0 -Zi -D MYDEFINE=1 -Fo myshader.bin -Fd myshader.pdb -Qstrip_reflect
				//
				auto entryL = to_wstring(entry);
				auto binaryOutput = (name + L".bin");
				auto debugOutput = (name + L".pdb");

				LPCWSTR args[] =
				{
					name.c_str(),
					L"-E", entryL.c_str(),
					L"-T", target.c_str(),
					L"-Zi",
					L"-Fo",binaryOutput.c_str(),
					L"-Fd", debugOutput.c_str(),
					L"-Qstrip_reflect"
				};

				//
				// Open source file.  
				//
				COMPTR<IDxcBlobEncoding> pSource;
				auto hr = utils->LoadFile(filePathW.c_str(), nullptr, &pSource);
				DxcBuffer source;
				source.Ptr = pSource->GetBufferPointer();
				source.Size = pSource->GetBufferSize();
				source.Encoding = DXC_CP_ACP;


				//
				// Compile it with specified arguments.
				//
				COMPTR<IDxcResult> result;
				compiler->Compile(&source, args, _countof(args), includeHandler.Get(), IID_PPV_ARGS(&result));

				//
				// Print errors if present.
				//
				COMPTR<IDxcBlobUtf8> error;
				result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error), nullptr);
				// Note that d3dcompiler would return null if no errors or warnings are present.  
				// IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if there are no warnings or errors.
				if (error != nullptr && error->GetStringLength() != 0)
				{
					OutputDebugStringA(error->GetStringPointer());
					TE_LOG_CORE_ERROR("the shader compilation error: {0}", error->GetStringPointer());
				}

				HRESULT h;
				result->GetStatus(&h);
				if (FAILED(h))
				{
					TE_LOG_CORE_ERROR("shader compilation failed!");
					TE_ASSERT_CORE(false, "!!!");
				}

				//
				// Save shader binary.
				//
				COMPTR<IDxcBlobUtf16> pShaderName;
				COMPTR<IDxcBlob> shaderBin;
				result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBin), &pShaderName);
				/*if (pShader != nullptr)
				{
					FILE* fp = NULL;

					_wfopen_s(&fp, pShaderName->GetStringPointer(), L"wb");
					fwrite(pShader->GetBufferPointer(), pShader->GetBufferSize(), 1, fp);
					fclose(fp);
				}*/

				//
				// Save PDB
				//
				COMPTR<IDxcBlob> pdb;
				COMPTR<IDxcBlobUtf16> pdbName;
				result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb), &pdbName);
				/*{
					FILE* fp = NULL;

					// Note that if you don't specify -Fd, a pdb name will be automatically generated. Use this file name to save the pdb so that PIX can find it quickly.
					_wfopen_s(&fp, pPDBName->GetStringPointer(), L"wb");
					fwrite(pPDB->GetBufferPointer(), pPDB->GetBufferSize(), 1, fp);
					fclose(fp);
				}*/


				//
				// Print hash.
				//
				/*CComPtr<IDxcBlob> pHash = nullptr;
				pResults->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&pHash), nullptr);
				if (pHash != nullptr)
				{
					wprintf(L"Hash: ");
					DxcShaderHash* pHashBuf = (DxcShaderHash*)pHash->GetBufferPointer();
					for (int i = 0; i < _countof(pHashBuf->HashDigest); i++)
						wprintf(L"%x", pHashBuf->HashDigest[i]);
					wprintf(L"\n");
				}*/

				//
				// Get separate reflection.
				//
				COMPTR<IDxcBlob> reflection;
				result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflection), nullptr);
				/*if (relection != nullptr)
				{
					// Optionally, save reflection blob for later here.

					// Create reflection interface.
					DxcBuffer ReflectionData;
					ReflectionData.Encoding = DXC_CP_ACP;
					ReflectionData.Ptr = reflection->GetBufferPointer();
					ReflectionData.Size = reflection->GetBufferSize();

					CComPtr< ID3D12ShaderReflection > pReflection;
					pUtils->CreateReflection(&ReflectionData, IID_PPV_ARGS(&pReflection));

					// Use reflection interface here.

				}*/

				return Core::Shader::ShaderCode{ shaderBin->GetBufferSize(), shaderBin->GetBufferPointer() };

			}

			TE_RESULT DX12ShaderManager::AddRootSignature(Core::Shader* shader)
			{
				COMPTR<ID3DBlob> errorBlob;
				COMPTR<ID3DBlob> signatureBlob;

				
				D3D12_DESCRIPTOR_RANGE rangeCBV[1];

				/*range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				range[0].RegisterSpace = shader->m_SignatureSR.RegisterSpace;
				range[0].BaseShaderRegister = shader->m_SignatureSR.BaseRegisterSlot;
				range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				range[0].NumDescriptors = shader->m_SignatureSR.InputNum;*/

				rangeCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				rangeCBV[0].RegisterSpace = shader->m_SignatureCB.RegisterSpace;
				rangeCBV[0].BaseShaderRegister = shader->m_SignatureCB.BaseRegisterSlot;
				rangeCBV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				rangeCBV[0].NumDescriptors = shader->m_SignatureCB.InputNum;

				

				D3D12_DESCRIPTOR_RANGE rangeSRV[1];
				rangeSRV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				rangeSRV[0].RegisterSpace = shader->m_SignatureSR.RegisterSpace;
				rangeSRV[0].BaseShaderRegister = shader->m_SignatureSR.BaseRegisterSlot;
				rangeSRV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				rangeSRV[0].NumDescriptors = shader->m_SignatureSR.InputNum;

				CD3DX12_ROOT_PARAMETER params[2];
				params[0].InitAsDescriptorTable(_countof(rangeSRV), rangeSRV, D3D12_SHADER_VISIBILITY_ALL);
				params[1].InitAsDescriptorTable(_countof(rangeCBV), rangeCBV, D3D12_SHADER_VISIBILITY_ALL);
				auto signatureDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(params), params, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
					| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
					| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
					| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
					| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS);

				if (FAILED(D3D12SerializeRootSignature(&signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf())))
				{
					OutputDebugString(L"the serialization of signature is failed!");
					exit(1);
				}

				if (FAILED(TE_INSTANCE_API_DX12_GRAPHICDEVICE->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSignatures[shader->m_ID].GetAddressOf()))))
				{
					OutputDebugString(L"the Creation of signature is failed!");
					exit(1);
				}

				return TE_SUCCESSFUL;
			}

			TruthEngine::API::DirectX12::DX12ShaderManager DX12ShaderManager::s_Instance;

		}
	}
}