#ifndef CRYAD_CLIST_H
#define CRYAD_CLIST_H

#include "cryad/list.h"

typedef struct slist slist;

cr_list *cr_slist_create(void (*)(void *));


#endif
