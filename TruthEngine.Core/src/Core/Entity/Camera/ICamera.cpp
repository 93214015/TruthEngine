#include "pch.h"
#include "ICamera.h"

/*
#include "Core/Event/Event.h"
#include "Core/Event/EventKey.h"
#include "Core/Event/EventMouse.h"

#include "Core/Application.h"
#include "Core/Input/InputManager.h"

using namespace DirectX;


namespace TruthEngine {


	ICamera::ICamera(std::string_view name)
		: m_Name(name),
		m_Position(0.0f, 0.0f, 0.0f),
		m_Right(1.0f, 0.0f, 0.0f),
		m_Up(0.0f, 1.0f, 0.0f),
		m_Look(0.0f, 0.0f, 1.0f),
		m_Speed(1.0),
		m_Active(false)
	{
		auto lambda_onMouseMove = [this](Event& event)
		{
			OnMouseMove(static_cast<EventMouseMoved&>(event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::MouseMoved, lambda_onMouseMove);



		auto lambda_onKeyPressed = [this](Event& event)
		{
			OnKeyPressed(static_cast<EventKeyPressed&>(event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::KeyPressed, lambda_onKeyPressed);
	}

	void ICamera::Active()
	{
		m_Active = true;
	}

	void ICamera::Deactive()
	{
		m_Active = false;
	}


	float3 ICamera::GetPosition()const
	{
		return m_Position;
	}


	void ICamera::SetPosition(float x, float y, float z)
	{
		m_Position = float3(x, y, z);
	}


	void ICamera::SetPosition(const float3& v)
	{
		m_Position = v;
	}



	float3 ICamera::GetRight()const
	{
		return m_Right;
	}



	float3 ICamera::GetUp()const
	{
		return m_Up;
	}



	float3 ICamera::GetLook()const
	{
		return m_Look;
	}


	float ICamera::GetNearZ()const
	{
		return m_NearZ;
	}


	float ICamera::GetFarZ()const
	{
		return m_FarZ;
	}


	void XM_CALLCONV ICamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
	{
		XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
		XMVECTOR U = XMVector3Cross(L, R);

		XMStoreFloat3(&m_Position, pos);
		XMStoreFloat3(&m_Look, L);
		XMStoreFloat3(&m_Right, R);
		XMStoreFloat3(&m_Up, U);

		UpdateViewMatrix();
	}


	void ICamera::LookAt(const float3& pos, const float3& target, const float3& up)
	{
		XMVECTOR P = ToXM(pos);
		XMVECTOR T = ToXM(target);
		XMVECTOR U = ToXM(up);

		LookAt(P, T, U);
	}



	float4x4 ICamera::GetView() const 
	{
		return m_View;
	}


	float4x4 ICamera::GetViewInv() const 
	{
		auto XMView = ToXM(m_View);

		const auto viewInv = XMMatrixInverse(nullptr, XMView);
		float4x4 r;
		XMStoreFloat4x4(&r, viewInv);
		return r;
	}


	float4x4 ICamera::GetProj() const 
	{
		return m_Proj;
	}


	float4x4 ICamera::GetProjInv() const
	{
		return Inverse(m_Proj);
	}


	float4x4 ICamera::GetViewProj() const
	{
		auto XMView = XMLoadFloat4x4(&m_View);
		auto XMProj = XMLoadFloat4x4(&m_Proj);

		float4x4 vp;
		XMStoreFloat4x4(&vp, XMMatrixMultiply(XMView, XMProj));
		return vp;
	}


	float4x4 ICamera::GetViewProjInv() const
	{
		auto XMView = XMLoadFloat4x4(&m_View);
		auto XMProj = XMLoadFloat4x4(&m_Proj);

		float4x4 vpInv;
		XMStoreFloat4x4(&vpInv, XMMatrixInverse(nullptr, XMMatrixMultiply(XMView, XMProj)));
		return vpInv;
	}


	float4 ICamera::GetPerspectiveValues()const 
	{
		return 	float4((1 / m_Proj._11), (1 / m_Proj._22), m_Proj._33, m_Proj._43);
	}


	void ICamera::Strafe(float d)
	{
		d *= m_Speed;
		// mPosition += d*mRight
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR r = XMLoadFloat3(&m_Right);
		XMVECTOR p = XMLoadFloat3(&m_Position);
		XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));

		UpdateViewMatrix();
	}


	void ICamera::Walk(float d)
	{
		d *= m_Speed;
		// mPosition += d*mLook
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR l = XMLoadFloat3(&m_Look);
		XMVECTOR p = XMLoadFloat3(&m_Position);
		XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));

		UpdateViewMatrix();
	}


	void ICamera::Pitch(float angle)
	{
		// Rotate up and look vector about the right vector.

		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

		XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));

		UpdateViewMatrix();
	}


	void ICamera::RotateY(float angle)
	{
		// Rotate the basis vectors about the world y-axis.

		XMMATRIX R = XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
		XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));

		UpdateViewMatrix();
	}


	void ICamera::RotateCamera(float d)
	{
		auto angle = XMConvertToRadians(20 * d);
		auto R = XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Position, XMVector3Transform(XMLoadFloat3(&m_Position), R));

		RotateY(angle);

		UpdateViewMatrix();
	}


	void ICamera::UpdateViewMatrix()
	{
		XMVECTOR R = XMLoadFloat3(&m_Right);
		XMVECTOR U = XMLoadFloat3(&m_Up);
		XMVECTOR L = XMLoadFloat3(&m_Look);
		XMVECTOR P = XMLoadFloat3(&m_Position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&m_Right, R);
		XMStoreFloat3(&m_Up, U);
		XMStoreFloat3(&m_Look, L);

		m_View(0, 0) = m_Right.x;
		m_View(1, 0) = m_Right.y;
		m_View(2, 0) = m_Right.z;
		m_View(3, 0) = x;

		m_View(0, 1) = m_Up.x;
		m_View(1, 1) = m_Up.y;
		m_View(2, 1) = m_Up.z;
		m_View(3, 1) = y;

		m_View(0, 2) = m_Look.x;
		m_View(1, 2) = m_Look.y;
		m_View(2, 2) = m_Look.z;
		m_View(3, 2) = z;

		m_View(0, 3) = 0.0f;
		m_View(1, 3) = 0.0f;
		m_View(2, 3) = 0.0f;
		m_View(3, 3) = 1.0f;

		CreateBoundingFrustum();
	}


	void ICamera::CreateBoundingFrustum()
	{
		auto XMView = XMLoadFloat4x4(&m_View);
		auto XMProj = XMLoadFloat4x4(&m_Proj);

		BoundingFrustum::CreateFromMatrix(m_BoundingFrustum, XMProj);
		const auto InvView = XMMatrixInverse(nullptr, XMView);
		m_BoundingFrustum.Transform(m_BoundingFrustum, InvView);
	}


	const BoundingFrustum& ICamera::GetBoundingFrustum()const
	{
		return m_BoundingFrustum;
	}


	ContainmentType ICamera::BoundingBoxContainment(const BoundingBox& _boundingBox) const
	{
		return m_BoundingFrustum.Contains(_boundingBox);
	}

	void ICamera::OnMouseMove(EventMouseMoved& event)
	{
		if (m_Active)
		{
			if (InputManager::IsMouseRightDown())
			{
				float dx_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDX()));
				float dy_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDY()));

				Pitch(dy_angle / 10);
				RotateY(dx_angle / 10);

				UpdateViewMatrix();
			}
		}
	}

	void ICamera::OnKeyPressed(EventKeyPressed& event)
	{
		if (m_Active)
		{
			float dt = TE_INSTANCE_APPLICATION->FrameTime();
			switch (event.GetKeyCode())
			{
			case 'W':
				Walk(dt);
				return;
			case 'S':
				Walk(-dt);
				return;
			case 'A':
				Strafe(-dt);
				break;
			case'D':
				Strafe(dt);
				break;
			case 'Q':
				RotateCamera(dt);
				break;
			default:
				break;
			}
		}
	}

}*/