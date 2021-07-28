#pragma once

namespace TruthEngine
{
	class Scene;

	class System
	{
	public:
		System(Scene* _Scene)
			: m_Scene(_Scene)
		{}
		
		virtual void OnUpdate(float DeltaSecond) = 0;

	protected:
		Scene* m_Scene;

	};
}