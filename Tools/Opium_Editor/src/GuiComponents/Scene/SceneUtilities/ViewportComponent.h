#pragma once

#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Renderer/Framebuffer.h>

namespace OP
{
	class ViewportComponent
	{
	public:
		ViewportComponent() = default;
		ViewportComponent(const Ref<Framebuffer>& framebuffer, const Ref<Framebuffer>& entityIDFramebuffer)
			: m_Framebuffer(framebuffer), m_EntityIDFramebuffer(entityIDFramebuffer) {}

		void OnImGuiRender();
		void SetFramebuffer(Ref<Framebuffer> framebuffer, Ref<Framebuffer> entityIDFramebuffer) { m_Framebuffer = framebuffer; m_EntityIDFramebuffer = entityIDFramebuffer; }

		void OnAttach();
		void ResizeFramebuffer();

		void ChangeSelection(Entity newSelection);

		// Make them private maybe?
		void SetSelectionContext(Entity entity);
		void SetHoveredEntity();
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		glm::vec2 GetViewportSize() { return m_ViewportSize; }

		void SetContext(Ref<Scene> scene) { m_Context = scene; }

	private:
		void HandleDragAndDrop();
		void HandleImGuizmo();

	private:
		Ref<Scene> m_Context;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Framebuffer> m_EntityIDFramebuffer;
		Entity m_SelectionContext;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f}};


		friend class EditorLayer;
		friend class SceneHierarchyComponent;

	};
}