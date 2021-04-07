#pragma once



namespace TruthEngine
{

	class BoneWeight;
	class SA_Animation;

	class SkinnedAnimationComponent
	{
	public:

		SkinnedAnimationComponent(SA_Animation* _Animation);
		virtual ~SkinnedAnimationComponent();
		SkinnedAnimationComponent(const SkinnedAnimationComponent&);
		SkinnedAnimationComponent(SkinnedAnimationComponent&&);
		SkinnedAnimationComponent& operator = (SkinnedAnimationComponent&&);

		void Update(double _dt);
		const std::vector <float4x4>* GetBoneTransforms() const;
		bool SetAnimation(const std::string& AnimName);
		bool SetAnimation(int AnimIndex);
		bool DisableAnimation(const std::string& AnimName);
		bool DisableAnimation(int AnimIndex);

	private:
		float mTimePos;

		SA_Animation* mAnimation;

		std::string mCurrentAnimName;

		//std::map<std::string> mAnimQueue;

		//void ExtractBoneWeightsForVertecies(aiMesh* mesh, std::map<UINT, std::vector<BoneWeight>>* VertexToBone);

		//virtual int InitBuffers() override;

	
	};
}
