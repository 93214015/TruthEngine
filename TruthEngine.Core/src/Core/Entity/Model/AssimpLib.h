#pragma once
#include "MeshHandle.h"


namespace TruthEngine
{
		class ModelManager;
		class MaterialManager;
		class Scene;
		class Mesh;
		class Entity;
		class Material;
		class SA_Animation;

		struct ImportedMeshMaterials
		{
			ImportedMeshMaterials(const char* _Name, MeshHandle _Mesh, Material* _Material, SA_Animation* _Animation)
				: mName(_Name), mMesh(_Mesh), mMaterial(_Material), mAnimation(_Animation)
			{}
			std::string mName;
			MeshHandle mMesh;
			Material* mMaterial;
			SA_Animation* mAnimation;
		};

		class AssimpLib
		{
		public:
			AssimpLib();
			~AssimpLib();

			std::vector<ImportedMeshMaterials> ImportModel(const char* filePath);

			static AssimpLib* GetInstance()
			{
				static AssimpLib s_Instance;
				return &s_Instance;
			}

		protected:
			void ProcessTextures(const aiScene* scene);
			void ProcessMaterials(const aiScene* aiscene, TE_IDX_MESH_TYPE _MeshType);
			void ProcessNodes(const aiNode* node, const aiScene* aiscene, const size_t meshOffset, const float4x4& _ParentTransform);
			void ProcessMeshes(const aiScene* aiscene, std::vector<ImportedMeshMaterials>& _MeshCollection);
			void ProcessMeshesSkinned(const aiScene* aiscene, std::vector<ImportedMeshMaterials>& _MeshCollection);

			void AddSpace(const aiScene* scene, TE_IDX_MESH_TYPE _MeshType);

		protected:
			bool m_IsLoadingAnimatedModel = false;
			size_t m_MeshOffset, /*m_ModelOffset,*/ m_MaterialOffset, m_BaseVertexOffset, m_BaseIndexOffset, m_TextureMaterialOffset;

			const char* m_ModelFilePath = nullptr;
			ModelManager* m_ModelManager;
			MaterialManager* m_MaterialManager;
			SA_Animation* m_LoadedAnimation = nullptr;

			std::unordered_map<std::string_view, uint32_t> mMapTexFileName;

			std::vector<std::future<void>> m_Futures;

		};
}
