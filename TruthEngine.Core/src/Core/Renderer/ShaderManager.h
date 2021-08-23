#pragma once
#include "Shader.h"


namespace TruthEngine
{
	class GraphicResource;

	class ShaderManager
	{
		

	public:
		ShaderManager() = default;
		virtual ~ShaderManager() = default;

		virtual TE_RESULT AddShader(Shader** outShader, TE_IDX_SHADERCLASS shaderClassID, TE_IDX_MESH_TYPE meshType, RendererStateSet states, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "") = 0;

		ShaderSignature* GetShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX);
		const ShaderRequiredResources* GetShaderRequiredResources(const TE_IDX_SHADERCLASS _ShaderClassIDX) const;

		Shader* GetShader(TE_IDX_SHADERCLASS shaderClassID, TE_IDX_MESH_TYPE _MeshType, RendererStateSet states);

		inline Shader* GetShader(std::string_view shaderName)
		{
			return m_ShadersNameMap[shaderName].get();
		}

		static ShaderManager* GetInstance()
		{
			static ShaderManager* s_Instance = Factory();
			return s_Instance;
		}

		static ShaderManager* Factory();

	protected:

		ShaderSignature* _CreateShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX);

		void _CreateInputElements(std::vector<ShaderInputElement> shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::TOTALNUM], TE_IDX_SHADERCLASS shaderClassIDX);
		/*std::vector<std::vector<ShaderSignature::ShaderConstantBufferViewSlot>> _CreateConstantBufferViewSlots(TE_IDX_SHADERCLASS shaderClassIDX);
		std::vector<std::vector<ShaderSignature::ShaderShaderResourceViewSlot>> _CreateShaderResourceViewSlots(TE_IDX_SHADERCLASS shaderClassIDX);*/

		void _GetShaderDefines(const RendererStateSet states, TE_IDX_MESH_TYPE _MeshType);

	protected:
		const RendererStateSet m_StateMask = BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_NORMAL | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_SPECULAR | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_METALLIC | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION | BIT_MASK_TE_RENDERER_STATE_ENABLED_HDR | BIT_MASK_TE_RENDERER_STATE_SHADING_MODEL;


		std::unordered_map<RendererStateSet, std::shared_ptr<Shader>> m_ShadersStateMap[static_cast<uint32_t>(TE_IDX_SHADERCLASS::TOTALNUM)][static_cast<uint32_t>(TE_IDX_MESH_TYPE::TOTALNUM)];

		std::unordered_map<std::string_view, std::shared_ptr<Shader>> m_ShadersNameMap;

		std::unordered_map<TE_IDX_SHADERCLASS, ShaderSignature> m_Map_ShaderSignatures;

		std::vector<std::wstring> m_Defines{};

		

		std::unordered_map<TE_IDX_SHADERCLASS, ShaderRequiredResources> m_Map_ShaderRequiedResources;
	};
}

#define TE_INSTANCE_SHADERMANAGER TruthEngine::ShaderManager::GetInstance()
