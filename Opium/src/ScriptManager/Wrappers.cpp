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