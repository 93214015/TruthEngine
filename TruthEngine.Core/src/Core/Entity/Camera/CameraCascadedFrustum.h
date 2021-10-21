#pragma once

#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Scene.h"


namespace TruthEngine
{

	void CreateFrustumPointsFromCascadeInterval(float frustumIntervalBegin, float frustumIntervalEnd, const BoundingFrustum& rCameraBoundingFrustum, XMVector outCornerPointsView[8]);
	void CreateFrustumPointsFromCascadeIntervalFromProjectionMatrix(float frustumIntervalBegin, float frustumIntervalEnd, const float4x4A& projectionMatrix, XMVector outCornerPointsView[8]);


	class CameraCascadedFrustumBase
	{

	public:
		CameraCascadedFrustumBase() = default;
		virtual ~CameraCascadedFrustumBase() = default;

		CameraCascadedFrustumBase(
			uint32_t id
			, TE_CAMERA_TYPE cameraType
			, const float3A& position
			, const float3A& look
			, const float3A& up
			, const float3A& right
		);

		//
		//Set Functions
		//

		void SetViewMatrix(const float3A& _Position, const float3A& _Look, const float3A& _Up, const float3A& _Right)
		{
			m_Position = _Position;
			m_Look = _Look;
			m_Right = _Right;
			m_Up = _Up;

			UpdateViewMatrix();
		}
		void SetViewMatrix(const float3& _Position, const float3& _Look, const float3& _Up, const float3& _Right)
		{
			m_Position = static_cast<float3A>(_Position);
			m_Look = static_cast<float3A>(_Look);
			m_Right = static_cast<float3A>(_Right);
			m_Up = static_cast<float3A>(_Up);

			UpdateViewMatrix();
		}

		inline void SetPosition(const float3A& position)
		{
			m_Position = position;
			UpdateViewMatrix();
		}
		inline void SetPosition(const float3& position)
		{
			m_Position = static_cast<float3A>(position);
			UpdateViewMatrix();
		}

		inline void SetLook(const float3A& _Look, const float3A& _Up, const float3A& _Right)
		{
			m_Look = _Look;
			m_Up = _Up;
			m_Right = _Right;
			UpdateViewMatrix();
		}
		inline void SetLook(const float3& _Look, const float3& _Up, const float3& _Right)
		{
			m_Look = static_cast<float3A>(_Look);
			m_Up = static_cast<float3A>(_Up);
			m_Right = static_cast<float3A>(_Right);

			UpdateViewMatrix();
		}

		//
		//Get Functions
		//
		inline const float3A& GetPosition()const noexcept
		{
			return m_Position;
		}

		inline const float3A& GetLook()const noexcept
		{
			return m_Look;
		}

		inline const float3A& GetUp()const noexcept
		{
			return m_Up;
		}

		inline const float3A& GetRight()const noexcept
		{
			return m_Right;
		}

		inline const float4x4A& GetView()const
		{
			return m_ViewMatrix;
		}

		inline bool operator==(const CameraCascadedFrustumBase& camera)
		{
			return m_ID == camera.m_ID;
		}



		virtual void UpdateFrustums(Scene* scene, Camera* referenceCamera, bool _UpdatePositinRegardingSceneAABB) = 0;
		virtual const float4x4A& GetViewProj(size_t splitIndex)const noexcept = 0;
		virtual const float4x4A& GetProjection(size_t splitIndex)const noexcept = 0;
		virtual const BoundingAABox& GetBoundingBox(size_t splitIndex)const noexcept = 0;
		virtual void SetCascadesConveringDepth(const float* _CascadeCoveringDepths) = 0;

		virtual size_t GetSplitNum() const noexcept = 0;

	protected:

		void UpdateViewMatrix();
		/*void CreateBoundingFrustum();*/

	protected:
		float4x4A m_ViewMatrix;

		float3A m_Position;
		float3A m_Look;
		float3A m_Up;
		float3A m_Right;



		uint32_t m_ID = 0;
		TE_CAMERA_TYPE m_CameraType;

		//
		// Friend Class
		//
		friend class CameraManager;
	};


	template<size_t TSplitNum>
	class CameraCascadedFrustum final : public CameraCascadedFrustumBase
	{
	public:
		CameraCascadedFrustum(
			uint32_t id
			, TE_CAMERA_TYPE cameraType
			, const float frustumSplitCoveringPercentage[TSplitNum]
			, const float3A& position
			, const float3A& look
			, const float3A& up
			, const float3A& right);
		CameraCascadedFrustum() = default;
		virtual ~CameraCascadedFrustum() = default;


		//
		//Set Functions
		//
		void UpdateFrustums(Scene* scene, Camera* referenceCamera, bool _UpdateViewRegardingSceneBB) override;

