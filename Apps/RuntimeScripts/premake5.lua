project "RuntimeScripts"
	kind "SharedLib"
	language "C#"

	targetdir ("%{wks.location}/Tools/Opium_Editor/assets/scripts")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	postbuildcommands
	{
		'{COPY} "src" "%{cfg.targetDir}"'
	}

	files
	{
		"src/**.cs"
	}

	links
	{
		"Opium-Script"
	}