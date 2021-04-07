#include "pch.h"
#include "SkinnedAnimationComponent.h"

#include "Core/AnimationEngine/CBone.h"
#include "Core/AnimationEngine/CSkinAnimation.h"

namespace TruthEngine
{
	SkinnedAnimationComponent::SkinnedAnimationComponent(SA_Animation* _Animation) : mAnimation(_Animation)
	{
		SetAnimation(0);
	}

	SkinnedAnimationComponent::~SkinnedAnimationComponent() = default;


	SkinnedAnimationComponent::SkinnedAnimationComponent(const SkinnedAnimationComponent & sm) : mAnimation(sm.mAnimation)
		, mCurrentAnimName(sm.mCurrentAnimName)
		, mTimePos(sm.mTimePos)
	{}

	SkinnedAnimationComponent::SkinnedAnimationComponent(SkinnedAnimationComponent && sm) : mAnimation(sm.mAnimation)
		, mCurrentAnimName(sm.mCurrentAnimName)
		, mTimePos(sm.mTimePos)
	{}

	SkinnedAnimationComponent& SkinnedAnimationComponent::operator=(SkinnedAnimationComponent&&) = default;


	void SkinnedAnimationComponent::Update(double _dt)
	{
		mAnimation->UpdateTimePos(_dt);
	}

	const std::vector<float4x4>* SkinnedAnimationComponent::GetBoneTransforms() const
	{
		return mAnimation->GetTransform();
	}

	bool SkinnedAnimationComponent::SetAnimation(const std::string & AnimName)
	{
		return mAnimation->AddAnimToQueue(AnimName);
	}

	bool SkinnedAnimationComponent::SetAnimation(int AnimIndex)
	{
		return mAnimation->AddAnimToQueue(AnimIndex);
	}

	bool SkinnedAnimationComponent::DisableAnimation(const std::string & AnimName)
	{
		return mAnimation->RemoveAnimToQueue(AnimName);
	}

	bool SkinnedAnimationComponent::DisableAnimation(int AnimIndex)
	{
		return mAnimation->RemoveAnimToQueue(AnimIndex);

	}
}