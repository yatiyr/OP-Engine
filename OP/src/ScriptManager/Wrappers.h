#pragma once

#include <ScriptManager/ScriptManager.h>
#include <Input/KeyCodes.h>
#include <Input/MouseButtonCodes.h>

#include <glm/glm.hpp>

extern "C"
{
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
}


namespace OP
{

	namespace Script
	{

		// TagComponent
		void OP_Get_Tag(uint32_t entityID, uint32_t sceneID, MonoString** str);
		void OP_Set_Tag(uint32_t entityID, uint32_t sceneID, MonoString** str);
		void OP_Get_TypeEnum(uint32_t entityID, uint32_t sceneID, int* typeEnum);
		void OP_Set_TypeEnum(uint32_t entityID, uint32_t sceneID, int* typeEnum);

		// Input wrapper
		bool OP_Input_IsKeyPressed(KeyCode key);
		bool OP_Input_IsMouseButtonPressed(MouseButtonCode mouseCode);

		// Cursor settings
		void OP_HideCursor();
		void OP_ShowCursor();
		void OP_GetMousePos(float* x, float* y);

		// Entity
		void OP_Entity_GetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* outTransform);
		void OP_Entity_SetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* inTransform);
		void OP_Entity_CreateComponent(uint32_t sceneID, uint32_t entityID, void* type);
		bool OP_Entity_HasComponent(uint32_t sceneID, uint32_t entityID, void* type);
		void OP_Entity_GetChild(uint32_t sceneID, uint32_t entityID, char* tag, MonoObject** obj);
		void OP_Entity_GetParent(uint32_t sceneID, uint32_t entityID, MonoObject** obj);

		// Transform Component
		void OP_Get_Transform(uint32_t sceneID, uint32_t entityID, glm::mat4* outTransform);
		void OP_Set_Transform(uint32_t sceneID, uint32_t entityID, glm::mat4* inTransform);
		void OP_Get_Translation(uint32_t sceneID, uint32_t entityID, glm::vec3* outTranslation);
		void OP_Set_Translation(uint32_t sceneID, uint32_t entityID, glm::vec3* inTranslation);
		void OP_Get_RotationEuler(uint32_t sceneID, uint32_t entityID, glm::vec3* outRotationEuler);
		void OP_Set_RotationEuler(uint32_t sceneID, uint32_t entityID, glm::vec3* inRotationEuler);
		void OP_Get_RotationQuat(uint32_t sceneID, uint32_t entityID, glm::quat* outRotationQuat);
		void OP_Set_RotationQuat(uint32_t sceneID, uint32_t entityID, glm::quat* inRotationQuat);
		void OP_Get_Scale(uint32_t sceneID, uint32_t entityID, glm::vec3* outScale);
		void OP_Set_Scale(uint32_t sceneID, uint32_t entityID, glm::vec3* inScale);
		void OP_RotateFromTwoVectors(uint32_t sceneID, uint32_t entityID, glm::vec3* axis1, glm::vec3* axis2);
		// Scene
		void OP_Get_Skybox(char* SkyboxName);
		void OP_Set_Skybox(char* SkyboxName);
		void OP_Get_ToneMap(bool* result);
		void OP_Set_ToneMap(bool result);
		void OP_Get_Exposure(float* result);
		void OP_Set_Exposure(float result);
		void OP_Get_Scene(char* SceneName);
		void OP_Set_Scene(char* SceneName);

		// Physics3DMaterial Component
		void OP_Get_Mass(uint32_t sceneID, uint32_t entityID, float* mass);
		void OP_Set_Mass(uint32_t sceneID, uint32_t entityID, float* mass);
		void OP_Get_Friction(uint32_t sceneID, uint32_t entityID, float* friction);
		void OP_Set_Friction(uint32_t sceneID, uint32_t entityID, float* friction);
		void OP_Get_RollingFriction(uint32_t sceneID, uint32_t entityID, float* rollingFriction);
		void OP_Set_RollingFriction(uint32_t sceneID, uint32_t entityID, float* rollingFriction);
		void OP_Get_SpinningFriction(uint32_t sceneID, uint32_t entityID, float* spinningFriction);
		void OP_Set_SpinningFriction(uint32_t sceneID, uint32_t entityID, float* spinningFriction);
		void OP_Get_Restitution(uint32_t sceneID, uint32_t entityID, float* restitution);
		void OP_Set_Restitution(uint32_t sceneID, uint32_t entityID, float* restitution);
		void OP_Get_AngularVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* outAngularVel);
		void OP_Set_AngularVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* inAngularVel);
		void OP_Get_LinearVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* outLinearVel);
		void OP_Set_LinearVelocity(uint32_t sceneID, uint32_t entityID, glm::vec3* inLinearVel);

		void OP_ApplyCentralForce(uint32_t sceneID, uint32_t entityID, float fX, float fY, float fZ);
		void OP_ApplyCentralImpulse(uint32_t sceneID, uint32_t entityID, float iX, float iY, float iZ);
		void OP_ApplyForce(uint32_t sceneID, uint32_t entityID, float fX, float fY, float fZ,
																float pX, float pY, float pZ);
		void OP_ApplyGravity(uint32_t sceneID, uint32_t entityID);
		void OP_ApplyImpulse(uint32_t sceneID, uint32_t entityID, float iX, float iY, float iZ,
																  float pX, float pY, float pZ);
		void OP_ApplyTorque(uint32_t sceneID, uint32_t entityID, float tX, float tY, float tZ);
		void OP_ApplyTorqueImpulse(uint32_t sceneID, uint32_t entityID, float tX, float tY, float tZ);
		void OP_ClearForces(uint32_t sceneID, uint32_t entityID);

		

		// Camera Component
		void OP_GetProjectionType(uint32_t sceneID, uint32_t entityID, int* pType);
		void OP_SetProjectionType(uint32_t sceneID, uint32_t entityID, int* pType);
		
		void OP_GetPrimary(uint32_t sceneID, uint32_t entityID, bool* primary);
		void OP_SetPrimary(uint32_t sceneID, uint32_t entityID, bool* primary);

		void OP_GetFixedAspectRatio(uint32_t sceneID, uint32_t entityID, bool* fixedAspectRatio);
		void OP_SetFixedAspectRatio(uint32_t sceneID, uint32_t entityID, bool* fixedAspectRatio);

		void OP_GetPerspectiveVerticalFOV(uint32_t sceneID, uint32_t entityID, float* vertFOV);
		void OP_SetPerspectiveVerticalFOV(uint32_t sceneID, uint32_t entityID, float* vertFOV);

		void OP_GetPerspectiveNearClip(uint32_t sceneID, uint32_t entityID, float* persNear);
		void OP_SetPerspectiveNearClip(uint32_t sceneID, uint32_t entityID, float* persNear);

		void OP_GetPerspectiveFarClip(uint32_t sceneID, uint32_t entityID, float* persFar);
		void OP_SetPerspectiveFarClip(uint32_t sceneID, uint32_t entityID, float* persFar);

		void OP_GetOrthographicSize(uint32_t sceneID, uint32_t entityID, float* orthoSize);
		void OP_SetOrthographicSize(uint32_t sceneID, uint32_t entityID, float* orthoSize);

		void OP_GetOrthographicNearClip(uint32_t sceneID, uint32_t entityID, float* orthoNear);
		void OP_SetOrthographicNearClip(uint32_t sceneID, uint32_t entityID, float* orthoNear);

		void OP_GetOrthographicFarClip(uint32_t sceneID, uint32_t entityID, float* orthoFar);
		void OP_SetOrthographicFarClip(uint32_t sceneID, uint32_t entityID, float* orthoFar);

	}
}