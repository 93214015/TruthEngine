#pragma once
#include "Shader.h"


namespace TruthEngine
{


	class ShaderManager
	{
	public:
		ShaderManager() = default;
		virtual ~ShaderManager() = default;

		virtual TE_RESULT AddShader(Shader** outShader, TE_IDX_SHADERCLASS shaderClassID, TE_IDX_MESH_TYPE meshType, std::string_view name, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "") = 0;
		virtual TE_RESULT AddShader(Shader** outShader, TE_IDX_SHADERCLASS shaderClassID, TE_IDX_MESH_TYPE meshType, RendererStateSet states, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "") = 0;

		ShaderSignature* GetShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX);

		Shader* GetShader(TE_IDX_SHADERCLASS shaderClassID, RendererStateSet states);

		inline Shader* GetShader(std::string_view shaderName)
		{
			return m_ShadersNameMap[shaderName].get();
		}

		static ShaderManager* GetInstance()
		{
			static std::shared_ptr<ShaderManager> s_Instance = Factory();
			return s_Instance.get();
		}

		static std::shared_ptr<ShaderManager> Factory();

	protected:

		ShaderSignature* _CreateShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX);

		void _CreateInputElements(std::vector<ShaderInputElement> shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::TOTALNUM], TE_IDX_SHADERCLASS shaderClassIDX);
		std::vector<std::vector<ShaderSignature::ShaderConstantBufferSlot>> _CreateConstantBufferSlots(TE_IDX_SHADERCLASS shaderClassIDX);
		std::vector<std::vector<ShaderSignature::ShaderTextureSlot>> _CreateTextureSlots(TE_IDX_SHADERCLASS shaderClassIDX);

		void _GetShaderDefines(const RendererStateSet states);

	protected:
		RendererStateSet m_StateMask = BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_NORMAL;


		std::unordered_map<RendererStateSet, std::shared_ptr<Shader>> m_ShadersStateMap[static_cast<uint32_t>(TE_IDX_SHADERCLASS::TOTALNUM)];

		std::unordered_map<std::string_view, std::shared_ptr<Shader>> m_ShadersNameMap;


		std::unordered_map<TE_IDX_SHADERCLASS, ShaderSignature> m_Map_ShaderSignatures;

		std::vector<std::wstring> m_Defines{};
	};
}

#define TE_INSTANCE_SHADERMANAGER TruthEngine::ShaderManager::GetInstance()
