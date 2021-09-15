#include "pch.h"
#include "Settings.h"

#include "Application.h"
#include "Core/Event/EventSettings.h"

namespace TruthEngine
{

	namespace Settings
	{

		namespace Graphics
		{

			bool IsEnabledHDR = false;
			TE_RENDERER_API RendererAPI = Graphics::TE_RENDERER_API::DirectX12;
			std::string RendererAPISTR = "DirectX 12";
			std::string GPUAdapterSTR = "AMD Radeon RX570";
			std::string CPUModelSTR = "AMD FX6300";
			std::string CPUThreadNumSTR = "6";
			TE_SETTING_MSAA MSAA = TE_SETTING_MSAA::X1;


			TE_SETTING_MSAA GetMSAA()
			{
				return MSAA;
			}
			TE_RENDERER_API GetRendererAPI()
			{
				return RendererAPI;
			}


			bool IsEnabledHDR()
			{
				return IsEnabledHDR;
			}
			bool IsEnabledMSAA()
			{
				return static_cast<int>(MSAA) > 1;
			}


			bool SetHDR(bool _EnabledHDR)
			{
				IsEnabledHDR = _EnabledHDR;

				EventSettingsGraphicsHDR _Event;

				TE_INSTANCE_APPLICATION->OnEvent(_Event);
			}
			void SetMSAA(TE_SETTING_MSAA _MSAA)
			{
				MSAA = _MSAA;
			}
		}

	}
}