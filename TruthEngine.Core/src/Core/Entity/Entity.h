#pragma once

#include "Core/Application.h"

namespace TruthEngine
{
		class Scene;

		class Entity
		{
		public:
			Entity();
			//Entity(Scene* scene);
			Entity(entt::entity entityHandle);

			float4x4A GetTransformHierarchy();

			template<class T, class... Args>
			inline T& AddComponent(Args&&... args) const
			{
				return Application::GetActiveScene()->AddComponent<T>(m_EntityHandle, std::forward<Args>(args)...);
			}

			template<class T>
			inline T& GetComponent() const
			{
				return Application::GetActiveScene()->GetComponent<T>(m_EntityHandle);
			}

			template<class T>
			inline bool HasComponent() const
			{
				return Application::GetActiveScene()->HasComponent<T>(m_EntityHandle);
			}

			template<class T>
			void RemoveComponent() const
			{
				Application::GetActiveScene()->RemoveComponent<T>(m_EntityHandle);
			}

			void SetTransform(const float4x4A& _Transform) const;

			operator bool() const
			{
				return m_EntityHandle != entt::null;
			}

			operator entt::entity() const
			{
				return m_EntityHandle;
			}

			bool operator==(const Entity& ent) const
			{
				return m_EntityHandle == ent.m_EntityHandle;
			}

			bool operator==(const entt::entity & ent) const
			{
				return m_EntityHandle == ent;
			}

			bool operator!=(const Entity& ent) const
			{
				return m_EntityHandle != ent.m_EntityHandle;
			}

			operator uint32_t()
			{
				return static_cast<uint32_t>(m_EntityHandle);
			}

			operator uint32_t() const
			{
				return static_cast<uint32_t>(m_EntityHandle);
			}


			/*float3 GetPosition() noexcept;*/


		private:



		private:
			entt::entity m_EntityHandle = entt::null;

			//Scene* m_Scene = nullptr;
			//entt::registry* m_Registery = nullptr;
		};
}
