#pragma once

namespace TruthEngine::Core {

	class EventMouseMoved;
	class EventKeyPressed;

	class ICamera
	{

	public:
		ICamera(std::string_view name);

		void Active();
		void Deactive();

		// Get/Set world camera position.
		DirectX::XMFLOAT3 GetPosition()const;
		void SetPosition(float x, float y, float z);
		void SetPosition(const DirectX::XMFLOAT3& v);

		// Get camera basis vectors.
		DirectX::XMFLOAT3 GetRight()const;
		DirectX::XMFLOAT3 GetUp()const;
		DirectX::XMFLOAT3 GetLook()const;

		// Get frustum properties.
		float GetNearZ()const;
		float GetFarZ()const;


		// Define camera space via LookAt parameters.
		void XM_CALLCONV LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
		void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

		// Get View/Proj matrices.
		DirectX::XMFLOAT4X4 GetView()const;

		DirectX::XMFLOAT4X4 GetViewInv()const;

		DirectX::XMFLOAT4X4 GetProj()const;

		DirectX::XMFLOAT4X4 GetProjInv()const;

		DirectX::XMFLOAT4X4 GetViewProj()const;

		DirectX::XMFLOAT4X4 GetViewProjInv()const;

		DirectX::XMFLOAT4 GetPerspectiveValues()const;

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
		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT3 m_Right;
		DirectX::XMFLOAT3 m_Up;
		DirectX::XMFLOAT3 m_Look;

		// Cache frustum properties.
		float m_NearZ;
		float m_FarZ;


		float m_Speed;

		// Cache View/Proj matrices.
		DirectX::XMFLOAT4X4 m_View;
		DirectX::XMFLOAT4X4 m_Proj;

		//Bounding Frustum
		DirectX::BoundingFrustum m_BoundingFrustum;

		bool m_Active;



		//
		// Friend Classes
		//
		friend class CameraManager;
	};

}
