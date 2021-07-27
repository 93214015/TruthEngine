#pragma once
#include "System.h"

namespace TruthEngine
{
	class SystemUpdateTransform final : public System
	{
	public:
		SystemUpdateTransform();

		void OnUpdate(float DeltaTime) final override;
	};
}
