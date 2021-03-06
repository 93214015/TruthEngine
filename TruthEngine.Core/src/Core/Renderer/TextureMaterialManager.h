#pragma once


namespace TruthEngine
{

	class TextureMaterial;

	class TextureMaterialManager
	{
	public:
		inline void AddSpace(size_t size)
		{
			auto s = m_Textures.size() + size;
			m_Textures.reserve(s);
			m_Map_Textures.reserve(s);
		}

		inline size_t GetOffset() const noexcept
		{
			return m_Textures.size();
		}

		virtual TextureMaterial* CreateTexture(const char* name, uint8_t* data, uint32_t width, uint32_t height, uint32_t dataSize, TE_RESOURCE_FORMAT format) = 0;
		virtual TextureMaterial* CreateTexture(const char* _texturefilePath, const char* _modelFilePath) = 0;

		inline TextureMaterial* GetTexture(uint32_t index)const
		{
			return m_Textures[index].get();
		}

		inline TextureMaterial* GetTexture(const char* name)const
		{
			auto itr = m_Map_Textures.find(name);
			if (itr == m_Map_Textures.end())
			{
				return nullptr;
			}

			return itr->second;
		}

		uint32_t GetTextureViewIndex(uint32_t textureID)const;



		//virtual uint32_t CreateTextureMaterialDiffuse(uint32_t texIndex) = 0;
		//virtual uint32_t CreateTextureMaterialNormal(uint32_t texIndex) = 0;
		//virtual uint32_t CreateTextureMaterialDisplacement(uint32_t texIndex) = 0;

		const std::vector<std::shared_ptr<TextureMaterial>>& GetAllTextures()const
		{
			return m_Textures;
		}

		static TextureMaterialManager* GetInstance()
		{
			static TextureMaterialManager* s_Instance = Factory();
			return s_Instance;
		}

		static TextureMaterialManager* Factory();

	protected:

	protected:
		std::vector<std::shared_ptr<TextureMaterial>> m_Textures;
		std::unordered_map<const char*, TextureMaterial*> m_Map_Textures;

	};

	#define TE_INSTANCE_TEXTUREMATERIALMANAGER TextureMaterialManager::GetInstance()

}
