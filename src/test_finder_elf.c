#include "test_finder.h"

#include <libelf.h>
#include <gelf.h>

//TODO use some custome mechanism
#include <err.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include <cryad/list.h>
#include <cryad/slist.h>

#include "mangler.h"
#include "suite_desc.h"
#include "test_desc.h"

static unsigned str_hash(const char *str) {
	unsigned hash = 0;
	size_t i = 0;

	if (str == NULL)
		return hash;

	for (i = 0; str[i]; i++) {
		hash = 31*hash + str[i];
	}

	return hash;
}

Table_T get_testnames_in_file(const char *file) {
	//for more info see http://sourceforge.net/apps/trac/elftoolchain/browser/trunk/readelf/readelf.c dump_symtab
	Elf *e;
	GElf_Sym sym;
	GElf_Shdr shdr;
	Elf_Scn *scn;
	Elf_Data *data;
	int fd;
	size_t si;

	char *sym_name;

	//cr_list *list = cr_slist_create((void (*)(void*))test_desc_free);
	Table_T suites = Table_new(0, (int (*)(const void *, const void *))strcmp, (unsigned int (*)(const void *))str_hash);

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
	
	scn = NULL;
	while ((scn = elf_nextscn(e, scn)) != NULL) {
		if (gelf_getshdr(scn, &shdr) != &shdr) {
			warn("error while getting section header. skipping the current section.");
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
					//ensure suite
					char * suite_name = mangler_extract_suite(sym_name);
					suite_desc *suite = Table_get(suites, suite_name);
					if (suite == NULL) {
						suite = suite_desc_new(suite_name);
						Table_put(suites, suite->name, suite);
					}
					free(suite_name);

					cr_list_add(suite->tests, test_desc_create(sym_name));
					//cr_list_add(list, test_desc_create(sym_name));
				}
			}	
		}
	}
	
	elf_end(e);
	close(fd);

        //return list;
	return suites;
}

