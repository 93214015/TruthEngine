#pragma once

namespace TruthEngine::Core {

	class GDevice {

	};

}

#ifdef TE_API_DX12

	#include "API/DX12/GDeviceDX12.h"

	#define TE_INSTANCE_API_DX12_GDEVICE			TruthEngine::API::DX12::GDeviceDX12::GetPrimaryDevice()
	#define TE_INSTANCE_API_DX12_COMMANDQUEUEDIRECT TruthEngine::API::DX12::GDeviceDX12::GetPrimaryDevice().GetCommandQueuDirect()
	#define TE_INSTANCE_API_DX12_COMMANDQUEUECOPY	TruthEngine::API::DX12::GDeviceDX12::GetPrimaryDevice().GetCommandQueuCopy()
	
#endif
