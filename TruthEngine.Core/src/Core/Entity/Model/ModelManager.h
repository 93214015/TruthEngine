#pragma once

//#include "Model3D.h"
#include "Mesh.h"

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
		

		Mesh& AddMesh(TE_IDX_MESH_TYPE _MeshType, uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum);
		Mesh& CopyMesh(const Mesh& mesh);

		template<typename T>
		T& GetVertexBuffer()
		{
			return std::get<T>(m_VertexBuffers);
		}

		/*void ImportModel(Scene* scene, const char* filePath, std::string _ModelName);*/


		Mesh& GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE type, float size_x, float size_y, float size_z);
		void GenerateEnvironmentMesh(Mesh** outMesh);

		void InitVertexAndIndexBuffer();


		BoundingAABox GenerateBoundingAABox(Mesh* _Mesh) const;

	protected:

		inline uint32_t GenerateMeshID()
		{
			static uint32_t s_ID = 0;
			return s_ID++;
		}

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

		//
		// Friend Classes
		//
		friend class AssimpLib;
		friend class MeshGenerator;

	};
}

#define TE_INSTANCE_MODELMANAGER TruthEngine::ModelManager::GetInstance()
