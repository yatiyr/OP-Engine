#include <Precomp.h>
#include <ScriptManager/Wrappers.h>

#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Components.h>

#include <glm/gtc/type_ptr.hpp>

#include <Input/Input.h>
#include <mono/jit/jit.h>

namespace Opium
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