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
		Component_RegisterType(Physics3DMaterial);
	}

	void InternalCallArranger::Arrange()
	{
		InitComponentTypes();

		//////////////////////////////// ARRANGE TAG COMPONENT FUNCTIONS ////////////////////////////////////////
		mono_add_internal_call("OP.TagComponent::GetTag_Native",      OP::Script::OP_Get_Tag);
		mono_add_internal_call("OP.TagComponent::SetTag_Native",      OP::Script::OP_Set_Tag);
		mono_add_internal_call("OP.TagComponent::GetTypeEnum_Native", OP::Script::OP_Get_TypeEnum);
		mono_add_internal_call("OP.TagComponent::SetTypeEnum_Native", OP::Script::OP_Set_TypeEnum);

		//////////////////////////////////// INPUT FUNCTIONS ////////////////////////////////////////////////////
		mono_add_internal_call("OP.Input::IsKeyPressed_Native",         OP::Script::OP_Input_IsKeyPressed);
		mono_add_internal_call("OP.Input::IsMouseButtonPressed_Native", OP::Script::OP_Input_IsMouseButtonPressed);

		////////////////////////////////// ARRANGE Window FUNCTIONS /////////////////////////////////////////////
		mono_add_internal_call("OP.Window::ShowCursor_Native",  OP::Script::OP_ShowCursor);
		mono_add_internal_call("OP.Window::HideCursor_Native",  OP::Script::OP_HideCursor);
		mono_add_internal_call("OP.Window::GetMousePos_Native", OP::Script::OP_GetMousePos);

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


		///////////////////////////////////// ARRANGE SCENE FUNCTIONS //////////////////////////////////////////////
			mono_add_internal_call("OP.Scene::GetSkybox_Native",   OP::Script::OP_Get_Skybox);
			mono_add_internal_call("OP.Scene::SetSkybox_Native",   OP::Script::OP_Set_Skybox);
			mono_add_internal_call("OP.Scene::GetToneMap_Native",  OP::Script::OP_Get_ToneMap);
			mono_add_internal_call("OP.Scene::SetToneMap_Native",  OP::Script::OP_Set_ToneMap);
			mono_add_internal_call("OP.Scene::GetExposure_Native", OP::Script::OP_Get_Exposure);
			mono_add_internal_call("OP.Scene::SetExposure_Native", OP::Script::OP_Set_Exposure);
			mono_add_internal_call("OP.Scene::GetScene_Native",    OP::Script::OP_Get_Scene);
			mono_add_internal_call("OP.Scene::SetScene_Native",    OP::Script::OP_Set_Scene);


		//////////////////////////// ARRANGE PHYSICS 3D MATERIAL COMPONENT FUNCTIONS ///////////////////////////////
			mono_add_internal_call("OP.Physics3DMaterial::GetMass_Native",             OP::Script::OP_Get_Mass);
			mono_add_internal_call("OP.Physics3DMaterial::SetMass_Native",             OP::Script::OP_Set_Mass);
			mono_add_internal_call("OP.Physics3DMaterial::GetFriction_Native",         OP::Script::OP_Get_Friction);
			mono_add_internal_call("OP.Physics3DMaterial::SetFriction_Native",         OP::Script::OP_Set_Friction);
			mono_add_internal_call("OP.Physics3DMaterial::GetRollingFriction_Native",  OP::Script::OP_Get_RollingFriction);
			mono_add_internal_call("OP.Physics3DMaterial::SetRollingFriction_Native",  OP::Script::OP_Set_RollingFriction);
			mono_add_internal_call("OP.Physics3DMaterial::GetSpinningFriction_Native", OP::Script::OP_Get_SpinningFriction);
			mono_add_internal_call("OP.Physics3DMaterial::SetSpinningFriction_Native", OP::Script::OP_Set_SpinningFriction);
			mono_add_internal_call("OP.Physics3DMaterial::GetRestitution_Native",      OP::Script::OP_Get_Restitution);
			mono_add_internal_call("OP.Physics3DMaterial::SetRestitution_Native",      OP::Script::OP_Set_Restitution);
			mono_add_internal_call("OP.Physics3DMaterial::GetAngularVelocity_Native",  OP::Script::OP_Get_AngularVelocity);
			mono_add_internal_call("OP.Physics3DMaterial::SetAngularVelocity_Native",  OP::Script::OP_Set_AngularVelocity);
			mono_add_internal_call("OP.Physics3DMaterial::GetLinearVelocity_Native",   OP::Script::OP_Get_LinearVelocity);
			mono_add_internal_call("OP.Physics3DMaterial::SetLinearVelocity_Native",   OP::Script::OP_Set_LinearVelocity);
			mono_add_internal_call("OP.Physics3DMaterial::ApplyCentralForce_Native",   OP::Script::OP_ApplyCentralForce);
			mono_add_internal_call("OP.Physics3DMaterial::ApplyCentralImpulse_Native", OP::Script::OP_ApplyCentralImpulse);
			mono_add_internal_call("OP.Physics3DMaterial::ApplyForce_Native",          OP::Script::OP_ApplyForce);
			mono_add_internal_call("OP.Physics3DMaterial::ApplyGravity_Native",        OP::Script::OP_ApplyGravity);
			mono_add_internal_call("OP.Physics3DMaterial::ApplyImpulse_Native",        OP::Script::OP_ApplyImpulse);
			mono_add_internal_call("OP.Physics3DMaterial::ApplyTorque_Native",         OP::Script::OP_ApplyTorque);
			mono_add_internal_call("OP.Physics3DMaterial::ApplyTorqueImpulse_Native",  OP::Script::OP_ApplyTorqueImpulse);
			mono_add_internal_call("OP.Physics3DMaterial::ClearForces_Native",         OP::Script::OP_ClearForces);							


		////////////////////////////////////////// ENTITY FUNCTIONS /////////////////////////////////////////////////
			mono_add_internal_call("OP.Entity::GetChildEntity_Native",                 OP::Script::OP_Entity_GetChild);
			mono_add_internal_call("OP.Entity::GetParentEntity_Native",                OP::Script::OP_Entity_GetParent);
			mono_add_internal_call("OP.Entity::GetTransform_Native",                   OP::Script::OP_Entity_GetTransform);
			mono_add_internal_call("OP.Entity::SetTransform_Native",                   OP::Script::OP_Entity_SetTransform);
			mono_add_internal_call("OP.Entity::CreateComponent_Native",                OP::Script::OP_Entity_CreateComponent);
			mono_add_internal_call("OP.Entity::HasComponent_Native",                   OP::Script::OP_Entity_HasComponent);
	}
}