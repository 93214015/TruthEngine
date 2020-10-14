#pragma once


#define TE_INSTANCE_GDEVICE TruthEngine::Core::GDevice::GetPrimaryDevice()


namespace TruthEngine::Core {

	class GDevice {
	public:
		virtual TE_RESULT Init(uint32_t adapterIndex) = 0;
		inline static GDevice& GetPrimaryDevice() { return *s_GDevice; }
	protected:
		static GDevice* s_GDevice;
	};

	TE_RESULT CreateGDevice(uint32_t adapterIndex);

}



