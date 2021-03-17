#pragma once

using BoundingBox = DirectX::BoundingBox;
using BoundingFrustum = DirectX::BoundingFrustum;
using XMMatrix = DirectX::XMMATRIX;
using XMVector = DirectX::XMVECTOR;
#define TE_PIDIV4 DirectX::XM_PIDIV4
#define TE_PI  DirectX::XM_PI

constexpr DirectX::XMVECTORF32 XMVectorOne = {1.0f, 1.0f, 1.0f, 1.0f };
constexpr DirectX::XMVECTORF32 XMVectorZero = {.0f, .0f, .0f, .0f };
constexpr DirectX::XMVECTORF32 XMVectorFLTMin = { -1.0f * FLT_MAX, -1.0f * FLT_MAX, -1.0f * FLT_MAX, -1.0f * FLT_MAX };
constexpr DirectX::XMVECTORF32 XMVectorFLTMax = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
constexpr DirectX::XMVECTORF32 XMVectorHalf   = { .5f, .5f , .5f , .5f };

struct float2 : DirectX::XMFLOAT2
{
	using DirectX::XMFLOAT2::XMFLOAT2;

	float2(const ImVec2& imVec2)
	{
		x = imVec2.x;
		y = imVec2.y;
	}

	operator ImVec2();
	operator ImVec2()const;
};

struct float3 : DirectX::XMFLOAT3
{
	using  DirectX::XMFLOAT3::XMFLOAT3;
	float3(const DirectX::XMFLOAT3&);

	operator physx::PxVec3();
	operator physx::PxVec3()const;
};

struct float4 : DirectX::XMFLOAT4
{
	using DirectX::XMFLOAT4::XMFLOAT4;

	operator physx::PxQuat();
	operator physx::PxVec4();
	operator physx::PxVec3();
};

struct float3x3 : DirectX::XMFLOAT3X3
{
	using DirectX::XMFLOAT3X3::XMFLOAT3X3;
};

struct float4x4 : DirectX::XMFLOAT4X4
{
	using DirectX::XMFLOAT4X4::XMFLOAT4X4;

	float4x4(const aiMatrix4x4& _aiMatrix4x4);

	operator physx::PxMat44();
	operator physx::PxMat44()const;
};


struct uint2
{
	union
	{
		struct
		{
			uint32_t x;
			uint32_t y;
		};		
	};

	uint32_t& operator[](uint32_t i)
	{
		return (&x)[i];
	}
};

struct uint3
{
	union
	{
		struct
		{
			uint32_t x;
			uint32_t y;
			uint32_t z;
		};
	};

	uint32_t& operator[](uint32_t i)
	{
		return (&x)[i];
	}
};

struct uint4
{
	union
	{
		struct
		{
			uint32_t x;
			uint32_t y;
			uint32_t z;
			uint32_t w;
		};
	};

	uint32_t& operator[](uint32_t i)
	{
		return (&x)[i];
	}
};


