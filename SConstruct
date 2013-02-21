""" HELPER BUILDERS """

def download_dep(env, name, repo_type, repo_url):
    """download and build a dependency"""
    commands = [
        "svn co " + repo_url + " deps/" + name,
        "cd ./deps/"+name + "; ./configure", "touch deps/"+name+"/Configure.marker",
        "cd ./deps/"+name + "; make", "touch deps/"+name+"/Makefile.marker"
    ]
    i1 = env.Command("deps/"+"mxml", "", commands);
    return i1

""" SCONS SETUP """

env = Environment(CPPPATH='include')
env.AddMethod(download_dep, "DownloadDep")


""" DEPS """
env.DownloadDep("mxml", "svn", "http://svn.easysw.com/public/mxml/tags/release-2.7")

""" BUILD """
env.Install('/usr/include/herc', 'include/herc/herc.h')

herc = env.SharedLibrary('herc', Glob('src/*.c'), LIBS = ['dl', 'elf'], CFLAGS='-Wall', LINKFLAGS='-rdynamic')
env.Install('/usr/lib', herc)

env.Alias('install', ['/usr/include/herc', '/usr/lib'])

