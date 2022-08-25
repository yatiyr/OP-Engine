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

		mono_add_internal_call("OP.Input::IsKeyPressed_Native", OP::Script::Opium_Input_IsKeyPressed);

		/////////////////////// ARRANGE TRANSFORM COMPONENT FUNCTIONS ///////////////////////////////////////////
			mono_add_internal_call("OP.TransformComponent::GetTransform_Native",     OP::Script::OP_Get_Transform);
			mono_add_internal_call("OP.TransformComponent::SetTransform_Native",     OP::Script::OP_Set_Transform);
			mono_add_internal_call("OP.TransformComponent::GetTranslation_Native",   OP::Script::OP_Get_Translation);
			mono_add_internal_call("OP.TransformComponent::SetTranslation_Native",   OP::Script::OP_Set_Translation);
			mono_add_internal_call("OP.TransformComponent::GetRotationEuler_Native", OP::Script::OP_Get_RotationEuler);
			mono_add_internal_call("OP.TransformComponent::SetRotationEuler_Native", OP::Script::OP_Set_RotationEuler);
			mono_add_internal_call("OP.TransformComponent::GetRotationQuat_Native",  OP::Script::OP_Get_RotationQuat);
			mono_add_internal_call("OP.TransformComponent::SetRotationQuat_Native",  OP::Script::OP_Set_RotationQuat);
			mono_add_internal_call("OP.TransformComponent::GetScale_Native",         OP::Script::OP_Get_Scale);
			mono_add_internal_call("OP.TransformComponent::SetScale_Native",         OP::Script::OP_Set_Scale);


		// OLD PART
		mono_add_internal_call("OP.Entity::GetTransform_Native", OP::Script::Opium_Entity_GetTransform);
		mono_add_internal_call("OP.Entity::SetTransform_Native", OP::Script::Opium_Entity_SetTransform);
		mono_add_internal_call("OP.Entity::CreateComponent_Native", OP::Script::Opium_Entity_CreateComponent);
		mono_add_internal_call("OP.Entity::HasComponent_Native", OP::Script::Opium_Entity_HasComponent);

	
	}
}