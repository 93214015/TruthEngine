#include "pch.h"
#include "CSkinAnimation.h"
#include "CBone.h"

using namespace DirectX;

namespace TruthEngine
{

	SA_Animation::SA_Animation(const aiScene* Scene)
	{

		mBoneSkeleton.reset(MakeSkeleton(Scene->mRootNode, nullptr));

		for (int k = 0; k < Scene->mNumMeshes; ++k)
		{

			auto mesh = Scene->mMeshes[k];

			for (UINT i = 0; i < mesh->mNumBones; ++i)
			{

				auto meshBone = mesh->mBones[i];

				auto boneName = meshBone->mName.C_Str();

				if (mBonesByName.count(boneName) > 0)
				{
					//auto it = std::find_if(mBones.begin(), mBones.end(), [](Bone* b) { return true;  });

					auto b = mBonesByName[boneName];

					auto m = meshBone->mOffsetMatrix.Transpose();

					b->mMatrix_Offset._11 = m.a1; b->mMatrix_Offset._12 = m.a2; b->mMatrix_Offset._13 = m.a3; b->mMatrix_Offset._14 = m.a4;
					b->mMatrix_Offset._21 = m.b1; b->mMatrix_Offset._22 = m.b2; b->mMatrix_Offset._23 = m.b3; b->mMatrix_Offset._24 = m.b4;
					b->mMatrix_Offset._31 = m.c1; b->mMatrix_Offset._32 = m.c2; b->mMatrix_Offset._33 = m.c3; b->mMatrix_Offset._34 = m.c4;
					b->mMatrix_Offset._41 = m.d1; b->mMatrix_Offset._42 = m.d2; b->mMatrix_Offset._43 = m.d3; b->mMatrix_Offset._44 = m.d4;

					size_t index = mBones.size();

					mBonesToIndex[boneName] = index;

					mBones.push_back(b);
				}

			}

		}

		InitAnimations(Scene);

		const float timestep = 1.0f / 30.0f;

		for (int i = 0; i < mAnimations.size(); ++i)
		{

			float dt = 0.0f;

			auto& animEval = mAnimations[i];

			for (float ticks = 0.0f; ticks < animEval.mDuration; ticks += (animEval.mTicksPerSecond / 30.0f))
			{
				dt += timestep;

				Calculate(dt);

				animEval.mTransforms.emplace_back();
				auto& transformList = animEval.mTransforms.back();

				for (auto bone : mBones)
				{


					transformList.emplace_back();
					auto& transf = transformList.back();


					auto offsetM = XMLoadFloat4x4(&bone->mMatrix_Offset);
					auto GlobalM = XMLoadFloat4x4(&bone->mMatrix_GlobalTransform);

					XMStoreFloat4x4(&transf, offsetM * GlobalM);

				}


			}

		}

		mFinalTransforms.resize(mBones.size());
	}


	SA_Animation::SA_Animation() : mBoneSkeleton(new Bone()), mSkeleton(new Bone())
	{

	}

	SA_Animation::SA_Animation(SA_Animation&& anim) : mName(std::move(anim.mName))
		, mBonesByName(std::move(anim.mBonesByName))
		, mBonesToIndex(std::move(anim.mBonesToIndex))
		, mBones(std::move(anim.mBones))
		, mBoneSkeleton(std::move(anim.mBoneSkeleton))
		, mSkeleton(std::move(anim.mSkeleton))
		, mAnimations(std::move(anim.mAnimations))
		, mAnimationsToIndex(std::move(anim.mAnimationsToIndex))
		, mCurrentAnimationIndex(anim.mCurrentAnimationIndex)
	{

	}

	SA_Animation::SA_Animation(const SA_Animation& anim) : mName(anim.mName)
		, mBonesByName(anim.mBonesByName)
		, mBonesToIndex(anim.mBonesToIndex)
		, mBones(anim.mBones)
		, mBoneSkeleton(new Bone(*anim.mBoneSkeleton))
		, mSkeleton(new Bone(*anim.mSkeleton))
		, mAnimations(anim.mAnimations)
		, mAnimationsToIndex(anim.mAnimationsToIndex)
		, mCurrentAnimationIndex(anim.mCurrentAnimationIndex)
	{

	}


	void SA_Animation::InitAnimations(const aiScene* Scene)
	{
		for (int i = 0; i < Scene->mNumAnimations; ++i)
		{

			auto animation = Scene->mAnimations[i];

			mAnimations.emplace_back(animation);

			mAnimationsToIndex[mAnimations.back().mName] = i;
		}

		mCurrentAnimationIndex = 0;
	}


