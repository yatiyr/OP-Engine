#include <GuiComponents/Scene/SceneUtilities/SceneHierarchyComponent.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <Scene/Components.h>
#include <Scene/SceneRenderer.h>

#include <Gui/Font/Font.h>

#include <Opium/ResourceManager.h>
#include <ScriptManager/ScriptManager.h>

namespace OP
{

	extern ImFont* ImGuiIconFontBg;
	extern ImFont* ImGuiIconFontMd;
	extern ImFont* ImGuiIconFontText;

	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyComponent::SceneHierarchyComponent(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyComponent::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyComponent::OnImGuiRender()
	{

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2, 10 });

		ImGui::Begin("Scene Graph");

		if (m_Context)
		{
			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth = ImGui::CalcTextSize(OP_ICON_INVENTORY " Scene Graph").x;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.1f);

			ImGui::PushFont(ImGuiIconFontText);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.0f, 0.6f, 1.0f));
			ImGui::Text(OP_ICON_INVENTORY);
			ImGui::PopStyleColor();
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::Text("Scene Graph");
			ImGui::Separator();

			ImGui::BeginTable("sceneGraph", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit);

			auto view = m_Context->m_Registry.view<RootComponent>();
			for (auto e : view)
			{
				Entity entity{ e, m_Context.get() };
				DrawEntityNode(entity);
			}

			ImGui::EndTable();

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				// m_SelectionContext = {};
			}

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					m_Context->CreateEntity("Empty Entity");
				}

				ImGui::EndPopup();
			}
		}

		// ImGui::PopFont();
		ImGui::End();
		ImGui::PopStyleVar();



		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoTitleBar);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::PopStyleVar();
		
		

		ImGui::End();
		ImGui::PopStyleVar();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 40));
		ImGui::Begin("SceneProperties", nullptr, ImGuiWindowFlags_NoTitleBar);

			std::unordered_map<uint32_t, Ref<EnvironmentMap>> envMaps = ResourceManager::GetEnvironmentMaps();
			std::vector<std::string> envMapNames;
			static int currentSelectedID = 0;

			for (auto& [id, m] : envMaps)
			{
				std::string envMapName = ResourceManager::GetNameFromID(id);
				envMapNames.push_back(envMapName);
			}

			std::string comboPreviewValue = "sky1";

			if (ImGui::BeginCombo("Environment", comboPreviewValue.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				for (int i = 0; i < envMapNames.size(); i++)
				{
					const bool isSelected = (currentSelectedID == i);
					if (ImGui::Selectable(envMapNames[i].c_str(), isSelected))
					{
						currentSelectedID = i;
						SceneRenderer::ChangeEnvironmentMap(envMapNames[i]);
						comboPreviewValue = envMapNames[i];
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::DragFloat("Exposure", SceneRenderer::GetExposure(), 0.01f, 0.0f, 20.0f);
			ImGui::Checkbox("ToneMap", SceneRenderer::GetHdr());

			ImGui::Text("SM Dir Time: %.1f", SceneRenderer::GetShadowMapDirPassMiliseconds());
			ImGui::Text("SM Point Time: %.1f", SceneRenderer::GetShadowMapPointPassMiliseconds());
			ImGui::Text("SM Blur Time: %.1f", SceneRenderer::GetShadowMapBlurMiliseconds());
			ImGui::Text("Final Render Time: %.1f", SceneRenderer::GetFinalRenderMiliseconds());
			ImGui::Text("Post Processing Time: %.1f", SceneRenderer::GetPostProcessingPassMiliseconds());

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SceneHierarchyComponent::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyComponent::DrawEntityNode(Entity entity)
	{

		ImGui::TableNextRow();


		ImGui::TableNextColumn();

		auto& tag = entity.GetComponent<TagComponent>().Tag;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}
		bool entityDeleted = false;
		bool entityAdded = false;
		bool entityDetached = false;
		// Right-click on entity
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Remove this Entity"))
			{
				entityDeleted = true;
			}
			
			if (ImGui::MenuItem("Add Entity"))
			{
				entityAdded = true;
			}

			if (ImGui::MenuItem("Detach Entity"))
			{
				entityDetached = true;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			auto relComp = entity.GetComponent<RelationshipComponent>();
			Entity first = m_Context->GetEntityWithUUID(relComp.first);
			if (entt::entity(first) != entt::null)
			{
				DrawEntityNode(first);
				auto firstRelComp = first.GetComponent<RelationshipComponent>();
				Entity next = m_Context->GetEntityWithUUID(firstRelComp.next);
				while (entt::entity(next) != entt::null)
				{
					DrawEntityNode(next);
					auto nextRelComp = next.GetComponent<RelationshipComponent>();
					next = m_Context->GetEntityWithUUID(nextRelComp.next);
				}
			}
			ImGui::TreePop();
		}


		if (entityDeleted)
		{
			m_Context->RemoveEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}

		if (entityAdded)
		{
			m_Context->CreateChildEntity(entity, "New Entity");
		}

		if (entityDetached)
		{
			m_Context->DetachChild(entity);
		}
		
	}


	static void DrawVec3Control(const std::string& label, glm::vec3& values, Entity& entity, float resetValue = 0.0f, float columnWidth = 100.0f)
	{

		ImFont* InterBoldFont = (ImFont*)Application::Get().GetImGuiLayer()->GetFontPtr("Inter-Bold-12");

		ImGui::PushID(label.c_str());

		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(label.c_str()).x;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.2f);
		ImGui::Text(label.c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0);

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.6f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.6f, 1.0f });

		ImGui::PushFont(InterBoldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			entity.Patch<TransformComponent>();
		}
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			entity.Patch<TransformComponent>();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.6f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.7f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.6f, 0.7f, 0.2f, 1.0f });

		ImGui::PushFont(InterBoldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			entity.Patch<TransformComponent>();
		}
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			entity.Patch<TransformComponent>();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.7f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.8f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.7f, 0.8f, 1.0f });

		ImGui::PushFont(InterBoldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			entity.Patch<TransformComponent>();
		}
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			entity.Patch<TransformComponent>();
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

	}


	static void DrawVec3RotationControl(const std::string& label, TransformComponent& component, Entity& entity, float resetValue = 0.0f, float columnWidth = 100.0f)
	{

		glm::vec3 rotation = glm::degrees(component.Rotation);

		ImFont* InterBoldFont = (ImFont*)Application::Get().GetImGuiLayer()->GetFontPtr("Inter-Bold-14");

		ImGui::PushID(label.c_str());

		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(label.c_str()).x;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.2f);
		ImGui::Text(label.c_str());
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0);

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.6f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.6f, 1.0f });

		ImGui::PushFont(InterBoldFont);
		if (ImGui::Button("X", buttonSize))
		{
			component.Rotation.x = resetValue;
			entity.Patch<TransformComponent>();
		}
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &rotation.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			component.Rotation = glm::radians(rotation);
			entity.Patch<TransformComponent>();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.6f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.7f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.6f, 0.7f, 0.2f, 1.0f });

		ImGui::PushFont(InterBoldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			component.Rotation.y = resetValue;
			entity.Patch<TransformComponent>();
		}
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &rotation.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			component.Rotation = glm::radians(rotation);
			entity.Patch<TransformComponent>();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.7f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.8f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.7f, 0.8f, 1.0f });

		ImGui::PushFont(InterBoldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			component.Rotation.z = resetValue;
			entity.Patch<TransformComponent>();
		}
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &rotation.z, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			component.Rotation = glm::radians(rotation);
			entity.Patch<TransformComponent>();
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}


	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen |
			                                     ImGuiTreeNodeFlags_AllowItemOverlap |
			                                     ImGuiTreeNodeFlags_Framed |
												 ImGuiTreeNodeFlags_SpanAvailWidth |
												 ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.5f);
	}

	void SceneHierarchyComponent::DrawComponents(Entity entity)
	{

		// Tag Field
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(OP_ICON_TRANSFORM " Object Properties").x;

		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.1f);

		ImGui::PushFont(ImGuiIconFontText);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.6f, 1.0f));
		ImGui::Text(OP_ICON_TRANSFORM);
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::Text("Object Properties");
		// ImGui::Separator();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Add"))
			ImGui::OpenPopup("AddComponent");

		ImGui::PushItemWidth(-1);


		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!m_SelectionContext.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<Rigidbody2DComponent>() &&
				!m_SelectionContext.HasComponent<Physics3DMaterial>())
			{
				if (ImGui::MenuItem("2D RigidBody"))
				{
					m_SelectionContext.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<Rigidbody2DComponent>() &&
				!m_SelectionContext.HasComponent<Physics3DMaterial>())
			{
				if (ImGui::MenuItem("3D PhysicsMaterial"))
				{
					m_SelectionContext.AddComponent<Physics3DMaterial>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>() &&
				!m_SelectionContext.HasComponent<Physics3DCollider>())
			{
				if (ImGui::MenuItem("2D Box Collider"))
				{
					m_SelectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>() &&
				!m_SelectionContext.HasComponent<Physics3DCollider>())
			{
				if (ImGui::MenuItem("3D Physics Collider"))
				{
					m_SelectionContext.AddComponent<Physics3DCollider>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<DirLightComponent>() &&
				!m_SelectionContext.HasComponent<SpotLightComponent>() &&
				!m_SelectionContext.HasComponent<PointLightComponent>())
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					m_SelectionContext.AddComponent<DirLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<SpotLightComponent>() &&
				!m_SelectionContext.HasComponent<DirLightComponent>() &&
				!m_SelectionContext.HasComponent<PointLightComponent>())
			{
				if (ImGui::MenuItem("Spot Light"))
				{
					m_SelectionContext.AddComponent<SpotLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<SpotLightComponent>() &&
				!m_SelectionContext.HasComponent<DirLightComponent>() &&
				!m_SelectionContext.HasComponent<PointLightComponent>())
			{
				if (ImGui::MenuItem("Point Light"))
				{
					m_SelectionContext.AddComponent<PointLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<MeshComponent>())
			{
				if (ImGui::MenuItem("Mesh Component"))
				{
					m_SelectionContext.AddComponent<MeshComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<MaterialComponent>())
			{
				if (ImGui::MenuItem("Material Component"))
				{
					m_SelectionContext.AddComponent<MaterialComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();


		DrawComponent<TransformComponent>("Transform", entity, [&](auto& component)
			{
				DrawVec3Control("Translation", component.Translation, entity);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0);
				DrawVec3RotationControl("Rotation", component, entity);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0);
				DrawVec3Control("Scale", component.Scale, entity, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{

					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical Fov", &verticalFov))
					{
						camera.SetPerspectiveVerticalFOV(glm::radians(verticalFov));
					}

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
					{
						camera.SetPerspectiveNearClip(perspectiveNear);
					}

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
					{
						camera.SetPerspectiveFarClip(perspectiveFar);
					}
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
					{
						camera.SetOrthographicSize(orthoSize);
					}

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
					{
						camera.SetOrthographicNearClip(orthoNear);
					}

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
					{
						camera.SetOrthographicFarClip(orthoFar);
					}

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				if (component.Texture)
					ImGui::ImageButton((ImTextureID)component.Texture.get()->GetRendererID(), {100.0f, 100.0f}, {0, 1}, {1, 0});
				else
					ImGui::Button("Texture", ImVec2(100.0f, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						component.Texture = Texture2D::Create(texturePath.string());
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});


		DrawComponent<Rigidbody2DComponent>("2D RigidBody", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic"};
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{

					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});

		DrawComponent<Physics3DMaterial>("3D Physics Material", entity, [](auto& component)
			{
				if (ImGui::DragFloat("Mass", &component.Mass, 0.01f, 0.0f, 10000000.0f))
				{
					component.OnMassChanged();
				}
				if (ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1000.0f))
				{
					component.OnFrictionChanged();
				}
				if (ImGui::DragFloat("RFriction", &component.RollingFriction, 0.01f, 0.0f, 1000.0f))
				{
					component.OnRollingFrictionChanged();
				}
				if (ImGui::DragFloat("SFriction", &component.SpinningFriction, 0.01f, 0.0f, 1000.0f))
				{
					component.OnSpinningFrictionChanged();
				}
				if (ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 100.0f))
				{
					component.OnRestitutionChanged();
				}
			});

		DrawComponent<Physics3DCollider>("3D Physics Collider", entity, [](auto& component)
			{
				int currentSelected = component.Shape;

				std::string curSelectedName;

				if (currentSelected == 0)
					curSelectedName = "Box";
				else if (currentSelected == 1)
					curSelectedName = "Sphere";

				std::vector<std::string> shapeNames = { "Box", "Sphere" };

				if (ImGui::BeginCombo("Collider", curSelectedName.c_str(), ImGuiComboFlags_PopupAlignLeft))
				{
					for (int i = 0; i < shapeNames.size(); i++)
					{
						const bool isSelected = (currentSelected == i);
						if (ImGui::Selectable(shapeNames[i].c_str(), isSelected))
						{
							currentSelected = i;
							component.Shape = i;
							component.OnShapeChanged();							
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::DragFloat3("Scale", glm::value_ptr(component.Scale)))
				{
					component.OnScaleChanged();
				}

				if (ImGui::Checkbox("Contact Response", &component.ContactResponse))
				{
					component.OnContactResponseChanged();
				}

			});

		DrawComponent<BoxCollider2DComponent>("2D Box Collider", entity, [](auto& component)
			{
				
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f);

			});


		DrawComponent<DirLightComponent>("Directional Light", entity, [](auto& component)
			{
				ImGui::DragInt("Cascades", &component.CascadeSize, 1.0, 1, 10);
				ImGui::DragFloat("Dist Factor", &component.FrustaDistFactor, 0.01f, 0.0f, 10.0f);
				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensity", &component.Intensity, 0.01f, 1.0f, 100000.0f);
				ImGui::Checkbox("Cast Shadows", &component.CastShadows);
			});


		DrawComponent<SpotLightComponent>("Spot Light", entity, [](auto& component)
			{
				ImGui::DragFloat("Cutoff", &component.Cutoff, 0.01f, 0.0f, 80.0f);
				ImGui::DragFloat("OuterCutoff", &component.OuterCutoff, 0.01f, 0.0f, 85.0f);
				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensity", &component.Intensity, 0.01f, 1.0f, 100000.0f);
				ImGui::DragFloat("Near", &component.NearDist, 0.01f, 0.01f, 5.0f);
				ImGui::DragFloat("Far", &component.FarDist, 0.01f, 0.02f, 5000.0f);
				ImGui::DragFloat("Bias", &component.Bias, 0.001f, 0.0f);
				ImGui::Checkbox("Cast Shadows", &component.CastShadows);
				
			});

		DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
			{
				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensity", &component.Intensity, 0.01f, 1.0f, 100000.0f);
				ImGui::DragFloat("Near", &component.NearDist, 0.01f, 0.01f, 5.0f);
				ImGui::DragFloat("Far", &component.FarDist, 0.01f, 0.02f, 5000.0f);
				ImGui::Checkbox("Cast Shadows", &component.CastShadows);
			});


		DrawComponent<MeshComponent>("Mesh", entity, [](auto& component)
		{
				std::unordered_map<uint32_t, Ref<Mesh>> meshes = ResourceManager::GetMeshMap();
				std::vector<std::string> meshNames;
				static int currentSelectedID = 0;

				for (auto &[id, m] : meshes)
				{
					std::string meshName = ResourceManager::GetNameFromID(id);
					meshNames.push_back(meshName);
					Ref<Mesh> mesh = m;
				}

				const char* comboPreviewValue = meshNames[currentSelectedID].c_str();

				if (ImGui::BeginCombo("Meshes", comboPreviewValue, ImGuiComboFlags_PopupAlignLeft))
				{
					for (int i = 0; i < meshNames.size(); i++)
					{
						const bool isSelected = (currentSelectedID == i);
						if (ImGui::Selectable(meshNames[i].c_str(), isSelected))
						{
							currentSelectedID = i;
							component.Mesh = ResourceManager::GetMesh(meshNames[i]);
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
		});

		DrawComponent<ScriptComponent>("Script", entity, [&](auto& component)
			{
				ImGui::Text("%s", component.ModuleName.c_str());

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path scriptPath = std::filesystem::path(g_AssetPath) / path;
						std::string fileName = scriptPath.stem().string();
						std::string extension = scriptPath.extension().string();
						if(extension == ".cs")
							component = entity.AddOrReplaceComponent<ScriptComponent>(fileName);
					}
					ImGui::EndDragDropTarget();
				}


				for (auto& [name, field] : component.PublicFields)
				{
					PublicField* pField = (PublicField*)field;

					if (pField->Type == FieldType::Int)
					{
						int val = pField->GetValue<int>();
						if (ImGui::DragInt(name.c_str(), &val))
						{
							pField->SetValue<int>(val);
						}
					}
					else if (pField->Type == FieldType::Float)
					{
						float val = pField->GetValue<float>();
						if (ImGui::DragFloat(name.c_str(), &val))
						{
							pField->SetValue<float>(val);
						}
					}
					else if (pField->Type == FieldType::UnsignedInt)
					{
						int val = pField->GetValue<uint32_t>();
						if (ImGui::DragInt(name.c_str(), &val, 1.0f, 0))
						{
							pField->SetValue<uint32_t>(val);
						}
					}
					else if (pField->Type == FieldType::Vec2)
					{
						glm::vec2 val = pField->GetValue<glm::vec2>();
						if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(val)))
						{
							pField->SetValue<glm::vec2>(val);
						}
					}
					else if (pField->Type == FieldType::Vec3)
					{
						glm::vec3 val = pField->GetValue<glm::vec3>();
						if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(val)))
						{
							pField->SetValue<glm::vec3>(val);
						}
					}
					else if (pField->Type == FieldType::Vec4)
					{
						glm::vec4 val = pField->GetValue<glm::vec4>();
						if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(val)))
						{
							pField->SetValue<glm::vec4>(val);
						}
					}
				}

			});

		DrawComponent<MaterialComponent>("Material", entity, [](auto& component)
		{
				Ref<MaterialInstance> instance = component.MatInstance;
				ImGui::Text(instance->Mat->m_Name.c_str());
				for (auto& [name, val] : instance->Float3s)
				{
					ImGui::ColorEdit3(name.c_str(), glm::value_ptr(val));
				}

				for (auto& [name, val] : instance->Float2s)
				{
					ImGui::DragFloat2(name.c_str(), glm::value_ptr(val), 0.001f, 0.000, 100.0);
				}

				for (auto& [name, val] : instance->Floats)
				{
					ImGui::DragFloat(name.c_str(), &val, 0.001f, 0.000, 100.0);
				}

				for (auto& [name, val] : instance->Ints)
				{
					ImGui::DragInt(name.c_str(), &val);
				}

				ImGui::Text("Textures - Metallic Workflow");

				ImGui::DragFloat("Texture Tiling Factor", &instance->TilingFactor, 0.01, 0.01, 10000);
				ImGui::DragFloat("Height Factor", &instance->HeightFactor, 0.001, 0.001, 1);
				ImGui::Checkbox("Clip Border", &instance->ClipBorder);
				for (auto& [name, val] : instance->Textures)
				{
					ImGui::Text(name.c_str());

					if (val)
						ImGui::ImageButton((ImTextureID)val.get()->GetRendererID(), { 100.0f, 100.0f }, { 0, 1 }, { 1, 0 });
					else
						ImGui::Button("Texture", ImVec2(100.0f, 0.0f));

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							val = Texture2D::Create(texturePath.string());
						}
						ImGui::EndDragDropTarget();
					}
				}
		});

	}
}
