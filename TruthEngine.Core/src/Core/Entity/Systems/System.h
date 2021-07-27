#pragma once

namespace TruthEngine
{
	class System
	{
	public:
		System(class Scene* _Scene)
			: m_Scene(_Scene)
		{}
		
		virtual void OnUpdate(float DeltaSecond) = 0;

	protected:
		class Scene* m_Scene;

	};
}