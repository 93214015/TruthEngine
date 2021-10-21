#pragma once
#include "pch.h"

namespace TruthEngine
{
	struct MovementComponent
	{
		bool IsAbsolutePostion = false;
		float3A MovementVector = float3A(0.0f, 0.0f, 0.0f);
	};
}