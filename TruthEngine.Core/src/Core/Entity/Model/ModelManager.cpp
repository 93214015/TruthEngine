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

	Mesh& ModelManager::AddMesh(TE_IDX_MESH_TYPE _MeshType, uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum)
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
			/*BoundingAABox bb;
			CreateBoundingAABoxFromPoints(bb, vertexNum, &_VertexBuffer.GetPosData()[VertexOffset].Position, sizeof(VertexData::Pos));*/
			_Mesh = &m_Meshes.emplace_back(GenerateMeshID(), IndexNum, IndexOffset, VertexOffset, vertexNum, &_VertexBuffer, &m_IndexBuffer);
			break;
		}
		case TE_IDX_MESH_TYPE::MESH_SKINNED:
		{
			VertexBufferSkinned& _VertexBuffer = _GetVertexBuffer<VertexBufferSkinned>();
			/*BoundingAABox bb;
			CreateBoundingAABoxFromPoints(bb, vertexNum, &_VertexBuffer.GetPosData()[VertexOffset].Position, sizeof(VertexData::Pos));*/
			_Mesh = &m_Meshes.emplace_back(GenerateMeshID(), IndexNum, IndexOffset, VertexOffset, vertexNum, &_VertexBuffer, &m_IndexBuffer);
			break;
		}
		default:
			throw;
			break;
		}

		return *_Mesh;
	}

	Mesh& ModelManager::GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE type, float size_x, float size_y, float size_z)
	{
		Mesh* mesh = nullptr;

		switch (type)
		{
		case TruthEngine::TE_PRIMITIVE_TYPE::BOX:
			mesh = MeshGenerator::GetInstance()->GenerateBox(size_x, size_y, size_z);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::ROUNDEDBOX:
			mesh = MeshGenerator::GetInstance()->GenerateRoundedBoxMesh(size_x, size_y, size_z);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::SPHERE:
			mesh = MeshGenerator::GetInstance()->GenerateSphere(size_x);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::CYLINDER:
			mesh = MeshGenerator::GetInstance()->GenerateCylinder(size_x);
			break;
		case TruthEngine::TE_PRIMITIVE_TYPE::CAPPEDCYLINDER:
			mesh = MeshGenerator::GetInstance()->GenerateCappedCylinder(size_x);
			break;
		case TE_PRIMITIVE_TYPE::PLANE:
			mesh = MeshGenerator::GetInstance()->GeneratePlane(size_x, size_z);
			break;
		default:
			break;
		}

		InitVertexAndIndexBuffer();


		/*auto scene = TE_INSTANCE_APPLICATION->GetActiveScene();
		Entity _ModelEntity = scene->AddModelEntity(_ModelName, IdentityMatrix);
		auto material = m_MaterialManager.AddDefaultMaterial(TE_IDX_MESH_TYPE::MESH_NTT);
		Entity _MeshEntity =  scene->AddMeshEntity(_meshName.c_str(), transform, mesh, material, _ModelEntity);*/

		return *mesh;
	}

	void ModelManager::GenerateEnvironmentMesh(Mesh** outMesh)
	{

		*outMesh = MeshGenerator::GetInstance()->GenerateSphere(1.0f);

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

}

