#pragma once

namespace TruthEngine
{
	class TE_IDX_ICONS
	{
	public:
		static uint32_t GetSpotLight()
		{
			return SpotLight;
		}

		static uint32_t GetPointLight()
		{
			return PointLight;
		}

	private:
		static uint32_t SpotLight;
		static uint32_t PointLight;

		friend class RendererLayer;
	};
}
