#pragma once
#include "System.h"

namespace TruthEngine
{
	class SystemDynamicLights final : public System
	{
	public:
		// Inherited via System
		void OnUpdate(Scene* _Scene, double DeltaSecond) override;
	};
}
