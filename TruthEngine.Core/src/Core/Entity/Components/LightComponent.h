#pragma once

namespace TruthEngine
{
	class ILight;

	class LightComponent
	{
	public:
		LightComponent(ILight* light)
			: m_ILight(light)
		{}

		inline ILight* GetLight()
		{
			return m_ILight;
		}

	private:

	private:
		ILight* m_ILight;
	};
}
