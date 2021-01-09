#pragma once

namespace TruthEngine
{
	namespace Core
	{
		class TransformComponent
		{
		public:
			TransformComponent();
			TransformComponent(const float4x4& transformMatrix);
			~TransformComponent();

			TransformComponent(const TransformComponent&);
			TransformComponent& operator=(const TransformComponent&);

			TransformComponent(TransformComponent&&) noexcept;
			TransformComponent& operator=(TransformComponent&&) noexcept;

			inline const float4x4& GetTransform()const noexcept
			{
				return m_Transform;
			}

			inline operator const float4x4& ()
			{
				return m_Transform;
			}

		private:

		private:
			float4x4 m_Transform = IdentityMatrix;
		};
	}
}
