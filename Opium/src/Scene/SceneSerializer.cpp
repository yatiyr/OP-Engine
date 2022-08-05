#include <Precomp.h>
#include <Scene/SceneSerializer.h>
#include <Scene/Entity.h>
#include <Scene/Components.h>

#include <fstream>
#include <yaml-cpp/yaml.h>

#include <Opium/ResourceManager.h>

namespace YAML
{

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};


	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace OP
{

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static: return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic: return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		OP_ENGINE_ASSERT(false, "Unknown body type");
		return "";
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static") return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;


		OP_ENGINE_ASSERT(false, "Unknown body type");
		return Rigidbody2DComponent::BodyType::Static;

	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{

		OP_ENGINE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // Tag Component

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap; // Tag Component
		}

		if (entity.HasComponent<RootComponent>())
		{
			out << YAML::Key << "RootComponent";
			out << YAML::BeginMap; // Root Component

			auto& root = entity.GetComponent<RootComponent>().root;
			out << YAML::Key << "Root" << YAML::Value << root;
			out << YAML::EndMap; // Root Component
		}
		else
		{
			out << YAML::Key << "RootComponent";
			out << YAML::BeginMap; // Root Component

			out << YAML::Key << "Root" << YAML::Value << false;
			out << YAML::EndMap; // Root Component
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			out << YAML::Key << "RelationshipComponent";
			out << YAML::BeginMap; // Relationship Component

			auto& comp = entity.GetComponent<RelationshipComponent>();

			out << YAML::Key << "Children" << YAML::Value << comp.children;
			out << YAML::Key << "Parent" << YAML::Value << static_cast<uint64_t>(comp.parent);
			out << YAML::Key << "First" << YAML::Value << static_cast<uint64_t>(comp.first);
			out << YAML::Key << "Prev" << YAML::Value << static_cast<uint64_t>(comp.prev);
			out << YAML::Key << "Next" << YAML::Value << static_cast<uint64_t>(comp.next);
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // Transform Component

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // Transform Component

		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // Camera component

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // Camera Component
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // Sprite Renderer Component

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2D Component

			auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;

			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;


			out << YAML::EndMap;
		}

		if (entity.HasComponent<DirLightComponent>())
		{
			out << YAML::Key << "DirLightComponent";
			out << YAML::BeginMap;

			auto& dirLightComponent = entity.GetComponent<DirLightComponent>();
			out << YAML::Key << "Color" << YAML::Value << dirLightComponent.Color;
			out << YAML::Key << "Intensity" << YAML::Value << dirLightComponent.Intensity;
			out << YAML::Key << "CascadeSize" << YAML::Value << dirLightComponent.CascadeSize;
			out << YAML::Key << "FrustaDistFactor" << YAML::Value << dirLightComponent.FrustaDistFactor;
			out << YAML::Key << "CastShadows" << YAML::Value << dirLightComponent.CastShadows;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpotLightComponent>())
		{
			out << YAML::Key << "SpotLightComponent";
			out << YAML::BeginMap;

			auto& spotLightComponent = entity.GetComponent<SpotLightComponent>();
			out << YAML::Key << "Color" << YAML::Value << spotLightComponent.Color;
			out << YAML::Key << "Intensity" << YAML::Value << spotLightComponent.Intensity;
			out << YAML::Key << "Cutoff" << YAML::Value << spotLightComponent.Cutoff;
			out << YAML::Key << "FarDist" << YAML::Value << spotLightComponent.FarDist;
			out << YAML::Key << "NearDist" << YAML::Value << spotLightComponent.NearDist;
			out << YAML::Key << "Kq" << YAML::Value << spotLightComponent.Kq;
			out << YAML::Key << "Kl" << YAML::Value << spotLightComponent.Kl;
			out << YAML::Key << "OuterCutoff" << YAML::Value << spotLightComponent.OuterCutoff;
			out << YAML::Key << "CastShadows" << YAML::Value << spotLightComponent.CastShadows;
		}

		if (entity.HasComponent<PointLightComponent>())
		{
			out << YAML::Key << "PointLightComponent";
			out << YAML::BeginMap;

			auto& pointLightComponent = entity.GetComponent<PointLightComponent>();
			out << YAML::Key << "Color" << YAML::Value << pointLightComponent.Color;
			out << YAML::Key << "Intensity" << YAML::Value << pointLightComponent.Intensity;
			out << YAML::Key << "NearDist" << YAML::Value << pointLightComponent.NearDist;
			out << YAML::Key << "FarDist" << YAML::Value << pointLightComponent.FarDist;
			out << YAML::Key << "Kq" << YAML::Value << pointLightComponent.Kq;
			out << YAML::Key << "Kl" << YAML::Value << pointLightComponent.Kl;
			out << YAML::Key << "CastShadows" << YAML::Value << pointLightComponent.CastShadows;
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;

			auto& meshComponent = entity.GetComponent<MeshComponent>();
			out << YAML::Key << "MeshName" << YAML::Value << meshComponent.MeshName;
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::SerializeText(const std::string& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Unitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;
				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeBinary(const std::string& filePath)
	{
		// Not implemented
	}

	bool SceneSerializer::DeserializeText(const std::string& filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		OP_ENGINE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();
				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				OP_ENGINE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					OP_ENGINE_TRACE("Color alpha is {0}", src.Color.a);
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
					
				}

				auto dirLightComponent = entity["DirLightComponent"];
				if (dirLightComponent)
				{
					auto& dLC = deserializedEntity.AddComponent<DirLightComponent>();
					dLC.Color = dirLightComponent["Color"].as<glm::vec3>();
					dLC.Intensity = dirLightComponent["Intensity"].as<float>();
					dLC.CascadeSize = dirLightComponent["CascadeSize"].as<int>();
					dLC.FrustaDistFactor = dirLightComponent["FrustaDistFactor"].as<float>();
					dLC.CastShadows = dirLightComponent["CastShadows"].as<bool>();
				}

				auto spotLightComponent = entity["SpotLightComponent"];
				if (spotLightComponent)
				{
					auto& sLC = deserializedEntity.AddComponent<SpotLightComponent>();
					sLC.Color = spotLightComponent["Color"].as<glm::vec3>();
					sLC.Intensity = spotLightComponent["Intensity"].as<float>();
					sLC.Cutoff = spotLightComponent["Cutoff"].as<float>();
					sLC.OuterCutoff = spotLightComponent["OuterCutoff"].as<float>();
					sLC.CastShadows = spotLightComponent["CastShadows"].as<float>();
					sLC.FarDist = spotLightComponent["FarDist"].as<float>();
					sLC.NearDist = spotLightComponent["NearDist"].as<float>();
					sLC.Kq = spotLightComponent["Kq"].as<float>();
					sLC.Kl = spotLightComponent["Kl"].as<float>();
				}

				auto pointLightComponent = entity["PointLightComponent"];
				if (pointLightComponent)
				{
					auto& pLC = deserializedEntity.AddComponent<PointLightComponent>();
					pLC.Color = pointLightComponent["Color"].as<glm::vec3>();
					pLC.Intensity = pointLightComponent["Intensity"].as<float>();
					pLC.NearDist= pointLightComponent["NearDist"].as<float>();
					pLC.FarDist = pointLightComponent["FarDist"].as<float>();
					pLC.Kq = pointLightComponent["Kq"].as<float>();
					pLC.Kl = pointLightComponent["Kl"].as<float>();
					pLC.CastShadows= pointLightComponent["CastShadows"].as<bool>();
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					auto& mC = deserializedEntity.AddComponent<MeshComponent>();
					mC.MeshName = meshComponent["MeshName"].as<std::string>();
					mC.Mesh = ResourceManager::GetMesh(mC.MeshName);
				}
			}

			// Fill in hiearchical parts
			for (auto entity : entities)
			{
				auto relComp = entity["RelationshipComponent"];
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				Entity deserializedEntity = m_Scene->GetEntityWithUUID(uuid);
				if (relComp)
				{
					auto& relCompDeserialized = deserializedEntity.GetComponent<RelationshipComponent>();
					uint64_t parentUUID = relComp["Parent"].as<uint64_t>();
					Entity parent = m_Scene->GetEntityWithUUID(parentUUID);
					if (entt::entity(parent) != entt::null)
					{
						m_Scene->AddChildEntity(parent, deserializedEntity);
					}
				}
			}
		}
		return true;
	}

	bool SceneSerializer::DeserializeBinary(const std::string& filePath)
	{
		// Not implemented
		OP_ENGINE_ASSERT(false);

		return false;
	}

}