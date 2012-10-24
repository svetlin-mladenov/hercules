env = Environment(CPPPATH='include')

env.Install('/usr/include/herc', 'include/herc.h')

herc = env.Program('herc', Glob('src/*.c'), LIBS = ['dl', 'elf'], CFLAGS='-Wall', LINKFLAGS='-rdynamic')
env.Install('/usr/bin', herc)

env.Alias('install', ['/usr/include/herc', '/usr/bin'])

