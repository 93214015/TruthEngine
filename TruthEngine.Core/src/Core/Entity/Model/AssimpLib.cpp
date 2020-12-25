#include "pch.h"
#include "AssimpLib.h"

#include "Core/Entity/Model/ModelManager.h"

namespace TruthEngine::Core
{
	AssimpLib::AssimpLib()
	{
	}

	AssimpLib::~AssimpLib()
	{
	}

	TE_RESULT AssimpLib::ImportModel(const char* filePath)
	{
		Assimp::Importer importer;

		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);

		auto scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

		if (scene == nullptr)
			return TE_FAIL;

		AddSpace(scene);

		ProcessTextures(scene);
		ProcessMaterials(scene);
		ProcessNodes(scene);

		return TE_SUCCESSFUL;
	}


	void AssimpLib::ProcessTextures(const aiScene* scene)
	{

	}


	void AssimpLib::ProcessMaterials(const aiScene* scene)
	{

	}


	void AssimpLib::ProcessNodes(const aiScene* scene)
	{
		for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
		{
			ProcessMeshes(scene->mMeshes[i], scene);
		}
	}


	void AssimpLib::ProcessMeshes(const aiMesh* mesh, const aiScene* scene)
	{

	}

	void AssimpLib::AddSpace(const aiScene* scene)
	{

		uint32_t vertexNum = 0;
		uint32_t indexNum = 0;

		for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			vertexNum += mesh->mNumVertices;
			indexNum += mesh->mNumFaces * 3.0f;
		}

		auto modelManager = ModelManager::GetInstance();
		modelManager->m_VertexBuffer_PosNormTex.AddSpace(vertexNum, 0);
		modelManager->m_IndexBuffer.AddSpace(indexNum);

	}

}