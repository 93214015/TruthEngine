#pragma once



namespace TruthEngine
{

	using BoundingAABox = DirectX::BoundingBox;
	using BoundingOrientedBox = DirectX::BoundingOrientedBox;
	using BoundingFrustum = DirectX::BoundingFrustum;
	using XMMatrix = DirectX::XMMATRIX;
	using XMVector = DirectX::XMVECTOR;
	using ContainmentType = DirectX::ContainmentType;



	constexpr DirectX::XMVECTORF32 XMVectorOne = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr DirectX::XMVECTORF32 XMVectorZero = { .0f, .0f, .0f, .0f };
	constexpr DirectX::XMVECTORF32 XMVectorOrigin = { .0f, .0f, .0f, 1.0f };
	constexpr DirectX::XMVECTORF32 XMVectorFLTMin = { -1.0f * FLT_MAX, -1.0f * FLT_MAX, -1.0f * FLT_MAX, -1.0f * FLT_MAX };
	constexpr DirectX::XMVECTORF32 XMVectorFLTMax = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
	constexpr DirectX::XMVECTORF32 XMVectorHalf = { .5f, .5f , .5f , .5f };
	constexpr DirectX::XMVECTORF32 XMVectorUp = { 0.0f, 1.0f , 0.0f , 0.0f };
	constexpr DirectX::XMVECTORF32 XMVectorForward = { 0.0f, 0.0f , 1.0f , 0.0f };
	constexpr DirectX::XMVECTORF32 XMVectorRight = { 1.0f, 0.0f , 0.0f , 0.0f };

	struct float2 : DirectX::XMFLOAT2
	{
		using DirectX::XMFLOAT2::XMFLOAT2;

		inline explicit operator XMVector() const
		{
			return DirectX::XMLoadFloat2(this);
		}

		float2(const ImVec2& imVec2)
		{
			x = imVec2.x;
			y = imVec2.y;
		}

		operator ImVec2()const
		{
			return ImVec2{ x, y };
		}

	};

	struct float2A : DirectX::XMFLOAT2A
	{
		using DirectX::XMFLOAT2A::XMFLOAT2A;

		inline explicit operator XMVector() const
		{
			return DirectX::XMLoadFloat2A(this);
		}

		float2A(const ImVec2& imVec2)
		{
			x = imVec2.x;
			y = imVec2.y;
		}

		operator ImVec2() const
		{
			return ImVec2{ x, y };
		}
	};

	struct float3A : DirectX::XMFLOAT3A
	{
		using DirectX::XMFLOAT3A::XMFLOAT3A;

		float3A(const struct float3& _f3);

		inline explicit operator XMVector() const
		{
			return DirectX::XMLoadFloat3A(this);
		}

		float3A& operator*=(const float& _f)
		{
			x *= _f;
			y *= _f;
			z *= _f;

			return *this;
		}

		float3A& operator+=(const float3A _f3A)
		{
			x += _f3A.x;
			y += _f3A.y;
			z += _f3A.z;

			return *this;
		}


	};

	struct float3 : DirectX::XMFLOAT3
	{
		using  DirectX::XMFLOAT3::XMFLOAT3;
		float3(const DirectX::XMFLOAT3&);

		inline explicit operator XMVector() const
		{
			return DirectX::XMLoadFloat3(this);
		}

		inline explicit operator float3A() const
		{
			return float3A{ x, y, z };
		}

		operator physx::PxVec3();
		operator physx::PxVec3()const;

		bool operator==(const float3& _in)
		{
			return x == _in.x && y == _in.y && z == _in.z;
		}

		bool operator==(const DirectX::XMFLOAT3& _in)
		{
			return x == _in.x && y == _in.y && z == _in.z;
		}

		float3& operator*=(const float3& _f1)
		{
			x *= _f1.x;
			y *= _f1.y;
			z *= _f1.z;

			return *this;
		}

		inline float3 operator+(const float3& v2) const
		{
			return float3{ x + v2.x, y + v2.y, z + v2.z };
		}

		inline float3 operator*(const float3& _f2) const
		{
			return float3{ x * _f2.x, y * _f2.y, z * _f2.z };
		}

