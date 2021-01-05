#include "pch.h"
#include "ModelManager.h"

#include "AssimpLib.h"

#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/GraphicDevice.h"

namespace TruthEngine::Core
{



	void ModelManager::ImportModel(const char* filePath)
	{
		TE_INSTANCE_GRAPHICDEVICE->WaitForGPU();

		AssimpLib::GetInstance()->ImportModel(filePath);

		InitVertexAndIndexBuffer();
	}

	void ModelManager::AddSampleModel()
	{
		m_MaterialManager.AddSampleMaterial();

		auto mesh = std::make_shared<Mesh>();
		mesh->m_IndexNum = 6;
		mesh->m_IndexOffset = 0;
		mesh->m_Material = m_MaterialManager.GetMaterial(0u);
		mesh->m_VertexOffset = 0;

		AddMesh(mesh);


		auto& model = m_Models3D.emplace_back(std::make_shared<Model3D>());
		model->m_Meshes.emplace_back(mesh.get());


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


	void ModelManager::AddMesh(std::shared_ptr<Mesh> mesh)
	{
		mesh->m_IndexBuffer = &m_IndexBuffer;
		mesh->m_VertexBuffer = &m_VertexBuffer_PosNormTanTex;

		m_Meshes.push_back(mesh);
	}

	Mesh* ModelManager::AddMesh(uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, Material* MaterialPtr)
	{
		return m_Meshes.emplace_back(std::make_shared<Mesh>(IndexNum, IndexOffset, VertexOffset, MaterialPtr, &m_VertexBuffer_PosNormTanTex, &m_IndexBuffer)).get();
	}

	TruthEngine::Core::Model3D* ModelManager::AddModel3D()
	{
		return m_Models3D.emplace_back(std::make_shared<Model3D>()).get();
	}


}