#include <Precomp.h>
#include <ScriptManager/ScriptManager.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>

#include <Windows.h>
#include <winioctl.h>

#include <ScriptManager/InternalCallArranger.h>
#include <Scene/Scene.h>

namespace OP
{

	static MonoDomain* s_MonoDomain = nullptr;
	static std::string s_AssemblyPath;

	static ScriptModuleFieldMap s_PublicFields;

	static MonoMethod* GetMethod(MonoImage* image, const std::string& methodDesc);

	struct EntityScriptClass
	{
		std::string FullName;
		std::string ClassName;
		std::string NamespaceName;

		MonoClass* Class;
		MonoMethod* OnCreateMethod;
		MonoMethod* OnDestroyMethod;
		MonoMethod* OnUpdateMethod;

		void InitClassMethods(MonoImage* image)
		{
			OnCreateMethod = GetMethod(image, FullName + ":OnCreate()");
			OnUpdateMethod = GetMethod(image, FullName + ":OnUpdate(single)");
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

	static std::unordered_map<std::string, EntityScriptClass> s_EntityClassMap;
	static std::unordered_map<uint32_t, EntityInstance> s_EntityInstanceMap;

	MonoAssembly* InitializeAssemblyFromFile(const char* filePath)
	{
		if (filePath == nullptr) return nullptr;

		HANDLE file = CreateFileA(filePath, FILE_READ_ACCESS, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (file == INVALID_HANDLE_VALUE) return nullptr;

		DWORD fileSize = GetFileSize(file, nullptr);
		if (fileSize == INVALID_FILE_SIZE)
		{
			CloseHandle(file);
			return nullptr;
		}

		void* fileData = malloc(fileSize);
		if (fileData == nullptr)
		{
			CloseHandle(file);
			return nullptr;
		}

		DWORD read = 0;
		ReadFile(file, fileData, fileSize, &read, nullptr);
		if (fileSize != read)
		{
			free(fileData);
			CloseHandle(file);
			return nullptr;
		}

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(reinterpret_cast<char*>(fileData), fileSize, 1, &status, 0);
		if (status != MONO_IMAGE_OK) return nullptr;

		MonoAssembly* loadedAssembly = mono_assembly_load_from_full(image, filePath, &status, 0);
		free(fileData);
		CloseHandle(file);
		mono_image_close(image);
		return loadedAssembly;
	}

	static MonoAssembly* InitializeAssembly(const std::string& path)
	{
		MonoAssembly* assembly = InitializeAssemblyFromFile(path.c_str());

		if (assembly == nullptr)
		{
			OP_ENGINE_ERROR("Could not load this assembly: {0} ", path);
		}
		else
		{
			OP_ENGINE_TRACE("Assembly Loaded Successfully: {0}", path);
		}

		return assembly;

	}

	static MonoImage* GetAssemblyImage(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		if (!image) OP_ENGINE_ERROR("mono_assembly_get_image has been failed");

		return image;
	}

	static MonoClass* GetClass(MonoImage* image, const EntityScriptClass& scriptClass)
	{
		MonoClass* monoClass = mono_class_from_name(image, scriptClass.NamespaceName.c_str(), scriptClass.ClassName.c_str());
		if (!monoClass) OP_ENGINE_ERROR("mono_class_from_name function has been failed: {0},  {1}", scriptClass.NamespaceName, scriptClass.ClassName);

		return monoClass;
	}

	static uint32_t Instantiate(EntityScriptClass& scriptClass)
	{
		MonoObject* instance = mono_object_new(s_MonoDomain, scriptClass.Class);
		if (!instance) OP_ENGINE_ERROR("mono_object_new has been failed");
		
		mono_runtime_object_init(instance);
		uint32_t handle = mono_gchandle_new(instance, false);
		return handle;
	}

	static MonoMethod* GetMethod(MonoImage* image, const std::string& methodDesc)
	{
		MonoMethodDesc* desc = mono_method_desc_new(methodDesc.c_str(), NULL);
		if (!desc) OP_ENGINE_ERROR("mono_method_desc_new has been failed");

		MonoMethod* method = mono_method_desc_search_in_image(desc, image);
		if (!method) OP_ENGINE_ERROR("mono_method_desc_search_in_image has been failed");

		return method;
	}

	static MonoObject* CallMethod(MonoObject* object, MonoMethod* method, void** params = nullptr)
	{
		MonoObject* pException = NULL;
		MonoObject* result = mono_runtime_invoke(method, object, params, &pException);
		return result;
	}

	static void PrintClassMethods(MonoClass* monoClass)
	{
		MonoMethod* iter;
		void* ptr = 0;
		while ((iter = mono_class_get_methods(monoClass, &ptr)) != NULL)
		{
			printf("*************************************");
			const char* name = mono_method_get_name(iter);
			MonoMethodDesc* methodDesc = mono_method_desc_from_method(iter);
			
			const char* paramNames = "";
			mono_method_get_param_names(iter, &paramNames);

			printf("Name: %s\n", name);
			printf("Full name: %s\n", mono_method_full_name(iter, true));
		}
	}

	static void PrintClassProperties(MonoClass* monoClass)
	{
		MonoProperty* iter;
		void* ptr = 0;
		while ((iter = mono_class_get_properties(monoClass, &ptr)) != NULL)
		{
			printf("****************************************");
			const char* name = mono_property_get_name(iter);

			printf("Name: %s\n", name);
		}
	}

	static void InitializeMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* domain = mono_jit_init("Opium");

		char* name = (char*)"OpiumRuntime";
		s_MonoDomain = mono_domain_create_appdomain(name, nullptr);
	}

	static MonoAssembly* s_AppAssembly = nullptr;
	static MonoAssembly* s_CoreAssembly = nullptr;

	MonoImage* s_AppAssemblyImage = nullptr;
	MonoImage* s_CoreAssemblyImage = nullptr;


	static MonoString* GetName()
	{
		return mono_string_new(s_MonoDomain, "Hello!");
	}

	static void LoadRuntimeAssembly(const std::string& path)
	{
		if (s_AppAssembly)
		{
			mono_domain_unload(s_MonoDomain);
			mono_assembly_close(s_AppAssembly);

			char* name = (char*)"OpiumRuntime";
			s_MonoDomain = mono_domain_create_appdomain(name, nullptr);
		}

		s_CoreAssembly = InitializeAssembly("assets/scripts/Opium-Script.dll");
		s_CoreAssemblyImage = GetAssemblyImage(s_CoreAssembly);


		s_AppAssembly = InitializeAssembly(path);
		s_AppAssemblyImage = GetAssemblyImage(s_AppAssembly);
		InternalCallArranger::Arrange();
	}

	void ScriptManager::InitializeManager(const std::string& assemblyPath)
	{
		s_AssemblyPath = assemblyPath;

		InitializeMono();

		LoadRuntimeAssembly(s_AssemblyPath);
	}

	void ScriptManager::ShutdownManager()
	{
		// shutdown
	}

	void ScriptManager::OnCreateEntity(Entity entity)
	{
		EntityInstance& entityInstance = s_EntityInstanceMap[(uint32_t)entity.m_EntityHandle];
		if (entityInstance.ScriptClass->OnCreateMethod)
			CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->OnCreateMethod);
	}


