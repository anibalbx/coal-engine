-- ---------------------------------
-- Anibal Simarro
-- CoalEngine
-- 2020
-- Genie Project Configuration.
-- ---------------------------------

solution ("CoalEngineSol" .. _ACTION)
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	language "C++"
	--kind "ConsoleApp"
	location ("build")

	projects = { "CoalEngineProj","demo", "luacomp"}

	for i, prj in ipairs(projects) do
		project (prj)
		targetname (prj)
		language "C++"
		prj_path = "./build/" .. prj

		location (prj_path .. "/" .. _ACTION)

		includedirs {
			"./include/",
			"./deps/",
			"./deps/include/",
			"./deps/glfw/include/",
			"./deps/px/",
			"./deps/glm/",
			"./deps/imgui/",
			"./deps/lua/",
			"./deps/glad/include/",
			"./deps/minitrace/"
		}


	    defines { "_CRT_SECURE_NO_WARNINGS" }
        flags { "ExtraWarnings" }

		configuration "vs2015"
		  --windowstargetplatformversion "8.1"

		configuration "vs2017"
		  --windowstargetplatformversion "10.0.15063.0"
			--windowstargetplatformversion "10.0.16299.0"
			--windowstargetplatformversion "10.0.17134.0"
			windowstargetplatformversion "10.0.17763.0"
			--windowstargetplatformversion "10.0.17134.471"
			--windowstargetplatformversion "15.9.17"

		configuration "Debug"
			defines { "DEBUG", "_GLFW_WIN32", "MTR_ENABLED" }
			targetdir ("bin/Debug")
			targetsuffix "_d"
			objdir ("build/" .. "Debug")
			flags { "Symbols", "NoPCH" }
			links { "opengl32" }


		configuration "Release"
			defines { "_GLFW_WIN32" }
			targetdir ("bin/Release")
			objdir ("build/" .. "Release")
			flags { "Optimize", "NoPCH" }
			links { "opengl32" }


	end

	    project "CoalEngineProj"
			kind "StaticLib"

			files {

			"./include/*.h",
			"./src/*.cpp",


			--GLFW compile
			"./deps/glfw/src/context.c",
			"./deps/glfw/src/init.c",
			"./deps/glfw/src/input.c",
			"./deps/glfw/src/monitor.c",
			"./deps/glfw/src/vulkan.c",
			"./deps/glfw/src/window.c",
			"./deps/glfw/src/win32_init.c",
			"./deps/glfw/src/win32_joystick.c",
			"./deps/glfw/src/win32_monitor.c",
			"./deps/glfw/src/win32_time.c",
			"./deps/glfw/src/win32_thread.c",
			"./deps/glfw/src/win32_window.c",
			"./deps/glfw/src/wgl_context.c",
			"./deps/glfw/src/egl_context.c",
			"./deps/glfw/src/osmesa_context.c",

			--GLAD
			"./deps/glad/src/*.c",

			--IMGUI
			"./deps/imgui/*.cpp",

			--MINITRACE
			"./deps/minitrace/*.c",
			"./deps/minitrace/*.h",


		}

		project "luacomp"
			kind "StaticLib"
			files{
			"./deps/lua/*.c",
			"./deps/lua/*.h",

			}


		project "demo"
			kind "ConsoleApp"
			files{
			"./demo/*.cpp",
			}

			links{
				"CoalEngineProj", "luacomp"
			}