namespace TruthEngine
{
	constexpr float4x4 IdentityMatrix = float4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	namespace Math
	{
		inline DirectX::XMMATRIX ToXM(const DirectX::XMFLOAT4X4& source)
		{
			return DirectX::XMLoadFloat4x4(&source);
		}

		inline DirectX::XMMATRIX ToXM(const DirectX::XMFLOAT3X3& source)
		{
			return DirectX::XMLoadFloat3x3(&source);
		}

		inline DirectX::XMVECTOR ToXM(const DirectX::XMFLOAT4& source)
		{
			return DirectX::XMLoadFloat4(&source);
		}

		inline DirectX::XMVECTOR ToXM(const DirectX::XMFLOAT3& source)
		{
			return DirectX::XMLoadFloat3(&source);
		}

		inline DirectX::XMVECTOR ToXM(const DirectX::XMFLOAT2& source)
		{
			return DirectX::XMLoadFloat2(&source);
		}

		inline DirectX::XMVECTOR ToXM(const float source)
		{
			return DirectX::XMLoadFloat(&source);
		}

		inline DirectX::XMFLOAT4X4 FromXM(const DirectX::FXMMATRIX& source)
		{
			DirectX::XMFLOAT4X4 dest;
			DirectX::XMStoreFloat4x4(&dest, source);
			return dest;
		}

		inline DirectX::XMFLOAT4 FromXM(const DirectX::FXMVECTOR& source)
		{
			DirectX::XMFLOAT4 dest;
			DirectX::XMStoreFloat4(&dest, source);
			return dest;
		}

		inline DirectX::XMFLOAT3 FromXM3(const DirectX::FXMVECTOR& source)
		{
			DirectX::XMFLOAT3 dest;
			DirectX::XMStoreFloat3(&dest, source);
			return dest;
		}

		inline DirectX::XMFLOAT2 FromXM2(const DirectX::FXMVECTOR& source)
		{
			DirectX::XMFLOAT2 dest;
			DirectX::XMStoreFloat2(&dest, source);
			return dest;
		}

		inline float FromXM1(const DirectX::FXMVECTOR& source)
		{
			float dest;
			DirectX::XMStoreFloat(&dest, source);
			return dest;
		}

		inline DirectX::XMMATRIX XMInverse(const DirectX::XMFLOAT4X4& source)
		{
			auto matrix = ToXM(source);

			return DirectX::XMMatrixInverse(nullptr, matrix);
		}

		inline DirectX::XMFLOAT4X4 Inverse(const DirectX::XMFLOAT4X4& source)
		{
			DirectX::XMFLOAT4X4 dest;

			auto matrix = ToXM(source);

			matrix = DirectX::XMMatrixInverse(nullptr, matrix);

			DirectX::XMStoreFloat4x4(&dest, matrix);

			return dest;
		}

		inline DirectX::XMFLOAT4X4 Transpose(const DirectX::XMFLOAT4X4& source)
		{
			DirectX::XMFLOAT4X4 dest;

			auto matrix = ToXM(source);

			matrix = DirectX::XMMatrixTranspose(matrix);

			DirectX::XMStoreFloat4x4(&dest, matrix);

			return dest;
		}

		inline float4x4 InverseTranspose(const float4x4& source)
		{
			float4x4 dest;

			auto matrix = ToXM(source);

			matrix.r[3] = DirectX::XMVectorSet(.0f, .0f, .0f, 1.0f);

			auto determinant = DirectX::XMMatrixDeterminant(matrix);

			DirectX::XMStoreFloat4x4(&dest, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&determinant, matrix)));

