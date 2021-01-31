#pragma once

namespace TruthEngine
{
	namespace Core
	{
		enum class TE_CAMERA_TYPE
		{
			Perspective,
			Orthographic
		};


		class Camera
		{
		public:
			Camera(uint32_t id, TE_CAMERA_TYPE cameraType, const float3& position, const float3& look, const float3& up
				, const float3& right, const float zNear, const float zFar, const float aspectRatio
				, const float fovY, const float fovX, const float nearWindowHeight, const float farWindowHeight
				, const float4x4& projMatrix);
			Camera() = default;
			virtual ~Camera() = default;


			//
			//Set Functions
			//
			inline void SetPorjectionMatrix(const float4x4& projMatrix)
			{
				m_ProjectionMatrix = projMatrix;
				m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;

				CreateBoundingFrustum();
			}
			inline void SetViewMatrix(const float4x4& viewMatrix)
			{
				m_ViewMatrix = viewMatrix;
				m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;

				CreateBoundingFrustum();
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

			inline const float4x4& GetViewProj()const noexcept
			{
				return m_ViewProjMatrix;
			}

			inline const float4x4& GetView()const
			{
				return m_ViewMatrix;
			}

			inline const float4x4& GetProjection()const
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
			
			inline bool operator==(const Camera& camera)
			{
				return m_ID == camera.m_ID;
			}

		protected:

			void UpdateViewMatrix();
			void CreateBoundingFrustum();

		protected:
			uint32_t m_ID = 0;

			float4x4 m_ViewMatrix;
			float4x4 m_ProjectionMatrix;
			float4x4 m_ViewProjMatrix;

			float3 m_Position;
			float3 m_Look;
			float3 m_Up;
			float3 m_Right;

			float m_ZNear;
			float m_ZFar;
			float m_AspectRatio;
			float m_FovY;
			float m_FovX;
			float m_NearWindowHeight;
			float m_FarWindowHeight;

			TE_CAMERA_TYPE m_CameraType;

			DirectX::BoundingFrustum m_BoundingFrustum;

			//
			// Friend Class
			//
			friend class CameraManager;
			friend class CameraController;
		};


	}
}