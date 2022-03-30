#pragma once

#include <Scene/Scene.h>
#include <Opium/Core.h>
#include <Opium/Logging.h>
#include <Scene/Entity.h>
#include <Opium.h>

namespace OP
{

	class SceneHierarchyComponent
	{
	public:
		SceneHierarchyComponent() {}
		SceneHierarchyComponent(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }

		void SetSelectedEntity(Entity entity);
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}