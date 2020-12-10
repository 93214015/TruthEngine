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

		class ModelManager
		{
		public:

			static std::shared_ptr<ModelManager> GetInstance()
			{
				static auto s_ModelManager = std::make_shared<ModelManager>();
				return s_ModelManager;
			}

			void Init(BufferManager* bufferManager);

			inline const std::vector<Model3D>& GetModel3D() const noexcept
			{
				return m_Models3D;
			}

			inline const std::vector<Material>& GetMaterials()
			{
				return m_MaterialManager.m_Materials;
			}

			void ImportModel(const char* filePath);

		protected:


		protected:
			std::vector<Model3D> m_Models3D;
			std::vector<Mesh> m_Meshes;

			VertexBuffer<VertexData::Pos, VertexData::NormTex> m_VertexBuffer_PosNormTex;
			IndexBuffer m_IndexBuffer;

			MaterialManager m_MaterialManager;

			BufferManager* m_BufferManager;

		};
	}
}

#define TE_INSTANCE_MODELMANAGER TruthEngine::Core::ModelManager::GetInstance()