	void SA_Animation::Calculate(float dt)
	{
		if (mCurrentAnimationIndex < 0 | mCurrentAnimationIndex > mAnimations.size())
			return;

		mAnimations[mCurrentAnimationIndex].Evaluate(dt, mBonesByName);

		UpdateTransforms(mBoneSkeleton.get());

	}


	void SA_Animation::UpdateTransforms(Bone* bone)
	{

		CalculateBoneWorldTransform(bone);


		for (int i = 0; i < bone->mChildren.size(); ++i)
		{
			UpdateTransforms(bone->mChildren[i]);
		}


	}


	int SA_Animation::GetBoneIndex(const std::string& BoneName)
	{
		return mBonesToIndex[BoneName];
	}


	const std::vector<float4x4>* SA_Animation::GetTransform()
	{
		/*return mAnimations[mCurrentAnimationIndex].GetTransforms(timePos);*/

		std::vector<const std::vector<float4x4>*> transforms;

		if (mAnimTimeQueue.size() > 0)
		{
			auto it = mAnimTimeQueue.begin();


			transforms.push_back(&mAnimations[it->first].GetTransforms(it->second));

			it++;

			for (auto e = it; e != mAnimTimeQueue.end(); e++)
			{
				transforms.push_back(&mAnimations[e->first].GetTransforms(e->second));
			}
		}

		if (transforms.size() == 1)
			return transforms[0];

		for (int i = 0; i < mBones.size(); ++i)
		{
			XMStoreFloat4x4(&mFinalTransforms[i], XMMatrixIdentity());

			for (auto t : transforms)
			{
				XMStoreFloat4x4(&mFinalTransforms[i], XMLoadFloat4x4(&mFinalTransforms[i]) * XMLoadFloat4x4(&(*t)[i]));
			}

		}

		return &mFinalTransforms;

	}


	bool SA_Animation::AddAnimToQueue(int index)
	{
		if (mAnimTimeQueue.count(index) < 1)
			mAnimTimeQueue[index] = 0.0f;

		return true;
	}

	bool SA_Animation::AddAnimToQueue(const std::string& animName)
	{
		auto it = mAnimationsToIndex.find(animName);

		if (it == mAnimationsToIndex.end())
			return false;

		int index = it->second;

		if (mAnimTimeQueue.count(index) < 1)
			mAnimTimeQueue[index] = 0.0f;

		return true;
	}

	bool SA_Animation::RemoveAnimToQueue(int index)
	{
		mAnimTimeQueue.erase(index);

		return true;
	}

	bool SA_Animation::RemoveAnimToQueue(const std::string& animName)
	{
		int index = -1;

		auto it = mAnimationsToIndex.find(animName);

		if (it == mAnimationsToIndex.end())
			return false;

		index = it->second;

		mAnimTimeQueue.erase(index);

		return true;
	}

	bool SA_Animation::UpdateTimePos(double dt)
	{
		for (auto& it : mAnimTimeQueue)
		{
			it.second += dt;

			if (it.second > mAnimations[it.first].GetDuration())
				it.second = 0.0f;

		}

		return true;
	}

	//bool SA_Animation::SetAnimation(std::string& AnimName)
	//{
	//
	//	int index;
	//
	//	if (mAnimationsToIndex.count(AnimName) > 0)
	//	{
	//		int oldIndex = mCurrentAnimationIndex;
	//		mCurrentAnimationIndex = mAnimationsToIndex[AnimName];
	//		return oldIndex != mCurrentAnimationIndex;
	//	}
	//
	//	return false;
	//}
	//
	//bool SA_Animation::SetAnimation(int AnimIndex)
	//{
	//	if (AnimIndex >= mAnimations.size())
	//		return false;
	//
	//	mCurrentAnimationIndex = AnimIndex;
	//	return true;
	//}

	float SA_Animation::GetDuration()
	{
		return (mAnimations[mCurrentAnimationIndex].mDuration / mAnimations[mCurrentAnimationIndex].mTicksPerSecond);
	}

