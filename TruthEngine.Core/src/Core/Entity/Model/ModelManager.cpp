#include "pch.h"
#include "ModelManager.h"

#include "Core/Renderer/BufferManager.h"

namespace TruthEngine::Core
{



	void ModelManager::ImportModel(const char* filePath)
	{

	}

	void ModelManager::Init(BufferManager* bufferManager)
	{
		m_BufferManager = bufferManager;
		m_MaterialManager.Init(m_BufferManager);

		auto& mesh = m_Meshes.emplace_back();
		mesh.m_IndexBuffer = &m_IndexBuffer;
		mesh.m_VertexBuffer = &m_VertexBuffer_PosNormTex;
		mesh.m_IndexNum = 3;
		mesh.m_IndexOffset = 0;
		mesh.m_Material = m_MaterialManager.GetMaterial(0u);
		mesh.m_VertexOffset = 0;


		auto& model = m_Models3D.emplace_back();
		model.m_Meshes.emplace_back(&mesh);



		VertexData::Pos pos;
		VertexData::NormTex normTex;

		using float3 = DirectX::XMFLOAT3;

		pos.Position = float3{ -0.5f, -0.5f, 0.0f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.0f, 0.5f, 0.0f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);

		pos.Position = float3{ 0.5f, -0.5, 0.0f };

		m_VertexBuffer_PosNormTex.AddVertex(pos, normTex);


		m_IndexBuffer.AddIndex(0);
		m_IndexBuffer.AddIndex(1);
		m_IndexBuffer.AddIndex(2);


		m_BufferManager->CreateVertexBuffer(&m_VertexBuffer_PosNormTex);
		m_BufferManager->CreateIndexBuffer(&m_IndexBuffer);

	}


}