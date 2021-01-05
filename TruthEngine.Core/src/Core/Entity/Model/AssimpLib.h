#pragma once


namespace TruthEngine
{

	namespace Core
	{
		class ModelManager;

		class AssimpLib
		{
		public:
			AssimpLib();
			~AssimpLib();

			TE_RESULT ImportModel(const char* filePath);


			static AssimpLib* GetInstance()
			{
				static AssimpLib s_Instance;
				return &s_Instance;
			}

		protected:
			void ProcessTextures(const aiScene* scene);
			void ProcessMaterials(const aiScene* scene);
			void ProcessNodes(const aiNode* node, const aiScene* scene, const size_t meshOffset);
			void ProcessMeshes(const aiScene* scene);

			void AddSpace(const aiScene* scene);

		protected:
			const char* m_ModelFilePath = nullptr;
			size_t m_MeshOffset, m_ModelOffset, m_MaterialOffset, m_VertexOffset, m_IndexOffset, m_TextureMaterialOffset;
			ModelManager* m_ModelManager;

		};
	}
}
