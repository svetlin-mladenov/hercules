#include "test_finder.h"

#include <libelf.h>
#include <gelf.h>

//TODO use some custome mechanism
#include <err.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "mangler.h"

struct list_testname *get_testnames_in_file(const char *file) {
	//for more info see http://sourceforge.net/apps/trac/elftoolchain/browser/trunk/readelf/readelf.c dump_symtab
	Elf *e;
	GElf_Sym sym;
	GElf_Shdr shdr;
	Elf_Scn *scn;
	Elf_Data *data;
	int fd;
	size_t si;

	char *sym_name;

        struct list_testname *list;
        
	if (elf_version(EV_CURRENT) == EV_NONE) {
		warnx("ELF initialization failed: %s", elf_errmsg(-1));
		return NULL;
	}

	if ((fd = open(file, O_RDONLY)) < 0) {
		warn("Openning %s failed", file);
		return NULL;
	}

	if ((e = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
		warn("efl begin for file %s failed: %s", file, elf_errmsg(-1));
		return NULL;
	}

	if (elf_kind(e) != ELF_K_ELF) {
		warn("%s is not an ELF object", file);
		return NULL;
	}
	
	list = list_testname_create();
	scn = NULL;
	while ((scn = elf_nextscn(e, scn)) != NULL) {
		if (gelf_getshdr(scn, &shdr) != &shdr) {
			warn("error while gettin section header. scipping the current section.");
			continue;
		}

		if (shdr.sh_type != SHT_DYNSYM) continue;

		data = NULL;
		while ((data = elf_getdata(scn, data)) != NULL) {
			si = 0;
			while (gelf_getsym(data, si, &sym) == &sym) {
				si++;
				//TODO check if the symbol is a function
				sym_name = elf_strptr(e, shdr.sh_link, sym.st_name);
				if (mangler_is_test(sym_name)) {
					list_testname_add(list, strdup(sym_name));
				}
			}	
		}
	}
	
	elf_end(e);
	close(fd);

        return list;
}

