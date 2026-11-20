project('glad')
	kind('StaticLib')
	location('./build')

	language('C')
	cdialect('C17')

	targetdir('./bin/%{cfg.buildcfg}')
	objdir('./bin-int/%{cfg.buildcfg}')

	files({
		'src/glad.c',
	})

	includedirs({
		'include'
	})

	filter('configurations:debug')	
		defines({
			'_DEBUG=1'
		})
		symbols('On')

	filter('configurations:release')
		defines({
			'NDEBUG=1'
		})
		optimize('Speed')
