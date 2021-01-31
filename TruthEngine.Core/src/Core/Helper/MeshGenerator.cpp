#include "pch.h"
#include "MeshGenerator.h"

#include "Core/Entity/Model/ModelManager.h"

#pragma comment(lib, "ilmola-GeoGenerator.lib")

#include "GeometryGenerator/BoxMesh.hpp"
#include "GeometryGenerator/RoundedBoxMesh.hpp"
#include "GeometryGenerator/SphereMesh.hpp"
#include "GeometryGenerator/CylinderMesh.hpp"
#include "GeometryGenerator/CappedCylinderMesh.hpp"



namespace TruthEngine::Core
{
	MeshGenerator::MeshGenerator()
	{}

	Mesh* MeshGenerator::GenerateBox(float size)
	{
		auto modelManager = ModelManager::GetInstance().get();
		auto& vertexBuffer = modelManager->m_VertexBuffer_PosNormTanTex;

		uint32_t indexOffset = modelManager->GetIndexOffset();
		uint32_t vertexOffset = modelManager->GetVertexOffset();
		
		generator::BoxMesh box({ size, size, size }, {1, 1, 1});
		auto vertecies = box.vertices();
		auto triangles = box.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = count(triangles) * 3;
		vertexBuffer.AddSpace(vertexNum, indexNum);


		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));
			
			vertecies.next();
		}
		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}
		/*std::for_each(begin(vertecies), end(vertecies), [modelManager, &vertexBuffer, &index](const generator::MeshVertex& vertex) 
			{			
				auto pos = (const float*)(vertex.position.data());
				auto normal = (const float*)vertex.normal.data();
				auto texCoord = (const float*)vertex.texCoord.data();
				vertexBuffer.AddVertex(VertexData::Pos{ float3{pos[0], pos[1], pos[2]} } , VertexData::NormTanTex(float3{ normal[0], normal[1], normal[2] }, float3{ .0f, .0f, .0f }, float2{texCoord[0], texCoord[1]}));
				modelManager->m_IndexBuffer.AddIndex(index);
				index++;
			}
		);	*/

		auto mesh = modelManager->AddMesh(indexNum, indexOffset, vertexOffset, vertexNum);

		return mesh;
	}

	Mesh* MeshGenerator::GenerateSphere(float size)
	{

		auto modelManager = ModelManager::GetInstance();
		auto& vertexBuffer = modelManager->m_VertexBuffer_PosNormTanTex;

		uint32_t indexOffset = modelManager->GetIndexOffset();
		uint32_t vertexOffset = modelManager->GetVertexOffset();

		generator::SphereMesh sphere(size);
		auto vertecies = sphere.vertices();
		auto triangles = sphere.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum, indexNum);
		
		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}
		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		return modelManager->AddMesh(indexNum, indexOffset, vertexOffset, vertexNum);
	}

	TruthEngine::Core::Mesh* MeshGenerator::GenerateRoundedBoxMesh(float size)
	{
		auto modelManager = ModelManager::GetInstance();
		auto& vertexBuffer = modelManager->m_VertexBuffer_PosNormTanTex;

		uint32_t indexOffset = modelManager->GetIndexOffset();
		uint32_t vertexOffset = modelManager->GetVertexOffset();

		generator::RoundedBoxMesh roundedBoxMesh(.25f, { size, size, size});
		auto vertecies = roundedBoxMesh.vertices();
		auto triangles = roundedBoxMesh.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum, indexNum);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}
		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		return modelManager->AddMesh(indexNum, indexOffset, vertexOffset, vertexNum);
	}

	TruthEngine::Core::Mesh* MeshGenerator::GenerateCylinder(float size)
	{
		auto modelManager = ModelManager::GetInstance();
		auto& vertexBuffer = modelManager->m_VertexBuffer_PosNormTanTex;

		uint32_t indexOffset = modelManager->GetIndexOffset();
		uint32_t vertexOffset = modelManager->GetVertexOffset();


		generator::CylinderMesh cylinder(size, size);
		auto vertecies = cylinder.vertices();
		auto triangles = cylinder.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum, indexNum);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}
		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		return modelManager->AddMesh(indexNum, indexOffset, vertexOffset, vertexNum);
	}

	TruthEngine::Core::Mesh* MeshGenerator::GenerateCappedCylinder(float size)
	{
		auto modelManager = ModelManager::GetInstance();
		auto& vertexBuffer = modelManager->m_VertexBuffer_PosNormTanTex;

		uint32_t indexOffset = modelManager->GetIndexOffset();
		uint32_t vertexOffset = modelManager->GetVertexOffset();

		generator::CappedCylinderMesh cylinder(size, size);
		auto vertecies = cylinder.vertices();
		auto triangles = cylinder.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum, indexNum);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}
		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		return modelManager->AddMesh(indexNum, indexOffset, vertexOffset, vertexNum);
	}

	TruthEngine::Core::Mesh* MeshGenerator::GeneratePlane(float size)
	{
		auto modelManager = ModelManager::GetInstance().get();
		auto& vertexBuffer = modelManager->m_VertexBuffer_PosNormTanTex;

		uint32_t indexOffset = modelManager->GetIndexOffset();
		uint32_t vertexOffset = modelManager->GetVertexOffset();

		generator::BoxMesh plane({ size, 0.1f, size }, { 1, 1, 1 });
		auto vertecies = plane.vertices();
		auto triangles = plane.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = count(triangles) * 3;
		vertexBuffer.AddSpace(vertexNum, indexNum);


		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		auto mesh = modelManager->AddMesh(indexNum, indexOffset, vertexOffset, vertexNum);

		return mesh;
	}

}