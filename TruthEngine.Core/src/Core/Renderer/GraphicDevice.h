#pragma once


#define TE_INSTANCE_GRAPHICDEVICE TruthEngine::GraphicDevice::GetPrimaryDevice()


namespace TruthEngine {

	class GraphicResource;

	class GraphicDevice {
	public:
		virtual TE_RESULT Init(uint32_t adapterIndex) = 0;
		virtual void WaitForGPU() = 0;
		inline static GraphicDevice* GetPrimaryDevice() { return s_GDevice; }



	protected:
		static GraphicDevice* s_GDevice;
	};

	TE_RESULT CreateGDevice(uint32_t adapterIndex);

}



