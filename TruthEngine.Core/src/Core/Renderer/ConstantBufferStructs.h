#pragma once

#include "Core/Entity/Light/ILight.h"

namespace TruthEngine::Core {

	struct ConstantBuffer_Data_Per_Frame
	{
		DirectX::XMFLOAT4X4 m_VP;
		DirectX::XMFLOAT4 m_Color{0.6f, 0.6f, 0.6f, 1.0f};
	};

	struct ConstantBuffer_Data_Per_DLight : DirectionalLightData
	{
	};

}
