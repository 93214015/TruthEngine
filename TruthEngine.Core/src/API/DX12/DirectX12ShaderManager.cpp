#include "pch.h"
#include "DirectX12ShaderManager.h"
#include "DirectX12GraphicDevice.h"
#include "DirectX12BufferManager.h"
#include "DirectX12Manager.h"


#include "DirectXShaderCompiler/inc/dxcapi.h"
#include "DirectXShaderCompiler/inc/d3d12shader.h"

namespace TruthEngine
{

	namespace API
	{
		namespace DirectX12
		{

			struct DirectX12ShaderManager::pImpl
			{
				pImpl()
				{
					m_ShaderBlobs.reserve(1000);
				}

				std::vector<COMPTR<IDxcBlob>> m_ShaderBlobs;
			};


			DirectX12ShaderManager::DirectX12ShaderManager() : m_pImpl(std::make_shared<pImpl>())
			{
			}

			ShaderHandle DirectX12ShaderManager::AddShader(TE_IDX_SHADERCLASS shaderClassID, TE_ShaderClass_UniqueIdentifier shaderUniqueIdentifier, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry, std::string_view dsEntry, std::string_view hsEntry, std::string_view gsEntry, const std::vector<const wchar_t*>& _DefinedMacros)
			{
				auto classID = static_cast<uint32_t>(shaderClassID);

				auto& map = m_Map_Shaders[classID];

				auto item = map.find(shaderUniqueIdentifier);

				if (item != map.end())
				{
					return item->second;
				}

				auto _ShaderIndex = static_cast<uint16_t>(m_ArrayShaders.size());

				TE_ShaderID _ShaderID = (static_cast<uint64_t>(classID) << 32) | shaderUniqueIdentifier;

				auto shader = &m_ArrayShaders.emplace_back(_ShaderID, GetShaderSignature(shaderClassID), filePath);

				map[shaderUniqueIdentifier] = ShaderHandle{ _ShaderIndex };

				m_Defines.clear();
				m_Defines.insert(m_Defines.end(), _DefinedMacros.begin(), _DefinedMacros.end());

				if (csEntry != "")
				{
					shader->m_CS = CompileShader_FXC(_ShaderID, filePath, csEntry, "cs");
				}
				if (vsEntry != "")
				{
					shader->m_VS = CompileShader_FXC(_ShaderID, filePath, vsEntry, "vs");
				}
				if (psEntry != "")
				{
					shader->m_PS = CompileShader_FXC(_ShaderID, filePath, psEntry, "ps");
				}
				if (gsEntry != "")
				{
					shader->m_GS = CompileShader_FXC(_ShaderID, filePath, gsEntry, "gs");
				}
				if (dsEntry != "")
				{
					shader->m_DS = CompileShader_FXC(_ShaderID, filePath, dsEntry, "ds");
				}
				if (hsEntry != "")
				{
					shader->m_HS = CompileShader_FXC(_ShaderID, filePath, hsEntry, "hs");
				}

				std::string _CompiledShaderName = "shader" + std::to_string(_ShaderID) + ".bin";
				std::fstream _fstream(_CompiledShaderName.c_str(), std::ios::out | std::ios::binary);

				boost::archive::binary_oarchive _ar(_fstream);

				_ar& shader;


				DirectX12Manager::GetInstance()->AddRootSignature(shader->GetShaderClassIDX());

				return ShaderHandle{ _ShaderIndex };

			}

