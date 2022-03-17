#pragma once

#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Renderer/Framebuffer.h>

namespace Opium
{
	class ViewportComponent
	{
	public:
		ViewportComponent() = default;
		ViewportComponent(const Ref<Framebuffer>& framebuffer)
			: m_Framebuffer(framebuffer) {}

		void OnImGuiRender();
		void SetFramebuffer(Ref<Framebuffer> framebuffer) { m_Framebuffer = framebuffer; }

		void OnAttach();
		void ResizeFramebuffer();


		// Make them private maybe?
		void SetHoveredEntity();
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

	private:
		void HandleDragAndDrop();
		void HandleImGuizmo();

	private:
		Ref<Scene> m_Context;
		Ref<Framebuffer> m_Framebuffer;
		Entity m_SelectionContext;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f}};

	};
}