#pragma once


namespace TruthEngine
{
	class HardwareInfo
	{
	public:
		struct SystemInfo
		{
			std::string mCPUModel = "";
			uint8_t mThreadNum = 0;
			uint32_t mInstalledRAM = 0;
			std::string mGPUDesc = "";
			uint32_t mGPUMemory = 0;
		};

		static SystemInfo GetSystemInfo();

	private:
		static SystemInfo RetrieveInfo();

	};
}