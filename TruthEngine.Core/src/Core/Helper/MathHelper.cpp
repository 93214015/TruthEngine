#include "pch.h"
#include "MathHelper.h"

using namespace DirectX;

namespace TruthEngine 
{
	float4x4::float4x4(const aiMatrix4x4& _aiMatrix4x4)
	{
		_11 = _aiMatrix4x4.a1; _12 = _aiMatrix4x4.a2; _13 = _aiMatrix4x4.a3; _14 = _aiMatrix4x4.a4;
		_21 = _aiMatrix4x4.b1; _22 = _aiMatrix4x4.b2; _23 = _aiMatrix4x4.b3; _24 = _aiMatrix4x4.b4;
		_31 = _aiMatrix4x4.c1; _32 = _aiMatrix4x4.c2; _33 = _aiMatrix4x4.c3; _34 = _aiMatrix4x4.c4;
		_41 = _aiMatrix4x4.d1; _42 = _aiMatrix4x4.d2; _43 = _aiMatrix4x4.d3; _44 = _aiMatrix4x4.d4;
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

	namespace Math
	{
		void DecomposeMatrix(const float4x4A& inMatrix, float4& scale, float4& translate, float4& quaternion)
		{
			auto xmMatrix = XMLoadFloat4x4A(&inMatrix);

			XMVECTOR _scale, _translate, _quaternion;

			auto r = XMMatrixDecompose(&_scale, &_quaternion, &_translate, xmMatrix);

			TE_ASSERT_CORE(r, "MathHelper: Decomposing Float4x4 Failed!");

			XMStoreFloat4(&scale, _scale);
			XMStoreFloat4(&translate, _translate);
			XMStoreFloat4(&quaternion, _quaternion);
		}

		float3A Rotate(const float3A& _Vector, const float4A& _RotationQuaternion)
		{
			float3A _Result;
			auto _ResultVector = DirectX::XMVector3Rotate(ToXM(_Vector), ToXM(_RotationQuaternion));
			_Result = FromXM3A(_ResultVector);

			return _Result;
		}

	}
	
	float3A::float3A(const float3& _f3)
		: XMFLOAT3A(_f3.x, _f3.y, _f3.z)
	{}

}