	Bone* SA_Animation::MakeSkeleton(aiNode* node, Bone* parent)
	{
		Bone* bone = new Bone();
		bone->mParent = parent;
		bone->mName = node->mName.C_Str();


		mBonesByName[bone->mName] = bone;

		auto trans = node->mTransformation.Transpose();

		bone->mMatrix_localTransform._11 = trans.a1;
		bone->mMatrix_localTransform._12 = trans.a2;
		bone->mMatrix_localTransform._13 = trans.a3;
		bone->mMatrix_localTransform._14 = trans.a4;
		bone->mMatrix_localTransform._21 = trans.b1;
		bone->mMatrix_localTransform._22 = trans.b2;
		bone->mMatrix_localTransform._23 = trans.b3;
		bone->mMatrix_localTransform._24 = trans.b4;
		bone->mMatrix_localTransform._31 = trans.c1;
		bone->mMatrix_localTransform._32 = trans.c2;
		bone->mMatrix_localTransform._33 = trans.c3;
		bone->mMatrix_localTransform._34 = trans.c4;
		bone->mMatrix_localTransform._41 = trans.d1;
		bone->mMatrix_localTransform._42 = trans.d2;
		bone->mMatrix_localTransform._43 = trans.d3;
		bone->mMatrix_localTransform._44 = trans.d4;

		bone->mMatrix_OriginlaLocalTransform = bone->mMatrix_localTransform;

		CalculateBoneWorldTransform(bone);

		for (int i = 0; i < node->mNumChildren; ++i)
		{
			Bone* child = MakeSkeleton(node->mChildren[i], bone);
			if (child != nullptr)
			{
				bone->mChildren.emplace_back();

				bone->mChildren.back() = child;
			}
		}

		return bone;
	}


	void SA_Animation::CalculateBoneWorldTransform(Bone* bone)
	{
		bone->mMatrix_GlobalTransform = bone->mMatrix_localTransform;
		auto parent = bone->mParent;

		while (parent != nullptr)
		{
			auto m1 = XMLoadFloat4x4(&bone->mMatrix_GlobalTransform);
			auto m2 = XMLoadFloat4x4(&parent->mMatrix_localTransform);
			m1 *= m2;

			XMStoreFloat4x4(&bone->mMatrix_GlobalTransform, m1);

			parent = parent->mParent;
		}
	}


	SA_AnimEvaluator::SA_AnimEvaluator(const aiAnimation* animation)
	{

		mName = animation->mName.C_Str();
		mTicksPerSecond = (float)animation->mTicksPerSecond > 0.0f ? (float)animation->mTicksPerSecond : 920.0f;
		mDuration = (float)animation->mDuration;
		mLastTime = 0.0f;


		mAnimNodes.clear();
		mAnimNodes.reserve(animation->mNumChannels);


		mLastPositions.clear();
		mLastPositions.resize(animation->mNumChannels);


		mPlayForward = true;


		for (int i = 0; i < animation->mNumChannels; ++i)
		{
			auto channel = animation->mChannels[i];

			auto& animNode = mAnimNodes.emplace_back();

			animNode.mName = channel->mNodeName.C_Str();

			animNode.mTranslations.reserve(channel->mNumPositionKeys);
			animNode.mScales.reserve(channel->mNumScalingKeys);
			animNode.mRotations.reserve(channel->mNumRotationKeys);

			for (int j = 0; j < channel->mNumPositionKeys; ++j)
			{
				auto& pk = channel->mPositionKeys[j];

				SA_KeyFrame_Translation sa_t;
				sa_t.mTime = (float)pk.mTime;
				sa_t.mTranslation = float3(pk.mValue.x, pk.mValue.y, pk.mValue.z);

				animNode.mTranslations.push_back(sa_t);
			}

			for (int j = 0; j < channel->mNumScalingKeys; ++j)
			{
				auto& sk = channel->mScalingKeys[j];

				SA_KeyFrame_Scaling sa_s;
				sa_s.mTime = (float)sk.mTime;
				sa_s.mScaling = float3(sk.mValue.x, sk.mValue.y, sk.mValue.z);

				animNode.mScales.push_back(sa_s);
			}

			for (int j = 0; j < channel->mNumRotationKeys; ++j)
			{
				auto& rk = channel->mRotationKeys[j];

				SA_KeyFrame_Rotation sa_r;
				sa_r.mTime = (float)rk.mTime;
				sa_r.mRotationQuaternion = float4(rk.mValue.x, rk.mValue.y, rk.mValue.z, rk.mValue.w);

				animNode.mRotations.push_back(sa_r);
			}

		}

	}


