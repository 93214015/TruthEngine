#pragma once


namespace TruthEngine
{
		class ModelManager;
		class Scene;
		class Mesh;
		class Entity;
		class Material;
		class SA_Animation;


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
			void ProcessMaterials(Scene* scene, const aiScene* aiscene, TE_IDX_MESH_TYPE _MeshType);
			void ProcessNodes(Scene* scene, const aiNode* node, const aiScene* aiscene, const size_t meshOffset, const float4x4& _ParentTransform);
			void ProcessMeshes(Scene* scene, const aiScene* aiscene);
			void ProcessMeshesSkinned(Scene* scene, const aiScene* aiscene);

			void AddSpace(const aiScene* scene, TE_IDX_MESH_TYPE _MeshType);

			void CenteralizeMeshVerteciesAddMeshEntity(const char* meshName, Mesh* mesh, Material* material, Scene* scene, const float4x4& transform);

		protected:
			bool m_IsLoadingAnimatedModel = false;
			size_t m_MeshOffset, /*m_ModelOffset,*/ m_MaterialOffset, m_BaseVertexOffset, m_BaseIndexOffset, m_TextureMaterialOffset;

			const char* m_ModelFilePath = nullptr;
			ModelManager* m_ModelManager;
			SA_Animation* m_LoadedAnimation = nullptr;

			std::unordered_map<std::string_view, uint32_t> mMapTexFileName;

			std::vector<std::future<void>> m_Futures;

		};
}
