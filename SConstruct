env = Environment(CPPPATH='include')

env.Install('/usr/include/herc', 'include/herc/herc.h')

herc = env.Program('herc', Glob('src/*.c'), LIBS = ['dl', 'elf', 'cryad'], CFLAGS='-Wall', LINKFLAGS='-rdynamic')
env.Install('/usr/bin', herc)

env.Alias('install', ['/usr/include/herc', '/usr/bin'])

