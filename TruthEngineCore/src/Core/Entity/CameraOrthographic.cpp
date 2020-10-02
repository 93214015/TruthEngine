#include "pch.h"
#include "CameraOrthographic.h"

using namespace DirectX;

namespace TruthEngine::Core {



	void CameraOrthographic::SetLens(float frustumWidth, float frustumHeight, float zn, float zf)
	{
		m_FrustumWidth = frustumWidth;
		m_FrustumHeight = frustumHeight;
		m_NearZ = zn;
		m_FarZ = zf;

		XMStoreFloat4x4(&m_Proj, XMMatrixOrthographicLH(frustumWidth, frustumHeight, zn, zf));
	}

	float CameraOrthographic::GetFrustumWidth() const noexcept
	{
		return m_FrustumWidth;
	}

	float CameraOrthographic::GetFrustumHeight() const noexcept
	{
		return m_FrustumHeight;
	}

}