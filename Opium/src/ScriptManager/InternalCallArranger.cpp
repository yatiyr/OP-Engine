#include <Precomp.h>
#include <ScriptManager/InternalCallArranger.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Scene/Entity.h>
#include <Scene/Components.h>
#include <ScriptManager/Wrappers.h>



namespace OP
{
	std::unordered_map<MonoType*, std::function<bool(Entity&)>> s_HasComponentFuncs;
	std::unordered_map<MonoType*, std::function<void(Entity&)>> s_CreateComponentFuncs;


	extern MonoImage* s_CoreAssemblyImage;

#define Component_RegisterType(Type) \
	{\
		MonoType* type = mono_reflection_type_from_name("OP." #Type, s_CoreAssemblyImage);\
		if (type) {\
			uint32_t id = mono_type_get_type(type);\
			s_HasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<Type>(); };\
			s_CreateComponentFuncs[type] = [](Entity& entity) { entity.AddComponent<Type>(); };\
		} else {\
			OP_ENGINE_ERROR("No C# component class found for " #Type "!");\
		}\
	}

	static void InitComponentTypes()
	{
		Component_RegisterType(TagComponent);
		Component_RegisterType(TransformComponent);
		Component_RegisterType(SpriteRendererComponent);
	}

	void InternalCallArranger::Arrange()
	{
		InitComponentTypes();

		mono_add_internal_call("Opium.Input::IsKeyPressed_Native", OP::Script::Opium_Input_IsKeyPressed);

		mono_add_internal_call("Opium.Entity::GetTransform_Native", OP::Script::Opium_Entity_GetTransform);
		mono_add_internal_call("Opium.Entity::SetTransform_Native", OP::Script::Opium_Entity_SetTransform);
		mono_add_internal_call("Opium.Entity::CreateComponent_Native", OP::Script::Opium_Entity_CreateComponent);
		mono_add_internal_call("Opium.Entity::HasComponent_Native", OP::Script::Opium_Entity_HasComponent);

		//mono_add_internal_call("Opium.Entity::GetTransform_Native", 
	}
}