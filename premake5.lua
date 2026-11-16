workspace('arcane')
	configurations({ 'debug', 'release' })
	location './build'
	startproject 'arcane-editor'

	include './arcane-editor'
	include './arcane-core'
