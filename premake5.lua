local _error = premake.error
premake.error = function(msg, ...)
	if select("#", ...) == 0 then error("** Error: " .. msg, 0) end
	return _error(msg, ...)
end

workspace('arcane')
	configurations({ 'debug', 'release' })
	location './build'
	startproject 'arcane-editor'

	toolset 'gcc'

	include './lib/glad'
	include './arcane-editor'
	include './arcane-core'
