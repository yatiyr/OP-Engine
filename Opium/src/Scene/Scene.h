#pragma once

#include <entt.hpp>
#include <Opium/Timestep.h>

namespace Opium
{
	class Scene 
	{
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		// REMOVE THIS
		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;
	};
}
