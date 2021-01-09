#include "pch.h"
#include "CameraController.h"

#include "Camera.h"
#include "Core/Event/EventMouse.h"
#include "Core/Event/EventKey.h"
#include "Core/Application.h"
#include "Core/Input/InputManager.h"

using namespace DirectX;

namespace TruthEngine::Core
{
	CameraController::CameraController()
	{
		auto lambda_onMouseMove = [this](Event& event)
		{
			if (m_Camera != nullptr)
			{
				OnMouseMove(static_cast<EventMouseMoved&>(event));
			}
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::MouseMoved, lambda_onMouseMove);

		auto lambda_onKeyPressed = [this](Event& event)
		{
			if (m_Camera != nullptr)
			{
				OnKeyPressed(static_cast<EventKeyPressed&>(event));
			}
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::KeyPressed, lambda_onKeyPressed);

	}
	void CameraController::Strafe(float d)
	{
		d *= m_Speed;
		// mPosition += d*mRight
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR r = XMLoadFloat3(&m_Camera->m_Right);
		XMVECTOR p = XMLoadFloat3(&m_Camera->m_Position);
		XMStoreFloat3(&m_Camera->m_Position, XMVectorMultiplyAdd(s, r, p));

		m_Camera->UpdateViewMatrix();
	}


	void CameraController::Walk(float d)
	{
		d *= m_Speed;
		// mPosition += d*mLook
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR l = XMLoadFloat3(&m_Camera->m_Look);
		XMVECTOR p = XMLoadFloat3(&m_Camera->m_Position);
		XMStoreFloat3(&m_Camera->m_Position, XMVectorMultiplyAdd(s, l, p));

		m_Camera->UpdateViewMatrix();
	}


	void CameraController::Pitch(float angle)
	{
		// Rotate up and look vector about the right vector.

		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Camera->m_Right), angle);

		XMStoreFloat3(&m_Camera->m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Camera->m_Up), R));
		XMStoreFloat3(&m_Camera->m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Camera->m_Look), R));

		m_Camera->UpdateViewMatrix();
	}


	void CameraController::RotateY(float angle)
	{
		// Rotate the basis vectors about the world y-axis.

		XMMATRIX R = XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Camera->m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Camera->m_Right), R));
		XMStoreFloat3(&m_Camera->m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Camera->m_Up), R));
		XMStoreFloat3(&m_Camera->m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Camera->m_Look), R));

		m_Camera->UpdateViewMatrix();
	}


	void CameraController::RotateCamera(float d)
	{
		auto angle = XMConvertToRadians(20 * d);
		auto R = XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Camera->m_Position, XMVector3Transform(XMLoadFloat3(&m_Camera->m_Position), R));

		RotateY(angle);

		m_Camera->UpdateViewMatrix();
	}


	void CameraController::OnMouseMove(EventMouseMoved& event)
	{
		if (InputManager::IsMouseRightDown())
		{
			float dx_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDX()));
			float dy_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDY()));

			Pitch(dy_angle / 10);
			RotateY(dx_angle / 10);

			m_Camera->UpdateViewMatrix();
		}
	}


	void CameraController::OnKeyPressed(EventKeyPressed& event)
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