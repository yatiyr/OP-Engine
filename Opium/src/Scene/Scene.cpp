#include <Precomp.h>
#include <Scene/Scene.h>
#include <glm/glm.hpp>

#include <Scene/Components.h>
#include <Renderer/Renderer2D.h>
#include <Scene/ScriptableEntity.h>

#include <Scene/Entity.h>

#include <ScriptManager/ScriptManager.h>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

namespace OP
{

	static b2BodyType RigidbodyTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
		case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
		case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		OP_ENGINE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	void Scene::TransformChangeCallback(entt::registry& reg, entt::entity ent)
	{
		auto& transform = reg.get<TransformComponent>(ent);
		transform.dirty = true;
		Entity e = { ent, this };
		e.UpdateTransforms();
	}

	Scene::Scene()
	{
		m_Registry.on_update<TransformComponent>().connect<&Scene::TransformChangeCallback>(this);
	}

	Scene::~Scene()
	{

	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();

		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
		{
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}
	}


	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		auto idView = srcSceneRegistry.view<IDComponent>();

		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}



		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<RelationshipComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<ScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		CopyComponent<DirLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpotLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<PointLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		CopyComponent<MeshComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name);
		entity.AddComponent<RootComponent>();
		return entity;
	}

	Entity Scene::CreateChildEntity(Entity parent, const std::string& name)
	{
		Entity child = CreateEntity(name);
		child.RemoveComponent<RootComponent>();
		AddChildEntity(parent, child);

		return child;
	}

	Entity Scene::GetEntityWithUUID(UUID id)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto e : view)
		{
			Entity ent = { e, this };
			if (ent.GetUUID() == id)
				return ent;
		}
		Entity entity;
		return entity;
	}

	Entity Scene::AddChildEntity(Entity parent, Entity child)
	{

		// We first detach the child from its former parent if exists.
		DetachChild(child);

		// Then we bind to new parent
		auto& parentRelationship = parent.GetComponent<RelationshipComponent>();
		auto& childRelationship = child.GetComponent<RelationshipComponent>();

		Entity firstChild = GetEntityWithUUID(parentRelationship.first);

		Entity iterator;
		Entity iterator2;

		iterator = firstChild;

		// Parent has no children yet
		if (entt::entity(iterator) == entt::null)
		{
			childRelationship.parent = parent.GetUUID();
			parentRelationship.first = child.GetUUID();
		}
		else
		{
			iterator2 = GetEntityWithUUID(firstChild.GetComponent<RelationshipComponent>().next);
			while (entt::entity(iterator2) != entt::null)
			{
				iterator = iterator2;
				iterator2 = GetEntityWithUUID(iterator2.GetComponent<RelationshipComponent>().next);
			}

			auto& iteratorRelationshipComp = iterator.GetComponent<RelationshipComponent>();

			iteratorRelationshipComp.next = child.GetUUID();
			childRelationship.prev = iterator.GetUUID();
			childRelationship.parent = parent.GetUUID();
		}

		child.RemoveComponent<RootComponent>();
		return child;
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RelationshipComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? tag.Tag = "Entity" : name;
		return entity;
	}

	void Scene::DetachChild(Entity child)
	{
		auto& entityRelationshipComp = child.GetComponent<RelationshipComponent>();

		Entity prev = GetEntityWithUUID(entityRelationshipComp.prev);
		Entity next = GetEntityWithUUID(entityRelationshipComp.next);
		Entity parent = GetEntityWithUUID(entityRelationshipComp.parent);

		if (entt::entity(parent) != entt::null)
		{
			// if entity is the first child
			if (entt::entity(prev) == entt::null)
			{
				Entity nullEntity;
				auto& parentRelComp = parent.GetComponent<RelationshipComponent>();
				parentRelComp.first = UINT64_MAX;
			}
			// Entity is the last child
			else if (entt::entity(next) == entt::null)
			{
				Entity nullEntity;
				auto& prevRelComp = prev.GetComponent<RelationshipComponent>();
				prevRelComp.next = UINT64_MAX;
			}
			// Entity is one of the middle children
			else
			{
				auto& prevRelComp = prev.GetComponent<RelationshipComponent>();
				auto& nextRelComp = next.GetComponent<RelationshipComponent>();

				prevRelComp.next = next.GetUUID();
				nextRelComp.prev = prev.GetUUID();
			}
		}

		if(!child.HasComponent<RootComponent>())
			child.AddComponent<RootComponent>();
	}

	void Scene::RemoveEntity(Entity entity)
	{

		DetachChild(entity);
		m_Registry.destroy(entity);

	}

	void Scene::OnRuntimeStart()
	{

		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = RigidbodyTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape polygonShape;
				polygonShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &polygonShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);


		/*m_Registry.view<ScriptComponent>().each([=](auto entity, auto& comp)
		{
				Entity e = { entity, this };
				ScriptManager::OnUpdateEntity(e, ts);
		});*/

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			Entity e = { entity, this };

			if (e.HasComponent<ScriptComponent>())
			{
				ScriptManager::OnUpdateEntity((uint32_t)e, ts);
			}

			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			// Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{

		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					// TODO: Move to Scene::OnScenePlay
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{ entity, this };
						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(ts);
				});
		}

		// Physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from box2d
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();


				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}

		// Render 2D
		Camera* PrimaryCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto group = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto [camera, transform] = group.get<CameraComponent, TransformComponent>(entity);

				if (camera.Primary)
				{
					PrimaryCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (PrimaryCamera)
		{


			Renderer2D::BeginScene(PrimaryCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				// Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}

			Renderer2D::EndScene();
		}

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<RelationshipComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<ScriptComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);

		CopyComponentIfExists<DirLightComponent>(newEntity, entity);
		CopyComponentIfExists<SpotLightComponent>(newEntity, entity);
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}


	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<RootComponent>(Entity entity, RootComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if(m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}


	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
		uint32_t sceneID = 0;
		ScriptManager::OnInitEntity(component, (uint32_t)entity, sceneID);
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}


	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<DirLightComponent>(Entity entity, DirLightComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{

	}
}