		//
		//Get Functions
		//
		const float4x4A& GetViewProj(size_t splitIndex)const noexcept override
		{
			return m_ViewProjMatrix[splitIndex];
		}

		const float4x4A& GetProjection(size_t splitIndex)const noexcept override
		{
			return m_ProjectionMatrix[splitIndex];
		}

		const BoundingAABox& GetBoundingBox(size_t splitIndex)const noexcept override
		{
			return m_BoundingFrustums[splitIndex];
		}


		size_t GetSplitNum() const noexcept override
		{
			return GetStaticSplitNum();
		}

		virtual void SetCascadesConveringDepth(const float* _CascadeCoveringDepths) override
		{
			memcpy(m_SplitFrustumCoveringPercentage, _CascadeCoveringDepths, sizeof(float) * TSplitNum);
		}

		constexpr size_t GetStaticSplitNum() const noexcept
		{
			return TSplitNum;
		}


	protected:

		//void CreateBoundingFrustum();

	protected:
		float4x4A m_ViewProjMatrix[TSplitNum];

		float4x4A m_ProjectionMatrix[TSplitNum];

		float m_SplitFrustumCoveringPercentage[TSplitNum];

		static constexpr size_t m_SplitNum = TSplitNum;


		BoundingAABox m_BoundingFrustums[TSplitNum];

		//
		// Friend Class
		//
		friend class CameraManager;
	};


	template<size_t TSplitNum>
	TruthEngine::CameraCascadedFrustum<TSplitNum>::CameraCascadedFrustum(uint32_t id
		, TE_CAMERA_TYPE cameraType
		, const float frustumSplitCoveringPercentage[TSplitNum]
		, const float3A& position
		, const float3A& look
		, const float3A& up
		, const float3A& right)
		: CameraCascadedFrustumBase(id, cameraType, position, look, up, right)
	{
		memcpy(m_SplitFrustumCoveringPercentage, frustumSplitCoveringPercentage, TSplitNum * sizeof(float));
	}

	/*template<uint32_t TSplitNum>
	void TruthEngine::CameraCascadedFrustum<TSplitNum>::CreateBoundingFrustum()
	{
		auto XMView = XMLoadFloat4x4(&m_ViewMatrix);
		const auto InvView = XMMatrixInverse(nullptr, XMView);

		for (uint32_t i = 0; i < TSplitNum; ++i)
		{
			auto XMProj = XMLoadFloat4x4(&m_ProjectionMatrix[i]);
			BoundingFrustum::CreateFromMatrix(m_BoundingFrustums[i], XMProj);

			m_BoundingFrustums[i].Transform(m_BoundingFrustums[i], InvView);
		}

	}*/