		inline float3 operator*(float _f) const
		{
			return float3{ x * _f, y * _f, z * _f };
		}

		inline float3& operator*=(float _f)
		{
			x *= _f;
			y *= _f;
			z *= _f;
			return *this;
		}

		inline float3& operator+=(const float3& _float3)
		{
			x += _float3.x;
			y += _float3.y;
			z += _float3.z;

			return *this;
		}

	};

	struct float4 : DirectX::XMFLOAT4
	{
		using DirectX::XMFLOAT4::XMFLOAT4;

		inline explicit operator XMVector() const
		{
			return DirectX::XMLoadFloat4(this);
		}

		operator physx::PxQuat();
		operator physx::PxVec4();
		operator physx::PxVec3();

		bool operator==(const float4& _in)
		{
			return x == _in.x && y == _in.y && z == _in.z && w == _in.w;
		}

		float4& operator*=(const float4& _f1)
		{
			x *= _f1.x;
			y *= _f1.y;
			z *= _f1.z;
			w *= _f1.w;

			return *this;
		}

		inline float4 operator+(const float4& v2)
		{
			return float4{ x + v2.x, y + v2.y, z + v2.z, w + v2.w };
		}

		inline float4 operator*(const float4& _f2)
		{
			return float4{ x * _f2.x, y * _f2.y, z * _f2.z, w * _f2.w };
		}

	};

	struct float4A : DirectX::XMFLOAT4A
	{
		using DirectX::XMFLOAT4A::XMFLOAT4A;

		inline bool operator==(const float4A& _RHS) const
		{
			return x == _RHS.x && y == _RHS.y && z == _RHS.z && w == _RHS.w;
		}

		inline bool operator!=(const float4A& _RHS) const
		{
			return x != _RHS.x || y != _RHS.y || z != _RHS.z || w != _RHS.w;
		}

		inline float4A& operator+=(const float4A& _RHS)
		{
			x += _RHS.x;
			y += _RHS.y;
			z += _RHS.z;
			w += _RHS.w;
		}

		inline explicit operator XMVector() const
		{
			return DirectX::XMLoadFloat4A(this);
		}
	};

	struct float3x3 : DirectX::XMFLOAT3X3
	{
		using DirectX::XMFLOAT3X3::XMFLOAT3X3;

		inline explicit operator XMMatrix() const
		{
			return DirectX::XMLoadFloat3x3(this);
		}
	};

	struct float4x4 : DirectX::XMFLOAT4X4
	{
		using DirectX::XMFLOAT4X4::XMFLOAT4X4;

		float4x4(const aiMatrix4x4& _aiMatrix4x4);

		inline explicit operator XMMatrix() const
		{
			return DirectX::XMLoadFloat4x4(this);
		}

		operator physx::PxMat44()const;

		inline float4x4 operator*(const float4x4& m2) const
		{

			const auto xm1 = DirectX::XMLoadFloat4x4(this);
			const auto xm2 = DirectX::XMLoadFloat4x4(&m2);

			auto xm3 = DirectX::XMMatrixMultiply(xm1, xm2);

			float4x4 r;
			DirectX::XMStoreFloat4x4(&r, xm3);
			return r;
		}
	};

	struct float4x4A : DirectX::XMFLOAT4X4A
	{
		using DirectX::XMFLOAT4X4A::XMFLOAT4X4A;

		float4x4A(const DirectX::XMFLOAT4X4A& _Float4x4)
			: XMFLOAT4X4A(_Float4x4)
		{}

		inline explicit operator XMMatrix() const
		{
			return DirectX::XMLoadFloat4x4A(this);
		}

		inline float4x4A operator*(const float4x4A& m2) const
		{

			const auto xm1 = DirectX::XMLoadFloat4x4A(this);
			const auto xm2 = DirectX::XMLoadFloat4x4A(&m2);

			auto xm3 = DirectX::XMMatrixMultiply(xm1, xm2);

			float4x4A r;
			DirectX::XMStoreFloat4x4A(&r, xm3);
			return r;
		}

	};

	struct uint2
	{
		uint32_t x = 0;
		uint32_t y = 0;

		uint32_t& operator[](uint32_t i)
		{
			return (&x)[i];
		}
	};

	struct uint3
	{
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;

