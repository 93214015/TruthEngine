#include "pch.h"
#include "AnimationManager.h"

namespace TruthEngine
{
	AnimationManager::AnimationManager()
	{
		mSAAnimations.reserve(100);
	}

	AnimationManager::~AnimationManager() = default;

	void AnimationManager::Update(double _dt)
	{
		for (SA_Animation& _Animation : mSAAnimations)
		{
			_Animation.UpdateTimePos(_dt);
		}
	}

	SA_Animation* AnimationManager::AddAnimation(const aiScene* _aiScene)
	{
		return &mSAAnimations.emplace_back(_aiScene);
	}
}