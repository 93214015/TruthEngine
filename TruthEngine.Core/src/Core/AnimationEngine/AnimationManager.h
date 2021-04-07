#pragma once
#include "CSkinAnimation.h"

namespace TruthEngine
{
	class AnimationManager
	{
	public:
		AnimationManager();
		~AnimationManager();

		void Update(double _dt);

		SA_Animation* AddAnimation(const aiScene* _aiScene);

		SA_Animation* GetAnimation(uint32_t _Index)
		{
			if (_Index >= mSAAnimations.size())
			{
				return nullptr;
			}
			return &mSAAnimations[_Index];
		}

		static AnimationManager* GetInstance()
		{
			static AnimationManager sInstance;
			return &sInstance;
		}

	private:

	private:

		bool m_Stopped;

		std::vector<SA_Animation> mSAAnimations;

	};
}

#define TE_INSTANCE_ANIMATIONMANAGER AnimationManager::GetInstance()