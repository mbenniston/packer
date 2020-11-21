workspace "packer"
	language "C"
	architecture "x86_64"
	configurations { "Debug", "Release" }
	cdialect 'C99' 

	filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"
	
	filter { "configurations:Release" }
        runtime "Release"
        optimize "On"
        defines { "NDEBUG" }
        
	filter { }
	
	targetdir (".")
    objdir (".")

project "packer"
	kind "ConsoleApp"

    files "packer.c"

    filter {}
