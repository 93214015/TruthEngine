#include "pch.h"
#include "ModelManager.h"

#include "AssimpLib.h"

#include "Core/Application.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/GraphicDevice.h"
#include "Core/Helper/MeshGenerator.h"
#include "Core/Entity/Components.h"

namespace TruthEngine
{


	ModelManager::ModelManager()
	{
		m_Meshes.reserve(100000);
		//m_Models3D.reserve(1000);
	}

	ModelManager::~ModelManager()
	{
	}

	void ModelManager::ImportModel(Scene* scene, const char* filePath)
	{

		AssimpLib::GetInstance()->ImportModel(scene, filePath);

		InitVertexAndIndexBuffer();
	}

	void ModelManager::InitVertexAndIndexBuffer()
	{

		TE_INSTANCE_GRAPHICDEVICE->WaitForGPU();

		auto _Lambda = [=](auto&& _VertexBuffer)
		{
			if (_VertexBuffer.GetVertexNum() > 0)
				m_BufferManager->CreateVertexBuffer(&_VertexBuffer);
		};

		std::apply([=](auto&&... _VertexBuffer) { ((_Lambda(_VertexBuffer)), ...);  }, m_VertexBuffers);
		m_BufferManager->CreateIndexBuffer(&m_IndexBuffer);

		m_RendererCommand->BeginGraphics();

		std::apply([_RendererCommand = m_RendererCommand.get()](auto&&... _VertexBuffer) { ((_RendererCommand->UploadData(&_VertexBuffer)), ...);  }, m_VertexBuffers);
		m_RendererCommand->UploadData(&m_IndexBuffer);

		m_RendererCommand->End();
	}


	void ModelManager::Init(BufferManager* bufferManager/*, RendererCommand* rendererCommand*/)
	{
		m_RendererCommand = std::make_shared<RendererCommand>();
		m_RendererCommand->Init(TE_IDX_RENDERPASS::NONE, TE_IDX_SHADERCLASS::NONE);

		m_BufferManager = bufferManager;
		m_MaterialManager.Init(m_BufferManager);
	}


	Mesh* ModelManager::AddMesh(TE_IDX_MESH_TYPE _MeshType, uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum)
	{
		Mesh* _Mesh = nullptr;

		switch (_MeshType)
		{
		case TE_IDX_MESH_TYPE::MESH_POINT:
			throw;
			break;
		case TE_IDX_MESH_TYPE::MESH_SIMPLE:
			throw;
			break;
		case TE_IDX_MESH_TYPE::MESH_NTT:
		{
			VertexBufferNTT& _VertexBuffer = _GetVertexBuffer<VertexBufferNTT>();
			BoundingBox bb;
			CreateBoundingBoxFromPoints(bb, vertexNum, &_VertexBuffer.GetPosData()[VertexOffset].Position, sizeof(VertexData::Pos));
			_Mesh = &m_Meshes.emplace_back(GenerateMeshID(), IndexNum, IndexOffset, VertexOffset, vertexNum, bb, &_VertexBuffer, &m_IndexBuffer);
			break;
		}
		case TE_IDX_MESH_TYPE::MESH_SKINNED:
		{
			VertexBufferSkinned& _VertexBuffer = _GetVertexBuffer<VertexBufferSkinned>();
			BoundingBox bb;
			CreateBoundingBoxFromPoints(bb, vertexNum, &_VertexBuffer.GetPosData()[VertexOffset].Position, sizeof(VertexData::Pos));
			_Mesh = &m_Meshes.emplace_back(GenerateMeshID(), IndexNum, IndexOffset, VertexOffset, vertexNum, bb, &_VertexBuffer, &m_IndexBuffer);
			break;
		}
		default:
			throw;
			break;
		}


		return _Mesh;
	}

	Entity ModelManager::GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE type, float size, const float4x4& transform, Entity modelEntity)
	{
		Mesh* mesh = nullptr;
		std::string _modelName, _meshName;

		switch (type)
		{
		case TruthEngine::TE_PRIMITIVE_TYPE::BOX:
			mesh = MeshGenerator::GetInstance()->GenerateBox(size);
			_modelName = "Model Box";
			_meshName = "Box";
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::ROUNDEDBOX:
			mesh = MeshGenerator::GetInstance()->GenerateRoundedBoxMesh(size);
			_modelName = "Model RoundedBox";
			_meshName = "RoundedBox";
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::SPHERE:
			mesh = MeshGenerator::GetInstance()->GenerateSphere(size);
			_modelName = "Model Sphere";
			_meshName = "Sphere";
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::CYLINDER:
			mesh = MeshGenerator::GetInstance()->GenerateCylinder(size);
			_modelName = "Model Cylinder";
			_meshName = "Cylinder";
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::CAPPEDCYLINDER:
			mesh = MeshGenerator::GetInstance()->GenerateCappedCylinder(size);
			_modelName = "Model Cylinder";
			_meshName = "Cylinder";
			break;
		case TE_PRIMITIVE_TYPE::PLANE:
			mesh = MeshGenerator::GetInstance()->GeneratePlane(size);
			_modelName = "Model Plane";
			_meshName = "Plane";
			break;
		default:
			break;
		}

		InitVertexAndIndexBuffer();


		auto scene = TE_INSTANCE_APPLICATION->GetActiveScene();

		auto material = m_MaterialManager.AddDefaultMaterial(TE_IDX_MESH_TYPE::MESH_NTT);

		return scene->AddMeshEntity(_meshName.c_str(), transform, mesh, material);

		/*auto meshEntity = scene->AddEntity(_meshName.c_str(), modelEntity, transform);
		meshEntity.AddComponent<MeshComponent>(mesh);
		meshEntity.AddComponent<MaterialComponent>(material);
		meshEntity.AddComponent<BoundingBoxComponent>(mesh->GetBoundingBox());
		return meshEntity;*/


	}

	void ModelManager::GenerateEnvironmentMesh(Mesh** outMesh)
	{
		std::string _modelName, _meshName;


		*outMesh = MeshGenerator::GetInstance()->GenerateSphere(1.0f);
		_modelName = "Model Environment";
		_meshName = "EnvironmentSphere";

		InitVertexAndIndexBuffer();

	}

	TruthEngine::Mesh* ModelManager::CopyMesh(Mesh* mesh)
	{
		auto& _newMesh = m_Meshes.emplace_back(GenerateMeshID(), mesh->m_IndexNum, mesh->m_IndexOffset, mesh->m_VertexOffset, mesh->m_VertexNum, mesh->m_BoundingBox, mesh->GetVertexBuffer(), &m_IndexBuffer);

		return &_newMesh;
	}


	size_t ModelManager::GetVertexOffset(TE_IDX_MESH_TYPE _MeshType) const noexcept
	{
		switch (_MeshType)
		{
		case TE_IDX_MESH_TYPE::MESH_POINT:
			throw;
			break;
		case TE_IDX_MESH_TYPE::MESH_SIMPLE:
			throw;
			break;
		case TE_IDX_MESH_TYPE::MESH_NTT:
			return std::get<0>(m_VertexBuffers).GetVertexOffset();
		case TE_IDX_MESH_TYPE::MESH_SKINNED:
			return std::get<1>(m_VertexBuffers).GetVertexOffset();
		default:
			throw;
			break;
		}
	}

	template<class T>
	T& ModelManager::_GetVertexBuffer()
	{
		return std::get<T>(m_VertexBuffers);
	}

}

