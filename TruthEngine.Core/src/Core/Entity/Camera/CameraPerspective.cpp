#include "pch.h"
#include "CameraPerspective.h"

/*
using namespace DirectX;

namespace TruthEngine {


	void CameraPerspective::SetLens(float fovY, float aspect, float zn, float zf)
	{
		// cache properties
		m_FovY = fovY;
		m_Aspect = aspect;
		m_NearZ = zn;
		m_FarZ = zf;

		m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
		m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

		const auto nearWindowWidth = m_Aspect * m_NearWindowHeight;
		m_FovX = 2.0f * atanf(nearWindowWidth / 2.0f / zn);

		XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
		XMStoreFloat4x4(&m_Proj, P);

	}


	float CameraPerspective::GetAspect()const
	{
		return m_Aspect;
	}


	float CameraPerspective::GetFovY()const
	{
		return m_FovY;
	}


	float CameraPerspective::GetFovX()const
	{
		float halfWidth = 0.5f * GetNearWindowWidth();
		return 2.0f * atan(halfWidth / m_NearZ);
	}


	float CameraPerspective::GetNearWindowWidth()const
	{
		return m_Aspect * m_NearWindowHeight;
	}


	float CameraPerspective::GetNearWindowHeight()const
	{
		return m_NearWindowHeight;
	}


	float CameraPerspective::GetFarWindowWidth()const
	{
		return m_Aspect * m_FarWindowHeight;
	}


	float CameraPerspective::GetFarWindowHeight()const
	{
		return m_FarWindowHeight;
	}

}*/