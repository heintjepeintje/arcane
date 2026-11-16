project('arcane-core')
	kind('StaticLib')
	location('./build')

	language('C++')
	cppdialect('C++23')

	targetdir('./bin/%{cfg.buildcfg}')
	objdir('./bin-int/%{cfg.buildcfg}')

	files({
		'src/**.cc',
	})

	includedirs({
		'%{wks.location}/../lib/minilog/include',
		'src'
	})

	filter('configurations:debug')	
		defines({
			'AR_BUILD_CONFIG_DEBUG=1',
			'_DEBUG=1'
		})
		symbols('On')

	filter('configurations:release')
		defines({
			'AR_BUILD_CONFIG_RELEASE=1',
			'NDEBUG=1'
		})
		optimize('Speed')