		uint32_t& operator[](uint32_t i)
		{
			return (&x)[i];
		}
	};

	struct uint4
	{

		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;
		uint32_t w = 0;

		uint32_t& operator[](uint32_t i)
		{
			return (&x)[i];
		}
	};

	struct int2
	{
		int32_t x = 0;
		int32_t y = 0;

		int32_t& operator[](uint32_t i)
		{
			return (&x)[i];
		}
	};

	struct int3
	{
		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;

		int32_t& operator[](uint32_t i)
		{
			return (&x)[i];
		}
	};

	struct int4
	{
		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;
		int32_t w = 0;

		int32_t& operator[](uint32_t i)
		{
			return (&x)[i];
		}
	};

#pragma region GlobalOperators

	inline bool operator==(const DirectX::XMFLOAT3& _in1, const float3& _in2)
	{
		return _in1.x == _in2.x && _in1.y == _in2.y && _in1.z == _in2.z;
	}

#pragma endregion



	constexpr float4x4A IdentityMatrix = float4x4A(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);





	inline void CreateBoundingAABoxFromPoints(BoundingAABox& outBoundingBox, const size_t vertexNum, const float3* vertecies, size_t strideSize)
	{
		DirectX::BoundingBox::CreateFromPoints(outBoundingBox, vertexNum, vertecies, strideSize);
	}

	inline void CreateBoundingAABoxMerged(BoundingAABox& outBoundingBox, const BoundingAABox& boundingBox1, const BoundingAABox& boundingBox2)
	{
		DirectX::BoundingBox::CreateMerged(outBoundingBox, boundingBox1, boundingBox2);
	}



	namespace Math
	{

		constexpr float4A IdentityQuaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
		constexpr float4A IdentityTranslate = { 0.0f, 0.0f, 0.0f, 0.0f };
		constexpr float4A IdentityScale = { 1.0f, 1.0f, 1.0f, 0.0f };

		constexpr DirectX::XMVECTORF32 XMIdentityTranslate = { 0.0f, 0.0f, 0.0f, 0.0f };
		constexpr DirectX::XMVECTORF32 XMIdentityQuaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
		constexpr DirectX::XMVECTORF32 XMIdentityScale = { 1.0f, 1.0f, 1.0f, 0.0f };

		constexpr float PI = DirectX::XM_PI;
		constexpr float PI_RCP = 1.0f / PI;
		constexpr float PIDIV4 = PI / 4.0f;
		constexpr float _1DIV180 = 1 / 180.0f;


		constexpr DirectX::XMVECTOR _XMVectorRow1 = { 1.0f, 0.0f, 0.0f, 0.0f };
		constexpr DirectX::XMVECTOR _XMVectorRow2 = { 0.0f, 1.0f, 0.0f, 0.0f };
		constexpr DirectX::XMVECTOR _XMVectorRow3 = { 0.0f, 0.0f, 1.0f, 0.0f };
		constexpr DirectX::XMVECTOR _XMVectorRow4 = { 0.0f, 0.0f, 0.0f, 1.0f };
		constexpr XMMatrix XMIdentityMatrix = { _XMVectorRow1, _XMVectorRow2, _XMVectorRow3, _XMVectorRow4 };

		float Min(const float3& _f3);
		float Max(const float3& _f3);

		inline constexpr float DegreeToRadian(float _DegreeAngle)
		{
			return _DegreeAngle * PI * _1DIV180;
		}

		inline constexpr float RadianToDegree(float _RadianAngle)
		{
			return _RadianAngle * PI_RCP * 180.0f;
		}

		inline DirectX::XMMATRIX ToXM(const DirectX::XMFLOAT4X4& source)
		{
			return DirectX::XMLoadFloat4x4(&source);
		}

