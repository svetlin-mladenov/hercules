env = Environment(CPPPATH='include')

env.Install('/usr/include/herc', 'include/herc/herc.h')

herc = env.SharedLibrary('herc', Glob('src/*.c'), LIBS = ['dl', 'elf'], CFLAGS='-Wall', LINKFLAGS='-rdynamic')
env.Install('/usr/lib', herc)

env.Alias('install', ['/usr/include/herc', '/usr/lib'])

