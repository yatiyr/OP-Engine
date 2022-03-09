#pragma once

#include <Scene/Scene.h>
#include <Opium/Core.h>
#include <Opium/Logging.h>
#include <Scene/Entity.h>
#include <Opium.h>

namespace Opium
{
	class SceneHierarchyComponent
	{
	public:
		SceneHierarchyComponent() {}
		SceneHierarchyComponent(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}