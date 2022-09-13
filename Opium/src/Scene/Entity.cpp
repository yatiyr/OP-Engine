#include <Precomp.h>
#include <Scene/Entity.h>

namespace OP
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	Entity Entity::GetChild(std::string tag)
	{
		auto& relC = GetComponent<RelationshipComponent>();
		UUID iterator;
		iterator = relC.first;

		Entity iteratorEntity = m_Scene->GetEntityWithUUID(iterator);

		while (entt::entity(iteratorEntity) != entt::null)
		{
			auto& childRelC = iteratorEntity.GetComponent<RelationshipComponent>();
			auto& tagC = iteratorEntity.GetComponent<TagComponent>();
			if (tag == tagC.Tag)
				return iteratorEntity;

			iteratorEntity = m_Scene->GetEntityWithUUID(childRelC.next);
		}

		return Entity();
	}

	Entity Entity::GetParent()
	{
		auto& relC = GetComponent<RelationshipComponent>();		
		UUID parentUUID = relC.parent;

		return m_Scene->GetEntityWithUUID(parentUUID);
	}

}