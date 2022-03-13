project "ExampleOpiumApp"
	kind "SharedLib"
	language "C#"

	targetdir ("%{wks.location}/Tools/Opium_Editor/assets/scripts")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.cs"
	}

	links
	{
		"Opium-Script"
	}