#include "pch.h"
#include "MaterialManager.h"

namespace TruthEngine::Core
{

	void MaterialManager::Init(BufferManager* bufferManager)
	{
		m_BufferManager = bufferManager;

		auto material = std::make_shared<Material>();
		material->m_ID = 0;

		m_Map_Materials[0] = material;
		m_Materials.push_back(material.get());

	}

}