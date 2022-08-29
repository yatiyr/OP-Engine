#pragma once

#include <ScriptManager/ScriptManager.h>
#include <Input/KeyCodes.h>

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

		// Input wrapper
		bool Opium_Input_IsKeyPressed(KeyCode key);

		// Entity
		void Opium_Entity_GetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* outTransform);
		void Opium_Entity_SetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* inTransform);
		void Opium_Entity_CreateComponent(uint32_t sceneID, uint32_t entityID, void* type);
		bool Opium_Entity_HasComponent(uint32_t sceneID, uint32_t entityID, void* type);

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

		// Scene
		void OP_Get_Skybox(char* SkyboxName);
		void OP_Set_Skybox(char* SkyboxName);
		void OP_Get_ToneMap(bool* result);
		void OP_Set_ToneMap(bool result);
		void OP_Get_Exposure(float* result);
		void OP_Set_Exposure(float result);
		void OP_Get_Scene(char* SceneName);
		void OP_Set_Scene(char* SceneName);
		
		
	}
}