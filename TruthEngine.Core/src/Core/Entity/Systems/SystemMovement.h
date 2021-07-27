#pragma once
#include "System.h"

namespace TruthEngine
{
	class SystemMovement final : public System
	{
	public:
		SystemMovement();

		void OnUpdate(float DeltaTime) final override;
	};
}

