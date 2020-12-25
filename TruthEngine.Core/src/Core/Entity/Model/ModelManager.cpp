#include "pch.h"
#include "ModelManager.h"

#include "AssimpLib.h"

#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/RendererCommand.h"

namespace TruthEngine::Core
{



	void ModelManager::ImportModel(const char* filePath)
	{
		AssimpLib::GetInstance()->ImportModel(filePath);
	}

	void ModelManager::Init(BufferManager* bufferManager, RendererCommand* rendererCommand)
	{
		m_BufferManager = bufferManager;
		m_MaterialManager.Init(m_BufferManager);

		auto mesh = std::make_shared<Mesh>();
		mesh->m_IndexNum = 6;
		mesh->m_IndexOffset = 0;
		mesh->m_Material = m_MaterialManager.GetMaterial(0u);
		mesh->m_VertexOffset = 0;

		AddMesh(mesh);


		auto& model = m_Models3D.emplace_back(std::make_shared<Model3D>());
		model->m_Meshes.emplace_back(mesh.get());



		VertexData::Pos pos;
		VertexData::NormTex normTex;

		using float3 = DirectX::XMFLOAT3;

		/*pos.Position = float3{ -0.5f, -0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.0f, 0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, -0.5, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);*/

		pos.Position = float3{ -0.5f, -0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ -0.5f, 0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, 0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		
		
		pos.Position = float3{ -0.5f, -0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, 0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, -0.5f, 0.5f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);


		m_IndexBuffer.AddIndex(0);
		m_IndexBuffer.AddIndex(1);
		m_IndexBuffer.AddIndex(2);
		m_IndexBuffer.AddIndex(3);
		m_IndexBuffer.AddIndex(4);
		m_IndexBuffer.AddIndex(5);



		m_BufferManager->CreateVertexBuffer(&m_VertexBuffer_PosNormTex);
		m_BufferManager->CreateIndexBuffer(&m_IndexBuffer);

		rendererCommand->UploadData(&m_VertexBuffer_PosNormTex);
		rendererCommand->UploadData(&m_IndexBuffer);

	}


	void ModelManager::AddMesh(std::shared_ptr<Mesh> mesh)
	{
		mesh->m_IndexBuffer = &m_IndexBuffer;
		mesh->m_VertexBuffer = &m_VertexBuffer_PosNormTex;

		m_Meshes.push_back(mesh);
	}

	Mesh* ModelManager::AddMesh(uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset)
	{
		return m_Meshes.emplace_back(std::make_shared<Mesh>(IndexNum, IndexOffset, VertexOffset)).get();
	}


	TruthEngine::Core::Model3D* ModelManager::AddModel3D()
	{
		return m_Models3D.emplace_back(std::make_shared<Model3D>()).get();
	}


}