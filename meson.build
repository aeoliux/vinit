project('vinit', 'c')

initsrc = [
	'init/inittab.c',
	'init/main.c',
	'init/util.c'
]

init = executable('init', initsrc, install: true)
initreq = executable('initreq', 'initreq/initreq.c', install: true)