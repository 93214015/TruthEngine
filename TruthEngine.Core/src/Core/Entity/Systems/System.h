#pragma once

namespace TruthEngine
{
	class Scene;

	class System
	{
	public:
		
		virtual void OnUpdate(Scene* _Scene, double DeltaSecond) = 0;

	protected:

	};
}