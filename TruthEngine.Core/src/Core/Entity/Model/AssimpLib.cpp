#include "pch.h"
#include "AssimpLib.h"

#include "Core/Entity/Model/ModelManager.h"

#include "Core/Renderer/TextureMaterial.h"
#include "Core/Renderer/TextureMaterialManager.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Entity.h"
#include "Core/Entity/Components.h"
#include "Core/ThreadPool.h"
#include "Core/AnimationEngine/CSkinAnimation.h"
#include <Core/AnimationEngine/AnimationManager.h>
#include <Core/Entity/Components/SkinnedAnimationComponent.h>


namespace TruthEngine
{
	AssimpLib::AssimpLib()
	{
		m_ModelManager = ModelManager::GetInstance().get();
	}

	AssimpLib::~AssimpLib()
	{
	}

	TE_RESULT AssimpLib::ImportModel(Scene* scene, const char* filePath)
	{
		TE_TIMER_SCOPE_FUNC;

		TE_ASSERT_CORE(scene && filePath != NULL, "");
		//if (!(scene)) 
		//{ 
		//	//TE_LOG_CORE_ERROR("TE_Error: {0}", __VA_ARGS__); 
		//	::TruthEngine::Log::GetCoreLogger()->error("");
		//	__debugbreak(); 
		//}

		m_ModelFilePath = filePath;
		m_IsLoadingAnimatedModel = false;
		m_LoadedAnimation = nullptr;

		Assimp::Importer importer;

		//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);

		TimerProfile assimpTimer;
		assimpTimer.Start();
		auto aiscene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
		//auto aiscene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		auto totalTime = assimpTimer.GetTotalTime();
		TE_LOG_CORE_INFO("Assimp Library: The ReadFile() time was : {0} ms", totalTime);

		if (aiscene == nullptr)
			return TE_FAIL;

		auto meshOffset = ModelManager::GetInstance().get()->m_Meshes.size();

		TE_IDX_MESH_TYPE _MeshType = TE_IDX_MESH_TYPE::MESH_NTT;
		if (aiscene->HasAnimations())
		{
			_MeshType = TE_IDX_MESH_TYPE::MESH_SKINNED;
			m_IsLoadingAnimatedModel = true;
			m_LoadedAnimation = AnimationManager::GetInstance()->AddAnimation(aiscene);
		}

		m_ModelManager->GetOffsets(m_BaseVertexOffset, m_BaseIndexOffset, m_MeshOffset, m_MaterialOffset, _MeshType);
		m_TextureMaterialOffset = TextureMaterialManager::GetInstance()->GetOffset();

		AddSpace(aiscene, _MeshType);

		{
			assimpTimer.Start();
			ProcessTextures(aiscene);
			auto t = assimpTimer.GetTotalTime();
			TE_LOG_CORE_INFO("Import Model: Process Textures toke {0} ms", t);
		}
		{
			assimpTimer.Start();
			ProcessMaterials(scene, aiscene, _MeshType);
			auto t = assimpTimer.GetTotalTime();
			TE_LOG_CORE_INFO("Import Model: Process Materials toke {0} ms", t);
		}

		{
			assimpTimer.Start();
			switch (_MeshType)
			{
			case TE_IDX_MESH_TYPE::MESH_NTT:
				ProcessMeshes(scene, aiscene);
				break;
			case TE_IDX_MESH_TYPE::MESH_SKINNED:
				ProcessMeshesSkinned(scene, aiscene);
				break;
			default:
				throw;
				break;
			}
			auto t = assimpTimer.GetTotalTime();
			TE_LOG_CORE_INFO("Import Model: Process Meshes toke {0} ms", t);
		}

		{
			assimpTimer.Start();

			float4x4 _matrix(
				.02f, .0f, .0f, .0f,
				.0f, .02f, .0f, .0f,
				.0f, .0f, .02f, 0.f,
				.0f, .0f, .0f, 1.0f
			);

			ProcessNodes(scene, aiscene->mRootNode, aiscene, meshOffset, _matrix);
			auto t = assimpTimer.GetTotalTime();
			TE_LOG_CORE_INFO("Import Model: Process Nodes toke {0} ms", t);
		}

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

			auto _matTex = texManager->CreateTexture(fileName.c_str(), reinterpret_cast<uint8_t*>(aiTex->pcData), aiTex->mWidth, aiTex->mHeight, dataSize, TE_RESOURCE_FORMAT::UNKNOWN);

			mMapTexFileName[name] = _matTex->GetViewIndex();
		}
	}


	void AssimpLib::ProcessMaterials(Scene* scene, const aiScene* aiscene, TE_IDX_MESH_TYPE _MeshType)
	{
		if (!aiscene->HasMaterials())
			return;

		ModelManager* modelManager = ModelManager::GetInstance().get();

		for (uint32_t i = 0; i < aiscene->mNumMaterials; ++i)
		{
			auto states = InitRenderStates();

			auto aiMaterial = aiscene->mMaterials[i];
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

				if (diffuseTextureCount > 0)
				{
					SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_TRUE);
				}

				for (UINT j = 0; j < diffuseTextureCount; ++j)
				{
					//if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, j, &ais) == AI_SUCCESS)
					if (aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), ais) == AI_SUCCESS)
					{
						//if (ais.C_Str()[0] == '*')
						if (auto aitex = aiscene->GetEmbeddedTexture(ais.C_Str()); aitex)
						{
							std::string_view name = aitex->mFilename.C_Str();
							auto index = name.find_last_of('/');
							name = name.substr(index);
							//auto index = std::atoi(&ais.C_Str()[1]);

							auto& _Itr = mMapTexFileName.find(name);
							if (_Itr == mMapTexFileName.end())
							{
								throw;
							}

							diffuseMapViewIndex = _Itr->second;
						}
						else
						{
							diffuseMapViewIndex = texManager->CreateTexture(ais.C_Str(), m_ModelFilePath)->GetViewIndex();
						}

					}
				}
			}

			uint32_t normalMapViewIndex = -1;

			{
				const auto normalTextureCount = aiMaterial->GetTextureCount(aiTextureType_NORMALS);

				if (normalTextureCount > 0)
				{
					SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, TE_RENDERER_STATE_ENABLED_MAP_NORMAL_TRUE);
				}

				for (UINT j = 0; j < normalTextureCount; ++j)
				{
					if (aiMaterial->GetTexture(aiTextureType_NORMALS, j, &ais) == AI_SUCCESS)
					{
						if (auto aitex = aiscene->GetEmbeddedTexture(ais.C_Str()); aitex)
						{
							std::string_view name = aitex->mFilename.C_Str();
							auto index = name.find_last_of('/');
							name = name.substr(index);
							//auto index = std::atoi(&ais.C_Str()[1]);

							auto& _Itr = mMapTexFileName.find(name);
							if (_Itr == mMapTexFileName.end())
							{
								throw;
							}

							normalMapViewIndex = _Itr->second;
						}
						/*if (ais.C_Str()[0] == '*')
						{
							auto index = std::atoi(&ais.C_Str()[1]);

							normalMapViewIndex = index + m_TextureMaterialOffset;
						}*/
						else
						{
							normalMapViewIndex = texManager->CreateTexture(ais.C_Str(), m_ModelFilePath)->GetViewIndex();
						}

					}
				}
			}


			{
				const auto heightTextureCount = aiMaterial->GetTextureCount(aiTextureType_HEIGHT);

				if (heightTextureCount > 0)
				{
					SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, TE_RENDERER_STATE_ENABLED_MAP_NORMAL_TRUE);
				}

				for (UINT j = 0; j < heightTextureCount; ++j)
				{
					if (aiMaterial->GetTexture(aiTextureType_HEIGHT, j, &ais) == AI_SUCCESS)
					{

						if (auto aitex = aiscene->GetEmbeddedTexture(ais.C_Str()); aitex)
						{
							std::string_view name = aitex->mFilename.C_Str();
							auto index = name.find_last_of('/');
							name = name.substr(index);
							//auto index = std::atoi(&ais.C_Str()[1]);

							auto& _Itr = mMapTexFileName.find(name);
							if (_Itr == mMapTexFileName.end())
							{
								throw;
							}

							normalMapViewIndex = _Itr->second;
						}
						else
						{
							normalMapViewIndex = texManager->CreateTexture(ais.C_Str(), m_ModelFilePath)->GetViewIndex();
						}

					}
				}
			}

			modelManager->m_MaterialManager.AddMaterial(
				states
				, float4{ aiColorDiffuse.r, aiColorDiffuse.g, aiColorDiffuse.b, 1.0f }
				, float3{ aiColorSpecular.r, aiColorSpecular.g, aiColorSpecular.b }
				, aiShininess
				, float2{ 1.0f, 1.0f }
				, float2{ .0f, .0f }
				, diffuseMapViewIndex
				, normalMapViewIndex
				, -1
				, 0, 0.0f, 0.0f, _MeshType);
		}
	}

	void AssimpLib::CenteralizeMeshVerteciesAddMeshEntity(const char* meshName, Mesh* mesh, Material* material, Scene* scene, const float4x4& transform)
	{
		auto& _aabb = mesh->GetBoundingBox();
		float4x4 _Transform(transform);
		/*if (_aabb.Center.x == 0 && _aabb.Center.y == 0 && _aabb.Center.z == 0)
		{
			return;
		}
		else
		{

			auto _xmMatrixTranslate = DirectX::XMMatrixTranslation(_aabb.Center.x, _aabb.Center.y, _aabb.Center.z);
			auto _xmMatrixTranslateInv = DirectX::XMMatrixInverse(nullptr, _xmMatrixTranslate);

			auto& _vertexPosData = mesh->GetVertexBuffer()->GetPosData();

			auto _vertexOffset = mesh->GetVertexOffset();
			for (uint32_t _vertexIndex = 0; _vertexIndex < mesh->GetVertexNum(); ++_vertexIndex)
			{
				auto& pos = _vertexPosData[_vertexOffset + _vertexIndex].Position;
				auto _xmVertex = XMLoadFloat3(&pos);
				_xmVertex = XMVector3TransformCoord(_xmVertex, _xmMatrixTranslateInv);
				XMStoreFloat3(&pos, _xmVertex);
			}

			auto _XMTransform = DirectX::XMMatrixMultiply(_xmMatrixTranslate, DirectX::XMLoadFloat4x4(&_Transform));
			XMStoreFloat4x4(&_Transform, _XMTransform);

			_aabb.Center = float3{ .0f, .0f, .0f };
		}*/

		Entity _MeshEntity = scene->AddMeshEntity(meshName, _Transform, mesh, material);

		if (m_IsLoadingAnimatedModel)
		{
			_MeshEntity.AddComponent<SkinnedAnimationComponent>(m_LoadedAnimation);
		}

	}

	void AssimpLib::ProcessNodes(Scene* scene, const aiNode* node, const aiScene* aiscene, const size_t meshOffset, const float4x4& _ParentTransform)
	{
		auto nodeMeshNum = node->mNumMeshes;

		float4x4 _NodeTransform(node->mTransformation);

		_NodeTransform = _ParentTransform * _NodeTransform;

		if (nodeMeshNum > 0)
		{
			//auto entity_model = scene->AddEntity(node->mName.C_Str());
			//auto& modelComponent = entity_model.AddComponent<ModelComponent>();
			//BoundingBox modelBoundingBox;

			for (uint32_t i = 0; i < nodeMeshNum; ++i)
			{
				auto aimesh = aiscene->mMeshes[node->mMeshes[i]];

				auto material = m_ModelManager->m_MaterialManager.GetMaterial(aimesh->mMaterialIndex + m_MaterialOffset);
				//auto entity_mesh = scene->AddEntity(aimesh->mName.C_Str(), entity_model);
				auto mesh = &m_ModelManager->m_Meshes[node->mMeshes[i] + meshOffset];
				auto meshName = aimesh->mName.C_Str();


				CenteralizeMeshVerteciesAddMeshEntity(meshName, mesh, material, scene, _NodeTransform);

				/*std::function<void()> func([this, mesh, material, meshName, scene]()
					{
						CenteralizeMeshVerteciesAddMeshEntity(meshName, mesh, material, scene);
					});

				m_Futures.emplace_back(TE_INSTANCE_THREADPOOL.Queue(func));*/


				/*if (modelBoundingBox.Extents.x == 1 && modelBoundingBox.Extents.y == 1 && modelBoundingBox.Extents.z == 1)
				{
					modelBoundingBox = meshBoundingBox;
				}
				else
				{
					CreateBoundingBoxMerged(modelBoundingBox, modelBoundingBox, meshBoundingBox);
				}*/
			}

			//entity_model.AddComponent<BoundingBoxComponent>(modelBoundingBox);
		}

		for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
		{
			ProcessNodes(scene, node->mChildren[childIndex], aiscene, meshOffset, _NodeTransform);
		}
	}

	void AssimpLib::ProcessMeshes(Scene* scene, const aiScene* aiscene)
	{

		VertexBufferNTT& _VertexBuffer = m_ModelManager->GetVertexBuffer<VertexBufferNTT>();

		for (uint32_t j = 0; j < aiscene->mNumMeshes; ++j)
		{

			auto indexOffset = m_ModelManager->GetIndexOffset();
			auto vertexOffset = _VertexBuffer.GetVertexOffset();

			auto aimesh = aiscene->mMeshes[j];


			for (uint32_t i = 0; i < aimesh->mNumVertices; ++i)
			{
				VertexData::Pos vPos;
				VertexData::NormTanTex vNormTanTex;

				auto vertex = aimesh->mVertices[i];

				vPos.Position = float3{ vertex.x, vertex.y, vertex.z };

				if (aimesh->HasNormals())
				{
					auto& normal = aimesh->mNormals[i];
					vNormTanTex.Normal = float3{ normal.x, normal.y, normal.z };
				}

				if (aimesh->HasTextureCoords(0))
				{
					auto& uv = aimesh->mTextureCoords[0][i];
					vNormTanTex.TexCoord = float2{ uv.x, uv.y };
				}

				if (aimesh->HasTangentsAndBitangents())
				{
					auto& tangent = aimesh->mTangents[i];
					vNormTanTex.Tangent = float3{ tangent.x, tangent.y, tangent.z };
				}

				_VertexBuffer.AddVertex(vPos, vNormTanTex);
			}

			uint32_t indexNum = 0;

			for (uint32_t i = 0; i < aimesh->mNumFaces; ++i)
			{
				aiFace& face = aimesh->mFaces[i];
				indexNum += face.mNumIndices;

				for (uint32_t j = 0; j < face.mNumIndices; ++j)
				{
					m_ModelManager->m_IndexBuffer.AddIndex(face.mIndices[j]);
				}
			}

			auto mesh = m_ModelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, aimesh->mNumVertices);

			auto material = m_ModelManager->m_MaterialManager.GetMaterial(aimesh->mMaterialIndex + m_MaterialOffset);

		}
	}


	void ExtractBoneWeightsForVertecies(aiMesh* mesh, std::map<UINT, std::vector<BoneWeight>>& VertexToBone, SA_Animation* _SAAnimation)
	{
		for (int i = 0; i < mesh->mNumBones; ++i)
		{
			auto bone = mesh->mBones[i];

			std::string bname = bone->mName.C_Str();

			auto BoneIndex = _SAAnimation->GetBoneIndex(bname);

			for (int j = 0; j < bone->mNumWeights; ++j)
			{
				auto& weight = bone->mWeights[j];

				VertexToBone[weight.mVertexId].push_back(BoneWeight(BoneIndex, weight.mWeight));
			}

		}
	}

	void AssimpLib::ProcessMeshesSkinned(Scene* scene, const aiScene* aiscene)
	{
		VertexBufferSkinned& _VertexBuffer = m_ModelManager->GetVertexBuffer<VertexBufferSkinned>();


		for (uint32_t j = 0; j < aiscene->mNumMeshes; ++j)
		{

			auto indexOffset = m_ModelManager->GetIndexOffset();
			auto vertexOffset = _VertexBuffer.GetVertexOffset();

			auto aimesh = aiscene->mMeshes[j];


			std::map<uint32_t, std::vector<BoneWeight>> mVertexToBone;

			ExtractBoneWeightsForVertecies(aimesh, mVertexToBone, m_LoadedAnimation);

			for (uint32_t i = 0; i < aimesh->mNumVertices; ++i)
			{
				VertexData::Pos vPos;
				VertexData::NormTanTex vNormTanTex;

				auto vertex = aimesh->mVertices[i];

				vPos.Position = float3{ vertex.x, vertex.y, vertex.z };

				if (aimesh->HasNormals())
				{
					auto& normal = aimesh->mNormals[i];
					vNormTanTex.Normal = float3{ normal.x, normal.y, normal.z };
				}

				if (aimesh->HasTextureCoords(0))
				{
					auto& uv = aimesh->mTextureCoords[0][i];
					vNormTanTex.TexCoord = float2{ uv.x, uv.y };
				}

				if (aimesh->HasTangentsAndBitangents())
				{
					auto& tangent = aimesh->mTangents[i];
					vNormTanTex.Tangent = float3{ tangent.x, tangent.y, tangent.z };
				}


				VertexData::Bone _Bone;
				int index = 0;

				float weight[3] = { 0.0f, 0.0f, 0.0f };

				for (auto& a : mVertexToBone[i])
				{
					if (index < 4)
					{
						_Bone.BoneIndex[index] = (byte)a.mBoneIndex;

						if (index < 3)
							weight[index] = a.mBoneWeight;

						++index;
					}
					else
					{
						break;
					}
				}

				_Bone.BoneWeights = float3(weight);

				_VertexBuffer.AddVertex(vPos, vNormTanTex, _Bone);
			}


			uint32_t indexNum = 0;

			for (uint32_t i = 0; i < aimesh->mNumFaces; ++i)
			{
				aiFace& face = aimesh->mFaces[i];
				indexNum += face.mNumIndices;

				for (uint32_t j = 0; j < face.mNumIndices; ++j)
				{
					m_ModelManager->m_IndexBuffer.AddIndex(face.mIndices[j]);
				}
			}

			auto mesh = m_ModelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_SKINNED, indexNum, indexOffset, vertexOffset, aimesh->mNumVertices);

			auto material = m_ModelManager->m_MaterialManager.GetMaterial(aimesh->mMaterialIndex + m_MaterialOffset);

		}
	}

	void AssimpLib::AddSpace(const aiScene* scene, TE_IDX_MESH_TYPE _MeshType)
	{

		uint32_t vertexNum = 0;
		uint32_t indexNum = 0;

		auto modelManager = ModelManager::GetInstance();

		auto meshNum = scene->mNumMeshes;

		modelManager->AddSpace(meshNum);

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

		switch (_MeshType)
		{
		case TE_IDX_MESH_TYPE::MESH_NTT:
			modelManager->GetVertexBuffer<VertexBufferNTT>().AddSpace(vertexNum);
			break;
		case TE_IDX_MESH_TYPE::MESH_SKINNED:
			modelManager->GetVertexBuffer<VertexBufferSkinned>().AddSpace(vertexNum);
			break;
		default:
			throw;
			break;
		}


		modelManager->m_IndexBuffer.AddSpace(indexNum);

	}

}