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
vinit_logger = executable('vinit-logger', 'vinit-logger/main.c', install: true)

daemonutilsrc = [
	'vinit-daemonutil/checkpid.c',
	'vinit-daemonutil/main.c',
	'vinit-daemonutil/start.c',
	'vinit-daemonutil/stop.c',
	'vinit-daemonutil/util.c'
]
vinit_daemonutil = executable('vinit-daemonutil', daemonutilsrc, install: true)