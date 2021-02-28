#pragma once


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

namespace TruthEngine
{

	class Settings
	{
	public:

		inline static bool IsMSAAEnabled()
		{
			return static_cast<int>(MSAA) > 1;
		}


		static TE_RENDERER_API RendererAPI;
		static std::string RendererAPISTR;
		static std::string GPUAdapterSTR;
		static std::string CPUModelSTR;
		static std::string CPUThreadNumSTR;

		static TE_SETTING_MSAA MSAA;
	};

}
