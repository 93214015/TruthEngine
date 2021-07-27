#include "pch.h"
#include "MeshGenerator.h"

#include "Core/Entity/Model/ModelManager.h"

#pragma comment(lib, "ilmola-GeoGenerator.lib")

#include "GeometryGenerator/BoxMesh.hpp"
#include "GeometryGenerator/RoundedBoxMesh.hpp"
#include "GeometryGenerator/SphereMesh.hpp"
#include "GeometryGenerator/CylinderMesh.hpp"
#include "GeometryGenerator/CappedCylinderMesh.hpp"

#include "DirectXMesh/DirectXMesh.h"

namespace TruthEngine
{
	template<class T>
	Mesh* GenerateTEMesh(T& _PrimitveMesh)
	{
		auto modelManager = ModelManager::GetInstance();
		auto& vertexBuffer = modelManager->GetVertexBuffer<VertexBufferNTT>();

		size_t indexOffset = modelManager->GetIndexOffset();
		size_t vertexOffset = modelManager->GetVertexOffset(TE_IDX_MESH_TYPE::MESH_NTT);

		auto vertecies = _PrimitveMesh.vertices();
		auto triangles = _PrimitveMesh.triangles();
		auto vertexNum = static_cast<size_t>(generator::count(vertecies));
		auto indexNum = static_cast<size_t>(generator::count(triangles) * 3);
		vertexBuffer.AddSpace(vertexNum);

		std::vector<float3> _PositionData;
		std::vector<float3> _NormalData;
		std::vector<float2> _UVData;

		size_t _VertexCount = static_cast<size_t>(generator::count(vertecies));

		_PositionData.reserve(_VertexCount);
		_NormalData.reserve(_VertexCount);
		_UVData.reserve(_VertexCount);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			//vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			_PositionData.emplace_back((float)pos[0], (float)pos[1], (float)pos[2]);
			_NormalData.emplace_back((float)normal[0], (float)normal[1], (float)normal[2]);
			_UVData.emplace_back((float)texCoord[0], (float)texCoord[1]);
			//m_NTTData.emplace_back(VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		size_t _TriangleCount = static_cast<size_t>(generator::count(triangles));

		std::vector<uint32_t> _IndeciesData;
		_IndeciesData.reserve(_TriangleCount * 3);


		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		{
			DirectX::XMFLOAT4* _TangentData = new DirectX::XMFLOAT4[_VertexCount];

			HRESULT _HResult = DirectX::ComputeTangentFrame(_IndeciesData.data(), _TriangleCount, _PositionData.data(), _NormalData.data(), _UVData.data(), _VertexCount, _TangentData);
			assert(SUCCEEDED(_HResult));

			for (size_t i = 0; i < _VertexCount; ++i)
			{
				vertexBuffer.AddVertex(VertexData::Pos{ _PositionData[i] }, VertexData::NormTanTex{ _NormalData[i], float3{ _TangentData[i].x, _TangentData[i].y, _TangentData[i].z }, _UVData[i] });
			}

			delete[] _TangentData;
		}

		IndexBuffer& _IndexBuffer = modelManager->GetIndexBuffer();

		for (size_t i = 0; i < _TriangleCount * 3; ++i)
		{
			_IndexBuffer.AddIndex(_IndeciesData[i]);
		}


		return &modelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, vertexNum);
	}



	MeshGenerator::MeshGenerator()
	{}

	Mesh* MeshGenerator::GenerateBox(float size_x, float size_y, float size_z)
	{
		generator::BoxMesh box({ size_x, size_y, size_z }, { 1, 1, 1 });

		return GenerateTEMesh(box);

		/*auto vertecies = box.vertices();
		auto triangles = box.triangles();
		auto vertexNum = static_cast<size_t>(generator::count(vertecies));
		auto indexNum = static_cast<size_t>(generator::count(triangles) * 3);
		vertexBuffer.AddSpace(vertexNum);

		std::vector<float3> _PositionData;
		std::vector<float3> _NormalData;
		std::vector<float2> _UVData;

		size_t _VertexCount = static_cast<size_t>(generator::count(vertecies));

		_PositionData.reserve(_VertexCount);
		_NormalData.reserve(_VertexCount);
		_UVData.reserve(_VertexCount);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			//vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			_PositionData.emplace_back( (float)pos[0], (float)pos[1], (float)pos[2]);
			_NormalData.emplace_back( (float)normal[0], (float)normal[1], (float)normal[2] );
			_UVData.emplace_back((float)texCoord[0], (float)texCoord[1] );
			//m_NTTData.emplace_back(VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		size_t _TriangleCount = static_cast<size_t>(generator::count(triangles));

		std::vector<uint32_t> _IndeciesData;
		_IndeciesData.reserve(_TriangleCount * 3);


		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		{
			DirectX::XMFLOAT4* _TangentData = new DirectX::XMFLOAT4[_VertexCount];

			HRESULT _HResult = DirectX::ComputeTangentFrame(_IndeciesData.data(), _TriangleCount, _PositionData.data(), _NormalData.data(), _UVData.data(), _VertexCount, _TangentData);
			assert(SUCCEEDED(_HResult));

			for (size_t i = 0; i < _VertexCount; ++i)
			{
				vertexBuffer.AddVertex(VertexData::Pos{ _PositionData[i] }, VertexData::NormTanTex{ _NormalData[i], float3{ _TangentData[i].x, _TangentData[i].y, _TangentData[i].z }, _UVData[i]});
			}

			delete[] _TangentData;
		}

		for (size_t i = 0; i < _TriangleCount * 3; ++i)
		{
			modelManager->m_IndexBuffer.AddIndex(_IndeciesData[i]);
		}


		return modelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, vertexNum);*/
	}

	Mesh* MeshGenerator::GenerateSphere(float size)
	{
		generator::SphereMesh sphere(size);

		return GenerateTEMesh(sphere);

		/*auto vertecies = sphere.vertices();
		auto triangles = sphere.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum);


		std::vector<DirectX::XMFLOAT3> _PositionData;
		std::vector<DirectX::XMFLOAT3> _NormalData;
		std::vector<DirectX::XMFLOAT2> _UVData;

		auto _VertexCount = static_cast<uint32_t>(count(vertecies));

		_PositionData.reserve(_VertexCount);
		_NormalData.reserve(_VertexCount);
		_UVData.reserve(_VertexCount);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			//vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			_PositionData.emplace_back(float3{ (float)pos[0], (float)pos[1], (float)pos[2] });
			_NormalData.emplace_back(float3{ (float)normal[0], (float)normal[1], (float)normal[2] });
			_UVData.emplace_back(float2{ (float)texCoord[0], (float)texCoord[1] });
			//m_NTTData.emplace_back(VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		auto _TriangleCount = static_cast<uint32_t>(count(triangles));

		std::vector<uint32_t> _IndeciesData;
		_IndeciesData.reserve(_TriangleCount * 3);


		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		DirectX::XMFLOAT4* _TangentData = new DirectX::XMFLOAT4[_VertexCount];

		DirectX::ComputeTangentFrame(_IndeciesData.data(), (size_t)_TriangleCount, _PositionData.data(), _NormalData.data(), _UVData.data(), (size_t)_VertexCount, _TangentData);

		for (uint32_t i = 0; i < _VertexCount; ++i)
		{
			vertexBuffer.AddVertex(VertexData::Pos{ _PositionData[i] }, VertexData::NormTanTex{ (float3)_NormalData[i], float3{ _TangentData[i].x, _TangentData[i].y, _TangentData[i].z }, float2{_UVData[i].x, _UVData[i].y} });
		}

		delete[] _TangentData;

		for (uint32_t i = 0; i < _TriangleCount * 3; ++i)
		{
			modelManager->m_IndexBuffer.AddIndex(_IndeciesData[i]);
		}


		return modelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, vertexNum);*/
	}

	Mesh* MeshGenerator::GenerateRoundedBoxMesh(float size_x, float size_y, float size_z)
	{
		generator::RoundedBoxMesh roundedBoxMesh(.25f, { size_x, size_y, size_z });

		return GenerateTEMesh(roundedBoxMesh);

		/*auto vertecies = roundedBoxMesh.vertices();
		auto triangles = roundedBoxMesh.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum);

		std::vector<DirectX::XMFLOAT3> _PositionData;
		std::vector<DirectX::XMFLOAT3> _NormalData;
		std::vector<DirectX::XMFLOAT2> _UVData;

		auto _VertexCount = static_cast<uint32_t>(count(vertecies));

		_PositionData.reserve(_VertexCount);
		_NormalData.reserve(_VertexCount);
		_UVData.reserve(_VertexCount);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			//vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			_PositionData.emplace_back(float3{ (float)pos[0], (float)pos[1], (float)pos[2] });
			_NormalData.emplace_back(float3{ (float)normal[0], (float)normal[1], (float)normal[2] });
			_UVData.emplace_back(float2{ (float)texCoord[0], (float)texCoord[1] });
			//m_NTTData.emplace_back(VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		auto _TriangleCount = static_cast<uint32_t>(count(triangles));

		std::vector<uint32_t> _IndeciesData;
		_IndeciesData.reserve(_TriangleCount * 3);


		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		DirectX::XMFLOAT4* _TangentData = new DirectX::XMFLOAT4[_VertexCount];

		DirectX::ComputeTangentFrame(_IndeciesData.data(), (size_t)_TriangleCount, _PositionData.data(), _NormalData.data(), _UVData.data(), (size_t)_VertexCount, _TangentData);

		for (uint32_t i = 0; i < _VertexCount; ++i)
		{
			vertexBuffer.AddVertex(VertexData::Pos{ _PositionData[i] }, VertexData::NormTanTex{ (float3)_NormalData[i], float3{ _TangentData[i].x, _TangentData[i].y, _TangentData[i].z }, float2{_UVData[i].x, _UVData[i].y} });
		}

		delete[] _TangentData;

		for (uint32_t i = 0; i < _TriangleCount * 3; ++i)
		{
			modelManager->m_IndexBuffer.AddIndex(_IndeciesData[i]);
		}


		return modelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, vertexNum);*/
	}

	Mesh* MeshGenerator::GenerateCylinder(float size)
	{
		generator::CylinderMesh cylinder(size, size);

		return GenerateTEMesh(cylinder);

		/*auto vertecies = cylinder.vertices();
		auto triangles = cylinder.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum);

		std::vector<DirectX::XMFLOAT3> _PositionData;
		std::vector<DirectX::XMFLOAT3> _NormalData;
		std::vector<DirectX::XMFLOAT2> _UVData;

		auto _VertexCount = static_cast<uint32_t>(count(vertecies));

		_PositionData.reserve(_VertexCount);
		_NormalData.reserve(_VertexCount);
		_UVData.reserve(_VertexCount);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			//vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			_PositionData.emplace_back(float3{ (float)pos[0], (float)pos[1], (float)pos[2] });
			_NormalData.emplace_back(float3{ (float)normal[0], (float)normal[1], (float)normal[2] });
			_UVData.emplace_back(float2{ (float)texCoord[0], (float)texCoord[1] });
			//m_NTTData.emplace_back(VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		auto _TriangleCount = static_cast<uint32_t>(count(triangles));

		std::vector<uint32_t> _IndeciesData;
		_IndeciesData.reserve(_TriangleCount * 3);


		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		DirectX::XMFLOAT4* _TangentData = new DirectX::XMFLOAT4[_VertexCount];

		DirectX::ComputeTangentFrame(_IndeciesData.data(), (size_t)_TriangleCount, _PositionData.data(), _NormalData.data(), _UVData.data(), (size_t)_VertexCount, _TangentData);

		for (uint32_t i = 0; i < _VertexCount; ++i)
		{
			vertexBuffer.AddVertex(VertexData::Pos{ _PositionData[i] }, VertexData::NormTanTex{ (float3)_NormalData[i], float3{ _TangentData[i].x, _TangentData[i].y, _TangentData[i].z }, float2{_UVData[i].x, _UVData[i].y} });
		}

		delete[] _TangentData;

		for (uint32_t i = 0; i < _TriangleCount * 3; ++i)
		{
			modelManager->m_IndexBuffer.AddIndex(_IndeciesData[i]);
		}


		return modelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, vertexNum);*/
	}

	Mesh* MeshGenerator::GenerateCappedCylinder(float size)
	{
		
		generator::CappedCylinderMesh cylinder(size, size);
		
		return GenerateTEMesh(cylinder);

		/*auto vertecies = cylinder.vertices();
		auto triangles = cylinder.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = static_cast<uint32_t>(count(triangles)) * 3;
		vertexBuffer.AddSpace(vertexNum);

		std::vector<DirectX::XMFLOAT3> _PositionData;
		std::vector<DirectX::XMFLOAT3> _NormalData;
		std::vector<DirectX::XMFLOAT2> _UVData;

		auto _VertexCount = static_cast<uint32_t>(count(vertecies));

		_PositionData.reserve(_VertexCount);
		_NormalData.reserve(_VertexCount);
		_UVData.reserve(_VertexCount);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			//vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			_PositionData.emplace_back(float3{ (float)pos[0], (float)pos[1], (float)pos[2] });
			_NormalData.emplace_back(float3{ (float)normal[0], (float)normal[1], (float)normal[2] });
			_UVData.emplace_back(float2{ (float)texCoord[0], (float)texCoord[1] });
			//m_NTTData.emplace_back(VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		auto _TriangleCount = static_cast<uint32_t>(count(triangles));

		std::vector<uint32_t> _IndeciesData;
		_IndeciesData.reserve(_TriangleCount * 3);


		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		DirectX::XMFLOAT4* _TangentData = new DirectX::XMFLOAT4[_VertexCount];

		DirectX::ComputeTangentFrame(_IndeciesData.data(), (size_t)_TriangleCount, _PositionData.data(), _NormalData.data(), _UVData.data(), (size_t)_VertexCount, _TangentData);

		for (uint32_t i = 0; i < _VertexCount; ++i)
		{
			vertexBuffer.AddVertex(VertexData::Pos{ _PositionData[i] }, VertexData::NormTanTex{ (float3)_NormalData[i], float3{ _TangentData[i].x, _TangentData[i].y, _TangentData[i].z }, float2{_UVData[i].x, _UVData[i].y} });
		}

		delete[] _TangentData;

		for (uint32_t i = 0; i < _TriangleCount * 3; ++i)
		{
			modelManager->m_IndexBuffer.AddIndex(_IndeciesData[i]);
		}


		return modelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, vertexNum);*/
	}

	Mesh* MeshGenerator::GeneratePlane(float size_x, float size_z)
	{		
		generator::BoxMesh plane({ size_x, 0.1f, size_z }, { 1, 1, 1 });

		return GenerateTEMesh(plane);

		/*auto vertecies = plane.vertices();
		auto triangles = plane.triangles();
		uint32_t vertexNum = static_cast<uint32_t>(count(vertecies));
		uint32_t indexNum = count(triangles) * 3;
		vertexBuffer.AddSpace(vertexNum);


		std::vector<DirectX::XMFLOAT3> _PositionData;
		std::vector<DirectX::XMFLOAT3> _NormalData;
		std::vector<DirectX::XMFLOAT2> _UVData;

		auto _VertexCount = static_cast<uint32_t>(count(vertecies));

		_PositionData.reserve(_VertexCount);
		_NormalData.reserve(_VertexCount);
		_UVData.reserve(_VertexCount);

		while (!vertecies.done())
		{
			auto vertex = vertecies.generate();
			auto pos = vertex.position;
			auto normal = vertex.normal;
			auto texCoord = vertex.texCoord;
			//vertexBuffer.AddVertex(VertexData::Pos{ float3{(float)pos[0], (float)pos[1], (float)pos[2]} }, VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			_PositionData.emplace_back(float3{ (float)pos[0], (float)pos[1], (float)pos[2] });
			_NormalData.emplace_back(float3{ (float)normal[0], (float)normal[1], (float)normal[2] });
			_UVData.emplace_back(float2{ (float)texCoord[0], (float)texCoord[1] });
			//m_NTTData.emplace_back(VertexData::NormTanTex(float3{ (float)normal[0], (float)normal[1], (float)normal[2] }, float3{ .0f, .0f, .0f }, float2{ (float)texCoord[0], (float)texCoord[1] }));

			vertecies.next();
		}

		auto _TriangleCount = static_cast<uint32_t>(count(triangles));

		std::vector<uint32_t> _IndeciesData;
		_IndeciesData.reserve(_TriangleCount * 3);


		while (!triangles.done())
		{
			auto triangle = triangles.generate();
			auto index = triangle.vertices[0];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[1];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			index = triangle.vertices[2];
			_IndeciesData.emplace_back((uint32_t)index);
			//modelManager->m_IndexBuffer.AddIndex((uint32_t)index);
			triangles.next();
		}

		DirectX::XMFLOAT4* _TangentData = new DirectX::XMFLOAT4[_VertexCount];

		DirectX::ComputeTangentFrame(_IndeciesData.data(), (size_t)_TriangleCount, _PositionData.data(), _NormalData.data(), _UVData.data(), (size_t)_VertexCount, _TangentData);

		for (uint32_t i = 0; i < _VertexCount; ++i)
		{
			vertexBuffer.AddVertex(VertexData::Pos{ _PositionData[i] }, VertexData::NormTanTex{ (float3)_NormalData[i], float3{ _TangentData[i].x, _TangentData[i].y, _TangentData[i].z }, float2{_UVData[i].x, _UVData[i].y} });
		}

		delete[] _TangentData;

		for (uint32_t i = 0; i < _TriangleCount * 3; ++i)
		{
			modelManager->m_IndexBuffer.AddIndex(_IndeciesData[i]);
		}


		return modelManager->AddMesh(TE_IDX_MESH_TYPE::MESH_NTT, indexNum, indexOffset, vertexOffset, vertexNum);*/
	}

}