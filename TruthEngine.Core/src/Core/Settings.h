#pragma once




namespace TruthEngine
{
	namespace Settings
	{
		namespace Graphics
		{

			enum class TE_RENDERER_API
			{
				DirectX12,
				DirectX11,
				OpenGL,
				Vulkan
			};

			enum class TE_SETTING_MSAA
			{
				X1 = 1,
				X2 = 2,
				X4 = 4,
				X8 = 8
			};

			enum class TE_SETTINGS_FRAMELIMIT : int
			{
				_30 = 30,
				_60 = 60,
				Unlimited = 0
			};

			bool IsEnabledMSAA();
			bool IsEnabledHDR();

			TE_SETTING_MSAA GetMSAA();
			TE_RENDERER_API GetRendererAPI();

			//Return frame time's limit setting
			TE_SETTINGS_FRAMELIMIT GetFrameLimit();

			//Return frame time's limit in seconds
			double GetFrameLimitTime();
			
			void SetMSAA(TE_SETTING_MSAA _MSAA);
			void SetHDR(bool _EnabledHDR);
			void SetFrameLimit(TE_SETTINGS_FRAMELIMIT _SettingsFrameLimit);
			
		};
	}

	

}
