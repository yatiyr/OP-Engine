#include <Precomp.h>
#include <ScriptManager/Wrappers.h>

#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Components.h>

#include <glm/gtc/type_ptr.hpp>

#include <Input/Input.h>
#include <mono/jit/jit.h>

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


		// Input Wrapper
		bool Opium_Input_IsKeyPressed(KeyCode key)
		{
			return Input::IsKeyPressed(key);
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
		 
		
		// Entity wrappers
		void Opium_Entity_GetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* outTransform)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			glm::mat4 transform = transformComponent.GetTransform();
			memcpy(outTransform, glm::value_ptr(transform), sizeof(glm::mat4));
		}

		void Opium_Entity_SetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* inTransform)
		{
			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			
			// for now set transform to glm::mat4(1.0f) TODO: CHANGE THIS!!
			glm::mat4 newTransform = glm::mat4(1.0f);
			memcpy(glm::value_ptr(newTransform), inTransform, sizeof(glm::mat4));
		}

		void Opium_Entity_CreateComponent(uint32_t sceneID, uint32_t entityID, void* type)
		{
			MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);

			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			s_CreateComponentFuncs[monoType](entity);
		}

		bool Opium_Entity_HasComponent(uint32_t sceneID, uint32_t entityID, void* type)
		{
			MonoType* monoType = mono_reflection_type_get_type((MonoReflectionType*)type);

			Scene* scene = s_ActiveScene;
			Entity entity((entt::entity)entityID, scene);
			bool result = s_HasComponentFuncs[monoType](entity);
			return result;
		}
	}
}