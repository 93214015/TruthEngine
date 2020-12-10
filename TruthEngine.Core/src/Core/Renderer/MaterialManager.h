#pragma once
#include "Material.h"

namespace TruthEngine
{

	namespace Core
	{
		class BufferManager;

		class MaterialManager
		{
		public:

			void Init(BufferManager* bufferManager);

			inline Material* GetMaterial(const char* materialName)
			{
				return &m_Materials[0];
			}
			inline Material* GetMaterial(const uint32_t materialID)
			{
				return &m_Materials[materialID];
			}

		protected:

		protected:
			std::vector<Material> m_Materials;

			BufferManager* m_BufferManager;


			//
			//Friend Classes
			//
			friend class ModelManager;
		};
	}

}
