#pragma once

//#include "Model3D.h"
#include "Mesh.h"

#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/MaterialManager.h"
#include "Core/Entity/Entity.h"

namespace TruthEngine
{

	namespace Core
	{

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

			static std::shared_ptr<ModelManager> GetInstance()
			{
				static auto s_ModelManager = std::make_shared<ModelManager>();
				return s_ModelManager;
			}

			void Init(BufferManager* bufferManager/*, RendererCommand* rendererCommand*/);

// 			inline const std::vector<Model3D>& GetModel3D() const noexcept
// 			{
// 				return m_Models3D;
// 			}

			inline const std::vector<Material*>& GetMaterials()
			{
				return m_MaterialManager.m_Materials;
			}

			inline void AddSpace(/*size_t Model3DNum, */size_t MeshNum)
			{
				/*if (auto freeSpace = m_Models3D.capacity() - m_Models3D.size(); freeSpace < Model3DNum)
				{
					auto currentSpace = m_Models3D.size();
					m_Models3D.reserve(currentSpace + Model3DNum);
				}*/

				if (auto freeSpace = m_Meshes.capacity() - m_Meshes.size(); freeSpace < MeshNum)
				{
					auto currentSpace = m_Meshes.size();
					m_Meshes.reserve(currentSpace + MeshNum);
				}
			}

			inline void GetOffsets(size_t& outVertexOffset, size_t& outIndexOffset, /*size_t& outModelOffset,*/ size_t& outMeshOffset, size_t& outMaterialOffset)
			{
				outVertexOffset = m_VertexBuffer_PosNormTanTex.GetVertexOffset();
				outIndexOffset = m_IndexBuffer.GetIndexOffset();
				//outModelOffset = m_Models3D.size();
				outMeshOffset = m_Meshes.size();
				outMaterialOffset = m_MaterialManager.GetMatrialOffset();
			}

			inline size_t GetVertexOffset()const noexcept
			{
				return m_VertexBuffer_PosNormTanTex.GetVertexOffset();
			}

			inline size_t GetIndexOffset()const noexcept
			{
				return m_IndexBuffer.GetIndexOffset();
			}

			/*inline size_t GetModelOffset()const noexcept
			{
				return m_Models3D.size();
			}*/

			inline size_t GetMeshOffset()const noexcept
			{
				return m_Meshes.size();
			}

			inline size_t GetMaterialOffset()const noexcept
			{
				return m_MaterialManager.GetMatrialOffset();
			}

			//Model3D* AddModel3D();

			//void AddMesh(std::shared_ptr<Mesh> mesh);
			Mesh* AddMesh(uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, size_t vertexNum);

			void ImportModel(Scene* scene, const char* filePath);

			void AddSampleModel();

			Entity GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE type, float size = 1.0f, const float4x4& transform = IdentityMatrix, Entity modelEntity = Entity());


		protected:
			void InitVertexAndIndexBuffer();

		protected:
			//std::vector<Model3D> m_Models3D;
			std::vector<Mesh> m_Meshes;

			VertexBuffer<VertexData::Pos, VertexData::NormTanTex> m_VertexBuffer_PosNormTanTex;
			IndexBuffer m_IndexBuffer;

			MaterialManager m_MaterialManager;

			BufferManager* m_BufferManager;

			std::shared_ptr<RendererCommand> m_RendererCommand;

			//
			// Friend Classes
			//
			friend class AssimpLib;
			friend class MeshGenerator;

		};
	}
}

#define TE_INSTANCE_MODELMANAGER TruthEngine::Core::ModelManager::GetInstance()