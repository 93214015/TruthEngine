#pragma once
#include "Core/Renderer/ShaderManager.h"


namespace TruthEngine
{

	namespace API::DirectX12 
	{


		class DirectX12ShaderManager : public ShaderManager
		{
		public:
			DirectX12ShaderManager();

			ShaderHandle AddShader(TE_IDX_SHADERCLASS shaderClassID, TE_ShaderClass_UniqueIdentifier shaderUniqueIdentifier, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry, std::string_view dsEntry, std::string_view hsEntry, std::string_view gsEntry, const std::vector<const wchar_t*>& _DefinedMacros) override;



			static DirectX12ShaderManager* GetInstance()
			{
				static DirectX12ShaderManager s_Instance;
				return &s_Instance;
			}
		protected:
			ShaderCode CompileShader_DXC(TE_ShaderID shaderID, std::string_view filePath, std::string_view entry, std::string_view shaderStage);
			ShaderCode CompileShader_FXC(TE_ShaderID shaderID, std::string_view filePath, std::string_view entry, std::string_view shaderStage);


		protected:
			std::wstring compileTargetVersion = L"_6_0";

			struct pImpl;

			std::shared_ptr<pImpl> m_pImpl;

		};

		


	}
}

