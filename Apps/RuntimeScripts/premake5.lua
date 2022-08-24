project "RuntimeScripts"
	kind "SharedLib"
	language "C#"

	targetdir ("%{wks.location}/Tools/Opium_Editor/assets/scripts/bin")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	postbuildcommands
	{
		'{COPY} "%{wks.location}/Apps/RuntimeScripts/src" "%{wks.location}/Tools/Opium_Editor/assets/scripts/src"'
	}

	files
	{
		"src/**.cs"
	}

	links
	{
		"Opium-Script"
	}