	void ScriptManager::OnUpdateEntity(uint32_t entityID, Timestep ts)
	{
		OP_ENGINE_ASSERT(s_EntityInstanceMap.find(entityID) != s_EntityInstanceMap.end(), "Could not find entity in instance map!");

		auto& entity = s_EntityInstanceMap[entityID];

		if (entity.ScriptClass->OnUpdateMethod)
		{
			void* args[] = { &ts };
			CallMethod(entity.GetInstance(), entity.ScriptClass->OnUpdateMethod, args);
		}
	}

	static FieldType GetOpiumFieldType(MonoType* monoType)
	{
		int type = mono_type_get_type(monoType);
		switch (type)
		{
			case MONO_TYPE_R4: return FieldType::Float;
			case MONO_TYPE_I4: return FieldType::Int;
			case MONO_TYPE_U4: return FieldType::UnsignedInt;
			case MONO_TYPE_STRING: return FieldType::String;
			case MONO_TYPE_VALUETYPE:
			{
				char* name = mono_type_get_name(monoType);
				if (strcmp(name, "Opium.Vector2") == 0) return FieldType::Vec2;
				if (strcmp(name, "Opium.Vector3") == 0) return FieldType::Vec3;
				if (strcmp(name, "Opium.Vector4") == 0) return FieldType::Vec4;
			}
		}

		return FieldType::None;
	}

