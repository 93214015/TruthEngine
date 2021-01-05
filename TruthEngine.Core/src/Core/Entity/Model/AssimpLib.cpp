#include "pch.h"
#include "AssimpLib.h"

#include "Core/Entity/Model/ModelManager.h"

#include "Core/Renderer/TextureMaterial.h"
#include "Core/Renderer/TextureMaterialManager.h"


namespace TruthEngine::Core
{
	AssimpLib::AssimpLib()
	{
		m_ModelManager = ModelManager::GetInstance().get();
	}

	AssimpLib::~AssimpLib()
	{
	}

	TE_RESULT AssimpLib::ImportModel(const char* filePath)
	{
		m_ModelFilePath = filePath;

		m_ModelManager->GetOffsets(m_VertexOffset, m_IndexOffset, m_ModelOffset, m_MeshOffset, m_MaterialOffset);
		m_TextureMaterialOffset = TextureMaterialManager::GetInstance()->GetOffset();

		Assimp::Importer importer;

		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);

		auto scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

		if (scene == nullptr)
			return TE_FAIL;

		auto meshOffset = ModelManager::GetInstance().get()->m_Meshes.size();

		AddSpace(scene);

		ProcessTextures(scene);
		ProcessMaterials(scene);
		ProcessMeshes(scene);
		ProcessNodes(scene->mRootNode, scene, meshOffset);

