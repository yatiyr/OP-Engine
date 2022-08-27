#include <Precomp.h>
#include <Scene/SceneSerializer.h>
#include <Scene/Entity.h>
#include <Scene/Components.h>

#include <fstream>
#include <yaml-cpp/yaml.h>

#include <Opium/ResourceManager.h>

#include <ScriptManager/ScriptManager.h>

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

			out << YAML::EndMap; // Relationship Component
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;

			auto& sC = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "Name" << YAML::Value << sC.ModuleName;

			out << YAML::Key << "Fields";
			out << YAML::BeginMap; // Fields
			for (auto& [name, field] : sC.PublicFields)
			{
				PublicField* pField = (PublicField*)field;

				if (pField->Type == FieldType::Int)
				{

					int val = pField->GetValue<int>();
					out << YAML::Key << name + "_Int" << YAML::Value << val;
				}
				else if (pField->Type == FieldType::Float)
				{
					float val = pField->GetValue<float>();
					out << YAML::Key << name + "_Float" << YAML::Value << val;
				}
				else if (pField->Type == FieldType::String)
				{
					std::string val = pField->GetValue<std::string>();
					out << YAML::Key << name + "_String" << YAML::Value << val;
				}
				else if (pField->Type == FieldType::UnsignedInt)
				{
					uint32_t val = pField->GetValue<uint32_t>();
					out << YAML::Key << name + "_Uint" << YAML::Value << val;
				}
				else if (pField->Type == FieldType::Vec2)
				{
					glm::vec2 val = pField->GetValue<glm::vec2>();
					out << YAML::Key << name + "_Vec2" << YAML::Value << val;
				}
				else if (pField->Type == FieldType::Vec3)
				{
					glm::vec3 val = pField->GetValue<glm::vec3>();
					out << YAML::Key << name + "_Vec3" << YAML::Value << val;
				}
				else if (pField->Type == FieldType::Vec4)
				{
					glm::vec4 val = pField->GetValue<glm::vec4>();
					out << YAML::Key << name + "_Vec4" << YAML::Value << val;
				}
			}
			out << YAML::EndMap; // Fields

			out << YAML::EndMap; // ScriptComponent
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

			out << YAML::EndMap; // Rigidbody2DComponent
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


			out << YAML::EndMap; // BoxCollider2D Component
		}

		if (entity.HasComponent<DirLightComponent>())
		{
			out << YAML::Key << "DirLightComponent";
			out << YAML::BeginMap; // DirLightComponent

			auto& dirLightComponent = entity.GetComponent<DirLightComponent>();
			out << YAML::Key << "Color" << YAML::Value << dirLightComponent.Color;
			out << YAML::Key << "Intensity" << YAML::Value << dirLightComponent.Intensity;
			out << YAML::Key << "CascadeSize" << YAML::Value << dirLightComponent.CascadeSize;
			out << YAML::Key << "FrustaDistFactor" << YAML::Value << dirLightComponent.FrustaDistFactor;
			out << YAML::Key << "CastShadows" << YAML::Value << dirLightComponent.CastShadows;

			out << YAML::EndMap; // DirLightComponent
		}

		if (entity.HasComponent<SpotLightComponent>())
		{
			out << YAML::Key << "SpotLightComponent";
			out << YAML::BeginMap; // SpotLightComponent

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

			out << YAML::EndMap; // SpotLightComponent
		}

		if (entity.HasComponent<PointLightComponent>())
		{
			out << YAML::Key << "PointLightComponent";
			out << YAML::BeginMap; // PointLightComponent

			auto& pointLightComponent = entity.GetComponent<PointLightComponent>();
			out << YAML::Key << "Color" << YAML::Value << pointLightComponent.Color;
			out << YAML::Key << "Intensity" << YAML::Value << pointLightComponent.Intensity;
			out << YAML::Key << "NearDist" << YAML::Value << pointLightComponent.NearDist;
			out << YAML::Key << "FarDist" << YAML::Value << pointLightComponent.FarDist;
			out << YAML::Key << "Kq" << YAML::Value << pointLightComponent.Kq;
			out << YAML::Key << "Kl" << YAML::Value << pointLightComponent.Kl;
			out << YAML::Key << "CastShadows" << YAML::Value << pointLightComponent.CastShadows;

			out << YAML::EndMap; // PointLightComponent
		}

		if (entity.HasComponent<Physics3DMaterial>())
		{
			out << YAML::Key << "Physics3DMaterialComponent";
			out << YAML::BeginMap; // Physics3DMaterialComponent

			auto& physics3DMaterial = entity.GetComponent<Physics3DMaterial>();
			out << YAML::Key << "Mass" << physics3DMaterial.Mass;
			out << YAML::Key << "Friction" << physics3DMaterial.Friction;
			out << YAML::Key << "RollingFriction" << physics3DMaterial.RollingFriction;
			out << YAML::Key << "SpinningFriction" << physics3DMaterial.SpinningFriction;
			out << YAML::Key << "Restitution" << physics3DMaterial.Restitution;

			out << YAML::EndMap; // Physics3DMaterialComponent
		}

		if (entity.HasComponent<Physics3DCollider>())
		{
			out << YAML::Key << "Physics3DColliderComponent";
			out << YAML::BeginMap; // Physics3DColliderComponent

			auto& physics3DCollider = entity.GetComponent<Physics3DCollider>();

			out << YAML::Key << "Shape" << physics3DCollider.Shape;
			if (physics3DCollider.Shape == 0)
				out << YAML::Key << "Scale" << physics3DCollider.Scale;
			else if (physics3DCollider.Shape == 1)
				out << YAML::Key << "Radius" << physics3DCollider.Radius;
			out << YAML::Key << "ContactResponse" << physics3DCollider.ContactResponse;

			out << YAML::EndMap; // Physics3DColliderComponent
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap; // MeshComponent

			auto& meshComponent = entity.GetComponent<MeshComponent>();
			out << YAML::Key << "MeshName" << YAML::Value << meshComponent.MeshName;

			out << YAML::EndMap; // MeshComponent
		}

		if (entity.HasComponent<MaterialComponent>())
		{
			out << YAML::Key << "MaterialComponent";
			out << YAML::BeginMap; // MaterialComponent

			auto& materialComponent = entity.GetComponent<MaterialComponent>();
			Ref<MaterialInstance> matInstance = materialComponent.MatInstance;

			out << YAML::Key << "MaterialName" << YAML::Value << matInstance->Mat->m_Name;
			out << YAML::Key << "TilingFactor" << YAML::Value << matInstance->TilingFactor;
			out << YAML::Key << "HeightFactor" << YAML::Value << matInstance->HeightFactor;
			out << YAML::Key << "ClipBorder" << YAML::Value << matInstance->ClipBorder;
			// Write float values
			out << YAML::Key << "FloatValues";
			out << YAML::BeginMap; // FloatValues
			for (auto& [name, val] : matInstance->Floats)
			{
				out << YAML::Key << name << YAML::Value << val;
			}
			out << YAML::EndMap; // FloatValues

			// Write float2 values
			out << YAML::Key << "Float2Values";
			out << YAML::BeginMap; // Float2 Values
			for (auto& [name, val] : matInstance->Float2s)
			{
				out << YAML::Key << name << YAML::Value << val;
			}
			out << YAML::EndMap; // Float2 Values

			// Write float3 values
			out << YAML::Key << "Float3Values";
			out << YAML::BeginMap; // Float3 Values
			for (auto& [name, val] : matInstance->Float3s)
			{
				out << YAML::Key << name << YAML::Value << val;
			}
			out << YAML::EndMap; // Float3 Values

			// Write int values
			out << YAML::Key << "IntValues";
			out << YAML::BeginMap; // IntValues
			for (auto& [name, val] : matInstance->Ints)
			{
				out << YAML::Key << name << YAML::Value << val;
			}
			out << YAML::EndMap; // IntValues

			// Write textures
			out << YAML::Key << "Textures";
			out << YAML::BeginMap; // Textures
			for (auto& [name, tex] : matInstance->Textures)
			{
				out << YAML::Key << name << YAML::Value << tex->m_Name;
			}
			out << YAML::EndMap; // Textures


			out << YAML::EndMap; // MaterialComponent
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::SerializeText(const std::string& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_Name;
		out << YAML::Key << "Skybox" << YAML::Value << m_Scene->m_Skybox;
		out << YAML::Key << "ToneMap" << YAML::Value << m_Scene->m_ToneMap;
		out << YAML::Key << "Exposure" << YAML::Value << m_Scene->m_Exposure;

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
		std::string skybox    = data["Skybox"].as<std::string>();
		bool tonemap          = data["ToneMap"].as<bool>();
		float exposure        = data["Exposure"].as<float>();
		
		m_Scene->m_Name = sceneName;
		m_Scene->m_Skybox = skybox;
		m_Scene->m_ToneMap = tonemap;
		m_Scene->m_Exposure = exposure;

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

				auto rootComponent = entity["RootComponent"];
				if (rootComponent)
				{
					deserializedEntity.AddComponent<RootComponent>();
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

				auto physics3DMaterialComponent = entity["Physics3DMaterialComponent"];
				if (physics3DMaterialComponent)
				{
					auto& p3DMc            = deserializedEntity.AddComponent<Physics3DMaterial>();
					p3DMc.Mass             = physics3DMaterialComponent["Mass"].as<float>();
					p3DMc.Friction         = physics3DMaterialComponent["Friction"].as<float>();
					p3DMc.RollingFriction  = physics3DMaterialComponent["RollingFriction"].as<float>();
					p3DMc.SpinningFriction = physics3DMaterialComponent["SpinningFriction"].as<float>();
					p3DMc.Restitution      = physics3DMaterialComponent["Restitution"].as<float>();
				}

				auto physics3DColliderComponent = entity["Physics3DColliderComponent"];
				if (physics3DColliderComponent)
				{
					auto& p3DCC = deserializedEntity.AddComponent<Physics3DCollider>();
					p3DCC.Shape = physics3DColliderComponent["Shape"].as<int>();
					if (p3DCC.Shape == 0)
						p3DCC.Scale = physics3DColliderComponent["Scale"].as<glm::vec3>();
					else if (p3DCC.Shape == 1)
						p3DCC.Radius = physics3DColliderComponent["Radius"].as<float>();
					p3DCC.ContactResponse = physics3DColliderComponent["ContactResponse"].as<bool>();
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
					sLC.CastShadows = spotLightComponent["CastShadows"].as<bool>();
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

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					std::string moduleName = scriptComponent["Name"].as<std::string>();
					auto& sC = deserializedEntity.AddComponent<ScriptComponent>(moduleName);

					// set field values
					auto fields = scriptComponent["Fields"];
					for (const auto& element : fields)
					{
						std::string fullFieldName = element.first.as<std::string>();
						std::string fieldName = fullFieldName.substr(0, fullFieldName.find_first_of('_'));
						std::string typeName = fullFieldName.substr(fullFieldName.find_first_of('_'), fullFieldName.size() - 1);
						if (typeName == "Int")
						{
							int val = element.second.as<int>();
							PublicField* pField = (PublicField*)sC.PublicFields[fieldName];
							pField->SetValue<int>(val);
						}
						else if (typeName == "Float")
						{
							float val = element.second.as<float>();
							PublicField* pField = (PublicField*)sC.PublicFields[fieldName];
							pField->SetValue<float>(val);
						}
						else if (typeName == "String")
						{
							std::string val = element.second.as<std::string>();
							PublicField* pField = (PublicField*)sC.PublicFields[fieldName];
							pField->SetValue<std::string>(val);
						}
						else if (typeName == "Uint")
						{
							uint32_t val = element.second.as<uint32_t>();
							PublicField* pField = (PublicField*)sC.PublicFields[fieldName];
							pField->SetValue<uint32_t>(val);
						}
						else if (typeName == "Vec2")
						{
							glm::vec2 val = element.second.as<glm::vec2>();
							PublicField* pField = (PublicField*)sC.PublicFields[fieldName];
							pField->SetValue<glm::vec2>(val);
						}
						else if (typeName == "Vec3")
						{
							glm::vec3 val = element.second.as<glm::vec3>();
							PublicField* pField = (PublicField*)sC.PublicFields[fieldName];
							pField->SetValue<glm::vec3>(val);
						}
						else if (typeName == "Vec4")
						{
							glm::vec4 val = element.second.as<glm::vec4>();
							PublicField* pField = (PublicField*)sC.PublicFields[fieldName];
							pField->SetValue<glm::vec4>(val);
						}
					}
				}

				auto materialComponent = entity["MaterialComponent"];
				if (materialComponent)
				{
					auto& mC = deserializedEntity.AddOrReplaceComponent<MaterialComponent>();
					Ref<Material> mat = ResourceManager::GetMaterial(materialComponent["MaterialName"].as<std::string>());
					mC.MatInstance = MaterialInstance::Create(mat);
					mC.MatInstance->TilingFactor = materialComponent["TilingFactor"].as<float>();
					mC.MatInstance->HeightFactor = materialComponent["HeightFactor"].as<float>();
					mC.MatInstance->ClipBorder = materialComponent["ClipBorder"].as<bool>();
					// Read float values
					auto floatValues = materialComponent["FloatValues"];
					uint32_t floatValCounter = 0;
					for (const auto& element : floatValues)
					{
						mC.MatInstance->Floats[floatValCounter] = {element.first.as<std::string>(), element.second.as<float>()};
						floatValCounter++;
					}

					// Read float2 values
					auto float2Values = materialComponent["Float2Values"];
					uint32_t float2ValCounter = 0;
					for (const auto& element : float2Values)
					{
						mC.MatInstance->Float2s[float2ValCounter] = { element.first.as<std::string>(), element.second.as<glm::vec2>() };
						float2ValCounter++;
					}

					// Read float3 values
					auto float3Values = materialComponent["Float3Values"];
					uint32_t float3ValCounter = 0;
					for (const auto& element : float3Values)
					{
						mC.MatInstance->Float3s[float3ValCounter] = {element.first.as<std::string>(), element.second.as<glm::vec3>()};
						float3ValCounter++;
					}

					// Read Int values
					auto intValues = materialComponent["IntValues"];
					uint32_t intValCounter = 0;
					for (const auto& element : intValues)
					{
						mC.MatInstance->Ints[intValCounter] = {element.first.as<std::string>(), element.second.as<int>()};
						intValCounter++;
					}

					// Read Textures
					auto textures = materialComponent["Textures"];
					uint32_t texCounter = 0;
					for (const auto& element : textures)
					{
						std::string textureType = element.first.as<std::string>();
						std::string textureName = element.second.as<std::string>();
						Ref<Texture> texture = ResourceManager::GetTexture(textureName);
						mC.MatInstance->Textures[texCounter] = { textureType, texture };
						texCounter++;
					}

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