			ShaderCode DirectX12ShaderManager::CompileShader_FXC(TE_ShaderID shaderID, std::string_view filePath, std::string_view entry, std::string_view shaderStage)
			{
				std::string target = shaderStage.data() + std::string("_5_1");

				ID3DBlob* codeBlob;
				COMPTR<ID3DBlob> errorBlob;

				std::vector <D3D_SHADER_MACRO> macros;

				std::vector<std::pair<std::string, std::string>> _MacroValue;
				_MacroValue.reserve(m_Defines.size());

				for (const auto& d : m_Defines)
				{
					std::string _Macro = std::string(d.begin(), d.end());
					auto& _pair = _MacroValue.emplace_back(std::pair(std::string(""), std::string("")));
					size_t _equalOperator = _Macro.find_first_of('=');
					if (_equalOperator != std::string::npos)
					{
						_pair.first = std::string(_Macro.begin(), _Macro.begin() + _equalOperator);
						_pair.second = std::string(_Macro.begin() + _equalOperator + 1, _Macro.end());
					}
					else
					{
						_pair.first = std::string(_Macro);
					}
					D3D_SHADER_MACRO m{ _pair.first.c_str() , _pair.second.c_str() };
					macros.push_back(m);
				}

				macros.emplace_back(D3D_SHADER_MACRO{ NULL, NULL });

				auto hr = D3DCompileFromFile(to_wstring(filePath).c_str(), macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.data(), target.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &codeBlob, errorBlob.GetAddressOf());
				if (FAILED(hr))
				{
					TE_LOG_CORE_ERROR("The Shader Compilation was failed;\n file: {0}\n error: {1}", filePath.data(), static_cast<const char*>(errorBlob->GetBufferPointer()));
					exit(-1);
				}

				return ShaderCode(codeBlob->GetBufferSize(), codeBlob->GetBufferPointer());
			}

			ShaderCode DirectX12ShaderManager::CompileShader_DXC(TE_ShaderID shaderID, std::string_view filePath, std::string_view entry, std::string_view shaderStage)
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

				std::filesystem::path _Path{ filePath };

				std::wstring target = to_wstring(shaderStage) + compileTargetVersion;
				std::wstring name = _Path.filename().wstring() + std::to_wstring(shaderID);

				//
				// COMMAND LINE: dxc myshader.hlsl -E main -T ps_6_0 -Zi -D MYDEFINE=1 -Fo myshader.bin -Fd myshader.pdb -Qstrip_reflect
				//
				auto entryL = to_wstring(entry);
				//auto binaryOutput = (name + L".bin");
				//auto debugOutput = (L"./" + name + L".pdb");

				std::vector<LPCWSTR> vargs{
					name.c_str(),
					L"-E", entryL.c_str(),
					L"-T", target.c_str(),
					L"-I", L"/Assets/Shaders", //Include File Path
					/*L"-Fo",binaryOutput.c_str(),
					L"-Fd", debugOutput.c_str(),
					L"-Qstrip_reflect"*/ };

#ifdef TE_DEBUG
				vargs.emplace_back(L"-Od");
				vargs.emplace_back(L"-Qembed_debug");
				vargs.emplace_back(L"-Zi");

#else
				vargs.emplace_back(L"-O3");
#endif

				for (auto& d : m_Defines)
				{
					vargs.emplace_back(L"-D");
					vargs.emplace_back(d.c_str());
				}


				/*LPCWSTR args[] =
				{
					name.c_str(),
					L"-E", entryL.c_str(),
					L"-T", target.c_str(),
					L"-Zi",
					L"-Fo",binaryOutput.c_str(),
					L"-Fd", debugOutput.c_str(),
					L"-Qstrip_reflect"
				};*/

				//
				// Open source file.  
				//
				COMPTR<IDxcBlobEncoding> pSource;
				auto hr = utils->LoadFile(_Path.c_str(), nullptr, &pSource);
				TE_ASSERT_CORE(SUCCEEDED(hr), "DirectX12 Shader Manager: The shader file is not found");
				DxcBuffer source{};
				source.Ptr = pSource->GetBufferPointer();
				source.Size = pSource->GetBufferSize();
				source.Encoding = DXC_CP_ACP;


				//
				// Compile it with specified arguments.
				//
				COMPTR<IDxcResult> result;
				compiler->Compile(&source, vargs.data(), vargs.size(), includeHandler.Get(), IID_PPV_ARGS(&result));

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
					TE_LOG_CORE_ERROR("the shader compilation error:\n {0} \n {1}", filePath.data(), error->GetStringPointer());
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
				COMPTR<IDxcBlob>& shaderBin = m_pImpl->m_ShaderBlobs.emplace_back();
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

				return ShaderCode{ shaderBin->GetBufferSize(), shaderBin->GetBufferPointer() };

			}



		}
	}
}