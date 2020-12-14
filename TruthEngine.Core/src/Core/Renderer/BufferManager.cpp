#include "pch.h"
#include "BufferManager.h"

#include "API/DX12/DX12BufferManager.h"

namespace TruthEngine::Core {



	std::shared_ptr<TruthEngine::Core::BufferManager> BufferManager::Factory()
	{
		switch (Settings::RendererAPI)
		{
		case TE_RENDERER_API::DirectX12 :
			return API::DirectX12::DX12BufferManager::GetInstance();
			break;
		default:
			return nullptr;
		}
	}

}