	void SA_AnimEvaluator::Evaluate(float dt, std::unordered_map<std::string, Bone*>& BonesByName)
	{

		dt *= mTicksPerSecond;

		auto time = 0.0f;

		if (mDuration > 0.0f)
		{
			time = std::fmod(dt, mDuration);
		}

		for (size_t i = 0; i < mAnimNodes.size(); ++i)
		{
			auto& animNode = mAnimNodes[i];

			if (BonesByName.count(animNode.mName) <= 0)
				continue;

			auto translationsCount = animNode.mTranslations.size();

			float3 position;

			if (translationsCount > 0)
			{

				int frame = (time >= mLastTime) ? std::get<0>(mLastPositions[i]) : 0;

				while (frame < translationsCount - 1)
				{

					if (time < animNode.mTranslations[frame + 1].mTime)
						break;

					frame++;
				}

				if (frame > translationsCount)
					frame = 0;

				int nextFrame = (frame + 1) % translationsCount;

				auto key = animNode.mTranslations[frame];
				auto nextKey = animNode.mTranslations[nextFrame];

				auto diffTime = nextKey.mTime - key.mTime;

				if (diffTime < 0.0f)
				{
					diffTime += mDuration;
				}

				if (diffTime > 0.0)
				{

					auto factor = (float)((time - key.mTime) / diffTime);

					auto nextKeyTrans = XMLoadFloat3(&nextKey.mTranslation);

					auto keyTrans = XMLoadFloat3(&key.mTranslation);

					auto positionVector = XMVectorSubtract(nextKeyTrans, keyTrans);

					positionVector = XMVectorScale(positionVector, factor);

					positionVector = XMVectorAdd(positionVector, keyTrans);

					XMStoreFloat3(&position, positionVector);

				}
				else
					position = key.mTranslation;

				std::get<0>(mLastPositions[i]) = frame;

			}

			float4 rotation;

			auto rotationCount = animNode.mRotations.size();

			if (rotationCount > 0)
			{

				int frame = (time >= mLastTime) ? std::get<1>(mLastPositions[i]) : 0;

				while (frame < rotationCount - 1)
				{

					if (time < animNode.mRotations[frame + 1].mTime)
						break;

					frame++;

				}

				if (frame >= rotationCount)
					frame = 0;

				int nextFrame = (frame + 1) % rotationCount;

				auto key = animNode.mRotations[frame];
				auto nextKey = animNode.mRotations[nextFrame];

				auto diffTime = nextKey.mTime - key.mTime;

				if (diffTime < 0)
					diffTime += mDuration;

				if (diffTime > 0)
				{

					auto factor = (float)((time - key.mTime) / diffTime);

					auto vector1 = XMLoadFloat4(&key.mRotationQuaternion);
					auto vector2 = XMLoadFloat4(&nextKey.mRotationQuaternion);

					auto rotationVector = XMQuaternionSlerp(vector1, vector2, factor);

					XMStoreFloat4(&rotation, rotationVector);

				}

				else
					rotation = key.mRotationQuaternion;

				std::get<1>(mLastPositions[i]) = frame;

			}


			float3 scale(1.0f, 1.0f, 1.0f);

			auto scaleCount = animNode.mScales.size();

			if (scaleCount > 0)
			{
				int frame = (time >= mLastTime) ? std::get<2>(mLastPositions[i]) : 0;

				while (frame < scaleCount - 1)
				{

					if (time < animNode.mScales[frame + 1].mTime)
						break;

					frame++;

				}

				if (frame >= scaleCount)
					frame = 0;

				std::get<2>(mLastPositions[i]) = frame;
			}


			//Create Combined Transformation MAtrix
			XMMATRIX mat = XMMatrixIdentity();

			mat *= XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
			mat *= XMMatrixTranslationFromVector(XMLoadFloat3(&position));

			XMStoreFloat4x4(&BonesByName[animNode.mName]->mMatrix_localTransform, mat);


		}

		mLastTime = time;
	}


	const std::vector<float4x4>& SA_AnimEvaluator::GetTransforms(float timePos) const
	{
		return mTransforms[GetFrameIndexAt(timePos)];
	}


	float SA_AnimEvaluator::GetDuration() const
	{

		return mDuration / mTicksPerSecond;

	}

	int SA_AnimEvaluator::GetFrameIndexAt(float dt) const
	{

		dt *= mTicksPerSecond;

		float time = 0.0f;

		if (mDuration > 0.0f)
		{
			time = std::fmod(dt, mDuration);
		}

		auto percent = time / mDuration;

		if (!mPlayForward)
			percent = (1.0f - percent);

		int frameIndexAt = (int)(mTransforms.size() * percent);

		return frameIndexAt;

	}

}