		inline DirectX::XMMATRIX ToXM(const DirectX::XMFLOAT4X4A& source)
		{
			return DirectX::XMLoadFloat4x4A(&source);
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

		inline XMVector XMVectorSet(float _X, float _Y, float _Z, float _W)
		{
			return DirectX::XMVectorSet(_X, _Y, _Z, _W);
		}

		inline float4x4 FromXM(const XMMatrix& source)
		{
			float4x4 dest;
			DirectX::XMStoreFloat4x4(&dest, source);
			return dest;
		}

		inline float4x4A FromXMA(const XMMatrix& _Source)
		{
			float4x4A _Result;
			DirectX::XMStoreFloat4x4A(&_Result, _Source);
			return _Result;
		}

		inline DirectX::XMFLOAT4 FromXM(const DirectX::FXMVECTOR& source)
		{
			DirectX::XMFLOAT4 dest;
			DirectX::XMStoreFloat4(&dest, source);
			return dest;
		}

		inline float4A FromXMA(const XMVector& source)
		{
			float4A dest;
			DirectX::XMStoreFloat4A(&dest, source);
			return dest;
		}

		inline float3 FromXM3(const XMVector& source)
		{
			float3 dest;
			DirectX::XMStoreFloat3(&dest, source);
			return dest;
		}

		inline float3A FromXM3A(const XMVector& source)
		{
			float3A dest;
			DirectX::XMStoreFloat3A(&dest, source);
			return dest;
		}

		inline float2 FromXM2(const XMVector& source)
		{
			float2 dest;
			DirectX::XMStoreFloat2(&dest, source);
			return dest;
		}

		inline float2A FromXM2A(const XMVector& source)
		{
			float2A dest;
			DirectX::XMStoreFloat2A(&dest, source);
			return dest;
		}

		inline float FromXM1(const DirectX::FXMVECTOR& source)
		{
			float dest;
			DirectX::XMStoreFloat(&dest, source);
			return dest;
		}


		inline XMMatrix XMInverse(const XMMatrix& _Source)
		{
			return DirectX::XMMatrixInverse(nullptr, _Source);
		}

		inline DirectX::XMFLOAT4X4 Inverse(const DirectX::XMFLOAT4X4& source)
		{
			DirectX::XMFLOAT4X4 dest;

			auto matrix = ToXM(source);

			matrix = DirectX::XMMatrixInverse(nullptr, matrix);

			DirectX::XMStoreFloat4x4(&dest, matrix);

			return dest;
		}

		inline DirectX::XMFLOAT4X4A Inverse(const DirectX::XMFLOAT4X4A& source)
		{
			DirectX::XMFLOAT4X4A dest;

			auto matrix = ToXM(source);

			matrix = DirectX::XMMatrixInverse(nullptr, matrix);

			DirectX::XMStoreFloat4x4A(&dest, matrix);

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

		inline DirectX::XMFLOAT4X4A Transpose(const DirectX::XMFLOAT4X4A& source)
		{
			DirectX::XMFLOAT4X4A dest;

			auto matrix = ToXM(source);

			matrix = DirectX::XMMatrixTranspose(matrix);

			DirectX::XMStoreFloat4x4A(&dest, matrix);

			return dest;
		}

		inline XMMatrix XMTranspose(const XMMatrix& source)
		{
			auto matrix = source;

			return DirectX::XMMatrixTranspose(matrix);
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

		inline float4x4A InverseTranspose(const float4x4A& source)
		{
			float4x4A dest;

			auto matrix = ToXM(source);

			matrix.r[3] = DirectX::XMVectorSet(.0f, .0f, .0f, 1.0f);

			auto determinant = DirectX::XMMatrixDeterminant(matrix);

			DirectX::XMStoreFloat4x4A(&dest, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&determinant, matrix)));

			return dest;
		}

		inline float4x4A InverseTranspose(const XMMatrix& source)
		{
			auto matrix = source;

			matrix.r[3] = DirectX::XMVectorSet(.0f, .0f, .0f, 1.0f);

			auto determinant = DirectX::XMMatrixDeterminant(matrix);

			float4x4A dest;
			DirectX::XMStoreFloat4x4A(&dest, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&determinant, matrix)));

			return dest;
		}

