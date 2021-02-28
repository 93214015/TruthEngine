#pragma once

#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Scene.h"


namespace TruthEngine
{

	void CreateFrustumPointsFromCascadeInterval(float frustumIntervalBegin, float frustumIntervalEnd, const BoundingFrustum& rCameraBoundingFrustum, XMVector outCornerPointsView[8]);
	void CreateFrustumPointsFromCascadeIntervalFromProjectionMatrix(float frustumIntervalBegin, float frustumIntervalEnd, const float4x4& projectionMatrix, XMVector outCornerPointsView[8]);


	class CameraCascadedFrustumBase
	{

	public:
		CameraCascadedFrustumBase() = default;
		virtual ~CameraCascadedFrustumBase() = default;

		CameraCascadedFrustumBase(
			uint32_t id
			, TE_CAMERA_TYPE cameraType
			, const float3& position
			, const float3& look
			, const float3& up
			, const float3& right
		);

		//
		//Set Functions
		//

		inline void SetViewMatrix(const float4x4& viewMatrix)
		{
			m_ViewMatrix = viewMatrix;
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

		inline const float4x4& GetView()const
		{
			return m_ViewMatrix;
		}

		inline bool operator==(const CameraCascadedFrustumBase& camera)
		{
			return m_ID == camera.m_ID;
		}

		virtual void UpdateFrustums(Scene* scene, Camera* referenceCamera) = 0;
		virtual const float4x4& GetViewProj(uint32_t splitIndex)const noexcept = 0;
		virtual const float4x4& GetProjection(uint32_t splitIndex)const = 0;
		virtual const BoundingFrustum& GetBoundingFrustum(uint32_t splitIndex)const = 0;

		virtual uint32_t GetSplitNum() const noexcept = 0;

	protected:

		void UpdateViewMatrix();
		/*void CreateBoundingFrustum();*/

	protected:
		uint32_t m_ID = 0;

		float4x4 m_ViewMatrix;

		float3 m_Position;
		float3 m_Look;
		float3 m_Up;
		float3 m_Right;

		TE_CAMERA_TYPE m_CameraType;

		//
		// Friend Class
		//
		friend class CameraManager;
	};


	template<uint32_t TSplitNum>
	class CameraCascadedFrustum : public CameraCascadedFrustumBase
	{
	public:
		CameraCascadedFrustum(
			uint32_t id
			, TE_CAMERA_TYPE cameraType
			, const float frustumSplitCoveringPercentage[TSplitNum]
			, const float3& position
			, const float3& look
			, const float3& up
			, const float3& right);
		CameraCascadedFrustum() = default;
		virtual ~CameraCascadedFrustum() = default;


		//
		//Set Functions
		//
		void UpdateFrustums(Scene* scene, Camera* referenceCamera) override;

		//
		//Get Functions
		//
		const float4x4& GetViewProj(uint32_t splitIndex)const noexcept override
		{
			return m_ViewProjMatrix[splitIndex];
		}

		const float4x4& GetProjection(uint32_t splitIndex)const override
		{
			return m_ProjectionMatrix[splitIndex];
		}

		const BoundingFrustum& GetBoundingFrustum(uint32_t splitIndex)const override
		{
			return m_BoundingFrustums[splitIndex];
		}

		uint32_t GetSplitNum() const noexcept override
		{
			return GetStaticSplitNum();
		}

		constexpr uint32_t GetStaticSplitNum() const noexcept
		{
			return TSplitNum;
		}

	protected:

		void CreateBoundingFrustum();

	protected:
		static constexpr uint32_t m_SplitNum = TSplitNum;

		float4x4 m_ProjectionMatrix[m_SplitNum];
		float4x4 m_ViewProjMatrix[m_SplitNum];

		float m_SplitFrustumCoveringPercentage[TSplitNum];

		BoundingFrustum m_BoundingFrustums[TSplitNum];

		//
		// Friend Class
		//
		friend class CameraManager;
	};


	template<uint32_t TSplitNum>
	TruthEngine::CameraCascadedFrustum<TSplitNum>::CameraCascadedFrustum(uint32_t id
		, TE_CAMERA_TYPE cameraType
		, const float frustumSplitCoveringPercentage[TSplitNum]
		, const float3& position
		, const float3& look
		, const float3& up
		, const float3& right)
		: CameraCascadedFrustumBase(id, cameraType, position, look, up, right)
	{
		memcpy(m_SplitFrustumCoveringPercentage, frustumSplitCoveringPercentage, TSplitNum * sizeof(float));
	}

	template<uint32_t TSplitNum>
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

	}


