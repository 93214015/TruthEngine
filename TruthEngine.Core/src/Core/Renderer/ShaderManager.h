#pragma once
#include "Shader.h"


namespace TruthEngine
{
	namespace Core
	{
		class ShaderManager
		{
		public:
			ShaderManager() = default;
			virtual ~ShaderManager() = default;

			virtual TE_RESULT AddShader(Shader** outShader, std::string_view name, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "") = 0;
			virtual TE_RESULT AddShader(Shader** outShader, RendererStateSet states, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "") = 0;

			inline Shader* GetShader(RendererStateSet states)
			{
				states &= m_StateMask;
				return m_ShadersStateMap[states].get();
			}

			inline Shader* GetShader(std::string_view shaderName)
			{
				return m_ShadersNameMap[shaderName].get();
			}

			static std::shared_ptr<ShaderManager> GetInstance()
			{
				static std::shared_ptr<ShaderManager> s_Instance = Factory();
				return s_Instance;
			}

			static std::shared_ptr<ShaderManager> Factory();

		protected:

		protected:
			std::unordered_map<std::string_view, std::shared_ptr<Shader>> m_ShadersNameMap;
			std::unordered_map<RendererStateSet, std::shared_ptr<Shader>> m_ShadersStateMap;

			RendererStateSet m_StateMask = BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_NORMAL;

		};
	}
}

#define TE_INSTANCE_SHADERMANAGER TruthEngine::Core::ShaderManager::GetInstance()