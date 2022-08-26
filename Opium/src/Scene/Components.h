#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <Scene/SceneCamera.h>
// #include <Scene/ScriptableEntity.h>
#include <Opium/UUID.h>
#include <Renderer/Texture.h>

#include <Math/Math.h>

#include <Geometry/Mesh.h>
#include <Renderer/Material.h>


#include <PhysicsManager/PhysicsManager.h>

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
		bool dirty = false;

		glm::vec3 LocalTranslation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalRotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalScale = { 1.0f, 1.0f, 1.0f };

		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };


		glm::mat4 globalTransformation = glm::mat4(1.0f);
		glm::mat4 globalTransformationInv = glm::mat4(1.0f);

		glm::mat4 localTransformation = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}



		void computeModelMatrix()
		{
			globalTransformation = GetTransform();
			globalTransformationInv = glm::inverse(globalTransformation);
		}

		void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix, const glm::mat4& parentGlobalTransformInv)
		{
			if (dirty)
			{
				globalTransformation = GetTransform();
				globalTransformationInv = glm::inverse(globalTransformation);
				computeLocalTransform(parentGlobalTransformInv);
				dirty = false;
			}
			else
			{
				globalTransformation = parentGlobalModelMatrix * localTransformation;
				globalTransformationInv = glm::inverse(globalTransformation);
				Math::DecomposeTransform(globalTransformation, Translation, Rotation, Scale);
			}

		}

		void computeLocalTransform(const glm::mat4& parentGlobalTransformInv)
		{
			localTransformation = parentGlobalTransformInv * globalTransformation;
		}

		glm::mat4 GetLocalTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(LocalRotation));

			return glm::translate(glm::mat4(1.0f), LocalTranslation)
				* rotation
				* glm::scale(glm::mat4(1.0f), LocalScale);
		}

		void SetTransform(glm::mat4 transform)
		{
			Math::DecomposeTransform(transform, Translation, Rotation, Scale);
		}

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
		std::string ModuleName = "NullScript";
		std::unordered_map<std::string, void*> PublicFields;

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

	struct Rigidbody3DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		
	};

	// Directional Light Component
	struct DirLightComponent
	{
		int CascadeSize;
		float FrustaDistFactor;
		float Intensity = 1.0f;
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
		float Intensity = 1.0f;
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
		float Intensity = 1.0f;
		glm::vec3 Color = glm::vec3(0.5f, 0.5f, 0.5f);
		bool CastShadows;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};

	struct MeshComponent
	{
		std::string MeshName;
		Ref<Mesh> Mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};

	struct MaterialComponent
	{
		Ref<MaterialInstance> MatInstance;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

	struct Physics3DMaterial
	{
		// Motion state will give us physical world coordinates and orientation of
		// the entity
		void* RuntimeMotionState = nullptr;
		void* RuntimeBody = nullptr;

		float Mass = 1.0;
		float Friction = 0.1f;
		float RollingFriction = 0.1f;
		float SpinningFriction = 0.1f;
		float Restitution = 0.5f;

		void OnMassChanged()
		{
			if (RuntimeBody != nullptr)
			{
				btRigidBody* rB = (btRigidBody*)RuntimeBody;
				rB->setMassProps(Mass, rB->getLocalInertia());
			}
			
		}

		void OnFrictionChanged()
		{
			if (RuntimeBody != nullptr)
			{
				btRigidBody* rB = (btRigidBody*)RuntimeBody;
				rB->setFriction(Friction);
			}
		}

		void OnRollingFrictionChanged()
		{
			if (RuntimeBody != nullptr)
			{
				btRigidBody* rB = (btRigidBody*)RuntimeBody;
				rB->setRollingFriction(RollingFriction);
			}
		}

		void OnSpinningFrictionChanged()
		{
			if (RuntimeBody != nullptr)
			{
				btRigidBody* rB = (btRigidBody*)RuntimeBody;
				rB->setSpinningFriction(SpinningFriction);
			}
		}

		void OnRestitutionChanged()
		{
			if (RuntimeBody != nullptr)
			{
				btRigidBody* rB = (btRigidBody*)RuntimeBody;
				rB->setRestitution(Restitution);
			}
		}

		Physics3DMaterial() = default;
		Physics3DMaterial(const Physics3DMaterial&) = default;
	};

	struct Physics3DCollider
	{
		// Shape = 0 -> Box
		// Shape = 1 -> Sphere
		int Shape = 0;

		// Scale for box, radius for sphere
		glm::vec3 Scale = glm::vec3(1.0, 1.0, 1.0);
		float Radius = 1.0f;

		// If this is false, there will be no force applied
		// on contact. Objects can go through it
		bool ContactResponse = true;

		// Runtime Shape
		void* RuntimeBody = nullptr;
		void* RuntimeCollisionShape = nullptr;
		int runtimeShapeIndex = -1;

		void OnShapeChanged()
		{
			if (RuntimeBody != nullptr)
			{
				btRigidBody* rB = (btRigidBody*)RuntimeBody;
				btCollisionShape* cS = (btCollisionShape*)RuntimeCollisionShape;
				
				btCollisionShape* newCS = nullptr;

				if (Shape == 0)
				{
					newCS = new btSphereShape(Radius);

					rB->setCollisionShape(newCS);
					delete cS;
					cS = newCS;
				}
				else if (Shape == 1)
				{
					newCS = new btBoxShape(btVector3(Scale.x, Scale.y, Scale.z));

					rB->setCollisionShape(newCS);
					delete cS;
					cS = newCS;
				}


			}
		}

		void OnScaleChanged()
		{
			if (RuntimeBody != nullptr && RuntimeCollisionShape != nullptr)
			{
				btCollisionShape* cS = (btCollisionShape*)RuntimeCollisionShape;
				cS->setLocalScaling(btVector3(Scale.x, Scale.y, Scale.z));
			}
		}

		void OnRadiusChanged()
		{
			if (RuntimeBody != nullptr && RuntimeCollisionShape != nullptr)
			{

			}
		}

		void OnContactResponseChanged()
		{
			if (RuntimeBody != nullptr)
			{
				btRigidBody* rB = (btRigidBody*)RuntimeBody;
				if (ContactResponse)
				{
					rB->setCollisionFlags(rB->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
				}
				else
				{
					rB->setCollisionFlags(rB->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
				}
			}
		}

		Physics3DCollider() = default;
		Physics3DCollider(const Physics3DCollider&) = default;
	};
}