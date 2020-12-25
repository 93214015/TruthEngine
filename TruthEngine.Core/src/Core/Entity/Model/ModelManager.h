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

			static std::shared_ptr<ModelManager> GetInstance()
			{
				static auto s_ModelManager = std::make_shared<ModelManager>();
				return s_ModelManager;
			}

			void Init(BufferManager* bufferManager, RendererCommand* rendererCommand);

			inline const std::vector<std::shared_ptr<Model3D>>& GetModel3D() const noexcept
			{
				return m_Models3D;
			}

			inline const std::vector<Material>& GetMaterials()
			{
				return m_MaterialManager.m_Materials;
			}

			void ImportModel(const char* filePath);

		protected:
			ModelManager() = default;

		protected:
			std::vector<std::shared_ptr<Model3D>> m_Models3D;
			std::vector<std::shared_ptr<Mesh>> m_Meshes;

			VertexBuffer<VertexData::Pos, VertexData::NormTex> m_VertexBuffer_PosNormTex;
			IndexBuffer m_IndexBuffer;

			MaterialManager m_MaterialManager;

			BufferManager* m_BufferManager;


			//
			// Friend Classes
			//
			friend class AssimpLib;

		};
	}
}

#define TE_INSTANCE_MODELMANAGER TruthEngine::Core::ModelManager::GetInstance()