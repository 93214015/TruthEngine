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
			bool EnabledHDR = true;
			TE_RENDERER_API RendererAPI = Graphics::TE_RENDERER_API::DirectX12;
			std::string RendererAPISTR = "DirectX 12";
			std::string GPUAdapterSTR = "AMD Radeon RX570";
			std::string CPUModelSTR = "AMD FX6300";
			std::string CPUThreadNumSTR = "6";
			TE_SETTING_MSAA MSAA = TE_SETTING_MSAA::X1;
			TE_SETTINGS_FRAMELIMIT FrameLimit = TE_SETTINGS_FRAMELIMIT::_60;
			double FrameLimitTime = 1.0 / static_cast<double>(FrameLimit);


			TE_SETTING_MSAA GetMSAA()
			{
				return MSAA;
			}
			TE_RENDERER_API GetRendererAPI()
			{
				return RendererAPI;
			}

			TE_SETTINGS_FRAMELIMIT GetFrameLimit()
			{
				return FrameLimit;
			}

			double GetFrameLimitTime()
			{
				return FrameLimitTime;
			}


			bool IsEnabledHDR()
			{
				return EnabledHDR;
			}
			bool IsEnabledMSAA()
			{
				return static_cast<int>(MSAA) > 1;
			}


			void SetHDR(bool _EnabledHDR)
			{
				EnabledHDR = _EnabledHDR;

				EventSettingsGraphicsHDR _Event;
				TE_INSTANCE_APPLICATION->OnEvent(_Event);
			}
			void SetMSAA(TE_SETTING_MSAA _MSAA)
			{
				MSAA = _MSAA;
			}
			void SetFrameLimit(TE_SETTINGS_FRAMELIMIT _FrameLimit)
			{
				FrameLimit = _FrameLimit;

				FrameLimitTime = _FrameLimit == TE_SETTINGS_FRAMELIMIT::Unlimited ? 0.0 : 1.0 / static_cast<double>(FrameLimit);

				EventSettingsGraphicsFrameLimit _Event;
				TE_INSTANCE_APPLICATION->OnEvent(_Event);
			}
		}

	}
}