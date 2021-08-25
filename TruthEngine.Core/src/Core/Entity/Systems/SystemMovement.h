#pragma once
#include "System.h"

namespace TruthEngine
{
	class SystemMovement final : public System
	{
	public:
		void OnUpdate(Scene* _Scene, double DeltaSecond) final override;
	};
}

