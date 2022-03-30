#pragma once

#include <Scene/Scene.h>
#include <EditorLayer.h>

namespace OP
{
	class SceneUI
	{
	public:
		SceneUI() {}
		SceneUI(const Ref<Scene&> scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		void OnKeyPressed();

	private:
		Ref<Scene> m_Context;

		
	};
}