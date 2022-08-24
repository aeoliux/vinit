project('vinit', 'c', version: '0.1.0')

maxinitreq = get_option('maximum-init-request')
inittab = get_option('inittab-path')

cflags = [
	'-DINIT_MAX_REQUEST=@0@'.format(maxinitreq),
	'-DINITTABFILE="@0@"'.format(inittab)
]
add_global_arguments(cflags, language: 'c')

initsrc = [
	'init/inittab.c',
	'init/main.c',
	'init/util.c'
]

init = executable('init', initsrc, install: true)
initreq = executable('initreq', 'initreq/initreq.c', install: true)
reboot = executable('reboot', 'initreq/reboot.c', install: true)
poweroff = executable('poweroff', 'initreq/poweroff.c', install: true)
halt = executable('halt', 'initreq/halt.c', install: true)