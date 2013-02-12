#ifndef CRYAD_LIST_H
#define CRYAD_LIST_H

#include <stdlib.h>
#include <stdbool.h>

struct cr_list_iter;

typedef struct cr_list {
	void (*dtor)(struct cr_list*);
        int (*add)(struct cr_list *, void *);
        int (*add_first)(struct cr_list *, void *);
	void *(*at)(struct cr_list *, size_t);
	void *(*remove)(struct cr_list*, size_t);
	size_t (*length)(struct cr_list *);
	void *(*find)(struct cr_list *, void *, int (*)(void*, void*));
	struct cr_list_iter *(*iter_create)(struct cr_list *);
} cr_list;

typedef struct cr_list_iter {
	void (*next)(struct cr_list_iter *);
	void *(*get)(struct cr_list_iter *);
	bool (*past_end)(struct cr_list_iter *);
	void (*dtor)(struct cr_list_iter *);
} cr_list_iter;

void cr_list_free(cr_list *);

/* adds an element to the end of the list */
int cr_list_add(cr_list *, void *);

/* adds an element to the beginning of the list */
int cr_list_add_first(cr_list *, void *);

void *cr_list_at(cr_list *, size_t);

/* returns the length of the list */
size_t cr_list_length(cr_list *);

/* removes and returns the ith element of the list */
void *cr_list_remove(cr_list *, size_t);

/* finds and elment with a concreat property */
void *cr_list_find(cr_list *, void *, int (*)(void*, void*));

/* returns an iterato pointing to the first element of the list */
cr_list_iter *cr_list_iter_create(cr_list *);

void cr_list_iter_next(cr_list_iter *);

void *cr_list_iter_get(cr_list_iter *);

bool cr_list_iter_past_end(cr_list_iter *);

void cr_list_iter_free(cr_list_iter *);

/* gets the first element of the list */

/* gets the last element of the list */

/* gets and removes the first element of the list */

/* gets and removes the last elemen of the list */

#endif
