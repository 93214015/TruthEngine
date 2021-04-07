#include "pch.h"
#include "CameraCascadedFrustum.h"

using namespace DirectX;

void TruthEngine::CreateFrustumPointsFromCascadeInterval(float frustumIntervalBegin, float frustumIntervalEnd, const BoundingFrustum& rCameraBoundingFrustum, XMVector outCornerPointsView[8])
{
	BoundingFrustum _frustumAABB(rCameraBoundingFrustum);
	_frustumAABB.Near = frustumIntervalBegin;
	_frustumAABB.Far = frustumIntervalEnd;

	static const XMVECTORU32 vGrabY = { 0x00000000,0xFFFFFFFF,0x00000000,0x00000000 };
	static const XMVECTORU32 vGrabX = { 0xFFFFFFFF,0x00000000,0x00000000,0x00000000 };

	XMVECTORF32 _vRightTop = { _frustumAABB.RightSlope, _frustumAABB.TopSlope, 1.0f, 1.0f };

	XMVECTORF32 _vNear = { _frustumAABB.Near, _frustumAABB.Near , _frustumAABB.Near , 1.0f };
	XMVECTORF32 _vFar = { _frustumAABB.Far, _frustumAABB.Far , _frustumAABB.Far , 1.0f };

	XMVector _vRightTopNear = XMVectorMultiply(_vRightTop, _vNear);
	XMVector _vRightTopFar = XMVectorMultiply(_vRightTop, _vFar);


	XMVECTORF32 _vLeftBottom = { _frustumAABB.LeftSlope, _frustumAABB.BottomSlope, 1.0f, 1.0f };

	XMVector _vLeftBottomNear = XMVectorMultiply(_vLeftBottom, _vNear);
	XMVector _vLeftBottomFar = XMVectorMultiply(_vLeftBottom, _vFar);

	outCornerPointsView[0] = _vRightTopNear;
	outCornerPointsView[1] = XMVectorSelect(_vRightTopNear, _vLeftBottomNear, vGrabX);
	outCornerPointsView[2] = _vLeftBottomNear;
	outCornerPointsView[3] = XMVectorSelect(_vRightTopNear, _vLeftBottomNear, vGrabY);

	outCornerPointsView[4] = _vRightTopFar;
	outCornerPointsView[5] = XMVectorSelect(_vRightTopFar, _vLeftBottomFar, vGrabX);
	outCornerPointsView[6] = _vLeftBottomFar;
	outCornerPointsView[7] = XMVectorSelect(_vRightTopFar, _vLeftBottomFar, vGrabY);
}

