#pragma once


using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;

namespace TruthEngine
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
}