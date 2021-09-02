#include "pch.h"
#include "ModelManager.h"

#include "MeshHandle.h"
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
	}

	ModelManager::~ModelManager()
	{
	}

	void ModelManager::InitVertexAndIndexBuffer()
	{

		TE_INSTANCE_GRAPHICDEVICE->WaitForGPU();

		auto _Lambda = [_BufferManager = m_BufferManager](auto&& _VertexBuffer)
		{
			if (_VertexBuffer.GetVertexNum() > 0)
				_BufferManager->CreateVertexBuffer(&_VertexBuffer);
		};

		std::apply([=](auto&&... _VertexBuffer) { ((_Lambda(_VertexBuffer)), ...);  }, m_VertexBuffers);
		m_BufferManager->CreateIndexBuffer(&m_IndexBuffer);

		m_RendererCommand->BeginGraphics();

		std::apply([_RendererCommand = m_RendererCommand.get()](auto&&... _VertexBuffer) { ((_RendererCommand->UploadData(&_VertexBuffer)), ...);  }, m_VertexBuffers);
		m_RendererCommand->UploadData(&m_IndexBuffer);

		m_RendererCommand->End();
	}

	void ModelManager::Init(BufferManager* bufferManager)
	{
		m_RendererCommand = std::make_shared<RendererCommand>();
		m_RendererCommand->Init(TE_IDX_RENDERPASS::NONE, TE_IDX_SHADERCLASS::NONE);

		m_BufferManager = bufferManager;
	}

	MeshHandle ModelManager::AddMesh(TE_IDX_MESH_TYPE _MeshType, uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum)
	{

		VertexBufferBase* _VertexBufferBase = nullptr;

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
			_VertexBufferBase = &_VertexBuffer;
			/*BoundingAABox bb;
			CreateBoundingAABoxFromPoints(bb, vertexNum, &_VertexBuffer.GetPosData()[VertexOffset].Position, sizeof(VertexData::Pos));*/
			m_Meshes.emplace_back(GenerateMeshID(), IndexNum, IndexOffset, VertexOffset, vertexNum, &_VertexBuffer, &m_IndexBuffer);
			break;
		}
		case TE_IDX_MESH_TYPE::MESH_SKINNED:
		{
			VertexBufferSkinned& _VertexBuffer = _GetVertexBuffer<VertexBufferSkinned>();
			_VertexBufferBase = &_VertexBuffer;
			/*BoundingAABox bb;
			CreateBoundingAABoxFromPoints(bb, vertexNum, &_VertexBuffer.GetPosData()[VertexOffset].Position, sizeof(VertexData::Pos));*/
			break;
		}
		default:
			throw;
			break;
		}

		MeshHandle _MeshHandle{ static_cast<uint32_t>(m_Meshes.size()) };
		m_Meshes.emplace_back(GenerateMeshID(), IndexNum, IndexOffset, VertexOffset, vertexNum, _VertexBufferBase, &m_IndexBuffer);
		return _MeshHandle;
	}

	MeshHandle ModelManager::GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE type, const float3& size, float radius, const int3& segments, const int3& slices)
	{
		MeshHandle _MeshHandle{0};

		switch (type)
		{
		case TruthEngine::TE_PRIMITIVE_TYPE::BOX:
			_MeshHandle = MeshGenerator::GetInstance()->GenerateBox(size, segments);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::ROUNDEDBOX:
			_MeshHandle = MeshGenerator::GetInstance()->GenerateRoundedBoxMesh(radius, size, slices.x, segments);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::SPHERE:
			_MeshHandle = MeshGenerator::GetInstance()->GenerateSphere(radius, slices.x, segments.x);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::CYLINDER:
			_MeshHandle = MeshGenerator::GetInstance()->GenerateCylinder(radius, size.x, slices.x, segments.x);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::CAPPEDCYLINDER:
			_MeshHandle = MeshGenerator::GetInstance()->GenerateCappedCylinder(radius, size.x, slices.x, segments.x, 8);
			break;
		case TE_PRIMITIVE_TYPE::PLANE:
			_MeshHandle = MeshGenerator::GetInstance()->GeneratePlane(float2{ size.x, size.y }, int2{ segments.x, segments.y });
			break;
		default:
			break;
		}

		InitVertexAndIndexBuffer();


		/*auto scene = GetActiveScene();
		Entity _ModelEntity = scene->AddModelEntity(_ModelName, IdentityMatrix);
		auto material = m_MaterialManager.AddDefaultMaterial(TE_IDX_MESH_TYPE::MESH_NTT);
		Entity _MeshEntity =  scene->AddMeshEntity(_meshName.c_str(), transform, mesh, material, _ModelEntity);*/

		return _MeshHandle;
	}

	void ModelManager::GenerateEnvironmentMesh(Mesh** outMesh)
	{

		*outMesh = &MeshGenerator::GetInstance()->GenerateSphere(1.0f, 32, 16).GetMesh();

		InitVertexAndIndexBuffer();

	}

	Mesh& ModelManager::CopyMesh(const Mesh& mesh)
	{
		auto& _newMesh = m_Meshes.emplace_back(GenerateMeshID(), mesh.m_IndexNum, mesh.m_IndexOffset, mesh.m_VertexOffset, mesh.m_VertexNum, mesh.GetVertexBuffer(), &m_IndexBuffer);

		return _newMesh;
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

	BoundingAABox ModelManager::GenerateBoundingAABox(Mesh* _Mesh) const
	{
		BoundingAABox _AABB;
		CreateBoundingAABoxFromPoints(_AABB, _Mesh->GetVertexNum(), &_Mesh->GetVertexBuffer()->GetPosData()[_Mesh->GetVertexOffset()].Position, sizeof(VertexData::Pos));
		return _AABB;
	}

	template<class T>
	T& ModelManager::_GetVertexBuffer()
	{
		return std::get<T>(m_VertexBuffers);
	}

	Mesh& ModelManager::GetMesh(MeshHandle _MeshHandle)
	{
		return m_Meshes[_MeshHandle.m_MeshIndex];
	}

	Mesh& ModelManager::GetMesh(uint32_t _MeshIndex)
	{
		return m_Meshes[_MeshIndex];
	}

}

