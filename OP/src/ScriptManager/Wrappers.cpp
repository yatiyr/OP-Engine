#include <Precomp.h>
#include <ScriptManager/Wrappers.h>

#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Components.h>

#include <glm/gtc/type_ptr.hpp>

#include <Input/Input.h>
#include <mono/jit/jit.h>

#include <Op/Application.h>

namespace OP
{
	//extern std::unordered_map<uint32_t, Scene*> s_ActiveScenes;
	// we have one active scene for now
	extern Scene* s_ActiveScene;
	extern std::unordered_map<MonoType*, std::function<bool(Entity&)>> s_HasComponentFuncs;
	extern std::unordered_map<MonoType*, std::function<void(Entity&)>> s_CreateComponentFuncs;

	namespace Script
	{
		enum class ComponentID
		{
			None = 0,
			Transform = 1,
			Script = 2,
			SpriteRenderer = 3
		};


		void OP_Get_Tag(uint32_t entityID, uint32_t sceneID, MonoString** str)
		{
			Entity e{ (entt::entity)entityID, s_ActiveScene };

			auto& tC = e.GetComponent<TagComponent>();

			MonoDomain* domain = ScriptManager::GetDomain();

			MonoString* string = mono_string_new(domain, tC.Tag.c_str());

			*str = string;

		}

		void OP_Set_Tag(uint32_t entityID, uint32_t sceneID, MonoString** str)
		{
			Entity e{ (entt::entity)entityID, s_ActiveScene };

			auto& tC = e.GetComponent<TagComponent>();

			MonoString* string = *str;
			tC.Tag = mono_string_to_utf8(string);			
		}

		void OP_Get_TypeEnum(uint32_t entityID, uint32_t sceneID, int* typeEnum)
		{
			Entity e{ (entt::entity)entityID, s_ActiveScene };
			auto& tC = e.GetComponent<TagComponent>();

			*typeEnum = tC.TypeEnum;
		}

		void OP_Set_TypeEnum(uint32_t entityID, uint32_t sceneID, int* typeEnum)
		{
			Entity e{ (entt::entity)entityID, s_ActiveScene };
			auto& tC = e.GetComponent<TagComponent>();

			tC.TypeEnum = *typeEnum;
		}

		// Input Wrapper
		bool OP_Input_IsKeyPressed(KeyCode key)
		{
			return Input::IsKeyPressed(key);
		}

		bool OP_Input_IsMouseButtonPressed(MouseButtonCode mouseCode)
		{
			return Input::IsMouseButtonPressed(mouseCode);
		}

		//////////////////////////////////// TRANSFORM COMPONENT WRAPPERS ///////////////////////////////////////////
		void OP_Get_Transform(uint32_t sceneID, uint32_t entityID, glm::mat4* outTransform)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::mat4 transform = transformComponent.GetTransform();
			memcpy(outTransform, glm::value_ptr(transform), sizeof(glm::mat4));
		}

