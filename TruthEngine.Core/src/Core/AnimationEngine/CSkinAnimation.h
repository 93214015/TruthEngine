#pragma once


namespace TruthEngine
{
	class Bone;

	using Vector_Matrix = std::vector<float4x4>;
	using Pair_AnimationTimePos = std::pair<int, float>;


	class SA_KeyFrame_Translation
	{

	public:
		float mTime;

		float3 mTranslation;

	private:

	};


	class SA_KeyFrame_Scaling
	{
	public:
		float mTime;

		float3 mScaling;
	};


	class SA_KeyFrame_Rotation
	{
	public:
		float mTime;

		float4 mRotationQuaternion;
	};


	class SA_AnimNode
	{
	public:
		std::string mName;

		std::vector<SA_KeyFrame_Translation> mTranslations;
		std::vector<SA_KeyFrame_Scaling> mScales;
		std::vector<SA_KeyFrame_Rotation> mRotations;
	};


	class SA_AnimEvaluator
	{
	public:


		SA_AnimEvaluator(const aiAnimation* animation);


		std::string mName;


		bool mPlayForward;
		float mLastTime;
		float mTicksPerSecond;
		float mDurationInTicks;
		float mDurationInSecond;


		std::vector<Vector_Matrix> mTransforms;


		std::vector<std::tuple<int, int, int>> mLastPositions;


		void Evaluate(float dt, std::unordered_map<std::string, Bone*>& BonesByName);

		const std::vector<float4x4>& GetTransforms(float timePos) const;

		float GetDuration() const;


	private:

		std::vector<SA_AnimNode> mAnimNodes;


		int GetFrameIndexAt(float dt) const;

	};


	class SA_Animation
	{

	public:
		//Constructors&Destructors
		SA_Animation();
		SA_Animation(const aiScene* scene);
		virtual ~SA_Animation() = default;

		//Copy Constructors
		SA_Animation(const SA_Animation&);
		SA_Animation(SA_Animation&&);


		//Members
		std::string mName;


		//Member Functions
		int GetBoneIndex(const std::string& BoneName);

		const std::vector<float4x4>* GetTransform();

		bool AddAnimToQueue(int index);
		bool AddAnimToQueue(const std::string& animName);
		bool RemoveAnimToQueue(int index);
		bool RemoveAnimToQueue(const std::string& animName);
		bool UpdateTimePos(double dt);
		/*bool SetAnimation(std::string& AnimName);
		bool SetAnimation(int AnimIndex);*/
		float GetDuration();

	private:

		std::unordered_map<std::string, Bone*> mBonesByName;
		std::unordered_map<std::string, int> mBonesToIndex;
		std::vector<Bone*> mBones;


		std::unique_ptr<Bone> mBoneSkeleton;


		std::unique_ptr<Bone> mSkeleton;


		std::vector<SA_AnimEvaluator> mAnimations;


		std::unordered_map<std::string, int> mAnimationsToIndex;


		int mCurrentAnimationIndex;


		std::unordered_map<int, float> mAnimTimeQueue;


		Vector_Matrix mFinalTransforms;


		Bone* MakeSkeleton(aiNode* node, Bone* parent);


		void InitAnimations(const aiScene* Scene);


		void Calculate(float dt);


		static void CalculateBoneWorldTransform(Bone* bone);


		static void UpdateTransforms(Bone* bone);


	};
}

