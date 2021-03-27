#pragma once

namespace TruthEngine
{
		class TransformComponent
		{
		public:
			TransformComponent();
			TransformComponent(const float4x4& transformMatrix, const float3& _WorldCenterOffset);
			~TransformComponent();

			TransformComponent(const TransformComponent&);
			TransformComponent& operator=(const TransformComponent&);

			TransformComponent(TransformComponent&&) noexcept;
			TransformComponent& operator=(TransformComponent&&) noexcept;


			inline float4x4& GetTransform() noexcept
			{
				return m_Transform;
			}

			inline const float4x4& GetTransform() const noexcept
			{
				return m_Transform;
			}

			float4x4 GetTransformFromWorldCenter() const
			{
				float4x4 _result = m_Transform;
				_result._41 += m_WorldCenterOffset.x;
				_result._42 += m_WorldCenterOffset.y;
				_result._43 += m_WorldCenterOffset.z;
				return _result;
			}

			const float3& GetWorldCenterOffset() const
			{
				return m_WorldCenterOffset;
			}

			inline operator float4x4&()
			{
				return m_Transform;
			}

		private:

		private:
			float4x4 m_Transform = IdentityMatrix;

			float3 m_WorldCenterOffset = float3{ 0.f,.0f,.0f };
		};
}