		inline XMMatrix XMInverseTranspose(const XMMatrix& source)
		{
			auto matrix = source;

			matrix.r[3] = DirectX::XMVectorSet(.0f, .0f, .0f, 1.0f);

			auto determinant = DirectX::XMMatrixDeterminant(matrix);

			return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&determinant, matrix));

		}


		inline void Normalize(float2& source)
		{
			DirectX::XMStoreFloat2(&source, DirectX::XMVector2Normalize(XMLoadFloat2(&source)));
		}

		inline void Normalize(float2A& source)
		{
			DirectX::XMStoreFloat2A(&source, DirectX::XMVector2Normalize(XMLoadFloat2A(&source)));
		}

		inline void Normalize(float3& source)
		{
			DirectX::XMStoreFloat3(&source, DirectX::XMVector3Normalize(XMLoadFloat3(&source)));
		}

		inline void Normalize(float3A& source)
		{
			DirectX::XMStoreFloat3A(&source, DirectX::XMVector3Normalize(XMLoadFloat3A(&source)));
		}

		inline void Normalize(float4& source)
		{
			DirectX::XMStoreFloat4(&source, DirectX::XMVector4Normalize(XMLoadFloat4(&source)));
		}

		inline void Normalize(float4A& source)
		{
			DirectX::XMStoreFloat4A(&source, DirectX::XMVector4Normalize(XMLoadFloat4A(&source)));
		}

		inline XMVector Normalize(const XMVector& _Source)
		{
			return DirectX::XMVector4Normalize(_Source);
		}


		inline void NormalizeEst(float2& source)
		{
			DirectX::XMStoreFloat2(&source, DirectX::XMVector2NormalizeEst(XMLoadFloat2(&source)));
		}

		inline void NormalizeEst(float3& source)
		{
			DirectX::XMStoreFloat3(&source, DirectX::XMVector3NormalizeEst(XMLoadFloat3(&source)));
		}

		inline void NormalizeEst(float4& source)
		{
			DirectX::XMStoreFloat4(&source, DirectX::XMVector4NormalizeEst(XMLoadFloat4(&source)));
		}

		inline void NormalizeEst(float4A& source)
		{
			DirectX::XMStoreFloat4A(&source, DirectX::XMVector4NormalizeEst(XMLoadFloat4(&source)));
		}

		inline XMVector XMNormalizeEst(XMVector& source)
		{
			return DirectX::XMVector4NormalizeEst(source);
		}


		void DecomposeMatrix(const float4x4A& inMatrix, float4& scale, float4& translate, float4& quaternion);

		inline void XMDecomposeMatrix(const XMMatrix& _Matrix, XMVector& _outScale, XMVector& _outQuaternion, XMVector& _outTranslate)
		{
			DirectX::XMMatrixDecompose(&_outScale, &_outQuaternion, &_outTranslate, _Matrix);
		}


		inline float3 TransformPoint(const float3& point, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformCoord(XMLoadFloat3(&point), XMLoadFloat4x4(&transform)));
			return result;
		}

		inline float3 TransformPoint(const float3& point, const float4x4A& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformCoord(XMLoadFloat3(&point), XMLoadFloat4x4A(&transform)));
			return result;
		}

		inline float4 TransformPoint(const float4& point, const float4x4& transform)
		{
			float4 result;
			XMStoreFloat4(&result, XMVector4Transform(XMLoadFloat4(&point), XMLoadFloat4x4(&transform)));
			return result;
		}

		inline float4 TransformPoint(const float4& point, const float4x4A& transform)
		{
			float4 result;
			XMStoreFloat4(&result, XMVector4Transform(XMLoadFloat4(&point), XMLoadFloat4x4A(&transform)));
			return result;
		}

		inline XMVector XMTransformPoint(const XMVector& _Point, const XMMatrix& _Matrix)
		{
			return XMVector3TransformCoord(_Point, _Matrix);
		}


		inline float3 TransformVector(const float3& vector, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformNormal(XMLoadFloat3(&vector), XMLoadFloat4x4(&transform)));
			return result;
		}

		inline float3 TransformVector(const float3& vector, const float4x4A& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformNormal(XMLoadFloat3(&vector), XMLoadFloat4x4A(&transform)));
			return result;
		}

		inline XMVector XMTransformVector2Normal(const XMVector& _Vector, const XMMatrix& _Matrix)
		{
			return DirectX::XMVector2TransformNormal(_Vector, _Matrix);
		}

		inline XMVector XMTransformVector2Point(const XMVector& _Vector, const XMMatrix& _Matrix)
		{
			return DirectX::XMVector2TransformCoord(_Vector, _Matrix);
		}

		inline XMVector XMTransformVector3Normal(const XMVector& _Vector, const XMMatrix& _Matrix)
		{
			return DirectX::XMVector3TransformNormal(_Vector, _Matrix);
		}

		inline XMVector XMTransformVector3Point(const XMVector& _Vector, const XMMatrix& _Matrix)
		{
			return DirectX::XMVector3TransformCoord(_Vector, _Matrix);
		}

		inline XMVector XMTransformVector4(const XMVector& _Vector, const XMMatrix& _Matrix)
		{
			return DirectX::XMVector4Transform(_Vector, _Matrix);
		}

		inline float4 Transform(const float4& vector, const float4x4& transform)
		{
			float4 result;
			XMStoreFloat4(&result, XMVector4Transform(XMLoadFloat4(&vector), XMLoadFloat4x4(&transform)));
			return result;
		}

		inline float4 Transform(const float4& vector, const float4x4A& transform)
		{
			float4 result;
			XMStoreFloat4(&result, XMVector4Transform(XMLoadFloat4(&vector), XMLoadFloat4x4A(&transform)));
			return result;
		}


		inline float3 TransformInversePoint(const float3& point, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformCoord(XMLoadFloat3(&point), XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform))));
			return result;
		}

		inline float3 TransformInversePoint(const float3& point, const float4x4A& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformCoord(XMLoadFloat3(&point), XMMatrixInverse(nullptr, XMLoadFloat4x4A(&transform))));
			return result;
		}

		inline float3 TransformInverseVector(const float3& vector, const float4x4& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformNormal(XMLoadFloat3(&vector), XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform))));
			return result;
		}

		inline float3 TransformInverseVector(const float3& vector, const float4x4A& transform)
		{
			float3 result;
			XMStoreFloat3(&result, XMVector3TransformNormal(XMLoadFloat3(&vector), XMMatrixInverse(nullptr, XMLoadFloat4x4A(&transform))));
			return result;
		}


		inline BoundingAABox TransformBoundingBox(const BoundingAABox& _BoundingBox, const float4x4& _Transform)
		{
			BoundingAABox _aabb;
			_BoundingBox.Transform(_aabb, XMLoadFloat4x4(&_Transform));
			return _aabb;
		}

		inline BoundingAABox TransformBoundingBox(const BoundingAABox& _BoundingBox, const float4x4A& _Transform)
		{
			BoundingAABox _aabb;
			_BoundingBox.Transform(_aabb, XMLoadFloat4x4A(&_Transform));
			return _aabb;
		}

		inline BoundingAABox XMTransformBoundingBox(const BoundingAABox& _BoundingBox, const XMMatrix& _Matrix)
		{
			BoundingAABox _aabb;
			_BoundingBox.Transform(_aabb, _Matrix);
			return _aabb;
		}


		inline float4x4A TransformMatrix(const XMVector& _Scale, const XMVector& _Quaternion, const XMVector& _Translate)
		{
			float4x4A _Result;
			XMMatrix _Matrix = DirectX::XMMatrixAffineTransformation(_Scale, XMVectorZero, _Quaternion, _Translate);
			DirectX::XMStoreFloat4x4A(&_Result, _Matrix);
			return _Result;
		}

		inline float4x4A TransformMatrix(const float4A& _Scale, const float4A& _Quaternion, const float4A& _Translate)
		{
			XMMatrix _TransformMatrix = DirectX::XMMatrixAffineTransformation(ToXM(_Scale), XMVectorZero, ToXM(_Quaternion), ToXM(_Translate));
			return FromXMA(_TransformMatrix);
		}

		XMMatrix TransformMatrix(const float3A& _Forward);

		XMMatrix TransformMatrix(const float3A& _Forward, const float3A& _Position);

		inline XMMatrix XMTransformMatrix(const XMVector& _Scale, const XMVector& _Quaternion, const XMVector& _Translate)
		{
			return DirectX::XMMatrixAffineTransformation(_Scale, XMVectorZero, _Quaternion, _Translate);
		}

		inline XMMatrix XMTransformMatrix(const float4A& _Scale, const float4A& _Quaternion, const float4A& _Translate)
		{
			return DirectX::XMMatrixAffineTransformation(ToXM(_Scale), XMVectorZero, ToXM(_Quaternion), ToXM(_Translate));
		}

		XMMatrix XMTransformMatrix(const XMVector& _Forward);

		XMMatrix XMTransformMatrix(const XMVector& _Forward, const XMVector& _Position);

		inline float4x4A TransformMatrixTranslate(const float3& _Translate)
		{
			float4x4A _Result;
			XMMatrix _Matrix = DirectX::XMMatrixTranslation(_Translate.x, _Translate.y, _Translate.z);
			DirectX::XMStoreFloat4x4(&_Result, _Matrix);
			return _Result;
		}

		inline XMMatrix XMTransformMatrixTranslate(const float3& _Translate)
		{
			return DirectX::XMMatrixTranslation(_Translate.x, _Translate.y, _Translate.z);
		}

		inline XMMatrix XMTransformMatrixTranslate(const XMVector& _XMTranslate)
		{
			return DirectX::XMMatrixTranslationFromVector(_XMTranslate);
		}

		inline float4x4A TransformMatrixScale(const float3& _Scale)
		{
			float4x4A _Result;
			XMMatrix _Matrix = DirectX::XMMatrixScaling(_Scale.x, _Scale.y, _Scale.z);
			DirectX::XMStoreFloat4x4A(&_Result, _Matrix);
			return _Result;
		}

		inline XMMatrix XMTransformMatrixScale(const float3& _Scale)
		{
			return DirectX::XMMatrixScaling(_Scale.x, _Scale.y, _Scale.z);
		}

		inline XMMatrix XMTransformMatrixScale(const XMVector& _XMScale)
		{
			return DirectX::XMMatrixScalingFromVector(_XMScale);
		}

		inline float4x4A TransformMatrixRotation(const float4& _RotateQuaternion)
		{
			float4x4A _Result;
			XMMatrix _Matrix = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&_RotateQuaternion));
			DirectX::XMStoreFloat4x4A(&_Result, _Matrix);
			return _Result;
		}

		inline float4x4A TransformMatrixRotation(float _RotationAngle, const float3& _RotationNormal, const float3& _RotationOrigin)
		{
			float4x4A _Result;

			XMVector _RotationQuaternion = DirectX::XMQuaternionRotationNormal(XMLoadFloat3(&_RotationNormal), _RotationAngle);

			XMMatrix _Matrix = XMMatrixTransformation(XMVectorZero, DirectX::XMQuaternionIdentity(), XMVectorOne, XMLoadFloat3(&_RotationOrigin), _RotationQuaternion, XMVectorZero);
			DirectX::XMStoreFloat4x4A(&_Result, _Matrix);
			return _Result;
		}

		inline XMMatrix XMTransformMatrixRotation(const float4& _Quaternion)
		{
			return DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&_Quaternion));
		}

		inline XMMatrix XMTransformMatrixRotation(const XMVector& _XMQuaternion)
		{
			return DirectX::XMMatrixRotationQuaternion(_XMQuaternion);
		}

		inline XMMatrix XMTransformMatrixRotation(float _RotationAngle, const float3& _RotationNormal, const float3& _RotationOrigin)
		{
			XMVector _RotationQuaternion = DirectX::XMQuaternionRotationNormal(DirectX::XMLoadFloat3(&_RotationNormal), _RotationAngle);

			return DirectX::XMMatrixTransformation(XMVectorZero, DirectX::XMQuaternionIdentity(), XMVectorOne, DirectX::XMLoadFloat3(&_RotationOrigin), _RotationQuaternion, XMVectorZero);
		}



		inline void Translate(float4x4A& _OutTransform, const float3& _Translation)
		{
			_OutTransform._14 += _Translation.x;
			_OutTransform._24 += _Translation.y;
			_OutTransform._34 += _Translation.z;
		}

		inline void Scale(float4x4A& _OutTransform, const float3& _Scaling)
		{
			_OutTransform._11 *= _Scaling.x;
			_OutTransform._22 *= _Scaling.y;
			_OutTransform._33 *= _Scaling.z;
		}

		inline void Rotate(float4x4A& _OutTransform, const float4& _RotationQuaternion)
		{
			XMMatrix _Matrix = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&_RotationQuaternion));
			XMMatrix _Result = DirectX::XMMatrixMultiply(XMLoadFloat4x4A(&_OutTransform), _Matrix);
			XMStoreFloat4x4(&_OutTransform, _Result);
		}

		float3A Rotate(const float3A& _Vector, const float4A& _RotationQuaternion);


		inline XMMatrix XMMatrixView(const XMVector& _Position, const XMVector& _Look, const XMVector& _Up)
		{
			return DirectX::XMMatrixLookToLH(_Position, _Look, _Up);
		}

		inline XMMatrix XMMatrixInv(const XMMatrix& _Matrix)
		{
			return DirectX::XMMatrixInverse(nullptr, _Matrix);
		}


		inline float4x4A Multiply(const float4x4A& _M0, const float4x4A& _M1)
		{
			return FromXMA(DirectX::XMMatrixMultiply(ToXM(_M0), ToXM(_M1)));
		}

		inline float4x4A Multiply(const XMMatrix& _M0, const XMMatrix& _M1)
		{
			return FromXMA(DirectX::XMMatrixMultiply(_M0, _M1));
		}

		inline XMVector XMMultiply(const XMVector& _V0, const XMVector& _V1) noexcept
		{
			return DirectX::XMVectorMultiply(_V0, _V1);
		}

		inline XMMatrix XMMultiply(const XMMatrix& _M0, const XMMatrix& _M1)
		{
			return DirectX::XMMatrixMultiply(_M0, _M1);
		}


		inline XMVector XMAdd(const XMVector& _V0, const XMVector& _V1)
		{
			return DirectX::XMVectorAdd(_V0, _V1);
		}

		XMVector XMDot(const XMVector& _V0, const XMVector& _V1);

		XMVector XMCross(const XMVector& _V0, const XMVector& _V1);


		inline bool XMEqual2(const XMVector& _V0, const XMVector& _V1)
		{
			return DirectX::XMVector2Equal(_V0, _V1);
		}

		inline bool XMNotEqual2(const XMVector& _V0, const XMVector& _V1)
		{
			return DirectX::XMVector2NotEqual(_V0, _V1);
		}

		inline bool XMEqual3(const XMVector& _V0, const XMVector& _V1)
		{
			return DirectX::XMVector3Equal(_V0, _V1);
		}

		inline bool XMNotEqual3(const XMVector& _V0, const XMVector& _V1)
		{
			return DirectX::XMVector3NotEqual(_V0, _V1);
		}

		inline bool XMEqual4(const XMVector& _V0, const XMVector& _V1)
		{
			return DirectX::XMVector4Equal(_V0, _V1);
		}

		inline bool XMNotEqual4(const XMVector& _V0, const XMVector& _V1)
		{
			return DirectX::XMVector4NotEqual(_V0, _V1);
		}


		inline float4A QuaternionMultiply(const float4A& _Quaternion1, const float4A& _Quaternion2)
		{
			return FromXMA(DirectX::XMQuaternionMultiply(ToXM(_Quaternion1), ToXM(_Quaternion2)));
		}

		inline float4A QuaternionMultiply(const XMVector& _Quaternion1, const XMVector& _Quaternion2)
		{
			return FromXMA(DirectX::XMQuaternionMultiply(_Quaternion1, _Quaternion2));
		}

		inline XMVector XMQuaternionMultiply(const XMVector& _Quaternion1, const XMVector& _Quaternion2)
		{
			return DirectX::XMQuaternionMultiply(_Quaternion1, _Quaternion2);
		}

		XMVector XMSelect(const XMVector& _V1, const XMVector& _V2, const XMVector& _VSelect);

		XMVector XMSelectVector(uint32_t _S0, uint32_t _S1, uint32_t _S2, uint32_t _S3);


		namespace TriangleTests
		{
			inline bool Intersects(const XMVector& _RayOrigin, const XMVector& _RayDirection, const XMVector& _V0, const XMVector& _V1, const XMVector& _V2, float& _OutDistance)
			{
				return DirectX::TriangleTests::Intersects(_RayOrigin, _RayDirection, _V0, _V1, _V2, _OutDistance);
			}
		}

	}


}