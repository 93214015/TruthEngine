#include "pch.h"
#include "ModelManager.h"

#include "AssimpLib.h"

#include "Core/Application.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/GraphicDevice.h"
#include "Core/Helper/MeshGenerator.h"
#include "Core/Entity/Components.h"

namespace TruthEngine::Core
{


	ModelManager::ModelManager()
	{
		m_Meshes.reserve(10000);
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

	void ModelManager::AddSampleModel()
	{
		m_MaterialManager.AddSampleMaterial();


		auto& mesh = m_Meshes.emplace_back();
		mesh.m_IndexNum = 6;
		mesh.m_IndexOffset = 0;
		//mesh.m_Material = m_MaterialManager.GetMaterial(0u);
		mesh.m_VertexOffset = 0;


		//auto& model = m_Models3D.emplace_back();
		//model.m_Meshes.emplace_back(&mesh);


		VertexData::Pos pos;
		VertexData::NormTanTex normTex{ float3{0.0f, 0.0f, -1.0f}, float3{0.0f, 0.0f, 0.0f}, float2{0.0f, 0.0f} };

		pos.Position = float3{ -0.5f, -0.5f, 0.5f };

		m_VertexBuffer_PosNormTanTex.AddVertex(pos, normTex);

		pos.Position = float3{ -0.5f, 0.5f, 0.5f };

		m_VertexBuffer_PosNormTanTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, 0.5f, 0.5f };

		m_VertexBuffer_PosNormTanTex.AddVertex(pos, normTex);


		pos.Position = float3{ -0.5f, -0.5f, 0.5f };

		m_VertexBuffer_PosNormTanTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, 0.5f, 0.5f };

		m_VertexBuffer_PosNormTanTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, -0.5f, 0.5f };

		m_VertexBuffer_PosNormTanTex.AddVertex(pos, normTex);


		m_IndexBuffer.AddIndex(0);
		m_IndexBuffer.AddIndex(1);
		m_IndexBuffer.AddIndex(2);
		m_IndexBuffer.AddIndex(3);
		m_IndexBuffer.AddIndex(4);
		m_IndexBuffer.AddIndex(5);

		InitVertexAndIndexBuffer();
	}

	void ModelManager::InitVertexAndIndexBuffer()
	{

		TE_INSTANCE_GRAPHICDEVICE->WaitForGPU();


		m_BufferManager->CreateVertexBuffer(&m_VertexBuffer_PosNormTanTex);
		m_BufferManager->CreateIndexBuffer(&m_IndexBuffer);

		m_RendererCommand->Begin();

		m_RendererCommand->UploadData(&m_VertexBuffer_PosNormTanTex);
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


	/*void ModelManager::AddMesh(std::shared_ptr<Mesh> mesh)
	{
		mesh->m_IndexBuffer = &m_IndexBuffer;
		mesh->m_VertexBuffer = &m_VertexBuffer_PosNormTanTex;

		m_Meshes.push_back(mesh);
	}*/

	Mesh* ModelManager::AddMesh(uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum)
	{
		BoundingBox bb;
		CreateBoundingBoxFromPoints(bb, vertexNum, &m_VertexBuffer_PosNormTanTex.GetVertexData<VertexData::Pos>()[VertexOffset].Position, sizeof(VertexData::Pos));


		auto& mesh = m_Meshes.emplace_back(IndexNum, IndexOffset, VertexOffset, vertexNum, bb, &m_VertexBuffer_PosNormTanTex, &m_IndexBuffer);
		return &mesh;
	}

	Entity ModelManager::GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE type, float size, const float4x4& transform, Entity modelEntity)
	{
		Mesh* mesh = nullptr;
		std::string _modelName, _meshName;

		switch (type)
		{
		case TruthEngine::Core::TE_PRIMITIVE_TYPE::BOX:
			mesh = MeshGenerator::GetInstance()->GenerateBox(size);
			_modelName = "Model Box";
			_meshName = "Box";
			break;
		case TruthEngine::Core::TE_PRIMITIVE_TYPE::ROUNDEDBOX:
			mesh = MeshGenerator::GetInstance()->GenerateRoundedBoxMesh(size);
			_modelName = "Model RoundedBox";
			_meshName = "RoundedBox";
			break;
		case TruthEngine::Core::TE_PRIMITIVE_TYPE::SPHERE:
			mesh = MeshGenerator::GetInstance()->GenerateSphere(size);
			_modelName = "Model Sphere";
			_meshName = "Sphere";
			break;
		case TruthEngine::Core::TE_PRIMITIVE_TYPE::CYLINDER:
			mesh = MeshGenerator::GetInstance()->GenerateCylinder(size);
			_modelName = "Model Cylinder";
			_meshName = "Cylinder";
			break;
		case TruthEngine::Core::TE_PRIMITIVE_TYPE::CAPPEDCYLINDER:
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

		if (!modelEntity)
		{
			modelEntity = scene->AddEntity(_modelName.c_str());
			modelEntity.AddComponent<ModelComponent>();
		}

		auto material = m_MaterialManager.AddDefaultMaterial(TE_IDX_MESH_TYPE::MESH_NTT);

		auto meshEntity = scene->AddEntity(_meshName.c_str(), modelEntity, transform);
		meshEntity.AddComponent<MeshComponent>(mesh);
		meshEntity.AddComponent<MaterialComponent>(material);
		meshEntity.AddComponent<BoundingBoxComponent>(mesh->GetBoundingBox());

		return meshEntity;

	}

	TruthEngine::Core::Mesh* ModelManager::CopyMesh(Mesh* mesh)
	{
		auto& _newMesh = m_Meshes.emplace_back(mesh->m_IndexNum , mesh->m_IndexOffset, mesh->m_VertexOffset, mesh->m_VertexNum, mesh->m_BoundingBox, &m_VertexBuffer_PosNormTanTex, &m_IndexBuffer);

		return &_newMesh;
	}

	/*TruthEngine::Core::Model3D* ModelManager::AddModel3D()
	{
		return &m_Models3D.emplace_back();
	}*/


}