#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
//#include <Scene/Entity.h>
#include <Scene/Components.h>
#include <Scene/Scene.h>

extern "C"
{
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClassField MonoClassField;
}

namespace OP
{

	enum class FieldType
	{
		None = 0, Float, Int, UnsignedInt, String, Vec2, Vec3, Vec4
	};

	static MonoMethod* GetMethod(MonoImage* coreImage, MonoImage* appImage, const std::string& fullName, const std::string& methodDesc)
	{
		std::string fullDesc = fullName + methodDesc;

		MonoMethodDesc* desc = mono_method_desc_new(fullDesc.c_str(), NULL);
		if (!desc) OP_ENGINE_ERROR("mono_method_desc_new has been failed");

		MonoMethod* method = mono_method_desc_search_in_image(desc, appImage);
		if (!method)
		{
			method = mono_method_desc_search_in_image(desc, coreImage);
		}
			
		if (!method)
		{
			std::string baseDescStr = "Entity" + methodDesc;
			MonoMethodDesc* baseDesc = mono_method_desc_new(baseDescStr.c_str(), NULL);
			method = mono_method_desc_search_in_image(baseDesc, coreImage);
		}
		
		if (!method)
		{
			OP_ENGINE_ERROR("mono_method_desc_search_in_image has been failed");
		}
		

		return method;
	}


	static MonoMethod* SearchFromClassRecursive(MonoClass* monoClass, const std::string& methodDesc)
	{
		MonoClass* classIterator = monoClass;
		MonoMethodDesc* desc;
		MonoMethod* resultMethod = nullptr;

		desc = mono_method_desc_new(methodDesc.c_str(), true);

		while (classIterator != nullptr && resultMethod == nullptr)
		{
			resultMethod = mono_method_desc_search_in_class(desc, classIterator);
			if (resultMethod == nullptr)
			{
				classIterator = mono_class_get_parent(classIterator);
			}
		}
		mono_method_desc_free(desc);

		if (!resultMethod)
			OP_ENGINE_ERROR("mono_method_desc_search_in_class has been failed");

		return resultMethod;
	}

	struct EntityStruct
	{
		uint32_t sceneID;
		uint32_t EntityID;
	};

	struct CollisionPointStruct
	{
		float x;
		float y;
		float z;
	};

	struct EntityScriptClass
	{
		std::string FullName;
		std::string ClassName;
		std::string NamespaceName;

		MonoClass* Class;
		MonoMethod* OnCreateMethod;
		MonoMethod* OnDestroyMethod;
		MonoMethod* OnUpdateMethod;
		MonoMethod* OnCollisionMethod;
		MonoMethod* OnCollisionStartedMethod;
		MonoMethod* OnCollisionEndedMethod;

		void InitClassMethods(MonoImage* coreImage, MonoImage* appImage)
		{
			OnCreateMethod = SearchFromClassRecursive(Class, ":OnCreate()");
			OnUpdateMethod = SearchFromClassRecursive(Class, ":OnUpdate(single)");
			OnCollisionMethod = SearchFromClassRecursive(Class, ":OnCollision(uint,single,single,single)");
			OnCollisionStartedMethod = SearchFromClassRecursive(Class, ":OnCollisionStarted(uint)");
			OnCollisionEndedMethod = SearchFromClassRecursive(Class, ":OnCollisionEnded(uint)");
		}

	};
	struct EntityInstance
	{
		EntityScriptClass* ScriptClass;
		uint32_t Handle;
		Scene* SceneInstance;

		MonoObject* GetInstance()
		{
			return mono_gchandle_get_target(Handle);
		}
	};

	struct PublicField
	{
		std::string Name;
		FieldType Type;

		PublicField(const std::string& name, FieldType type)
			: Name(name), Type(type) {}

		template<typename T>
		T GetValue() const
		{
			T value;
			GetValue_Internal(&value);
			return value;
		}

		template<typename T>
		void SetValue(T value) const
		{
			SetValue_Internal(&value);
		}
	private:
		EntityInstance* m_EntityInstance;
		MonoClassField* m_MonoClassField;

		void SetValue_Internal(void* value) const
		{
			mono_field_set_value(m_EntityInstance->GetInstance(), m_MonoClassField, value);
		}
		void GetValue_Internal(void* outValue) const
		{
			mono_field_get_value(m_EntityInstance->GetInstance(), m_MonoClassField, outValue);
		}

		friend class ScriptManager;
	};

	using ScriptModuleFieldMap = std::unordered_map<std::string, std::vector<PublicField>>;

	class ScriptManager
	{
	public:
		static void ScriptManager::InitializeManager(const std::string& assemblyPath);
		static void ScriptManager::ReloadManager();
		static void ScriptManager::ShutdownManager();

		static void OnCreateEntity(uint32_t entity);
		static void OnUpdateEntity(uint32_t entityID, Timestep ts);
		static void OnCollision(uint32_t entityID, uint32_t otherEntityID, glm::vec3 collisionPoint);
		static void OnCollisionStarted(uint32_t entityID, uint32_t otherEntityID);
		static void OnCollisionEnded(uint32_t entityID, uint32_t otherEntityID);

		static std::unordered_map<std::string, void*> OnInitEntity(ScriptComponent& script, uint32_t entityID, uint32_t sceneID);
		static std::unordered_map<uint32_t, EntityInstance> GetInstanceMap();

		static const ScriptModuleFieldMap& GetFieldMap();
	};
}
