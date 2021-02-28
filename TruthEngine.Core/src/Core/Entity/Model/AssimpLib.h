#pragma once


namespace TruthEngine
{
		class ModelManager;
		class Scene;
		class Mesh;
		class Entity;
		class Material;


		class AssimpLib
		{
		public:
			AssimpLib();
			~AssimpLib();

			TE_RESULT ImportModel(Scene* scene, const char* filePath);

			static AssimpLib* GetInstance()
			{
				static AssimpLib s_Instance;
				return &s_Instance;
			}

		protected:
			void ProcessTextures(const aiScene* scene);
			void ProcessMaterials(Scene* scene, const aiScene* aiscene);
			void ProcessNodes(Scene* scene, const aiNode* node, const aiScene* aiscene, const size_t meshOffset);
			void ProcessMeshes(Scene* scene, const aiScene* aiscene);

			void AddSpace(const aiScene* scene);

			void CenteralizeMeshVerteciesAddMeshEntity(const char* meshName, Mesh* mesh, Material* material, Scene* scene);

		protected:
			const char* m_ModelFilePath = nullptr;
			size_t m_MeshOffset, /*m_ModelOffset,*/ m_MaterialOffset, m_BaseVertexOffset, m_BaseIndexOffset, m_TextureMaterialOffset;
			ModelManager* m_ModelManager;

			std::vector<std::future<void>> m_Futures;

		};
}
