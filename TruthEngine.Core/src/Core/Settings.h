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

			bool IsEnabledMSAA();
			bool IsEnabledHDR();

			TE_SETTING_MSAA GetMSAA();
			TE_RENDERER_API GetRendererAPI();
			
			void SetMSAA(TE_SETTING_MSAA _MSAA);
			void SetHDR(bool _EnabledHDR);
			
		};
	}

	

}
