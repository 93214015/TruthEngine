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
		ProcessMeshes(scene);
		ProcessNodes(scene->mRootNode, scene);

		return TE_SUCCESSFUL;
	}


	void AssimpLib::ProcessTextures(const aiScene* scene)
	{

	}


	void AssimpLib::ProcessMaterials(const aiScene* scene)
	{

	}


	void AssimpLib::ProcessNodes(const aiNode* node, const aiScene* scene)
	{
		ModelManager* modelManager = ModelManager::GetInstance().get();
		auto nodeMeshNum = node->mNumMeshes;
		if (nodeMeshNum > 0)
		{
			Model3D model;
			model.AddSpace(nodeMeshNum);
			for (uint32_t i = 0; i < nodeMeshNum; ++i)
			{
				model.AddMesh(modelManager->m_Meshes[node->mMeshes[i]].get());
			}
		}

		for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
		{
			ProcessNodes(node->mChildren[childIndex], scene);
		}
	}


	void AssimpLib::ProcessMeshes(const aiScene* scene)
	{
		auto modelManager = ModelManager::GetInstance();

		size_t vertexOffset, indexOffset;
		modelManager->GetOffsets(vertexOffset, indexOffset);

		for (uint32_t j = 0; j < scene->mNumMeshes; ++j)
		{
			auto mesh = scene->mMeshes[j];

			for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
			{
				VertexData::Pos vPos;
				VertexData::NormTex vNormTex;

				auto vertex = mesh->mVertices[i];

				vPos.Position = float3{ vertex.x, vertex.y, vertex.z };

				modelManager->m_VertexBuffer_PosNormTex.AddVertex(vPos, vNormTex);
			}

			uint32_t indexNum;

			for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace& face = mesh->mFaces[i];
				indexNum += face.mNumIndices;

				for (uint32_t j = 0; j < face.mNumIndices; ++j)
				{
					modelManager->m_IndexBuffer.AddIndex(face.mIndices[j]);
				}
			}

			modelManager->m_Meshes.emplace_back(std::make_shared<Mesh>(indexNum, indexOffset, vertexOffset));
		}
	}

	void AssimpLib::AddSpace(const aiScene* scene)
	{

		uint32_t vertexNum = 0;
		uint32_t indexNum = 0;

		auto modelManager = ModelManager::GetInstance();

		auto meshNum = scene->mNumMeshes;

		modelManager->AddSpace(1, meshNum);

		for (uint32_t i = 0; i < meshNum; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			vertexNum += mesh->mNumVertices;

			for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
			{
				aiFace& face = mesh->mFaces[j];
				indexNum += face.mNumIndices;
			}

		}

		modelManager->m_VertexBuffer_PosNormTex.AddSpace(vertexNum, 0);
		modelManager->m_IndexBuffer.AddSpace(indexNum);

	}

}