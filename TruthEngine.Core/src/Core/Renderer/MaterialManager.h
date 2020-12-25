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

			inline Material* GetMaterial(const uint32_t materialID)
			{
				return m_Materials[materialID].get();
			}

		protected:

		protected:
			std::unordered_map<uint32_t, std::shared_ptr<Material>> m_Materials;

			BufferManager* m_BufferManager;


			//
			//Friend Classes
			//
			friend class ModelManager;
		};
	}

}
