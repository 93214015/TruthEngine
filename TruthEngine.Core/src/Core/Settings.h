#pragma once


enum class TE_RENDERER_API
{
	DirectX12,
	DirectX11,
	OpenGL,
	Vulkan
};

namespace TruthEngine::Core
{

	class Settings
	{
	public:
		static TE_RENDERER_API RendererAPI;
		static std::string RendererAPISTR;
		static std::string GPUAdapterSTR;
		static std::string CPUModelSTR;
		static std::string CPUThreadNumSTR;
	};

}
