#pragma once

#include "Model3D.h"
#include "Mesh.h"

#include "Core/Renderer/VertexBuffer.h"
#include "Core/Renderer/IndexBuffer.h"
#include "Core/Renderer/MaterialManager.h"

namespace TruthEngine
{

	namespace Core
	{

		class BufferManager;
		class RendererCommand;

		class ModelManager
		{
		public:
			ModelManager() = default;

			static std::shared_ptr<ModelManager> GetInstance()
			{
				static auto s_ModelManager = std::make_shared<ModelManager>();
				return s_ModelManager;
			}

			void Init(BufferManager* bufferManager/*, RendererCommand* rendererCommand*/);

			inline const std::vector<std::shared_ptr<Model3D>>& GetModel3D() const noexcept
			{
				return m_Models3D;
			}

			inline const std::vector<Material*>& GetMaterials()
			{
				return m_MaterialManager.m_Materials;
			}

			inline void AddSpace(size_t Model3DNum, size_t MeshNum)
			{
				auto currentSpace = m_Models3D.size();
				m_Models3D.reserve(currentSpace + Model3DNum);

				currentSpace = m_Meshes.size();
				m_Meshes.reserve(currentSpace + MeshNum);
			}

			inline void GetOffsets(size_t& outVertexOffset, size_t& outIndexOffset)
			{
				outVertexOffset = m_VertexBuffer_PosNormTex.GetVertexOffset();
				outIndexOffset = m_IndexBuffer.GetIndexOffset();
			}

			Model3D* AddModel3D();

			void AddMesh(std::shared_ptr<Mesh> mesh);
			Mesh* AddMesh(uint32_t IndexNum, size_t IndexOffset, size_t VertexOffset, Material* MaterialPtr);

			void ImportModel(const char* filePath);

		protected:
			void InitVertexAndIndexBuffer();

		protected:
			std::vector<std::shared_ptr<Model3D>> m_Models3D;
			std::vector<std::shared_ptr<Mesh>> m_Meshes;

			VertexBuffer<VertexData::Pos, VertexData::NormTex> m_VertexBuffer_PosNormTex;
			IndexBuffer m_IndexBuffer;

			MaterialManager m_MaterialManager;

			BufferManager* m_BufferManager;

			std::shared_ptr<RendererCommand> m_RendererCommand;

			//
			// Friend Classes
			//
			friend class AssimpLib;

		};
	}
}

#define TE_INSTANCE_MODELMANAGER TruthEngine::Core::ModelManager::GetInstance()