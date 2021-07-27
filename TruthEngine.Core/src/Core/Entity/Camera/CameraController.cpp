#include "pch.h"
#include "CameraController.h"

#include "Camera.h"
#include "Core/Event/EventMouse.h"
#include "Core/Event/EventKey.h"
#include "Core/Application.h"
#include "Core/Input/InputManager.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Components/TransformComponent.h"

using namespace DirectX;

namespace TruthEngine
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
		up *= -m_Speed;
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


	void CameraController::OnMouseMove(EventMouseMoved& event)
	{
		if (!TE_INSTANCE_APPLICATION->IsHoveredSceneViewPort())
			return;

		if (InputManager::IsMouseRightDown())
		{
			float dx_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDX())) * 0.2;
			float dy_angle = XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDY())) * 0.2;

			Pitch(dy_angle);
			RotateY(dx_angle);

			if (InputManager::IsKeyPressed(Key::LeftAlt))
			{
				float3 _RotationOrigin = float3{ .0f, .0f, .0f };

				Scene* _Scene = TE_INSTANCE_APPLICATION->GetActiveScene();

				if (Entity _SelectedEntity = _Scene->GetSelectedEntity(); _SelectedEntity)
				{
					//_RotationOrigin = _RotationOrigin + _SelectedEntity.GetComponent<TransformComponent>().GetWorldCenterOffset();
					//_RotationOrigin += _Scene->GetTranslateHierarchy(_SelectedEntity);
					auto& _EntityTransform = _Scene->GetComponent<TransformComponent>(_SelectedEntity).GetTransform();
					float3 _EntityTranslation = float3{ _EntityTransform._41, _EntityTransform._42, _EntityTransform._43 };
					_RotationOrigin += _EntityTranslation;
				}

				float4x4A _Transform = Math::TransformMatrixRotation(dx_angle, float3{ .0f, 1.0f, .0f }, _RotationOrigin);
				_Transform = _Transform * Math::TransformMatrixRotation(dy_angle, m_Camera->m_Right, _RotationOrigin);

				m_Camera->m_Position = Math::TransformPoint(m_Camera->m_Position, _Transform);

				m_Camera->UpdateViewMatrix();
			}


			return;
		}

		if (InputManager::IsMouseMiddleDown())
		{
			const auto dt = TE_INSTANCE_APPLICATION->FrameTime();

			float dx = -InputManager::GetDX() * dt * 0.5;
			float dy = -InputManager::GetDY() * dt * 0.5;

			Panning(dx, dy);

			return;
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
		default:
			break;
		}
	}


}