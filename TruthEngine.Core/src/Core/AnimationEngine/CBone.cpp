#include "pch.h"
#include "CBone.h"

namespace TruthEngine
{
	BoneWeight::BoneWeight(UINT BoneIndex, float BoneWeight) : mBoneIndex(BoneIndex), mBoneWeight(BoneWeight)
	{}

	Bone::Bone() = default;

	Bone::~Bone()
	{
		ReleaseChildrens();
	}

	int Bone::ReleaseChildrens()
	{
		for (auto ptr : mChildren)
			delete ptr;

		return 0;
	}
}