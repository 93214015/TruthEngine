#pragma once
#include "GraphicResource.h"
#include "ShaderSignature.h"

namespace TruthEngine
{

	class ShaderRequiredResources
	{
	public:

		void AddResource(const ShaderSignature* _ShaderSignature);

		inline const std::vector<GraphicResource*>& GetSRVResources()const { return mSRV; }
		inline const std::vector<GraphicResource*>& GetCBVResources()const { return mCBV; }
		inline const std::vector<GraphicResource*>& GetUAVResources()const { return mUAV; }

	private:
		std::vector<GraphicResource*> mSRV;
		std::vector<GraphicResource*> mCBV;
		std::vector<GraphicResource*> mUAV;
	};
}
