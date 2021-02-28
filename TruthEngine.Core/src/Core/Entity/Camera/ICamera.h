#pragma once

/*
namespace TruthEngine {

	class EventMouseMoved;
	class EventKeyPressed;

	class ICamera
	{

	public:
		ICamera(std::string_view name);

		void Active();
		void Deactive();

		// Get/Set world camera position.
		float3 GetPosition()const;
		void SetPosition(float x, float y, float z);
		void SetPosition(const float3& v);

		// Get camera basis vectors.
		float3 GetRight()const;
		float3 GetUp()const;
		float3 GetLook()const;

		// Get frustum properties.
		float GetNearZ()const;
		float GetFarZ()const;

		inline void SetSpeed(const float speed)noexcept
		{
			m_Speed = speed;
		}


		// Define camera space via LookAt parameters.
		void XM_CALLCONV LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
		void LookAt(const float3& pos, const float3& target, const float3& up);

		// Get View/Proj matrices.
		float4x4 GetView()const;

		float4x4 GetViewInv()const;

		float4x4 GetProj()const;

		float4x4 GetProjInv()const;

		float4x4 GetViewProj()const;

		float4x4 GetViewProjInv()const;

		float4 GetPerspectiveValues()const;

		// Strafe/Walk the camera a distance d.
		void Strafe(float d);
		void Walk(float d);

		// Rotate the camera.
		void Pitch(float angle);
		void RotateY(float angle);
		void RotateCamera(float d);

		// After modifying camera position/orientation, call to rebuild the view matrix.
		void UpdateViewMatrix();

		//after modifying View or projection matrix call it to update Bounding Frustum
		void CreateBoundingFrustum();

		//return Bounding Frustum
		const DirectX::BoundingFrustum& GetBoundingFrustum()const;

		//check state of bounding box state relative to Camera Frustum
		DirectX::ContainmentType BoundingBoxContainment(const DirectX::BoundingBox& _boundingBox) const;

	protected:
		void OnMouseMove(EventMouseMoved& event);
		void OnKeyPressed(EventKeyPressed& event);

	protected:
		std::string m_Name;


		// Camera coordinate system with coordinates relative to world space.
		float3 m_Position;
		float3 m_Right;
		float3 m_Up;
		float3 m_Look;

		// Cache frustum properties.
		float m_NearZ;
		float m_FarZ;


		float m_Speed;

		// Cache View/Proj matrices.
		float4x4 m_View;
		float4x4 m_Proj;

		//Bounding Frustum
		DirectX::BoundingFrustum m_BoundingFrustum;

		bool m_Active;



		//
		// Friend Classes
		//
		friend class CameraManager;
	};

}*/
