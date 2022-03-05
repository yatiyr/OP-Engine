#pragma once

#include <Scene/Scene.h>
#include <entt.hpp>

namespace Opium
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;


		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{

			OP_ENGINE_ASSERT(!HasComponent<T>(), "Entity already has a component");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			OP_ENGINE_ASSERT(HasComponent<T>(), "Entity does not have this component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			OP_ENGINE_ASSERT(HasComponent<T>(), "Entity does not have this component");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		operator bool() const 
		{ 
			return m_EntityHandle != entt::null; 
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}