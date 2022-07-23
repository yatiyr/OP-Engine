#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <Scene/SceneCamera.h>
// #include <Scene/ScriptableEntity.h>
#include <Opium/UUID.h>
#include <Renderer/Texture.h>


namespace OP
{

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;

	};

	struct RootComponent
	{
		bool root = true;

		RootComponent() = default;
		RootComponent(const RootComponent&) = default;
	};

	struct RelationshipComponent
	{
		size_t children = 0;

		UUID parent;
		UUID first;
		UUID prev;
		UUID next;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}


		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetView() const
		{
			return glm::inverse(glm::translate(glm::mat4(1.0f), Translation) * glm::toMat4(glm::quat(Rotation)));
		}

		glm::vec3 GetDirection() const
		{
			return glm::normalize(glm::quat(Rotation) * glm::vec3(0.0, 0.0, -1.0));
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: Color(color) {}		
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: move to scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	// Forward declaration
	class ScriptableEntity;
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;



		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct ScriptComponent
	{
		std::string ModuleName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO: move into physics material
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.5f;
		float RestitutionThreshold = 0.5f;


		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	// Directional Light Component
	struct DirLightComponent
	{
		int CascadeSize;
		float FrustaDistFactor;
		glm::vec3 Color;
		bool CastShadows;

		DirLightComponent() = default;
		DirLightComponent(const DirLightComponent&) = default;
	};

	struct SpotLightComponent
	{
		float Cutoff = 10.0f;
		float OuterCutoff = 20.0f;
		float FarDist = 100.0f;
		float NearDist = 0.01f;
		float Kq = 0.0075f;
		float Kl = 0.045f;
		float Bias;
		glm::vec3 Color = glm::vec3(0.5f, 0.5f, 0.5f);
		bool CastShadows;

		SpotLightComponent() = default;
		SpotLightComponent(const SpotLightComponent&) = default;
	};

	struct PointLightComponent
	{
		float NearDist = 0.01;
		float FarDist = 100.0f;
		float Kq = 0.0075f;
		float Kl = 0.045f;
		float Bias;
		glm::vec3 Color = glm::vec3(0.5f, 0.5f, 0.5f);
		bool CastShadows;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};
}