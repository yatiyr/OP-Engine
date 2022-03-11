#pragma once

#include <ScriptManager/ScriptManager.h>
#include <Input/KeyCodes.h>

#include <glm/glm.hpp>

extern "C"
{
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
}


namespace Opium
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


		// TODO: EXTEND THIS IN THE FUTURE
	}
}