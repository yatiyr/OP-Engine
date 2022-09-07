#pragma once

#include <Scene/Scene.h>
#include <entt.hpp>
#include <Opium/UUID.h>
#include <Scene/Components.h>

namespace OP
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		void UpdateTransforms()
		{
			auto& tC = GetComponent<TransformComponent>();

			if (!tC.dirty)
				return;

			ForceUpdateTransforms();
		}

		void ForceUpdateTransforms()
		{
			auto& tC   = GetComponent<TransformComponent>();
			auto& relC = GetComponent<RelationshipComponent>();

			Entity parent = m_Scene->GetEntityWithUUID(relC.parent);

			if (entt::entity(parent) != entt::null)
			{
				tC.computeModelMatrix(parent.GetComponent<TransformComponent>().globalTransformation,
									  parent.GetComponent<TransformComponent>().globalTransformationInv);
			}
			else
			{
				tC.computeModelMatrix();
			}

			Entity child = m_Scene->GetEntityWithUUID(relC.first);
			while (entt::entity(child) != entt::null)
			{
				auto& relChild = child.GetComponent<RelationshipComponent>();
				child.ForceUpdateTransforms();
				child = m_Scene->GetEntityWithUUID(relChild.next);
			}
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			try
			{
				T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
				m_Scene->OnComponentAdded<T>(*this, component); // TODO: SORT THIS OUT
				return component;
			}
			catch (const std::exception&)
			{
				//OP_ENGINE_ERROR("Could not add Component to {0}", m_EntityHandle);
			}
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			try
			{
				T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
				m_Scene->OnComponentAdded<T>(*this, component); // TODO: SORT THIS OUT
				return component;
			}
			catch (const std::exception&)
			{

			}
		}

		template<typename T>
		void Patch()
		{
			try
			{
				m_Scene->Patch<T>(m_EntityHandle);
			}
			catch (const std::exception&)
			{

			}
		}

		template<typename T>
		T& GetComponent()
		{
			try
			{
				return m_Scene->m_Registry.get<T>(m_EntityHandle);
			}
			catch (const std::exception&)
			{

			}
		}

		template<typename T>
		void RemoveComponent()
		{
			try
			{
				T& component = m_Scene->m_Registry.get<T>(m_EntityHandle);
				m_Scene->OnComponentRemoved<T>(*this, component);
				m_Scene->m_Registry.remove<T>(m_EntityHandle);
			}
			catch (const std::exception&)
			{

			}
		}

		template<typename T>
		bool HasComponent()
		{
			try
			{
				return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
			}
			catch (const std::exception&)
			{

			}
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }


		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		bool operator == (const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator != (const Entity& other) const
		{
			return !(*this == other);
		}

		Scene* GetScene() { return m_Scene; }

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		friend class ScriptManager;
	};
}