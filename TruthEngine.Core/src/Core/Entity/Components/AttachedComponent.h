#pragma once

#include "Core/Entity/Entity.h"

namespace TruthEngine
{
	struct AttachedComponent
	{
		AttachedComponent()
		{
			AttachedEntities.reserve(10);
		}

		std::vector<Entity> AttachedEntities;
	};
}
