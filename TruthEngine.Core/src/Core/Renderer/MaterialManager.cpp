#include "pch.h"
#include "MaterialManager.h"

namespace TruthEngine::Core
{

	void MaterialManager::Init(BufferManager* bufferManager)
	{
		m_BufferManager = bufferManager;

		auto& mat = m_Materials.emplace_back();
		mat.m_ID = 0;

	}

}