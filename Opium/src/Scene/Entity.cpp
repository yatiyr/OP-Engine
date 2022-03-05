#include <Precomp.h>
#include <Scene/Entity.h>

namespace Opium
{
	Entity::Entity(entt::entity handle, Scene* scene, bool initialized)
		: m_EntityHandle(handle), m_Scene(scene), m_Initialized(initialized)
	{
	}

}