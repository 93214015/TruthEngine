#include "pch.h"
#include "MathHelper.h"

using namespace DirectX;

namespace TruthEngine::Math 
{
	void DecomposeMatrix(const float4x4& inMatrix, float4& scale, float4& translate, float4& quaternion)
	{
		auto xmMatrix = XMLoadFloat4x4(&inMatrix);

		XMVECTOR _scale, _translate, _quaternion;

		auto r = XMMatrixDecompose(&_scale, &_quaternion, &_translate, xmMatrix);

		TE_ASSERT_CORE( r, "MathHelper: Decomposing Float4x4 Failed!");

		XMStoreFloat4(&scale, _scale);
		XMStoreFloat4(&translate, _translate);
		XMStoreFloat4(&quaternion, _quaternion);
	}
}

float4x4::operator physx::PxMat44()
{
	return physx::PxMat44({ _11, _12, _13, _14 }, { _21, _22, _23, _24 }, { _31, _32, _33, _34 }, { _41, _42, _43, _44 });
}

float4x4::operator physx::PxMat44() const
{
	return physx::PxMat44({ _11, _12, _13, _14 }, { _21, _22, _23, _24 }, { _31, _32, _33, _34 }, { _41, _42, _43, _44 });
}

float4::operator physx::PxQuat()
{
	return physx::PxQuat(x, y, z, w);
}

float4::operator physx::PxVec4()
{
	return physx::PxVec4(x, y, z, w);
}

float3::operator physx::PxVec3()
{
	return physx::PxVec3(x, y, z);
}

float3::operator physx::PxVec3() const
{
	return physx::PxVec3(x, y, z);
}

float3::float3(const DirectX::XMFLOAT3& xmfloat3)
	: XMFLOAT3(xmfloat3)
{
}

float4::operator physx::PxVec3()
{
	return physx::PxVec3(x, y, z);
}

float2::operator ImVec2()
{
	return ImVec2{ x, y };
}

float2::operator ImVec2() const
{
	return ImVec2{ x, y };
}
