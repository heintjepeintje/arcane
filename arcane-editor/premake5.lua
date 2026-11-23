project('arcane-editor')
	kind('ConsoleApp')
	location('./build')

	language('C++')
	cppdialect('C++23')

	targetdir('./bin/%{cfg.buildcfg}')
	objdir('./bin-int/%{cfg.buildcfg}')

	files({
		'src/**.cc',
	})

	prebuildcommands({
		'glslc --target-env=opengl -fshader-stage=vertex -fentry-point=main -c %{prj.location}/../res/shaders/default/vertex.glsl -o %{prj.location}/../bin/shaders/default/vertex.spv.o',
		'glslc --target-env=opengl -fshader-stage=fragment -fentry-point=main -c %{prj.location}/../res/shaders/default/fragment.glsl -o %{prj.location}/../bin/shaders/default/fragment.spv.o',
	})

	includedirs({
		'%{wks.location}/../lib/minilog/include',
		'%{wks.location}/../lib/glfw/include',
		'%{wks.location}/../lib/glm',
		'%{wks.location}/../lib/glad/include',
		'%{wks.location}/../arcane-core/src',
		'src'
	})

	libdirs({
		'%{wks.location}/../lib/minilog/bin',
		'%{wks.location}/../lib/glfw/build/src'
	})

	links({
		'arcane-core',
		'minilog',
		'glfw3',
		'glad'
	})

	filter('configurations:debug')	
		defines({
			'AR_BUILD_CONFIG_DEBUG=1',
			'AR_NO_TRACE=1',
			'_DEBUG=1'
		})
		symbols('On')

	filter('configurations:release')
		defines({
			'AR_BUILD_CONFIG_RELEASE=1',
			'AR_DISABLE_LOGGING',
			'NDEBUG=1'
		})
		optimize('Speed')