	template<size_t TSplitNum>
	void TruthEngine::CameraCascadedFrustum<TSplitNum>::UpdateFrustums(Scene* scene, Camera* referenceCamera, bool _UpdateViewRegardingSceneBB)
	{
		using namespace DirectX;

		BoundingAABox _sceneAABBLightSpace;
		const auto& _sceneAABB = scene->GetBoundingBox();
		XMMatrix _xmView = Math::ToXM(m_ViewMatrix);
		_sceneAABB.Transform(_sceneAABBLightSpace, _xmView);

		XMVector _sceneAABBCornersLightSpace[8];
		float3 _tempsceneAABBCorners[8];
		_sceneAABB.GetCorners(_tempsceneAABBCorners);

		XMVector _sceneAABBLightSpaceMin = XMVectorFLTMax;
		XMVector _sceneAABBLightSpaceMax = XMVectorFLTMin;

		for (size_t cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
		{
			_sceneAABBCornersLightSpace[cornerIndex] = XMVector3Transform(XMLoadFloat3(&_tempsceneAABBCorners[cornerIndex]), _xmView);
			_sceneAABBLightSpaceMin = XMVectorMin(_sceneAABBLightSpaceMin, _sceneAABBCornersLightSpace[cornerIndex]);
			_sceneAABBLightSpaceMax = XMVectorMax(_sceneAABBLightSpaceMax, _sceneAABBCornersLightSpace[cornerIndex]);
		}

		XMMatrix _lightViewInv = XMMatrixInverse(nullptr, _xmView);

		//float rCameraViewRange = referenceCamera->GetZFarPlane() - referenceCamera->GetZNearPlane();

		XMMatrix _rCameraViewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4A(&referenceCamera->GetView()));

		const BoundingFrustum& _rCameraBoundingFrustumViewSpace = referenceCamera->GetBoundingFrustumViewSpace();

		for (size_t splitIndex = 0; splitIndex < TSplitNum; ++splitIndex)
		{
			float _splitIntervalBegin = 1;
			float _splitIntervalEnd = m_SplitFrustumCoveringPercentage[splitIndex] /* * rCameraViewRange*/;

			/*XMVector _rCameraFrustumCorners_0[8];
			CreateFrustumPointsFromCascadeIntervalFromProjectionMatrix(_splitIntervalBegin, _splitIntervalEnd, referenceCamera->GetProjection(), _rCameraFrustumCorners_0);*/

			XMVector _rCameraFrustumCorners[8];
			CreateFrustumPointsFromCascadeInterval(_splitIntervalBegin, _splitIntervalEnd, _rCameraBoundingFrustumViewSpace, _rCameraFrustumCorners);

			XMVector _vFrustumPointMin = XMVectorFLTMax;
			XMVector _vFrustumPointMax = XMVectorFLTMin;

			for (size_t cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
			{
				//transform corner point from referenceCamera View Space to World Space
				_rCameraFrustumCorners[cornerIndex] = DirectX::XMVector3TransformCoord(_rCameraFrustumCorners[cornerIndex], _rCameraViewInv);
				XMVector _tempCorner;
				//transform corner point from world space to camera view space
				_tempCorner = XMVector3TransformCoord(_rCameraFrustumCorners[cornerIndex], _xmView);
				_vFrustumPointMin = XMVectorMin(_tempCorner, _vFrustumPointMin);
				_vFrustumPointMax = XMVectorMax(_tempCorner, _vFrustumPointMax);
			}

			// This code removes the shimmering effect along the edges of shadows due to
			// the light changing to fit the camera.

			// Fit the ortho projection to the splits far plane and a near plane of zero. 
			// Pad the projection to be the size of the diagonal of the Frustum partition. 
			// 
			// To do this, we pad the ortho transform so that it is always big enough to cover 
			// the entire camera view frustum.
			XMVECTOR vDiagonal = _rCameraFrustumCorners[0] - _rCameraFrustumCorners[6];
			vDiagonal = XMVector3Length(vDiagonal);

			// The bound is the length of the diagonal of the frustum interval.
			/*float fSplitBound = XMVectorGetX(vDiagonal);*/

			// The offset calculated will pad the ortho projection so that it is always the same size 
			// and big enough to cover the entire cascade interval.
			XMVECTOR vBoarderOffset = (vDiagonal - (_vFrustumPointMax - _vFrustumPointMin)) * XMVectorHalf;
			//set z and w component to zero
			vBoarderOffset *= XMVECTORF32{ 1.0f, 1.0f, .0f, .0f };

			//Add the offset to min and max frustum points
			_vFrustumPointMin -= vBoarderOffset;
			_vFrustumPointMax += vBoarderOffset;

			// The world units per texel are used to snap the shadow the orthographic projection
			// to texel sized increments.  This keeps the edges of the shadows from shimmering.
			/*FLOAT fWorldUnitsPerTexel = fCascadeBound / (float)m_CopyOfCascadeConfig.m_iBufferSize;
			vWorldUnitsPerTexel = XMVectorSet(fWorldUnitsPerTexel, fWorldUnitsPerTexel, 0.0f, 0.0f);*/



			//we are using reverse depth so the near and far plane have reverse order
			float _nearPlane = XMVectorGetZ(_sceneAABBLightSpaceMin);
			float _farPlane = XMVectorGetZ(_sceneAABBLightSpaceMax);

			auto _splitProjectionMatrix = XMMatrixOrthographicOffCenterLH(XMVectorGetX(_vFrustumPointMin), XMVectorGetX(_vFrustumPointMax), XMVectorGetY(_vFrustumPointMin), XMVectorGetY(_vFrustumPointMax), _nearPlane, _farPlane);
			XMStoreFloat4x4A(&m_ProjectionMatrix[splitIndex], _splitProjectionMatrix);

			XMStoreFloat4x4A(&m_ViewProjMatrix[splitIndex], XMMatrixMultiply(_xmView, _splitProjectionMatrix));

			//
			// Create Bounding Box
			//
			static const XMVECTORU32 _vGrabZ = { 0x00000000,0x00000000,0xFFFFFFFF,0x00000000 };

			XMVector _AABBMin = XMVectorSelect(_vFrustumPointMin, _sceneAABBLightSpaceMin, _vGrabZ);
			XMVector _AABBMax = XMVectorSelect(_vFrustumPointMax, _sceneAABBLightSpaceMax, _vGrabZ);

			BoundingAABox& _AABB = m_BoundingFrustums[splitIndex];

			BoundingAABox::CreateFromPoints(_AABB, _AABBMin, _AABBMax);

			_AABB.Transform(_AABB, _lightViewInv);


		}


	}


}