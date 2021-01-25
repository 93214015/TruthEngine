#pragma once

using BoundingBox = DirectX::BoundingBox;
#define TE_PIDIV4 DirectX::XM_PIDIV4
#define TE_PI  DirectX::XM_PI


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

	operator physx::PxVec3();
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

	operator physx::PxMat44();
	operator physx::PxMat44()const;
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
		inline DirectX::XMMATRIX ToXM(const DirectX::XMFLOAT4X4 & source)
		{
			return DirectX::XMLoadFloat4x4(&source);
		}

		inline DirectX::XMMATRIX ToXM(const DirectX::XMFLOAT3X3 & source)
		{
			return DirectX::XMLoadFloat3x3(&source);
		}

		inline DirectX::XMVECTOR ToXM(const DirectX::XMFLOAT4 & source)
		{
			return DirectX::XMLoadFloat4(&source);
		}

		inline DirectX::XMVECTOR ToXM(const DirectX::XMFLOAT3 & source)
		{
			return DirectX::XMLoadFloat3(&source);
		}

		inline DirectX::XMVECTOR ToXM(const DirectX::XMFLOAT2 & source)
		{
			return DirectX::XMLoadFloat2(&source);
		}

		inline DirectX::XMVECTOR ToXM(const float source)
		{
			return DirectX::XMLoadFloat(&source);
		}

		inline DirectX::XMFLOAT4X4 FromXM(const DirectX::FXMMATRIX & source)
		{
			DirectX::XMFLOAT4X4 dest;
			DirectX::XMStoreFloat4x4(&dest, source);
			return dest;
		}

		inline DirectX::XMFLOAT4 FromXM(const DirectX::FXMVECTOR & source)
		{
			DirectX::XMFLOAT4 dest;
			DirectX::XMStoreFloat4(&dest, source);
			return dest;
		}

		inline DirectX::XMFLOAT3 FromXM3(const DirectX::FXMVECTOR & source)
		{
			DirectX::XMFLOAT3 dest;
			DirectX::XMStoreFloat3(&dest, source);
			return dest;
		}

		inline DirectX::XMFLOAT2 FromXM2(const DirectX::FXMVECTOR & source)
		{
			DirectX::XMFLOAT2 dest;
			DirectX::XMStoreFloat2(&dest, source);
			return dest;
		}

		inline float FromXM1(const DirectX::FXMVECTOR & source)
		{
			float dest;
			DirectX::XMStoreFloat(&dest, source);
			return dest;
		}

		inline DirectX::XMMATRIX XMInverse(const DirectX::XMFLOAT4X4 & source)
		{
			auto matrix = ToXM(source);

			return DirectX::XMMatrixInverse(nullptr, matrix);
		}

		inline DirectX::XMFLOAT4X4 Inverse(const DirectX::XMFLOAT4X4 & source)
		{
			DirectX::XMFLOAT4X4 dest;

			auto matrix = ToXM(source);

			matrix = DirectX::XMMatrixInverse(nullptr, matrix);

			DirectX::XMStoreFloat4x4(&dest, matrix);

			return dest;
		}

		inline DirectX::XMFLOAT4X4 Transpose(const DirectX::XMFLOAT4X4 & source)
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
		const auto xm1 = DirectX::XMLoadFloat4x4(&m1);
		const auto xm2 = DirectX::XMLoadFloat4x4(&m2);

		auto xm3 = xm1 * xm2;

		float4x4 r;
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