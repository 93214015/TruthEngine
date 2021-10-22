#pragma once
#include "Shader.h"


namespace TruthEngine
{
	class GraphicResource;

	class ShaderHandle
	{
	public:
		ShaderHandle() {};
		ShaderHandle(uint16_t _ShaderIndex) : m_ShaderIndex(_ShaderIndex) {};

		Shader* GetShader() const;
		bool IsValid() const;

	private:
		uint16_t m_ShaderIndex = -1;

		friend class ShaderManager;
	};

	class ShaderManager
	{
		

	public:
		ShaderManager() = default;
		virtual ~ShaderManager() = default;

		virtual ShaderHandle AddShader(TE_IDX_SHADERCLASS shaderClassID, TE_ShaderClass_UniqueIdentifier shaderUniqueIdentifier, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "", const std::vector<const wchar_t*>& _DefinedMacros = {}) = 0;

		ShaderSignature* GetShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX);
		const ShaderRequiredResources* GetShaderRequiredResources(const TE_IDX_SHADERCLASS _ShaderClassIDX) const;

		ShaderHandle GetShader(TE_IDX_SHADERCLASS shaderClassID, TE_ShaderClass_UniqueIdentifier shaderUnqiueIdentifier);

		static ShaderManager* GetInstance()
		{
			static ShaderManager* s_Instance = Factory();
			return s_Instance;
		}

		static ShaderManager* Factory();

	protected:

		ShaderSignature* _CreateShaderSignature(const TE_IDX_SHADERCLASS shaderClassIDX);

		/*void _CreateInputElements(std::vector<ShaderInputElement> shaderInputs[(uint32_t)TE_IDX_MESH_TYPE::TOTALNUM], TE_IDX_SHADERCLASS shaderClassIDX);
		std::vector<std::vector<ShaderSignature::ShaderConstantBufferViewSlot>> _CreateConstantBufferViewSlots(TE_IDX_SHADERCLASS shaderClassIDX);
		std::vector<std::vector<ShaderSignature::ShaderShaderResourceViewSlot>> _CreateShaderResourceViewSlots(TE_IDX_SHADERCLASS shaderClassIDX);*/

		void _GetShaderDefines(const RendererStateSet states, TE_IDX_MESH_TYPE _MeshType);

	protected:

		std::vector<Shader> m_ArrayShaders;

		std::unordered_map<TE_ShaderClass_UniqueIdentifier, ShaderHandle> m_Map_Shaders[static_cast<uint32_t>(TE_IDX_SHADERCLASS::TOTALNUM)];

		//const RendererStateSet m_StateMask = BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_NORMAL | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_SPECULAR | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_METALLIC | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION | BIT_MASK_TE_RENDERER_STATE_ENABLED_HDR | BIT_MASK_TE_RENDERER_STATE_SHADING_MODEL;

		//std::unordered_map<RendererStateSet, std::shared_ptr<Shader>> m_ShadersStateMap[static_cast<uint32_t>(TE_IDX_SHADERCLASS::TOTALNUM)][static_cast<uint32_t>(TE_IDX_MESH_TYPE::TOTALNUM)];

		//std::unordered_map<std::string_view, std::shared_ptr<Shader>> m_ShadersNameMap;

		std::unordered_map<TE_IDX_SHADERCLASS, ShaderSignature> m_Map_ShaderSignatures;

		std::vector<std::wstring> m_Defines{};		

		std::unordered_map<TE_IDX_SHADERCLASS, ShaderRequiredResources> m_Map_ShaderRequiedResources;

		friend class ShaderHandle;
	};
}

#define TE_INSTANCE_SHADERMANAGER TruthEngine::ShaderManager::GetInstance()