	void ScriptManager::OnInitEntity(ScriptComponent& script, uint32_t entityID, uint32_t sceneID)
	{
		EntityScriptClass& scriptClass = s_EntityClassMap[script.ModuleName];
		scriptClass.FullName = script.ModuleName;
		if (script.ModuleName.find('.') != std::string::npos)
		{
			scriptClass.NamespaceName = script.ModuleName.substr(0, script.ModuleName.find_last_of('.'));
			scriptClass.ClassName = script.ModuleName.substr(script.ModuleName.find_last_of('.') + 1);
		}
		else
		{
			scriptClass.ClassName = script.ModuleName;
		}

		scriptClass.Class = GetClass(s_AppAssemblyImage, scriptClass);
		scriptClass.InitClassMethods(s_AppAssemblyImage);

		EntityInstance& entityInstance = s_EntityInstanceMap[entityID];
		entityInstance.ScriptClass = &scriptClass;
		entityInstance.Handle = Instantiate(scriptClass);

		MonoProperty* entityIDProperty = mono_class_get_property_from_name(scriptClass.Class, "EntityID");
		mono_property_get_get_method(entityIDProperty);
		MonoMethod* entityIDSetMethod = mono_property_get_set_method(entityIDProperty);
		void* param[] = { &entityID };
		CallMethod(entityInstance.GetInstance(), entityIDSetMethod, param);

		MonoProperty* sceneIDProperty = mono_class_get_property_from_name(scriptClass.Class, "SceneID");
		mono_property_get_get_method(sceneIDProperty);
		MonoMethod* sceneIDSetMethod = mono_property_get_set_method(sceneIDProperty);
		param[0] = { &sceneID };
		CallMethod(entityInstance.GetInstance(), sceneIDSetMethod, param);

		if (scriptClass.OnCreateMethod)
			CallMethod(entityInstance.GetInstance(), scriptClass.OnCreateMethod);

		// Get public fields
		{
			MonoClassField* iter;
			void* ptr = 0;
			while ((iter = mono_class_get_fields(scriptClass.Class, &ptr)) != NULL)
			{
				const char* name = mono_field_get_name(iter);
				uint32_t flags = mono_field_get_flags(iter);
				if ((flags & MONO_FIELD_ATTR_PUBLIC) == 0)
					continue;

				MonoType* fieldType = mono_field_get_type(iter);
				FieldType opiumFieldType = GetOpiumFieldType(fieldType);

				// Attributes
				MonoCustomAttrInfo* attr = mono_custom_attrs_from_field(scriptClass.Class, iter);

				auto& publicField = s_PublicFields[script.ModuleName].emplace_back(name, opiumFieldType);
				publicField.m_EntityInstance = &entityInstance;
				publicField.m_MonoClassField = iter;
				// mono_field_set_value(entityInstance.Instance, iter, );
			}
		}
	}

	const OP::ScriptModuleFieldMap& ScriptManager::GetFieldMap()
	{
		return s_PublicFields;
	}

	void PublicField::SetValue_Internal(void* value) const
	{
		mono_field_set_value(m_EntityInstance->GetInstance(), m_MonoClassField, value);
	}

	void PublicField::GetValue_Internal(void* outValue) const
	{
		mono_field_get_value(m_EntityInstance->GetInstance(), m_MonoClassField, outValue);
	}
}