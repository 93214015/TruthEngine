#pragma once


namespace TruthEngine
{

	namespace Core
	{
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
			void ProcessNodes(const aiNode* node, const aiScene* scene);
			void ProcessMeshes(const aiScene* scene);

			void AddSpace(const aiScene* scene);

		protected:


		};
	}
}
