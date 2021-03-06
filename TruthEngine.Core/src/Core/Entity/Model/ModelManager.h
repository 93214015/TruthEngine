#pragma once

//#include "Model3D.h"
#include "Mesh.h"
#include "MeshHandle.h"

#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/MaterialManager.h"
#include "Core/Entity/Entity.h"
#include "Core/AnimationEngine/CBone.h"


namespace TruthEngine
{
	using VertexBufferNTT = VertexBuffer<VertexData::NormTanTex>;
	using VertexBufferSkinned = VertexBuffer<VertexData::NormTanTex, VertexData::Bone>;

	enum class TE_PRIMITIVE_TYPE
	{
		BOX,
		ROUNDEDBOX,
		SPHERE,
		CYLINDER,
		CAPPEDCYLINDER,
		PLANE,
	};

	struct PrimitiveMeshInstances
	{
		MeshHandle Sphere;
		MeshHandle Cone;
	};


	class BufferManager;
	class RendererCommand;
	class Scene;

	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		static ModelManager* GetInstance()
		{
			static ModelManager s_ModelManager;
			return &s_ModelManager;
		}

		void Init(BufferManager* bufferManager/*, RendererCommand* rendererCommand*/);


		inline void AddSpace(size_t MeshNum)
		{
			if (auto freeSpace = m_Meshes.capacity() - m_Meshes.size(); freeSpace < MeshNum)
			{
				auto currentSpace = m_Meshes.size();
				m_Meshes.reserve(currentSpace + MeshNum);
			}
		}


		inline void GetOffsets(size_t& outVertexOffset, size_t& outIndexOffset, size_t& outMeshOffset, TE_IDX_MESH_TYPE _MeshType)
		{
			outVertexOffset = GetVertexOffset(_MeshType);
			outIndexOffset = m_IndexBuffer.GetIndexOffset();
			outMeshOffset = m_Meshes.size();
		}


		size_t GetVertexOffset(TE_IDX_MESH_TYPE _MeshType)const noexcept;


		inline size_t GetIndexOffset()const noexcept
		{
			return m_IndexBuffer.GetIndexOffset();
		}

		inline size_t GetMeshOffset()const noexcept
		{
			return m_Meshes.size();
		}
		

		MeshHandle AddMesh(TE_IDX_MESH_TYPE _MeshType, uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum);
		Mesh& CopyMesh(const Mesh& mesh);
		Mesh& GetMesh(MeshHandle _MeshHandle);
		Mesh& GetMesh(uint32_t _MeshIndex);


		template<typename T>
		T& GetVertexBuffer()
		{
			return std::get<T>(m_VertexBuffers);
		}

		IndexBuffer& GetIndexBuffer()
		{
			return m_IndexBuffer;
		}

		/*void ImportModel(Scene* scene, const char* filePath, std::string _ModelName);*/


		MeshHandle GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE type, const float3& size, float radius, const int3& segments, const int3& slices);
		void GenerateEnvironmentMesh(Mesh** outMesh);

		void InitVertexAndIndexBuffer();

		BoundingAABox GenerateBoundingAABox(Mesh* _Mesh) const;

		const PrimitiveMeshInstances& GetPrimitiveMeshInstances() const;

	protected:

		uint32_t GenerateMeshID();

		void GeneratePrimitiveMeshInstances();


		template<class T>
		T& _GetVertexBuffer();

	protected:
		//std::vector<Model3D> m_Models3D;
		std::vector<Mesh> m_Meshes;

		std::tuple<VertexBufferNTT, VertexBufferSkinned> m_VertexBuffers;


		/*VertexBuffer<VertexData::Pos, VertexData::NormTanTex> m_VertexBuffer_PosNormTanTex;
		VertexBuffer<VertexData::Pos, VertexData::NormTanTex, VertexData::SAnimationData> m_VertexBuffer_PosNormTanTexSkinned;*/

		IndexBuffer m_IndexBuffer;

		//MaterialManager m_MaterialManager;

		BufferManager* m_BufferManager;

		std::shared_ptr<RendererCommand> m_RendererCommand;

		PrimitiveMeshInstances m_PrimitiveMeshInstances;

		//
		// Friend Classes
		//
		friend class AssimpLib;
		friend class MeshGenerator;

	};
}

#define TE_INSTANCE_MODELMANAGER TruthEngine::ModelManager::GetInstance()
