#include "pch.h"
#include "ICamera.h"

using namespace DirectX;


namespace TruthEngine::Core {


	ICamera::ICamera()
		: m_Position(0.0f, 0.0f, 0.0f),
		m_Right(1.0f, 0.0f, 0.0f),
		m_Up(0.0f, 1.0f, 0.0f),
		m_Look(0.0f, 0.0f, 1.0f),
		m_Speed(350.0)
	{
		
	}


	XMVECTOR ICamera::GetPositionXM()const
	{
		return XMLoadFloat3(&m_Position);
	}


	XMFLOAT3 ICamera::GetPosition()const
	{
		return m_Position;
	}


	void ICamera::SetPosition(float x, float y, float z)
	{
		m_Position = XMFLOAT3(x, y, z);
	}


	void ICamera::SetPosition(const XMFLOAT3& v)
	{
		m_Position = v;
	}


	XMVECTOR ICamera::GetRightXM()const
	{
		return XMLoadFloat3(&m_Right);
	}


	XMFLOAT3 ICamera::GetRight()const
	{
		return m_Right;
	}


	XMVECTOR ICamera::GetUpXM()const
	{
		return XMLoadFloat3(&m_Up);
	}


	XMFLOAT3 ICamera::GetUp()const
	{
		return m_Up;
	}


	XMVECTOR ICamera::GetLookXM()const
	{
		return XMLoadFloat3(&m_Look);
	}


	XMFLOAT3 ICamera::GetLook()const
	{
		return m_Look;
	}


	float ICamera::GetNearZ()const
	{
		return m_NearZ;
	}


	float ICamera::GetFarZ()const
	{
		return m_FarZ;
	}


	void XM_CALLCONV ICamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
	{
		XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
		XMVECTOR U = XMVector3Cross(L, R);

		XMStoreFloat3(&m_Position, pos);
		XMStoreFloat3(&m_Look, L);
		XMStoreFloat3(&m_Right, R);
		XMStoreFloat3(&m_Up, U);

		UpdateViewMatrix();
	}


	void ICamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
	{
		XMVECTOR P = XMLoadFloat3(&pos);
		XMVECTOR T = XMLoadFloat3(&target);
		XMVECTOR U = XMLoadFloat3(&up);

		LookAt(P, T, U);
	}


	XMMATRIX ICamera::GetViewXM()const
	{
		return XMLoadFloat4x4(&m_View);
	}


	XMMATRIX ICamera::GetViewInvXM()const
	{
		return XMMatrixInverse(nullptr, GetViewXM());
	}


	XMMATRIX ICamera::GetViewProjInvXM() const
	{
		//return XMMatrixInverse(nullptr, Proj()) * XMMatrixInverse(nullptr, View());
		return XMMatrixInverse(nullptr, XMMatrixMultiply(GetViewXM(), GetProjXM()));
	}


	XMMATRIX ICamera::GetProjXM()const
	{
		return XMLoadFloat4x4(&m_Proj);
	}


	XMMATRIX ICamera::GetProjInvXM() const
	{
		return XMMatrixInverse(nullptr, GetProjXM());
	}


	XMMATRIX ICamera::GetViewProjXM()const
	{
		return XMMatrixMultiply(GetViewXM(), GetProjXM());
	}


	XMFLOAT4X4 ICamera::GetView() const noexcept
	{
		return m_View;
	}


	XMFLOAT4X4 ICamera::GetViewInv() const noexcept
	{
		const auto viewInv = XMMatrixInverse(nullptr, GetViewXM());
		XMFLOAT4X4 r;
		XMStoreFloat4x4(&r, viewInv);
		return r;
	}


	XMFLOAT4X4 ICamera::GetProj() const noexcept
	{
		return m_Proj;
	}


	DirectX::XMFLOAT4X4 ICamera::GetProjInv() const noexcept
	{
		const auto viewInv = XMMatrixInverse(nullptr, GetProjXM());
		XMFLOAT4X4 r;
		XMStoreFloat4x4(&r, viewInv);
		return r;
	}


	XMFLOAT4X4 ICamera::GetViewProj() const noexcept
	{
		XMFLOAT4X4 vp;
		XMStoreFloat4x4(&vp, XMMatrixMultiply(GetViewXM(), GetProjXM()));
		return vp;
	}


	DirectX::XMFLOAT4X4 ICamera::GetViewProjInv() const
	{
		XMFLOAT4X4 vpInv;
		XMStoreFloat4x4(&vpInv, XMMatrixInverse(nullptr, XMMatrixMultiply(GetViewXM(), GetProjXM())));
		return vpInv;
	}


