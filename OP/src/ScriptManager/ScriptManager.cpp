#include <Precomp.h>
#include <ScriptManager/ScriptManager.h>

#include <Windows.h>
#include <winioctl.h>

#include <ScriptManager/InternalCallArranger.h>

namespace OP
{

	static MonoDomain* s_MonoDomain = nullptr;
	static MonoDomain* s_RootDomain = nullptr;
	static MonoDomain* s_DeletedDomain = nullptr;
	static std::string s_AssemblyPath;

	static ScriptModuleFieldMap s_PublicFields;

	//static MonoMethod* GetMethod(MonoImage* image, const std::string& methodDesc);

	/*struct EntityInstance
	{
		EntityScriptClass* ScriptClass;

		uint32_t Handle;
		Scene* SceneInstance;

		MonoObject* GetInstance()
		{
			return mono_gchandle_get_target(Handle);
		}
	};*/


	char* ReadBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		// If the file could not be opened
		if (!stream)
			return nullptr;

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		// If file is empty
		if (size == 0)
			nullptr;

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;

		return buffer;

	}

	static std::unordered_map<std::string, EntityScriptClass> s_EntityClassMap;
	static std::unordered_map<uint32_t, EntityInstance> s_EntityInstanceMap;


	MonoAssembly* LoadAssemblyFromFile(const std::string& filePath)
	{
		uint32_t fileSize = 0;
		char* fileData = ReadBytes(filePath, &fileSize);

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, filePath.c_str(), &status, 0);
		mono_image_close(image);

		delete[] fileData;

		return assembly;
	}

	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);
			
			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			printf("%s.%s", nameSpace, name);
		}
	}

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
		MonoAssembly* assembly = LoadAssemblyFromFile(path.c_str());

		if (assembly == nullptr)
		{
			OP_ENGINE_ERROR("Could not load this assembly: {0} ", path);
		}
		else
		{
			OP_ENGINE_TRACE("Assembly Loaded Successfully: {0}", path);
		}


		PrintAssemblyTypes(assembly);

		return assembly;

	}

	static MonoImage* GetAssemblyImage(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		if (!image) OP_ENGINE_ERROR("mono_assembly_get_image has been failed");

		return image;
	}

	static void ReloadEntityScripts()
	{

	}

	static MonoClass* GetClass(MonoImage* coreImage, MonoImage* appImage, const EntityScriptClass& scriptClass)
	{
		MonoClass* monoClass = mono_class_from_name(appImage, scriptClass.NamespaceName.c_str(), scriptClass.ClassName.c_str());
		if (!monoClass)
			monoClass = mono_class_from_name(coreImage, scriptClass.NamespaceName.c_str(), scriptClass.ClassName.c_str());
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


	static MonoObject* CallMethod(MonoObject* object, MonoMethod* method, void** params = nullptr)
	{
		MonoObject* pException;
		MonoMethod* possibleMethod = method;

		if (object)
			possibleMethod = mono_object_get_virtual_method(object, possibleMethod);

		MonoObject* result = mono_runtime_invoke(possibleMethod, object, params, &pException);

		if (pException)
			mono_print_unhandled_exception(pException);

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

		MonoDomain* domain = mono_jit_init("Op");
		s_RootDomain = domain;
		char* name = (char*)"OpRuntime";

		s_MonoDomain = mono_domain_create_appdomain(name, nullptr);
		mono_domain_set(s_MonoDomain, true);
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

			char* name = (char*)"OpRuntime";
			s_MonoDomain = mono_domain_create_appdomain(name, nullptr);
		}

		
		s_CoreAssembly = InitializeAssembly("Resources/Scripts/OP-Script.dll");
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



	void ScriptManager::ReloadManager()
	{

		OP_ENGINE_INFO("Reloading Assembly");

		mono_domain_set(s_RootDomain, 0);
		//mono_assembly_close(s_AppAssembly);
		//mono_assembly_close(s_CoreAssembly);

		//s_DeletedDomain = s_MonoDomain;
		//mono_domain_unload(s_MonoDomain);		
	
		s_MonoDomain = nullptr;
		s_AppAssembly = nullptr;
		s_AppAssemblyImage = nullptr;		
		s_CoreAssembly = nullptr;
		s_CoreAssemblyImage = nullptr;
		s_EntityClassMap.clear();
		s_EntityInstanceMap.clear();
		s_PublicFields.clear();		

		char* name = (char*)"OpRuntime";
		s_MonoDomain = mono_domain_create_appdomain(name, nullptr);
		mono_domain_set(s_MonoDomain, 0);

		LoadRuntimeAssembly(s_AssemblyPath);
		OP_ENGINE_INFO("Assembly has been reloaded");

		//mono_domain_set(s_DeletedDomain, 0);
		//mono_domain_unload(s_DeletedDomain);
	}

	void ScriptManager::ShutdownManager()
	{
		// mono_domain_unload(s_MonoDomain);
		s_MonoDomain = nullptr;
		// shutdown
		// mono_jit_cleanup(s_RootDomain);
		s_RootDomain = nullptr;
	}

	void ScriptManager::OnCreateEntity(uint32_t entity)
	{
		EntityInstance& entityInstance = s_EntityInstanceMap[entity];
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

	void ScriptManager::OnCollision(uint32_t entityID, uint32_t otherEntityID, glm::vec3 collisionPoint)
	{
		if (s_EntityInstanceMap.find(entityID) != s_EntityInstanceMap.end())
		{
			auto& entity = s_EntityInstanceMap[entityID];

			if (entity.ScriptClass->OnCollisionMethod)
			{
				
				uint32_t EntityID = otherEntityID;
				uint32_t SceneID = 0; // for now TODO: Figure it out

				
				float x = collisionPoint.x;
				float y = collisionPoint.y;
				float z = collisionPoint.z;			

				void* args[] = { &EntityID, &x, &y, &z};
				CallMethod(entity.GetInstance(), entity.ScriptClass->OnCollisionMethod, args);
			}
		}

	}

	void ScriptManager::OnCollisionStarted(uint32_t entityID, uint32_t otherEntityID)
	{

		if (s_EntityInstanceMap.find(entityID) != s_EntityInstanceMap.end())
		{

			auto& entity = s_EntityInstanceMap[entityID];

			if (entity.ScriptClass->OnCollisionStartedMethod)
			{
				uint32_t EntityID = otherEntityID;
				uint32_t SceneID = 0; // for now TODO: Figure it out


				void* args[] = { &EntityID };
				CallMethod(entity.GetInstance(), entity.ScriptClass->OnCollisionStartedMethod, args);
			}
		}
	}

	void ScriptManager::OnCollisionEnded(uint32_t entityID, uint32_t otherEntityID)
	{

		if (s_EntityInstanceMap.find(entityID) != s_EntityInstanceMap.end())
		{

			auto& entity = s_EntityInstanceMap[entityID];

			if (entity.ScriptClass->OnCollisionEndedMethod)
			{
				uint32_t EntityID = otherEntityID;
				uint32_t SceneID = 0; // for now TODO: Figure it out

				void* args[] = { &EntityID };

				CallMethod(entity.GetInstance(), entity.ScriptClass->OnCollisionEndedMethod, args);
			}
		}
	}

	static FieldType GetOpFieldType(MonoType* monoType)
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
				if (strcmp(name, "OP.Vec2") == 0) return FieldType::Vec2;
				if (strcmp(name, "OP.Vec3") == 0) return FieldType::Vec3;
				if (strcmp(name, "OP.Vec4") == 0) return FieldType::Vec4;
			}
		}

		return FieldType::None;
	}

	std::unordered_map<std::string, void*> ScriptManager::OnInitEntity(ScriptComponent& script, uint32_t entityID, uint32_t sceneID)
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

		
		scriptClass.Class = GetClass(s_CoreAssemblyImage, s_AppAssemblyImage, scriptClass);
		scriptClass.InitClassMethods(s_CoreAssemblyImage, s_AppAssemblyImage);


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

		//if (scriptClass.OnCreateMethod)
			// CallMethod(entityInstance.GetInstance(), scriptClass.OnCreateMethod);

		std::unordered_map<std::string, void*> fields;
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
				FieldType OpFieldType = GetOpFieldType(fieldType);

				// Attributes
				MonoCustomAttrInfo* attr = mono_custom_attrs_from_field(scriptClass.Class, iter);

				auto& publicField = s_PublicFields[script.ModuleName].emplace_back(name, OpFieldType);
				PublicField* publicFieldHandlerPtr = new PublicField(name, OpFieldType);
				publicFieldHandlerPtr->m_EntityInstance = &entityInstance;
				publicFieldHandlerPtr->m_MonoClassField = iter;
				publicField.m_EntityInstance = &entityInstance;
				publicField.m_MonoClassField = iter;
				fields[name] = publicFieldHandlerPtr;
				// mono_field_set_value(entityInstance.Instance, iter, );
			}
		}

		return fields;
	}

	std::unordered_map<uint32_t, EntityInstance> ScriptManager::GetInstanceMap()
	{
		return s_EntityInstanceMap;
	}

	MonoDomain* ScriptManager::GetDomain()
	{
		return s_MonoDomain;
	}

	const OP::ScriptModuleFieldMap& ScriptManager::GetFieldMap()
	{
		return s_PublicFields;
	}

	/*void PublicField::SetValue_Internal(void* value) const
	{
		mono_field_set_value(m_EntityInstance->GetInstance(), m_MonoClassField, value);
	}

	void PublicField::GetValue_Internal(void* outValue) const
	{
		mono_field_get_value(m_EntityInstance->GetInstance(), m_MonoClassField, outValue);
	}*/
}