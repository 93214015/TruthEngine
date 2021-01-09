#pragma once
#include "Shader.h"


namespace TruthEngine
{
	namespace Core
	{

		struct BindedResource
		{
			struct ShaderConstantBufferSlot
			{
				ShaderConstantBufferSlot(uint32_t registerSlot, uint32_t registerSpace, TE_IDX_CONSTANTBUFFER constantBufferIDX)
					: Register(registerSlot), RegisterSpace(registerSpace), ConstantBufferIDX(constantBufferIDX)
				{}

				uint32_t Register;
				uint32_t RegisterSpace;
				TE_IDX_CONSTANTBUFFER ConstantBufferIDX;
			};

			struct ShaderTextureSlot
			{
				ShaderTextureSlot(uint32_t registerSlot, uint32_t registerSpace, TE_IDX_TEXTURE textureIDX)
					: Register(registerSlot), RegisterSpace(registerSpace), TextureIDX(textureIDX)
				{}

				uint32_t Register;
				uint32_t RegisterSpace;
				TE_IDX_TEXTURE TextureIDX;
			};

			std::vector<std::vector<ShaderConstantBufferSlot>> ConstantBuffers;
			std::vector<std::vector<ShaderTextureSlot>> Textures;

			//std::vector<std::vector<TE_IDX_CONSTANTBUFFER>> ConstantBuffers;
			//std::vector<std::vector<TE_IDX_TEXTURE>> Textures;
		};

		class ShaderManager
		{
		public:
			ShaderManager() = default;
			virtual ~ShaderManager() = default;

			virtual TE_RESULT AddShader(Shader** outShader, TE_IDX_SHADERCLASS shaderClassID, std::string_view name, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "") = 0;
			virtual TE_RESULT AddShader(Shader** outShader, TE_IDX_SHADERCLASS shaderClassID, RendererStateSet states, std::string_view filePath, std::string_view vsEntry, std::string_view psEntry, std::string_view csEntry = "", std::string_view dsEntry = "", std::string_view hsEntry = "", std::string_view gsEntry = "") = 0;

			BindedResource* GetBindedResource(const TE_IDX_SHADERCLASS shaderClassIDX);

			Shader* GetShader(TE_IDX_SHADERCLASS shaderClassID, RendererStateSet states);			

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

			std::vector<ShaderInputElement>* GetInputElements(TE_IDX_SHADERCLASS shaderClassIDX);

		protected:

			BindedResource* CreateBindedResource(const TE_IDX_SHADERCLASS shaderClassIDX);

			std::vector<ShaderInputElement>* CreateInputElements(TE_IDX_SHADERCLASS shaderClassIDX);

			void GetShaderDefines(const RendererStateSet states);

		protected:
			
			std::unordered_map<RendererStateSet, std::shared_ptr<Shader>> m_ShadersStateMap[static_cast<uint32_t>(TE_IDX_SHADERCLASS::TOTALNUM)];

			std::unordered_map<std::string_view, std::shared_ptr<Shader>> m_ShadersNameMap;


			RendererStateSet m_StateMask = BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT | BIT_MASK_TE_RENDERER_STATE_ENABLED_MAP_NORMAL;


			std::unordered_map<TE_IDX_SHADERCLASS, BindedResource> m_Map_BindedResources;

			std::unordered_map<TE_IDX_SHADERCLASS, std::vector<ShaderInputElement>> m_Map_ShaderInputElements;

			std::vector<std::wstring> m_Defines{};
		};
	}
}

#define TE_INSTANCE_SHADERMANAGER TruthEngine::Core::ShaderManager::GetInstance()