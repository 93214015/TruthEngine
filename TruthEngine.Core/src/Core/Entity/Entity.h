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
				return GetActiveScene()->AddComponent<T>(m_EntityHandle, std::forward<Args>(args)...);
			}

			template<class T>
			inline T& GetComponent() const
			{
				return GetActiveScene()->GetComponent<T>(m_EntityHandle);
			}

			template<class T>
			inline bool HasComponent() const
			{
				return GetActiveScene()->HasComponent<T>(m_EntityHandle);
			}

			template<class T>
			void RemoveComponent() const
			{
				GetActiveScene()->RemoveComponent<T>(m_EntityHandle);
			}

			void Move(const float4A& _Translate) const;

			void Rotate(const float4A& _RoatationQuaternion) const;

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

			explicit operator uint32_t() const
			{
				return static_cast<uint32_t>(m_EntityHandle);
			}


			/*float3 GetPosition() noexcept;*/


		private:



		private:
			entt::entity m_EntityHandle = entt::null;

			//Scene* m_Scene = nullptr;
			//entt::registry* m_Registery = nullptr;

			//
			// friend classes
			//
			friend class Scene;
		};
}
