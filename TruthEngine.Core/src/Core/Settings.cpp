#include "pch.h"
#include "Settings.h"

namespace TruthEngine
{

	TE_RENDERER_API Settings::RendererAPI = TE_RENDERER_API::DirectX12 ;

	std::string Settings::RendererAPISTR = "DirectX 12";

	std::string Settings::GPUAdapterSTR = "AMD Radeon RX570";

	std::string Settings::CPUModelSTR = "AMD FX6300";

	std::string Settings::CPUThreadNumSTR = "6";

	TE_SETTING_MSAA Settings::MSAA = TE_SETTING_MSAA::X1;

}