		return TE_SUCCESSFUL;
	}


	void AssimpLib::ProcessTextures(const aiScene* scene)
	{
		auto texManager = TextureMaterialManager::GetInstance();

		texManager->AddSpace(scene->mNumTextures);

		for (uint32_t i = 0; i < scene->mNumTextures; ++i)
		{
			aiTexture* aiTex = scene->mTextures[i];

			std::string_view name = aiTex->mFilename.C_Str();
			auto index = name.find_last_of('/');
			name = name.substr(index);
			std::string fileName(name);

			uint32_t dataSize = aiTex->mHeight == 0 ? aiTex->mWidth : aiTex->mWidth * aiTex->mHeight;

			texManager->CreateTexture(fileName.c_str(), reinterpret_cast<uint8_t*>(aiTex->pcData), aiTex->mWidth, aiTex->mHeight, dataSize, TE_RESOURCE_FORMAT::UNKNOWN);
		}
	}


	void AssimpLib::ProcessMaterials(const aiScene* scene)
	{
		if (!scene->HasMaterials())
			return;

		ModelManager* modelManager = ModelManager::GetInstance().get();

		for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
		{
			auto aiMaterial = scene->mMaterials[i];
			aiColor3D aiColorDiffuse;
			aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColorDiffuse);
			aiColor3D aiColorAmbient;
			aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aiColorAmbient);
			aiColor3D aiColorSpecular;
			aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aiColorSpecular);
			float aiShininess;
			aiMaterial->Get(AI_MATKEY_SHININESS, aiShininess);

			aiString ais;

			auto texManager = TextureMaterialManager::GetInstance();

			uint32_t diffuseMapViewIndex = -1;

			{
				const auto diffuseTextureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
				uint32_t diffuseMapTexIndex = -1;


				for (UINT j = 0; j < diffuseTextureCount; ++j)
				{
					if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, j, &ais) == AI_SUCCESS)
					{
						if (ais.C_Str()[0] == '*')
						{
							auto index = std::atoi(&ais.C_Str()[1]);

							diffuseMapTexIndex = index + m_TextureMaterialOffset;
						}
						else
						{
							diffuseMapTexIndex = texManager->CreateTexture(ais.C_Str(), m_ModelFilePath);
						}

						diffuseMapViewIndex = texManager->CreateTextureMaterialDiffuse(diffuseMapTexIndex);
					}
				}
			}

			uint32_t normalMapViewIndex = -1;

			{
				const auto normalTextureCount = aiMaterial->GetTextureCount(aiTextureType_NORMALS);
				uint32_t normalMapTexIndex = -1;

				for (UINT j = 0; j < normalTextureCount; ++j)
				{
					if (aiMaterial->GetTexture(aiTextureType_NORMALS, j, &ais) == AI_SUCCESS)
					{
						if (ais.C_Str()[0] == '*')
						{
							auto index = std::atoi(&ais.C_Str()[1]);

							normalMapTexIndex = index + m_TextureMaterialOffset;
						}
						else
						{
							normalMapTexIndex = texManager->CreateTexture(ais.C_Str(), m_ModelFilePath);
						}

						normalMapViewIndex = texManager->CreateTextureMaterialNormal(normalMapTexIndex);
					}
				}
			}


			{
				const auto heightTextureCount = aiMaterial->GetTextureCount(aiTextureType_HEIGHT);
				uint32_t heightMapTexIndex = -1;

				for (UINT j = 0; j < heightTextureCount; ++j)
				{
					if (aiMaterial->GetTexture(aiTextureType_HEIGHT, j, &ais) == AI_SUCCESS)
					{
						if (ais.C_Str()[0] == '*')
						{
							auto index = std::atoi(&ais.C_Str()[1]);

							heightMapTexIndex = index + m_TextureMaterialOffset;
						}
						else
						{
							heightMapTexIndex = texManager->CreateTexture(ais.C_Str(), m_ModelFilePath);
						}

						normalMapViewIndex = texManager->CreateTextureMaterialNormal(heightMapTexIndex);
					}
				}
			}

			modelManager->m_MaterialManager.AddMaterial(InitRenderStates()
				, 0
				, float4{ aiColorDiffuse.r, aiColorDiffuse.g, aiColorDiffuse.b, 1.0f }
				, float3{ aiColorSpecular.r, aiColorSpecular.g, aiColorSpecular.b }
				, aiShininess
				, diffuseMapViewIndex
				, normalMapViewIndex
				, -1
				, 0, 0.0f, 0.0f);
		}
	}


	void AssimpLib::ProcessNodes(const aiNode* node, const aiScene* scene, const size_t meshOffset)
	{
		auto nodeMeshNum = node->mNumMeshes;
		if (nodeMeshNum > 0)
		{
			auto model3D = m_ModelManager->AddModel3D();
			model3D->AddSpace(nodeMeshNum);
			for (uint32_t i = 0; i < nodeMeshNum; ++i)
			{
				model3D->AddMesh(m_ModelManager->m_Meshes[node->mMeshes[i] + meshOffset].get());
			}
		}

		for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
		{
			ProcessNodes(node->mChildren[childIndex], scene, meshOffset);
		}
	}


	void AssimpLib::ProcessMeshes(const aiScene* scene)
	{

		for (uint32_t j = 0; j < scene->mNumMeshes; ++j)
		{
			auto mesh = scene->mMeshes[j];

			for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
			{
				VertexData::Pos vPos;
				VertexData::NormTanTex vNormTanTex;

				auto vertex = mesh->mVertices[i];

				vPos.Position = float3{ vertex.x, vertex.y, vertex.z };

				if (mesh->HasNormals())
				{
					auto& normal = mesh->mNormals[i];
					vNormTanTex.Normal = float3{ normal.x, normal.y, normal.z };
				}

				if (mesh->HasTextureCoords(0))
				{
					auto& uv = mesh->mTextureCoords[0][i];
					vNormTanTex.TexCoord = float2{ uv.x, uv.y };
				}

				if (mesh->HasTangentsAndBitangents())
				{
					auto& tangent = mesh->mTangents[i];
					vNormTanTex.Tanget = float3{ tangent.x, tangent.y, tangent.z };
				}

				m_ModelManager->m_VertexBuffer_PosNormTanTex.AddVertex(vPos, vNormTanTex);
			}


			uint32_t indexNum = 0;

			for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace& face = mesh->mFaces[i];
				indexNum += face.mNumIndices;

				for (uint32_t j = 0; j < face.mNumIndices; ++j)
				{
					m_ModelManager->m_IndexBuffer.AddIndex(face.mIndices[j]);
				}
			}

			m_ModelManager->AddMesh(indexNum, m_IndexOffset, m_VertexOffset, m_ModelManager->m_MaterialManager.GetMaterial(mesh->mMaterialIndex + m_MaterialOffset));
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

		modelManager->m_VertexBuffer_PosNormTanTex.AddSpace(vertexNum, 0);
		modelManager->m_IndexBuffer.AddSpace(indexNum);

	}

}