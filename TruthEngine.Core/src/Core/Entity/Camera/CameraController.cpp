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


	void CameraController::Panning(float right, float up)
	{
		right *= m_Speed;
		up *= m_Speed;
		// mPosition += d*mRight
		XMVECTOR s = XMVectorReplicate(right);
		XMVECTOR r = XMLoadFloat3(&m_Camera->m_Right);
		auto sr = XMVectorMultiply(s, r);


		s = XMVectorReplicate(up);
		XMVECTOR u = XMLoadFloat3(&m_Camera->m_Up);
		auto su = XMVectorMultiply(s, u);

		s = XMVectorAdd(sr, su);

		XMVECTOR p = XMLoadFloat3(&m_Camera->m_Position);
		XMStoreFloat3(&m_Camera->m_Position, XMVectorAdd(s, p));

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
		if (!TE_INSTANCE_APPLICATION->IsHoveredSceneViewPort())
			return;

		if (InputManager::IsMouseRightDown())
		{
			float dx_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDX()));
			float dy_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDY()));

			Pitch(dy_angle / 10);
			RotateY(dx_angle / 10);

		}

		if (InputManager::IsMouseMiddleDown())
		{
			const auto dt = TE_INSTANCE_APPLICATION->FrameTime();

			float dx = -InputManager::GetDX() * dt;
			float dy = -InputManager::GetDY() * dt;

			Panning(dx, dy);
		}
	}


	void CameraController::OnKeyPressed(EventKeyPressed& event)
	{
		if (!TE_INSTANCE_APPLICATION->IsHoveredSceneViewPort())
			return;

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
			Panning(-dt, 0.0f);
			break;
		case'D':
			Panning(dt, 0.0);
			break;
		case 'Q':
			RotateCamera(dt);
			break;
		default:
			break;
		}
	}


}