			return dest;
		}

		inline void NormalizeEst(float2& source)
		{
			DirectX::XMStoreFloat2(&source, DirectX::XMVector2NormalizeEst(XMLoadFloat2(&source)));
		}

		inline void Normalize(float2& source)
		{
			DirectX::XMStoreFloat2(&source, DirectX::XMVector2Normalize(XMLoadFloat2(&source)));
		}

		inline void NormalizeEst(float3& source)
		{
			DirectX::XMStoreFloat3(&source, DirectX::XMVector3NormalizeEst(XMLoadFloat3(&source)));
		}

		inline void Normalize(float3& source)
		{
			DirectX::XMStoreFloat3(&source, DirectX::XMVector3Normalize(XMLoadFloat3(&source)));
		}

		inline void NormalizeEst(float4& source)
		{
			DirectX::XMStoreFloat4(&source, DirectX::XMVector4NormalizeEst(XMLoadFloat4(&source)));
		}

		inline void Normalize(float4& source)
		{
			DirectX::XMStoreFloat4(&source, DirectX::XMVector4Normalize(XMLoadFloat4(&source)));
		}

		void DecomposeMatrix(const float4x4& inMatrix, float4& scale, float4& translate, float4& quaternion);

		inline float3 TransformPoint(const float3& point, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformCoord(XMLoadFloat3(&point), XMLoadFloat4x4(&transform)));
			return result;
		}

		inline float3 TransformInversePoint(const float3& point, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformCoord(XMLoadFloat3(&point), XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform))));
			return result;
		}

		inline float4 TransformPoint(const float4& point, const float4x4& transform)
		{
			float4 result;
			XMStoreFloat4(&result, XMVector4Transform(XMLoadFloat4(&point), XMLoadFloat4x4(&transform)));
			return result;
		}

		inline float3 TransformVector(const float3& vector, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformNormal(XMLoadFloat3(&vector), XMLoadFloat4x4(&transform)));
			return result;
		}

		inline float3 TransformInverseVector(const float3& vector, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformNormal(XMLoadFloat3(&vector), XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform))));
			return result;
		}

		inline BoundingBox TransformBoundingBox(const BoundingBox& _BoundingBox, const float4x4& _Transform)
		{
			BoundingBox _aabb;
			_BoundingBox.Transform(_aabb, XMLoadFloat4x4(&_Transform));
			return _aabb;
		}

		inline float4x4 TransformMatrix(const float3& _Translate, const float3& _Scale, const float4& _Quaternion)
		{
			float4x4 _Result;
			XMMatrix _Matrix = DirectX::XMMatrixAffineTransformation(XMLoadFloat3(&_Scale), XMVectorZero, XMLoadFloat4(&_Quaternion), XMLoadFloat3(&_Translate));
			DirectX::XMStoreFloat4x4(&_Result, _Matrix);
			return _Result;
		}

		inline float4x4 TransformMatrixTranslate(const float3& _Translate)
		{
			float4x4 _Result;
			XMMatrix _Matrix = DirectX::XMMatrixTranslation(_Translate.x, _Translate.y, _Translate.z);
			DirectX::XMStoreFloat4x4(&_Result, _Matrix);
			return _Result;
		}

		inline float4x4 TransformMatrixScale(const float3& _Scale)
		{
			float4x4 _Result;
			XMMatrix _Matrix = DirectX::XMMatrixScaling(_Scale.x, _Scale.y, _Scale.z);
			DirectX::XMStoreFloat4x4(&_Result, _Matrix);
			return _Result;
		}

		inline float4x4 TransformMatrixRotation(const float4& _RotateQuaternion)
		{
			float4x4 _Result;
			XMMatrix _Matrix = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&_RotateQuaternion));
			DirectX::XMStoreFloat4x4(&_Result, _Matrix);
			return _Result;
		}

		inline void Translate(float4x4& _OutTransform, const float3& _Translation)
		{
			_OutTransform._14 += _Translation.x;
			_OutTransform._24 += _Translation.y;
			_OutTransform._34 += _Translation.z;
		}

		inline void Scale(float4x4& _OutTransform, const float3& _Scaling)
		{
			_OutTransform._11 *= _Scaling.x;
			_OutTransform._22 *= _Scaling.y;
			_OutTransform._33 *= _Scaling.z;
		}

		inline void Rotate(float4x4& _OutTransform, const float4& _RotationQuaternion)
		{
			XMMatrix _Matrix = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&_RotationQuaternion));
			XMMatrix _Result =  DirectX::XMMatrixMultiply(XMLoadFloat4x4(&_OutTransform), _Matrix);
			XMStoreFloat4x4(&_OutTransform, _Result);
		}


	}

	inline float4 operator+(const float4& v1, const float4& v2)
	{
		return float4{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
	}

	inline float3 operator+(const float3& v1, const float3& v2)
	{
		return float3{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}

	inline float4x4 operator*(const float4x4& m1, const float4x4& m2)
	{
		float4x4 r;

		const auto xm1 = DirectX::XMLoadFloat4x4(&m1);
		const auto xm2 = DirectX::XMLoadFloat4x4(&m2);

		auto xm3 = DirectX::XMMatrixMultiply(xm1, xm2);

		DirectX::XMStoreFloat4x4(&r, xm3);
		return r;
	}

	inline void CreateBoundingBoxFromPoints(BoundingBox& outBoundingBox, const size_t vertexNum, const float3* vertecies, size_t strideSize)
	{
		DirectX::BoundingBox::CreateFromPoints(outBoundingBox, vertexNum, vertecies, strideSize);
	}

	inline void CreateBoundingBoxMerged(BoundingBox& outBoundingBox, const BoundingBox& boundingBox1, const BoundingBox& boundingBox2)
	{
		DirectX::BoundingBox::CreateMerged(outBoundingBox, boundingBox1, boundingBox2);
	}

}