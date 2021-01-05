#pragma once
#include "Core/Renderer/ShaderManager.h"


namespace TruthEngine
{

	namespace API::DirectX12 
	{



		



		class DirectX12ShaderManager : public Core::ShaderManager
		{
		public:
			DirectX12ShaderManager();

			TE_RESULT AddShader(Core::Shader** outShader, TE_IDX_SHADERCLASS shaderClassID, std::string_view name, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry, std::string_view dsEntry, std::string_view hsEntry, std::string_view gsEntry) override;
			TE_RESULT AddShader(Core::Shader** outShader, TE_IDX_SHADERCLASS shaderClassID, RendererStateSet states, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry, std::string_view dsEntry, std::string_view hsEntry, std::string_view gsEntry) override;


		protected:
			Core::Shader::ShaderCode CompileShader_OLD(std::string_view shaderName, uint32_t shaderID, std::string_view filePath, std::string_view entry, std::string_view shaderStage);
			Core::Shader::ShaderCode CompileShader(std::string_view shaderName, uint32_t shaderID, std::string_view filePath, std::string_view entry, std::string_view shaderStage);


		protected:
			uint32_t m_ShaderID = 0;

			std::wstring compileTargetVersion = L"_6_0";



			struct pImpl;

			std::shared_ptr<pImpl> m_pImpl;



		};

		


	}
}

