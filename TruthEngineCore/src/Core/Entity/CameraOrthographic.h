#pragma once
#include "ICamera.h"

namespace TruthEngine::Core {

	class CameraOrthographic : public ICamera {

	public:

		// Set frustum
		void SetLens(float frustumWidth, float frustumheight, float zn, float zf);

		float GetFrustumWidth()const noexcept;
		float GetFrustumHeight()const noexcept;

	private:

		float m_FrustumWidth;
		float m_FrustumHeight;

	};

}