	XMFLOAT4 ICamera::GetPerspectiveValues()const noexcept
	{
		return 	XMFLOAT4((1 / m_Proj._11), (1 / m_Proj._22), m_Proj._33, m_Proj._43);
	}


	void ICamera::Strafe(float d)
	{
		d *= m_Speed;
		// mPosition += d*mRight
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR r = XMLoadFloat3(&m_Right);
		XMVECTOR p = XMLoadFloat3(&m_Position);
		XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));
	}


	void ICamera::Walk(float d)
	{
		d *= m_Speed;
		// mPosition += d*mLook
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR l = XMLoadFloat3(&m_Look);
		XMVECTOR p = XMLoadFloat3(&m_Position);
		XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
	}


	void ICamera::Pitch(float angle)
	{
		// Rotate up and look vector about the right vector.

		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

		XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
	}


	void ICamera::RotateY(float angle)
	{
		// Rotate the basis vectors about the world y-axis.

		XMMATRIX R = XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
		XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
	}


	void ICamera::RotateCamera(float d)
	{
		auto angle = XMConvertToRadians(20 * d);
		auto R = XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Position, XMVector3Transform(XMLoadFloat3(&m_Position), R));

		RotateY(angle);

		//mUpdated = true;
	}


	void ICamera::UpdateViewMatrix()
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

		m_View(0, 0) = m_Right.x;
		m_View(1, 0) = m_Right.y;
		m_View(2, 0) = m_Right.z;
		m_View(3, 0) = x;

		m_View(0, 1) = m_Up.x;
		m_View(1, 1) = m_Up.y;
		m_View(2, 1) = m_Up.z;
		m_View(3, 1) = y;

		m_View(0, 2) = m_Look.x;
		m_View(1, 2) = m_Look.y;
		m_View(2, 2) = m_Look.z;
		m_View(3, 2) = z;

		m_View(0, 3) = 0.0f;
		m_View(1, 3) = 0.0f;
		m_View(2, 3) = 0.0f;
		m_View(3, 3) = 1.0f;

		CreateBoundingFrustum();
	}


	void ICamera::CreateBoundingFrustum()
	{
		BoundingFrustum::CreateFromMatrix(m_BoundingFrustum, GetProjXM());
		const auto InvView = XMMatrixInverse(nullptr, GetViewXM());
		m_BoundingFrustum.Transform(m_BoundingFrustum, InvView);
	}


	const BoundingFrustum& ICamera::GetBoundingFrustum()const
	{
		return m_BoundingFrustum;
	}


	ContainmentType ICamera::BoundingBoxContainment(const BoundingBox& _boundingBox) const
	{
		return m_BoundingFrustum.Contains(_boundingBox);
	}


	std::vector<std::vector<XMFLOAT4>> ICamera::SplitFrustum() const
	{
		const std::vector<float> viewDistance{ m_NearZ, m_NearZ + 50, m_NearZ + 150, m_NearZ + 500, m_FarZ };

		std::vector<XMFLOAT4> projSplitPlanes(4, XMFLOAT4(0.0, 0.0, 0.0, 1.0f));
		std::vector<std::vector<XMFLOAT4>> worldSplitPlanes(viewDistance.size(), std::vector<XMFLOAT4>(4, XMFLOAT4(0.0, 0.0, 0.0, 1.0f)));

		projSplitPlanes[0].x = -1.0f;
		projSplitPlanes[0].y = 1.0f;

		projSplitPlanes[1].x = 1;
		projSplitPlanes[1].y = 1;

		projSplitPlanes[2].x = 1;
		projSplitPlanes[2].y = -1;

		projSplitPlanes[3].x = -1;
		projSplitPlanes[3].y = -1;

		int i = 0;

		for (const auto d : viewDistance)
		{
			worldSplitPlanes[i].resize(4, XMFLOAT4(0.0, 0.0, 0.0, 1.0f));


			const auto p = XMFLOAT4(0, 0, viewDistance[i], 1);
			const auto v = XMVector4Transform(XMLoadFloat4(&p), GetProjXM());
			XMFLOAT4 v1;
			XMStoreFloat4(&v1, v);
			v1.z /= v1.w;



			for (int j = 0; j < 4; ++j)
			{

				projSplitPlanes[j].z = v1.z;

				//projSplitPlanes[j].z *= viewDistance[i];


				XMVECTOR worldpoint = XMLoadFloat4(&projSplitPlanes[j]);
				worldpoint = XMVector4Transform(XMLoadFloat4(&projSplitPlanes[j]), GetViewProjInvXM());
				worldpoint = XMVectorScale(worldpoint, 1 / XMVectorGetW(worldpoint));

				XMStoreFloat4(&worldSplitPlanes[i][j], worldpoint);

			}

			i++;
		}

		return worldSplitPlanes;
	}

}