		void OP_Set_Transform(uint32_t sceneID, uint32_t entityID, glm::mat4* inTransform)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			transformComponent.SetTransform(*inTransform);
			entity.Patch<TransformComponent>();
		}

		void OP_Get_Translation(uint32_t sceneID, uint32_t entityID, glm::vec3* outTranslation)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::vec3 translation = transformComponent.Translation;
			memcpy(outTranslation, glm::value_ptr(translation), sizeof(glm::vec3));
		}

		void OP_Set_Translation(uint32_t sceneID, uint32_t entityID, glm::vec3* inTranslation)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			transformComponent.Translation = *inTranslation;
			entity.Patch<TransformComponent>();
		}

		void OP_GetDirection(uint32_t sceneID, uint32_t entityID, glm::vec3* outDirection)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::vec3 direction = transformComponent.GetDirection();
			memcpy(outDirection, glm::value_ptr(direction), sizeof(glm::vec3));
		}

		void OP_Get_RotationEuler(uint32_t sceneID, uint32_t entityID, glm::vec3* outRotationEuler)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::vec3 rotEuler = transformComponent.Rotation;
			memcpy(outRotationEuler, glm::value_ptr(rotEuler), sizeof(glm::vec3));
		}

		void OP_Set_RotationEuler(uint32_t sceneID, uint32_t entityID, glm::vec3* inRotationEuler)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			transformComponent.Rotation = *inRotationEuler;
			entity.Patch<TransformComponent>();
		}

		void OP_Get_RotationQuat(uint32_t sceneID, uint32_t entityID, glm::quat* outRotationQuat)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::quat rotQuat = glm::quat(transformComponent.Rotation);
			memcpy(outRotationQuat, glm::value_ptr(rotQuat), sizeof(glm::quat));

		}

		void OP_Set_RotationQuat(uint32_t sceneID, uint32_t entityID, glm::quat* inRotationQuat)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			transformComponent.Rotation = glm::eulerAngles(*inRotationQuat);
			entity.Patch<TransformComponent>();
		}

		void OP_Get_Scale(uint32_t sceneID, uint32_t entityID, glm::vec3* outScale)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::vec3 scale = transformComponent.Scale;
			memcpy(outScale, glm::value_ptr(scale), sizeof(glm::vec3));
		}
		
		void OP_Set_Scale(uint32_t sceneID, uint32_t entityID, glm::vec3* inScale)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			transformComponent.Scale = *inScale;
			entity.Patch<TransformComponent>();
		}

		void OP_RotateFromTwoVectors(uint32_t sceneID, uint32_t entityID, glm::vec3* axis1, glm::vec3* axis2)
		{
			Entity entity((entt::entity)entityID, s_ActiveScene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::quat rot = glm::quat(*axis1, *axis2);
			transformComponent.RuntimeControlledRotation = glm::eulerAngles(rot);
			//entity.Patch<TransformComponent>();
		}

		///////////////////////////////////////// SCENE WRAPPERS //////////////////////////////////////////
		void OP_Get_Skybox(char* SkyboxName)
		{
			std::string sNameStdStr = s_ActiveScene->GetSkybox();
			uint32_t size = sNameStdStr.size();

			const char* sName = sNameStdStr.c_str();
			memcpy(SkyboxName, sName, size * sizeof(char));
		}
		
		void OP_Set_Skybox(char* SkyboxName)
		{
			char buffer[256];
			memset(buffer, 0x00, 256);
			memcpy(&buffer, SkyboxName, 256);
			std::string str(buffer);
			s_ActiveScene->SetSkybox(str);
		}

		void OP_Get_ToneMap(bool* result)
		{
			*result = s_ActiveScene->GetToneMap();
		}

		void OP_Set_ToneMap(bool result)
		{
			s_ActiveScene->SetToneMap(result);
		}

		void OP_Get_Exposure(float* result)
		{
			*result = s_ActiveScene->GetExposure();
		}

		void OP_Set_Exposure(float result)
		{
			s_ActiveScene->SetExposure(result);
		}

		void OP_Get_Scene(char* SceneName)
		{
			// TODO: TO BE IMPLEMENTED
		}

		void OP_Set_Scene(char* SceneName)
		{
			// TODO: TO BE IMPLEMENTED
		}

		void OP_Get_Mass(uint32_t sceneID, uint32_t entityID, float* mass)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			*mass = physics3DMaterial.Mass;
		}

		void OP_Set_Mass(uint32_t sceneID, uint32_t entityID, float* mass)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			physics3DMaterial.Mass = *mass;
			physics3DMaterial.OnMassChanged();
		}

		void OP_Get_Friction(uint32_t sceneID, uint32_t entityID, float* friction)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			*friction = physics3DMaterial.Friction;
		}

		void OP_Set_Friction(uint32_t sceneID, uint32_t entityID, float* friction)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			physics3DMaterial.Friction = *friction;
			physics3DMaterial.OnFrictionChanged();
		}

		void OP_Get_RollingFriction(uint32_t sceneID, uint32_t entityID, float* rollingFriction)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			*rollingFriction = physics3DMaterial.RollingFriction;
		}

		void OP_Set_RollingFriction(uint32_t sceneID, uint32_t entityID, float* rollingFriction)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			physics3DMaterial.RollingFriction = *rollingFriction;
			physics3DMaterial.OnRollingFrictionChanged();
		}

		void OP_Get_SpinningFriction(uint32_t sceneID, uint32_t entityID, float* spinningFriction)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			*spinningFriction = physics3DMaterial.SpinningFriction;
		}

		void OP_Set_SpinningFriction(uint32_t sceneID, uint32_t entityID, float* spinningFriction)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			physics3DMaterial.SpinningFriction = *spinningFriction;
			physics3DMaterial.OnSpinningFrictionChanged();
		}

		void OP_Get_Restitution(uint32_t sceneID, uint32_t entityID, float* restitution)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			*restitution = physics3DMaterial.Restitution;
		}

		void OP_Set_Restitution(uint32_t sceneID, uint32_t entityID, float* restitution)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			physics3DMaterial.Restitution = *restitution;
			physics3DMaterial.OnRestitutionChanged();
		}

		void OP_Get_AngularVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* outAngularVel)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 vel = body->getAngularVelocity();
			glm::vec3 velGlm = glm::vec3(vel.x(), vel.y(), vel.z());

			memcpy(outAngularVel, glm::value_ptr(velGlm), sizeof(glm::vec3));
		}

		void OP_Set_AngularVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* inAngularVel)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 newVel(inAngularVel->x, inAngularVel->y, inAngularVel->z);
			body->setAngularVelocity(newVel);
		}

		void OP_Get_LinearVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* outLinearVel)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 vel = body->getLinearVelocity();
			glm::vec3 velGlm = glm::vec3(vel.x(), vel.y(), vel.z());

			memcpy(outLinearVel, glm::value_ptr(velGlm), sizeof(glm::vec3));
		}

		void OP_Set_LinearVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* inLinearVel)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 newVel(inLinearVel->x, inLinearVel->y, inLinearVel->z);
			body->setLinearVelocity(newVel);
		}

		void OP_ApplyCentralForce(uint32_t sceneID, uint32_t entityID, float fX, float fY, float fZ)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 force(fX, fY, fZ);
			body->applyCentralForce(force);
		}

		void OP_ApplyCentralImpulse(uint32_t sceneID, uint32_t entityID, float iX, float iY, float iZ)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 impulse(iX, iY, iZ);
			body->applyCentralImpulse(impulse);
		}

		void OP_ApplyForce(uint32_t sceneID, uint32_t entityID, float fX, float fY, float fZ, float pX, float pY, float pZ)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 force(fX, fY, fZ);
			btVector3 point(pX, pY, pZ);
			body->applyForce(force, point);
		}

		void OP_ApplyGravity(uint32_t sceneID, uint32_t entityID)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			body->applyGravity();
		}

		void OP_ApplyImpulse(uint32_t sceneID, uint32_t entityID, float iX, float iY, float iZ, float pX, float pY, float pZ)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 impulse(iX, iY, iZ);
			btVector3 point(pX, pY, pZ);
			body->applyImpulse(impulse, point);
		}

		void OP_ApplyTorque(uint32_t sceneID, uint32_t entityID, float tX, float tY, float tZ)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 torque(tX, tY, tZ);
			body->applyTorque(torque);
		}

		void OP_ApplyTorqueImpulse(uint32_t sceneID, uint32_t entityID, float tX, float tY, float tZ)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			btVector3 torqueImpulse(tX, tY, tZ);
			body->applyTorqueImpulse(torqueImpulse);
		}

		void OP_ClearForces(uint32_t sceneID, uint32_t entityID)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			btRigidBody* body = (btRigidBody*)physics3DMaterial.RuntimeBody;
			body->clearForces();
		}

		void OP_GetProjectionType(uint32_t sceneID, uint32_t entityID, int* pType)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*pType = (int)cameraComponent.Camera.GetProjectionType();
		}

		void OP_SetProjectionType(uint32_t sceneID, uint32_t entityID, int* pType)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Camera.SetProjectionType((SceneCamera::ProjectionType) * pType);
		}

		void OP_GetPrimary(uint32_t sceneID, uint32_t entityID, bool* primary)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*primary = cameraComponent.Primary;
		}

		void OP_SetPrimary(uint32_t sceneID, uint32_t entityID, bool* primary)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Primary = *primary;
		}

		void OP_GetFixedAspectRatio(uint32_t sceneID, uint32_t entityID, bool* fixedAspectRatio)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*fixedAspectRatio = cameraComponent.FixedAspectRatio;
		}

		void OP_SetFixedAspectRatio(uint32_t sceneID, uint32_t entityID, bool* fixedAspectRatio)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.FixedAspectRatio = *fixedAspectRatio;
		}

		void OP_GetPerspectiveVerticalFOV(uint32_t sceneID, uint32_t entityID, float* vertFOV)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*vertFOV = cameraComponent.Camera.GetPerspectiveVerticalFOV();
		}

		void OP_SetPerspectiveVerticalFOV(uint32_t sceneID, uint32_t entityID, float* vertFOV)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Camera.SetPerspectiveVerticalFOV(*vertFOV);
		}

		void OP_GetPerspectiveNearClip(uint32_t sceneID, uint32_t entityID, float* persNear)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*persNear = cameraComponent.Camera.GetPerspectiveNearClip();
		}

		void OP_SetPerspectiveNearClip(uint32_t sceneID, uint32_t entityID, float* persNear)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Camera.SetPerspectiveNearClip(*persNear);
		}

		void OP_GetPerspectiveFarClip(uint32_t sceneID, uint32_t entityID, float* persFar)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*persFar = cameraComponent.Camera.GetPerspectiveFarClip();
		}

		void OP_SetPerspectiveFarClip(uint32_t sceneID, uint32_t entityID, float* persFar)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Camera.SetPerspectiveFarClip(*persFar);
		}

		void OP_GetOrthographicSize(uint32_t sceneID, uint32_t entityID, float* orthoSize)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*orthoSize = cameraComponent.Camera.GetOrthographicSize();
		}

		void OP_SetOrthographicSize(uint32_t sceneID, uint32_t entityID, float* orthoSize)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Camera.SetOrthographicSize(*orthoSize);
		}

		void OP_GetOrthographicNearClip(uint32_t sceneID, uint32_t entityID, float* orthoNear)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*orthoNear = cameraComponent.Camera.GetOrthographicNearClip();
		}

		void OP_SetOrthographicNearClip(uint32_t sceneID, uint32_t entityID, float* orthoNear)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Camera.SetOrthographicNearClip(*orthoNear);
		}

		void OP_GetOrthographicFarClip(uint32_t sceneID, uint32_t entityID, float* orthoFar)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			*orthoFar = cameraComponent.Camera.GetOrthographicFarClip();
		}

		void OP_SetOrthographicFarClip(uint32_t sceneID, uint32_t entityID, float* orthoFar)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			cameraComponent.Camera.SetOrthographicNearClip(*orthoFar);
		}

		void OP_HideCursor()
		{
			Application::Get().GetWindow().HideMouseCursor();
		}

		void OP_ShowCursor()
		{
			Application::Get().GetWindow().ShowMouseCursor();
		}

		void OP_GetMousePos(float* x, float* y)
		{
			auto [posX, posY] = Input::GetMousePos();
			*x = posX;
			*y = posY;
		}
		 
		
		// Entity wrappers
		void OP_Entity_GetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* outTransform)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::mat4 transform = transformComponent.GetTransform();
			memcpy(outTransform, glm::value_ptr(transform), sizeof(glm::mat4));
		}

		void OP_Entity_GetChild(uint32_t sceneID, uint32_t entityID, char* tag, MonoObject** obj)
		{
			char buffer[256];
			//memset(buffer, 0x00, 256);
			memcpy(&buffer, tag, 256);
			std::string tagStr(buffer);
			Entity entity((entt::entity)entityID, s_ActiveScene);

			Entity childEntity = entity.GetChild(tagStr);
			uint32_t childHandle = childEntity.GetEntityHandle();
			std::unordered_map< uint32_t, EntityInstance> iM = ScriptManager::GetInstanceMap();

			auto& entityInstance = iM[childHandle];

			*obj = entityInstance.GetInstance();
		}

		void OP_Entity_GetParent(uint32_t sceneID, uint32_t entityID, MonoObject** obj)
		{
			Entity entity((entt::entity)entityID, s_ActiveScene);

			Entity parentEntity = entity.GetParent();

			uint32_t parentHandle = parentEntity.GetEntityHandle();
			std::unordered_map< uint32_t, EntityInstance> iM = ScriptManager::GetInstanceMap();
			auto& entityInstance = iM[parentHandle];
			*obj = entityInstance.GetInstance();
		}

		void OP_Entity_SetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* inTransform)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			
			// for now set transform to glm::mat4(1.0f) TODO: CHANGE THIS!!
			glm::mat4 newTransform = glm::mat4(1.0f);
			memcpy(glm::value_ptr(newTransform), inTransform, sizeof(glm::mat4));
		}

		void OP_Entity_CreateComponent(uint32_t sceneID, uint32_t entityID, void* type)
		{
			MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);

			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			s_CreateComponentFuncs[monoType](entity);
		}

		bool OP_Entity_HasComponent(uint32_t sceneID, uint32_t entityID, void* type)
		{
			MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);

			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			bool result = s_HasComponentFuncs[monoType](entity);
			return result;
		}
	}
}