void TruthEngine::CreateFrustumPointsFromCascadeIntervalFromProjectionMatrix(float frustumIntervalBegin, float frustumIntervalEnd, const float4x4& projectionMatrix, XMVector outCornerPointsView[8])
{
	XMMatrix _projection = XMLoadFloat4x4(&projectionMatrix);
	XMVector _clipSpaceZNear = XMVector4Transform(XMVectorSet(0.0, .0f, frustumIntervalBegin, 1.0f), _projection);
	XMVector _clipSpaceZFar = XMVector4Transform(XMVectorSet(.0f, .0f, frustumIntervalEnd, 1.0f), _projection);
	_clipSpaceZNear = XMVectorScale(_clipSpaceZNear, 1.0f/XMVectorGetW(_clipSpaceZNear));
	_clipSpaceZFar = XMVectorScale(_clipSpaceZFar, 1.0f/XMVectorGetW(_clipSpaceZFar));

	outCornerPointsView[0] = XMVectorSet(1.0f, 1.0f, XMVectorGetZ(_clipSpaceZNear), 1.0f);
	outCornerPointsView[1] = XMVectorSet(-1.0f, 1.0f, XMVectorGetZ(_clipSpaceZNear), 1.0f);
	outCornerPointsView[2] = XMVectorSet(-1.0f, -1.0f, XMVectorGetZ(_clipSpaceZNear), 1.0f);
	outCornerPointsView[3] = XMVectorSet(1.0f, -1.0f, XMVectorGetZ(_clipSpaceZNear), 1.0f);

	outCornerPointsView[4] = XMVectorSet(1.0f, 1.0f, XMVectorGetZ(_clipSpaceZFar), 1.0f);
	outCornerPointsView[5] = XMVectorSet(-1.0f, 1.0f, XMVectorGetZ(_clipSpaceZFar), 1.0f);
	outCornerPointsView[6] = XMVectorSet(-1.0f, -1.0f, XMVectorGetZ(_clipSpaceZFar), 1.0f);
	outCornerPointsView[7] = XMVectorSet(1.0f, -1.0f, XMVectorGetZ(_clipSpaceZFar), 1.0f);

	XMMatrix _projectionInverse = XMMatrixInverse(nullptr, _projection);
	outCornerPointsView[0] = XMVector4Transform(outCornerPointsView[0], _projectionInverse);
	outCornerPointsView[0] = XMVectorDivide(outCornerPointsView[0], XMVectorReplicate(XMVectorGetW(outCornerPointsView[0])));
	outCornerPointsView[1] = XMVector4Transform(outCornerPointsView[1], _projectionInverse);
	outCornerPointsView[1] = XMVectorDivide(outCornerPointsView[1], XMVectorReplicate(XMVectorGetW(outCornerPointsView[1])));
	outCornerPointsView[2] = XMVector4Transform(outCornerPointsView[2], _projectionInverse);
	outCornerPointsView[2] = XMVectorDivide(outCornerPointsView[2], XMVectorReplicate(XMVectorGetW(outCornerPointsView[2])));
	outCornerPointsView[3] = XMVector4Transform(outCornerPointsView[3], _projectionInverse);
	outCornerPointsView[3] = XMVectorDivide(outCornerPointsView[3], XMVectorReplicate(XMVectorGetW(outCornerPointsView[3])));

	outCornerPointsView[4] = XMVector4Transform(outCornerPointsView[4], _projectionInverse);
	outCornerPointsView[4] = XMVectorDivide(outCornerPointsView[4], XMVectorReplicate(XMVectorGetW(outCornerPointsView[4])));
	outCornerPointsView[5] = XMVector4Transform(outCornerPointsView[5], _projectionInverse);
	outCornerPointsView[5] = XMVectorDivide(outCornerPointsView[5], XMVectorReplicate(XMVectorGetW(outCornerPointsView[5])));
	outCornerPointsView[6] = XMVector4Transform(outCornerPointsView[6], _projectionInverse);
	outCornerPointsView[6] = XMVectorDivide(outCornerPointsView[6], XMVectorReplicate(XMVectorGetW(outCornerPointsView[6])));
	outCornerPointsView[7] = XMVector4Transform(outCornerPointsView[7], _projectionInverse);
	outCornerPointsView[7] = XMVectorDivide(outCornerPointsView[7], XMVectorReplicate(XMVectorGetW(outCornerPointsView[7])));
}


TruthEngine::CameraCascadedFrustumBase::CameraCascadedFrustumBase(uint32_t id, TE_CAMERA_TYPE cameraType, const float3& position, const float3& look, const float3& up, const float3& right)
	: m_ID(id), m_CameraType(cameraType), m_Position(position), m_Look(look), m_Up(up), m_Right(right)
{
	UpdateViewMatrix();
}


void TruthEngine::CameraCascadedFrustumBase::UpdateViewMatrix()
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

	m_ViewMatrix(0, 0) = m_Right.x;
	m_ViewMatrix(1, 0) = m_Right.y;
	m_ViewMatrix(2, 0) = m_Right.z;
	m_ViewMatrix(3, 0) = x;

	m_ViewMatrix(0, 1) = m_Up.x;
	m_ViewMatrix(1, 1) = m_Up.y;
	m_ViewMatrix(2, 1) = m_Up.z;
	m_ViewMatrix(3, 1) = y;

	m_ViewMatrix(0, 2) = m_Look.x;
	m_ViewMatrix(1, 2) = m_Look.y;
	m_ViewMatrix(2, 2) = m_Look.z;
	m_ViewMatrix(3, 2) = z;

	m_ViewMatrix(0, 3) = 0.0f;
	m_ViewMatrix(1, 3) = 0.0f;
	m_ViewMatrix(2, 3) = 0.0f;
	m_ViewMatrix(3, 3) = 1.0f;
}
