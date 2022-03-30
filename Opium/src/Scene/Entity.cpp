#include <Precomp.h>
#include <Scene/Entity.h>

namespace OP
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}