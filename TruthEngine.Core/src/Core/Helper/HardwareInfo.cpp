#include "pch.h"
#include "HardwareInfo.h"

#include "Core/Helper/SysInfo/cpuinfodelegate.h"
#include "Core/Helper/SysInfo/motherboardinfodelegate.h"
#include "Core/Helper/SysInfo/raminfodelegate.h"
#include "Core/Helper/SysInfo/gpuinfodelegate.h"

#include "API/IDXGI.h"

namespace TruthEngine
{


	HardwareInfo::SystemInfo HardwareInfo::GetSystemInfo()
	{
		static SystemInfo sysInfo = RetrieveInfo();

		return sysInfo;
	}

	TruthEngine::HardwareInfo::SystemInfo HardwareInfo::RetrieveInfo()
	{
		CPUInfoDelegate* cpuInfo = new CPUInfoDelegate();
		//Then extract the separate CPUs into a vector (of CPUInfo objects)
		std::vector<CPUInfo> cpuInfoVector = cpuInfo->cpuInfoVector();

		MotherboardInfoDelegate* moboInfo = new MotherboardInfoDelegate();
		std::vector<MotherboardInfo> moboInfoVector = moboInfo->motherboardInfoVector();

		RAMInfoDelegate* ramInfo = new RAMInfoDelegate();
		std::vector<RAMInfo> ramInfoVector = ramInfo->ramInfoVector();

		GPUInfoDelegate* gpuInfo = new GPUInfoDelegate();
		std::vector<GPUInfo> gpuInfoVector = gpuInfo->gpuInfoVector();

		SystemInfo r;

		try
		{
			r.mCPUModel = cpuInfoVector[0].name();

			for (auto& ram : ramInfoVector)
			{
				r.mInstalledRAM += ram.capacity();
			}

			r.mThreadNum = std::thread::hardware_concurrency();

			auto primaryadapter = TE_INSTANCE_IDXGI.GetAdapters()[0];

			DXGI_ADAPTER_DESC3 desc;

			primaryadapter->GetDesc3(&desc);

			r.mGPUMemory = static_cast<uint32_t>(desc.DedicatedVideoMemory / 1000000000);
			auto gpudesc =	std::wstring(desc.Description);
			r.mGPUDesc = std::string(gpudesc.begin(), gpudesc.end());
		}
		catch (std::exception ex)
		{
		}

		return r;
	}

}