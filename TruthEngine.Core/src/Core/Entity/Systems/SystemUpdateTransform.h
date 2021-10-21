#pragma once
#include "System.h"

namespace TruthEngine
{
	class SystemUpdateTransform final : public System
	{
	public:
		void OnUpdate(Scene* _Scene, double DeltaSecond) override;
	};
}
