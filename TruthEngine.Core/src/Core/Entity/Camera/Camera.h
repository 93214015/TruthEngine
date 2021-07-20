#pragma once

namespace TruthEngine
{

	enum class TE_CAMERA_TYPE
	{
		Perspective,
		Orthographic
	};


	class Camera
	{
	public:
		/*Camera(uint32_t id, TE_CAMERA_TYPE cameraType, const float3& position, const float3& look, const float3& up
			, const float3& right, const float zNear, const float zFar, const float aspectRatio
			, const float fovY, const float fovX, const float nearWindowHeight, const float farWindowHeight
			, const float4x4& projMatrix, const BoundingFrustum& _BoundingFrustumView, bool _IsReveresedDepth, const void(*_FuncEditFrustum)(Camera*)
		);*/
		Camera
		(uint32_t id, TE_CAMERA_TYPE cameraType
			, const float3& position, const float3& look, const float3& up, const float3& right
			, const float zNear, const float zFar, const float aspectRatio, const float fovY
			, const float4x4A& projMatrix, const BoundingFrustum& _BoundingFrustumView, bool _IsReveresedDepth, void(*_FuncEditFrustum)(Camera*)
		);
		Camera() = default;
		virtual ~Camera() = default;


		//
		//Set Functions
		//

		inline void SetView(const float3& _Position, const float3& _Look, const float3& _Up, const float3& _Right)
		{
			m_Position = _Position;
			m_Look = _Look;
			m_Right = _Right;
			m_Up = _Up;

			UpdateViewMatrix();
		}

		void SetView(const float3& _Look, const float3& _Up, const float3& _Right)
		{
			m_Look = _Look;
			m_Up = _Up;
			m_Right = _Right;
			UpdateViewMatrix();
		}

		inline void SetPosition(const float3& position)
		{
			m_Position = position;
			UpdateViewMatrix();
		}


		inline void SetLook(const float3& look)
		{
			m_Look = look;
			UpdateViewMatrix();
		}


		void SetZNearPlane(const float zNearPlane);
		void SetZFarPlane(const float zFarPlane);
		void SetAspectRatio(const float aspectRatio);
		void SetFOVY(const float _FOVY);

		void SetFrustum(float width, float height, float zNearPlane, float zFarPlane);

		//
		//Get Functions
		//
		inline const float3& GetPosition()const noexcept
		{
			return m_Position;
		}

		inline const float3& GetLook()const noexcept
		{
			return m_Look;
		}

		inline const float3& GetUp()const noexcept
		{
			return m_Up;
		}

		inline const float3& GetRight()const noexcept
		{
			return m_Right;
		}

		inline const float4x4A& GetViewProj()const noexcept
		{
			return m_ViewProjMatrix;
		}

		inline const float4x4A& GetView()const noexcept
		{
			return m_ViewMatrix;
		}

		inline const float4x4A& GetViewInv() const noexcept
		{
			return m_ViewInvMatrix;
		}

		inline const float4x4A& GetProjection() const noexcept
		{
			return m_ProjectionMatrix;
		}

		inline float GetZNearPlane()const noexcept
		{
			return m_ZNear;
		}

		inline float GetZFarPlane()const noexcept
		{
			return m_ZFar;
		}

		inline BoundingFrustum& GetBoundingFrustumWorldSpace() noexcept
		{
			return m_BoundingFrustumWorldSpace;
		}
		inline const BoundingFrustum& GetBoundingFrustumWorldSpace() const noexcept
		{
			return m_BoundingFrustumWorldSpace;
		}
		inline BoundingFrustum& GetBoundingFrustumViewSpace() noexcept
		{
			return m_BoundingFrustumViewSpace;
		}
		inline const BoundingFrustum& GetBoundingFrustumViewSpace() const noexcept
		{
			return m_BoundingFrustumViewSpace;
		}
		inline bool IsReversedDepth()const noexcept
		{
			return m_IsReversedDepth;
		}

		inline bool operator==(const Camera& camera)
		{
			return m_ID == camera.m_ID;
		}

	protected:

		void UpdateViewMatrix();

	protected:
		float4x4A m_ViewProjMatrix;
		float4x4A  m_ViewMatrix;
		float4x4A  m_ViewInvMatrix;
		float4x4A  m_ProjectionMatrix;

		//Insert the float members between float3 to keep them 16-byte aligned
		float3 m_Position;
		float m_ZNear;
		float3 m_Look;
		float m_ZFar;
		float3 m_Up;
		float m_AspectRatio;
		float3 m_Right;
		float m_FovY;

		//float m_FovX;
		//float m_NearWindowHeight;
		//float m_FarWindowHeight;

		uint32_t m_ID = 0;
		TE_CAMERA_TYPE m_CameraType;
		bool m_IsReversedDepth = false;

		BoundingFrustum m_BoundingFrustumViewSpace;
		BoundingFrustum m_BoundingFrustumWorldSpace;

		//std::function<void(Camera*)> m_FuncEditFrustum;
		void(*m_FuncEditFrustum)(Camera*);

		//
		// Friend Class
		//
		friend class CameraManager;
		friend class CameraController;
	};

}