#pragma once

namespace TruthEngine
{


	class BoneWeight
	{
	public:

		BoneWeight(UINT BoneIndex, float BoneWeight);

		uint8_t mBoneIndex;
		float mBoneWeight;
	};


	class Bone
	{
	public:
		Bone();
		~Bone();
		Bone(Bone&&) = default;
		Bone& operator = (Bone&&) = default;
		Bone(Bone&) = default;
		Bone& operator = (Bone&) = default;

		std::string mName;

		float4x4 mMatrix_Offset;

		float4x4 mMatrix_localTransform;

		float4x4 mMatrix_GlobalTransform;

		float4x4 mMatrix_OriginlaLocalTransform;

		Bone* mParent;

		std::vector<Bone*> mChildren;


	private:

		int ReleaseChildrens();

	};

}
