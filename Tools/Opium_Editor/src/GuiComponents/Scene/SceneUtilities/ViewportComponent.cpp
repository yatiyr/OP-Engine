#include <Precomp.h>

#include "ViewportComponent.h"

#include <Opium/Application.h>

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <EditorLayer.h>
#include <Scene/Entity.h>

#include <glm/gtc/type_ptr.hpp>

#include <Math/Math.h>

#include <Renderer/Framebuffer.h>

namespace OP
{

	extern const std::filesystem::path  g_AssetPath;

	void ViewportComponent::OnAttach()
	{
	}

	void ViewportComponent::ResizeFramebuffer()
	{
		EditorLayer* elInstance = EditorLayer::GetEditor();
		FramebufferSpecification spec = m_Framebuffer->GetSpecification();

		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
			elInstance->m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			Entity primaryCam = elInstance->GetPrimaryCamera();
			if(primaryCam)
				elInstance->GetPrimaryCamera().GetComponent<CameraComponent>().Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			elInstance->m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);			
		}
	}

	void ViewportComponent::SetHoveredEntity()
	{
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;


		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x &&
			mouseY < (int)viewportSize.y)
		{
			EditorLayer* elInstance = EditorLayer::GetEditor();
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			elInstance->m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, elInstance->m_ActiveScene.get());
		}
	}

	bool ViewportComponent::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == (int)MouseButton::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyCode::LeftAlt))
				EditorLayer::GetEditor()->SetSelectedEntity();
		}

		return false;
	}

	void ViewportComponent::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);

		ImGui::Begin("##Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x,
								viewportMinRegion.y + viewportOffset.y };

		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x,
								viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
		{
			m_ViewportSize = { (uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y };
		}

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		HandleDragAndDrop();

		HandleImGuizmo();

		ImGui::End();


		ImGui::PopStyleVar(2);
	}

	void ViewportComponent::HandleDragAndDrop()
	{

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				EditorLayer::s_Instance->OpenScene(std::filesystem::path(g_AssetPath) / path);
			}

			ImGui::EndDragDropTarget();
		}
	}

	void ViewportComponent::HandleImGuizmo()
	{
		EditorLayer* editorInstance = EditorLayer::GetEditor();
		// TODO: Support orthographic in the future
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y,
			m_ViewportBounds[1].x - m_ViewportBounds[0].x,
			m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		// Editor Camera
		glm::mat4 cameraProjection = EditorLayer::s_Instance->m_EditorCamera.GetProjection();
		glm::mat4 cameraView = EditorLayer::s_Instance->m_EditorCamera.GetViewMatrix();

		Entity primaryCamera = editorInstance->GetPrimaryCamera();
		if (editorInstance->m_SceneState == EditorLayer::SceneState::Play && primaryCamera)
		{
			cameraProjection = primaryCamera.GetComponent<CameraComponent>().Camera.GetProjection();
			cameraView = primaryCamera.GetComponent<TransformComponent>().GetTransform();
		}

		Entity selectedEntity = editorInstance->m_SceneGraph.GetSelectedEntity();
		if (selectedEntity && editorInstance->m_GizmoType != -1)
		{

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snap
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // 0.5 m
			// Snap to 5 degrees if operation is rotation
			if (editorInstance->m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 5.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)editorInstance->m_GizmoType, ImGuizmo::LOCAL,
				glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 dRotation = rotation - tc.Rotation;

				tc.Translation = translation;
				tc.Rotation += dRotation;
				tc.Scale = scale;
			}

		}
		const float im[16] =
		{ 1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f };
		// ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), im, 100.0f);
		// ImVec2 viewManipulateBounds( 4 * m_ViewportBounds[1].x / 5, 4 * m_ViewportBounds[0].y / 5);
		ImVec2 viewManipulateBounds( m_ViewportBounds[1].x - 128, m_ViewportBounds[0].y);
		// ImGuizmo::ViewManipulate(glm::value_ptr(cameraView), 8.0f, viewManipulateBounds, ImVec2(128, 128), 0x00000000);
		

		if (ImGuizmo::IsUsing())
		{
		}
	}

}