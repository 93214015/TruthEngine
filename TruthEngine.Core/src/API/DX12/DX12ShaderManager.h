#pragma once
#include "Core/Renderer/ShaderManager.h"

#define TE_INSTANCE_API_DX12_SHADERMANAGER TruthEngine::API::DirectX12::DX12ShaderManager::Get()

namespace TruthEngine
{

	namespace API::DirectX12 
	{
		class DX12ShaderManager : public Core::ShaderManager
		{
		public:

			inline static DX12ShaderManager& Get()
			{
				return s_Instance;
			}

			TE_RESULT AddShader(Core::Shader** outShader, std::string_view name, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry, std::string_view dsEntry, std::string_view hsEntry, std::string_view gsEntry) override;
			TE_RESULT AddShader(Core::Shader** outShader, RendererStateSet states, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry, std::string_view dsEntry, std::string_view hsEntry, std::string_view gsEntry) override;

			inline ID3D12RootSignature* GetRootSignature(const Core::Shader* shader) noexcept
			{
				return m_RootSignatures[shader->m_ID].Get();
			}

		protected:
			/*Core::Shader::ShaderCode CompileShader(std::string_view filePath, std::string_view entry, std::string_view shaderStage);*/
			Core::Shader::ShaderCode CompileShader(std::string_view shaderName, std::string_view filePath, std::string_view entry, std::string_view shaderStage);

			TE_RESULT AddRootSignature(Core::Shader* shader);

		protected:
			uint32_t m_ShaderID = 0;

			std::wstring compileTargetVersion = L"_6_0";

			std::unordered_map<uint32_t, COMPTR<ID3D12RootSignature>> m_RootSignatures;

			static DX12ShaderManager s_Instance;

		};
	}
}

