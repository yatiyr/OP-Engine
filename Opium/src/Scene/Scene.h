#pragma once

#include <entt.hpp>
#include <Opium/Timestep.h>
#include <Renderer/EditorCamera.h>
#include <Opium/UUID.h>

class b2World;

namespace OP
{
	class Entity;

	class Scene 
	{
	public:
		Scene();
		~Scene();


		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());

		// Creates a new entity under a parent
		Entity CreateChildEntity(Entity parent, const std::string& name = std::string());

		// Add child entity to a parent, if child already has a parent, detach it first
		Entity AddChildEntity(Entity parent, Entity child);

		// Detaches the child entity from its former parent, if parent does not exist, no op
		void DetachChild(Entity child);

		// Detach the child and then remove it from the scene
		void RemoveEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		template<typename T>
		void Patch(entt::entity handle)
		{
			m_Registry.patch<T>(handle);
		}

		void TransformChangeCallback(entt::registry& reg, entt::entity ent);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		Entity GetEntityWithUUID(UUID id);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		template<typename T>
		void OnComponentRemoved(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyComponent;
		friend class SceneRenderer;
	};
}
