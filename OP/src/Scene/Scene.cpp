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

#include <Op/ResourceManager.h>

#include <Scene/SceneRenderer.h>

#include <JobSystem/JobSystem.h>

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

	std::string Scene::GetSceneName()
	{
		return m_Name;
	}

	void Scene::SetSceneName(std::string name)
	{
		m_Name = name;
	}

	void Scene::SetSkybox(std::string SkyboxName)
	{
		m_Skybox = SkyboxName;
		SceneRenderer::ChangeEnvironmentMap(SkyboxName);
	}

	void Scene::SetToneMap(bool ToneMap)
	{
		m_ToneMap = ToneMap;
		SceneRenderer::SetHdr(ToneMap);
	}

	void Scene::SetExposure(float Exposure)
	{
		m_Exposure = Exposure;
		SceneRenderer::SetExposure(Exposure);
	}

	std::string Scene::GetSkybox()
	{
		return m_Skybox;
	}

	bool Scene::GetToneMap()
	{
		return m_ToneMap;
	}

	float Scene::GetExposure()
	{
		return m_Exposure;
	}

	bool Scene::IsValid(Entity entity)
	{		
		return m_Registry.valid(entt::entity(entity));
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

	static void CopyScriptComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{

	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
		{
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}
	}

	static void CopyScriptComponent(Entity dst, Entity src)
	{
		if (src.HasComponent<ScriptComponent>())
		{
			auto& sC = src.GetComponent<ScriptComponent>();

			auto& sCDst = dst.AddOrReplaceComponent<ScriptComponent>(sC.ModuleName);

			for (auto& [name, field] : sC.PublicFields)
			{
				PublicField* pField = (PublicField*)field;

				if (pField->Type == FieldType::Int)
				{

					int val = pField->GetValue<int>();
					PublicField* pFieldNew = (PublicField*)sCDst.PublicFields[name];
					pFieldNew->SetValue<int>(val);
					
				}
				else if (pField->Type == FieldType::Float)
				{
					float val = pField->GetValue<float>();
					PublicField* pFieldNew = (PublicField*)sCDst.PublicFields[name];
					pFieldNew->SetValue<float>(val);					
				}
				else if (pField->Type == FieldType::String)
				{
					std::string val = pField->GetValue<std::string>();
					PublicField* pFieldNew = (PublicField*)sCDst.PublicFields[name];
					pFieldNew->SetValue<std::string>(val);

				}
				else if (pField->Type == FieldType::UnsignedInt)
				{
					uint32_t val = pField->GetValue<uint32_t>();
					PublicField* pFieldNew = (PublicField*)sCDst.PublicFields[name];
					pFieldNew->SetValue<uint32_t>(val);
					
				}
				else if (pField->Type == FieldType::Vec2)
				{
					glm::vec2 val = pField->GetValue<glm::vec2>();
					PublicField* pFieldNew = (PublicField*)sCDst.PublicFields[name];
					pFieldNew->SetValue<glm::vec2>(val);
					
				}
				else if (pField->Type == FieldType::Vec3)
				{
					glm::vec3 val = pField->GetValue<glm::vec3>();
					PublicField* pFieldNew = (PublicField*)sCDst.PublicFields[name];
					pFieldNew->SetValue<glm::vec3>(val);
					
				}
				else if (pField->Type == FieldType::Vec4)
				{
					glm::vec4 val = pField->GetValue<glm::vec4>();
					PublicField* pFieldNew = (PublicField*)sCDst.PublicFields[name];
					pFieldNew->SetValue<glm::vec4>(val);
				}
			}
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
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, (uint32_t)e, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		CopyComponent<TagComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<RootComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
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
		CopyComponent<MaterialComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		CopyComponent<Physics3DMaterial>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Physics3DCollider>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<ScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);


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


	Entity Scene::CreateEntityWithUUID(UUID uuid, uint32_t identifier, const std::string& name)
	{
		Entity entity = { m_Registry.create((entt::entity)identifier), this };
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
				// if entity has a next sibling
				if (entt::entity(next) != entt::null)
				{
					parentRelComp.first = next.GetUUID();
					auto& nextRelComp = next.GetComponent<RelationshipComponent>();
					nextRelComp.prev = UINT64_MAX;
				}
					
				else
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

	void Scene::GiveAllChildren(Entity parent, std::vector<Entity>& childList)
	{
		auto& relC = parent.GetComponent<RelationshipComponent>();
		Entity childIterator = GetEntityWithUUID(relC.first);
		while (entt::entity(childIterator) != entt::null)
		{
			auto& childRelC = childIterator.GetComponent<RelationshipComponent>();
			childList.push_back(childIterator);
			childIterator = GetEntityWithUUID(childRelC.next);
		}
	}

	void Scene::RemoveEntity(Entity entity)
	{
		// Recursively remove its children as well
		std::vector<Entity> childList;
		GiveAllChildren(entity, childList);

		for (int i = 0; i < childList.size(); i++)
		{
			DetachChild(childList[i]);
			m_Registry.destroy(childList[i]);
		}

		// If the entity is being removed during runtime and has physical properties
		// we also need to remove the entity from the physical world.
		if (entity.HasComponent<Physics3DMaterial>() && entity.HasComponent<Physics3DCollider>())
		{
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			auto& physics3DCollider = entity.GetComponent<Physics3DCollider>();

			if(physics3DMaterial.RuntimeBody)
				PhysicsManager::DeleteRigidBody(physics3DMaterial.RuntimeBody, physics3DCollider.runtimeShapeIndex);
		}


		DetachChild(entity);
		m_Registry.destroy(entity);

	}

	void Scene::OnRuntimeStart()
	{
		// OnCreate entity function is being triggered for 
		// the Entities having script component
		auto view = m_Registry.view<ScriptComponent>();
		for (auto& entity : view)
		{
			ScriptManager::OnCreateEntity((uint32_t)entity);
		}

		// We tell physics manager ot start the physics world
		PhysicsManager::StartWorld();

		// We will add entities which have physical properties to the world.
		auto group = m_Registry.group<Physics3DMaterial>(entt::get<Physics3DCollider>);
		for (auto entity : group)
		{
			Entity e = { entity, this };
			auto& physics3DMaterial  = e.GetComponent<Physics3DMaterial>();
			auto& physics3DCollider  = e.GetComponent<Physics3DCollider>();
			auto& transformComponent = e.GetComponent<TransformComponent>();

			RigidBodySpec spec;

			spec.Mass             = physics3DMaterial.Mass;
			spec.Friction         = physics3DMaterial.Friction;
			spec.RollingFriction  = physics3DMaterial.RollingFriction;
			spec.SpinningFriction = physics3DMaterial.SpinningFriction;
			spec.Restitution      = physics3DMaterial.Restitution;

			spec.Shape = physics3DCollider.Shape;
			spec.Radius = physics3DCollider.Radius;

			spec.FixRotX = physics3DMaterial.FixRotX;
			spec.FixRotY = physics3DMaterial.FixRotY;
			spec.FixRotZ = physics3DMaterial.FixRotZ;

			glm::quat orientation = glm::quat(transformComponent.Rotation);
			glm::vec3 translation = transformComponent.Translation;
			glm::vec3 scale       = transformComponent.Scale * physics3DCollider.Scale;

			spec.scale       = btVector3(scale.x, scale.y, scale.z);
			spec.Orientation = btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w);
			spec.Origin      = btVector3(translation.x, translation.y, translation.z);

			spec.ContactResponse = physics3DCollider.ContactResponse;
			spec.IsKinematic = physics3DMaterial.IsKinematic;

			Entity* entityPointer = new Entity(entity, this);

			AddRigidBodyResponse resp = PhysicsManager::AddRigidBody(spec,
					physics3DMaterial.RuntimeMotionState,
					physics3DMaterial.RuntimeBody,
					physics3DCollider.RuntimeCollisionShape,
				    entityPointer);

			
			physics3DCollider.runtimeShapeIndex = resp.collisionShapeIndex;
			physics3DMaterial.RuntimeMotionState = resp.motionState;
			physics3DMaterial.RuntimeBody = resp.body;
			physics3DCollider.RuntimeCollisionShape = resp.collisionShape;
			physics3DCollider.RuntimeBody = physics3DMaterial.RuntimeBody;

		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		SceneRenderer::Render(camera, this, ts);
	}

	void Scene::OnUpdateRuntime(Timestep ts, EditorCamera& camera)
	{
		// Update scripts
		auto scriptView = m_Registry.view<ScriptComponent>();
		for (auto entity : scriptView)
		{
			Entity e = { entity, this };
			ScriptManager::OnUpdateEntity((uint32_t)e, ts);
		}

		//JobSystem::Wait();


		// Physics
		{
			auto group = m_Registry.group<Physics3DMaterial>(entt::get<Physics3DCollider>);
			PhysicsManager::StepWorld(ts);


			for (auto entity : group)
			{
				Entity e = { entity, this };
				auto& physics3DMaterial = e.GetComponent<Physics3DMaterial>();
				auto& transformComponent = e.GetComponent<TransformComponent>();
				btMotionState* mS = (btMotionState*)physics3DMaterial.RuntimeMotionState;

				btTransform transform;
				mS->getWorldTransform(transform);

				btVector3 origin = transform.getOrigin();
				btQuaternion rot = transform.getRotation();

				glm::vec3 rotEuler(0.0f);
				transform.getRotation().getEulerZYX(rotEuler.z, rotEuler.y, rotEuler.x);

				transformComponent.Translation = glm::vec3(origin.x(), origin.y(), origin.z());
				if (transformComponent.RuntimeControlledRotation != glm::vec3(0.0f))
				{
					transformComponent.Rotation = transformComponent.RuntimeControlledRotation;
					transformComponent.RuntimeControlledRotation = glm::vec3(0.0f);
					glm::quat newQuat = glm::quat(transformComponent.RuntimeControlledRotation);

					btQuaternion newRot = btQuaternion(newQuat.x, newQuat.y, newQuat.z, newQuat.w);
					btTransform newTransform;
					newTransform.setRotation(newRot);
					newTransform.setOrigin(origin);
					mS->setWorldTransform(newTransform);
				}
				else
				{
					transformComponent.Rotation = rotEuler;
				}
				e.Patch<TransformComponent>();
			}


		}

		// Render
		Camera PrimaryCamera;
		glm::mat4 cameraTransform;
		{
			auto group = m_Registry.group<CameraComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto [camera, transform] = group.get<CameraComponent, TransformComponent>(entity);

				if (camera.Primary)
				{
					PrimaryCamera = camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		SceneRenderer::Render(camera, this, ts);
		//JobSystem::Execute([&] {SceneRenderer::Render(camera, this, ts); });

		//JobSystem::Wait();



		// TODO: IMPLEMENT THIS ASAP!!!
		/*if (PrimaryCamera)
		{

			SceneRenderer::Render(PrimaryCamera, this, ts);
			Renderer2D::BeginScene(PrimaryCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				// Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}

			Renderer2D::EndScene();
		} */

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


	void Scene::DuplicateRecursiveChildren(Entity newParent, Entity originalParent)
	{
		auto& relC = originalParent.GetComponent<RelationshipComponent>();

		// We recursively traverse every child inside the original entity and
		// add them to the new entity
		Entity childIterator = GetEntityWithUUID(relC.first);
		// Original entity has children
		while (entt::entity(childIterator) != entt::null)
		{
			auto& childRelC = childIterator.GetComponent<RelationshipComponent>();

			Entity newChild = CreateEntity(childIterator.GetName());
			CopyComponentIfExists<TagComponent>(newChild, childIterator);
			CopyComponentIfExists<TransformComponent>(newChild, childIterator);
			CopyComponentIfExists<SpriteRendererComponent>(newChild, childIterator);
			CopyComponentIfExists<CameraComponent>(newChild, childIterator);
			CopyComponentIfExists<Rigidbody2DComponent>(newChild, childIterator);
			CopyComponentIfExists<BoxCollider2DComponent>(newChild, childIterator);
			CopyScriptComponent(newChild, childIterator);
			CopyComponentIfExists<NativeScriptComponent>(newChild, childIterator);
			CopyComponentIfExists<DirLightComponent>(newChild, childIterator);
			CopyComponentIfExists<PointLightComponent>(newChild, childIterator);
			CopyComponentIfExists<SpotLightComponent>(newChild, childIterator);
			CopyComponentIfExists<MeshComponent>(newChild, childIterator);
			CopyMaterialIfExists(newChild, childIterator);
			CopyComponentIfExists<Physics3DMaterial>(newChild, childIterator);
			CopyComponentIfExists<Physics3DCollider>(newChild, childIterator);

			// Add duplicated child to new entity
			this->AddChildEntity(newParent, newChild);

			DuplicateRecursiveChildren(newChild, childIterator);

			// Get to next child
			childIterator = GetEntityWithUUID(childRelC.next);
		}
	}

	void Scene::CopyMaterialIfExists(Entity newEntity, Entity entity)
	{
		if (entity.HasComponent<MaterialComponent>())
		{
			auto& entityMaterialComponent = entity.GetComponent<MaterialComponent>();
			Ref<MaterialInstance> entityMaterialInstance = entityMaterialComponent.MatInstance;
			Ref<Material> entityMaterial = entityMaterialInstance->Mat;

			// new Entity Material Component
			auto& nEMC = newEntity.AddOrReplaceComponent<MaterialComponent>();
			nEMC.MatInstance = MaterialInstance::Create(entityMaterial);
			nEMC.MatInstance->TilingFactor = entityMaterialInstance->TilingFactor;
			nEMC.MatInstance->HeightFactor = entityMaterialInstance->HeightFactor;
			nEMC.MatInstance->ClipBorder   = entityMaterialInstance->ClipBorder;

			uint32_t floatValCounter = 0;
			for (const auto element : entityMaterialInstance->Floats)
			{
				nEMC.MatInstance->Floats[floatValCounter] = element;
				floatValCounter++;
			}

			uint32_t float2ValCounter = 0;
			for (const auto element : entityMaterialInstance->Float2s)
			{
				nEMC.MatInstance->Float2s[float2ValCounter] = element;
				float2ValCounter++;
			}

		
			uint32_t float3ValCounter = 0;
			for (const auto element : entityMaterialInstance->Float3s)
			{
				nEMC.MatInstance->Float3s[float3ValCounter] = element;
				float3ValCounter++;
			}

			uint32_t intValCounter = 0;
			for (const auto element : entityMaterialInstance->Ints)
			{
				nEMC.MatInstance->Ints[intValCounter] = element;
				intValCounter++;
			}

			uint32_t texCounter = 0;
			for (const auto element : entityMaterialInstance->Textures)
			{
				nEMC.MatInstance->Textures[texCounter] = element;
				texCounter++;
			}

		}
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());

		auto& relC = entity.GetComponent<RelationshipComponent>();
		CopyComponentIfExists<TagComponent>            (newEntity, entity);
		CopyComponentIfExists<TransformComponent>      (newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent> (newEntity, entity);
		CopyComponentIfExists<CameraComponent>         (newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>    (newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>  (newEntity, entity);
		CopyScriptComponent                            (newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>   (newEntity, entity);
		CopyComponentIfExists<DirLightComponent>       (newEntity, entity);
		CopyComponentIfExists<SpotLightComponent>      (newEntity, entity);
		CopyComponentIfExists<PointLightComponent>	   (newEntity, entity);
		CopyComponentIfExists<MeshComponent>           (newEntity, entity);
		CopyMaterialIfExists(newEntity, entity);
		CopyComponentIfExists<Physics3DMaterial>       (newEntity, entity);
		CopyComponentIfExists<Physics3DCollider>       (newEntity, entity);

		Entity parent = GetEntityWithUUID(relC.parent);

		// We check whether the entity has a parent
		// and if it does, we add it to the children list
		// of the parent
		if (entt::entity(parent) != entt::null)
			this->AddChildEntity(parent, newEntity);


		// We recursively traverse every child inside the original entity and
		// add them to the new entity
		Entity childIterator = GetEntityWithUUID(relC.first);
		// Original entity has children
		while (entt::entity(childIterator) != entt::null)
		{
			auto& childRelC = childIterator.GetComponent<RelationshipComponent>();

			Entity newChild = CreateEntity(childIterator.GetName());
			CopyComponentIfExists<TagComponent>            (newChild, childIterator);
			CopyComponentIfExists<TransformComponent>      (newChild, childIterator);
			CopyComponentIfExists<SpriteRendererComponent> (newChild, childIterator);
			CopyComponentIfExists<CameraComponent>         (newChild, childIterator);
			CopyComponentIfExists<Rigidbody2DComponent>    (newChild, childIterator);
			CopyComponentIfExists<BoxCollider2DComponent>  (newChild, childIterator);
			CopyScriptComponent                            (newChild, childIterator);
			CopyComponentIfExists<NativeScriptComponent>   (newChild, childIterator);
			CopyComponentIfExists<DirLightComponent>       (newChild, childIterator);
			CopyComponentIfExists<SpotLightComponent>      (newChild, childIterator);
			CopyComponentIfExists<PointLightComponent>	   (newChild, childIterator);
			CopyComponentIfExists<MeshComponent>           (newChild, childIterator);
			CopyMaterialIfExists(newChild, childIterator);
			CopyComponentIfExists<Physics3DMaterial>       (newChild, childIterator);
			CopyComponentIfExists<Physics3DCollider>       (newChild, childIterator);

			// Add duplicated child to new entity
			this->AddChildEntity(newEntity, newChild);

			DuplicateRecursiveChildren(newChild, childIterator);

			// Get to next child
			childIterator = GetEntityWithUUID(childRelC.next);
		}
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
	void Scene::OnComponentAdded<OutlineComponent>(Entity entity, OutlineComponent& component) {}
	template<>
	void Scene::OnComponentAdded<RootComponent>(Entity entity, RootComponent& component){}
	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component){}
	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component){}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component){}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if(m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component){}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component){}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component){}
	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
		uint32_t sceneID = 0;
		component.PublicFields = ScriptManager::OnInitEntity(component, (uint32_t)entity, sceneID);
	}
	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component){}
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component){}
	template<>
	void Scene::OnComponentAdded<DirLightComponent>(Entity entity, DirLightComponent& component){}
	template<>
	void Scene::OnComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component){}
	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component){}
	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
		entity.AddComponent<MaterialComponent>();
	}
	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
		Ref<Material> defaultPbr = ResourceManager::GetMaterial("DefaultPbr");
		component.MatInstance = MaterialInstance::Create(defaultPbr);
	}
	template<>
	void Scene::OnComponentAdded<Physics3DMaterial>(Entity entity, Physics3DMaterial& component){}
	template<>
	void Scene::OnComponentAdded<Physics3DCollider>(Entity enttiy, Physics3DCollider& component){}


	template<typename T>
	void Scene::OnComponentRemoved(Entity entity, T& component){static_assert(false);}
	template<>
	void Scene::OnComponentRemoved<OutlineComponent>(Entity entity, OutlineComponent& component) {}
	template<>
	void Scene::OnComponentRemoved<RootComponent>(Entity entity, RootComponent& component){}
	template<>
	void Scene::OnComponentRemoved<RelationshipComponent>(Entity entity, RelationshipComponent& component){}
	template<>
	void Scene::OnComponentRemoved<IDComponent>(Entity entity, IDComponent& component){}
	template<>
	void Scene::OnComponentRemoved<TransformComponent>(Entity entity, TransformComponent& component){}
	template<>
	void Scene::OnComponentRemoved<CameraComponent>(Entity entity, CameraComponent& component){}
	template<>
	void Scene::OnComponentRemoved<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component){}
	template<>
	void Scene::OnComponentRemoved<TagComponent>(Entity entity, TagComponent& component){}
	template<>
	void Scene::OnComponentRemoved<NativeScriptComponent>(Entity entity, NativeScriptComponent& component){}
	template<>
	void Scene::OnComponentRemoved<ScriptComponent>(Entity entity, ScriptComponent& component){}
	template<>
	void Scene::OnComponentRemoved<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component){}
	template<>
	void Scene::OnComponentRemoved<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component){}
	template<>
	void Scene::OnComponentRemoved<DirLightComponent>(Entity entity, DirLightComponent& component){}
	template<>
	void Scene::OnComponentRemoved<SpotLightComponent>(Entity entity, SpotLightComponent& component){}
	template<>
	void Scene::OnComponentRemoved<PointLightComponent>(Entity entity, PointLightComponent& component){}
	template<>
	void Scene::OnComponentRemoved<MeshComponent>(Entity entity, MeshComponent& component){}
	template<>
	void Scene::OnComponentRemoved<MaterialComponent>(Entity entity, MaterialComponent& component){}
	template<>
	void Scene::OnComponentRemoved<Physics3DMaterial>(Entity entity, Physics3DMaterial& component){}
	template<>
	void Scene::OnComponentRemoved<Physics3DCollider>(Entity enttiy, Physics3DCollider& component){}
}