	template<uint32_t TSplitNum>
	void TruthEngine::CameraCascadedFrustum<TSplitNum>::UpdateFrustums(Scene* scene, Camera* referenceCamera)
	{
		using namespace DirectX;

		BoundingBox _sceneAABBLightSpace;
		const auto& _sceneAABB = scene->GetBoundingBox();
		XMMatrix _xmView = XMLoadFloat4x4(&m_ViewMatrix);
		_sceneAABB.Transform(_sceneAABBLightSpace, _xmView);

		float rCameraViewRange = referenceCamera->GetZFarPlane() - referenceCamera->GetZNearPlane();

		auto _lightViewInv = XMMatrixInverse(nullptr, _xmView);
		auto _rCameraViewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&referenceCamera->GetView()));

		const BoundingFrustum& _rCameraBoundingFrustumViewSpace = referenceCamera->GetBoundingFrustumViewSpace();

		for (uint32_t splitIndex = 0; splitIndex < TSplitNum; ++splitIndex)
		{
			float _splitIntervalBegin = 1;
			float _splitIntervalEnd = m_SplitFrustumCoveringPercentage[splitIndex] * rCameraViewRange;

			/*XMVector _rCameraFrustumCorners_0[8];
			CreateFrustumPointsFromCascadeIntervalFromProjectionMatrix(_splitIntervalBegin, _splitIntervalEnd, referenceCamera->GetProjection(), _rCameraFrustumCorners_0);*/

			XMVector _rCameraFrustumCorners[8];
			CreateFrustumPointsFromCascadeInterval(_splitIntervalBegin, _splitIntervalEnd, _rCameraBoundingFrustumViewSpace, _rCameraFrustumCorners);

			XMVector _vFrustumPointMin = XMVectorFLTMax;
			XMVector _vFrustumPointMax = XMVectorFLTMin;

			for (uint32_t cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
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
			float fSplitBound = XMVectorGetX(vDiagonal);

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

			XMVector _sceneAABBCornersLightSpace[8];
			float3 _tempsceneAABBCorners[8];
			_sceneAABB.GetCorners(_tempsceneAABBCorners);

			XMVector _sceneAABBLightSpaceMin = XMVectorFLTMax;
			XMVector _sceneAABBLightSpaceMax = XMVectorFLTMin;

			for (uint32_t cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
			{
				_sceneAABBCornersLightSpace[cornerIndex] = XMVector3Transform(XMLoadFloat3(&_tempsceneAABBCorners[cornerIndex]), _xmView);
				_sceneAABBLightSpaceMin = XMVectorMin(_sceneAABBLightSpaceMin, _sceneAABBCornersLightSpace[cornerIndex]);
				_sceneAABBLightSpaceMax = XMVectorMax(_sceneAABBLightSpaceMax, _sceneAABBCornersLightSpace[cornerIndex]);
			}

			/*XMVector _sceneAABBCornersLightSpace[8];
			float3 _tempsceneAABBCornersViewSpace[8];
			_sceneAABBLightSpace.GetCorners(_tempsceneAABBCornersViewSpace);


			XMVector _sceneAABBLightSpaceMin = XMVectorFLTMax;
			XMVector _sceneAABBLightSpaceMax = XMVectorFLTMin;

			for (uint32_t cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
			{
				_sceneAABBCornersLightSpace[cornerIndex] = XMLoadFloat3(&_tempsceneAABBCornersViewSpace[cornerIndex]);
				_sceneAABBLightSpaceMin = XMVectorMin(_sceneAABBLightSpaceMin, _sceneAABBCornersLightSpace[cornerIndex]);
				_sceneAABBLightSpaceMax = XMVectorMax(_sceneAABBLightSpaceMax, _sceneAABBCornersLightSpace[cornerIndex]);
			}*/

			//we are using reverse depth so the near and far plane have reverse order
			float _farPlane = XMVectorGetZ(_sceneAABBLightSpaceMin);
			float _nearPlane = XMVectorGetZ(_sceneAABBLightSpaceMax);

			auto _splitProjectionMatrix = XMMatrixOrthographicOffCenterLH(XMVectorGetX(_vFrustumPointMin), XMVectorGetX(_vFrustumPointMax), XMVectorGetY(_vFrustumPointMin), XMVectorGetY(_vFrustumPointMax), _nearPlane, _farPlane);

			/*BoundingFrustum::CreateFromMatrix(m_BoundingFrustums[splitIndex], _splitProjectionMatrix);

			m_BoundingFrustums[splitIndex].Transform(m_BoundingFrustums[splitIndex], _lightViewInv);*/

			XMStoreFloat4x4(&m_ProjectionMatrix[splitIndex], _splitProjectionMatrix);

			XMStoreFloat4x4(&m_ViewProjMatrix[splitIndex], XMMatrixMultiply(XMLoadFloat4x4(&m_ViewMatrix), _splitProjectionMatrix));

		}

		CreateBoundingFrustum();

	}


}