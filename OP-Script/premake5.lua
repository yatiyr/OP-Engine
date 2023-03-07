project "OP-Script"
	kind "SharedLib"
	language "C#"
	clr "Unsafe"

	targetdir ("%{wks.location}/Tools/OP_Editor/Resources/Scripts")
	objdir ("%{wks.location}/Tools/OP_Editor/Resources/Scripts/Intermediates")

	files
	{
		"Source/**.cs",
		"Properties/**.cs"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"


	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"