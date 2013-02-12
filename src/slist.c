#include "cryad/list.h"
#include "cryad/slist.h"

#include "branch_annotation.h"

#include <stdlib.h>

typedef struct slist_item {
	void *elem;
	struct slist_item *next;
} slist_item;

typedef struct slist_iter {
	cr_list_iter parent;
	slist_item *item;
} slist_iter;

typedef struct slist {
	cr_list _l;

	slist_item *first;
	size_t len;
	void (*elem_dtor)(void*);
} slist;

static void dtor(slist *l) {
	slist_item *iter;
	slist_item *current;

	for (iter = l->first; iter;) {
		current = iter;
		iter = iter->next;

		l->elem_dtor(current->elem);
		free(current);
	}
}

static int add(slist *l, void *elem) {
	/*TODO optimize*/
	slist_item *item = calloc(1, sizeof(*item));
	if (unlikely(item == NULL)) return -1;
	item->elem = elem;

	slist_item **pos = &l->first;
	while (*pos != NULL) {
		pos = &(*pos)->next;
	}
	*pos = item;

	l->len++;
	return 0;
}

static int add_first(slist *l, void *elem) {
	slist_item *item = calloc(1, sizeof(*item));
	if (unlikely(item == NULL)) return -1;
	item->elem = elem;
	item->next = l->first;
	l->first = item;
	l->len++;
	return 0;
}

static size_t length(slist *l) {
        return l->len;
}

static void* at(slist *l, size_t index) {
	size_t i;	

	if (unlikely(index >= length(l))) {
		return NULL;
	}

	slist_item *ret = l->first;
	for (i = 0; i<index; i++) {
		ret = ret->next;
	}
	return ret->elem;
}

slist_item **get_ptr_to(slist *l, size_t index) {
	size_t i;
	slist_item **pos = &l->first;
	for (i = 0; i<index; i++) {
		pos = &(*pos)->next;
	}
	return pos;
}

static void *remove(slist *l, size_t index) {
	slist_item *item;
	void *elem;

        if (unlikely(index >= length(l))) {
                return NULL;
        }

        slist_item **pos = get_ptr_to(l, index);
	item = *pos;
        *pos = (*pos)->next;
	elem = item->elem;
	free(item);
	l->len--;

        return elem;
}

static void *find(slist *l, void *trait, int (*comparator)(void*, void*)) {
	slist_item *item = l->first;
	while (item != NULL) {
		if (comparator(trait, item->elem)==0) {
			return item->elem;
		}
		item = item->next;
	}
	return NULL;
}

static bool iter_past_end(cr_list_iter *_iter) {
        slist_iter *iter = (slist_iter*)_iter;
        return iter->item == NULL;
}


static void iter_next(cr_list_iter *_iter) {
	slist_iter *iter = (slist_iter*)_iter;
	if (!iter_past_end(_iter))
		iter->item = iter->item->next;
}

static void *iter_get(cr_list_iter *_iter) {
	slist_iter *iter = (slist_iter*)_iter;
	if (!iter_past_end(_iter))
		return iter->item->elem;
	else
		return NULL;
}

static void iter_dtor(cr_list_iter *_iter) {
	slist_iter *iter = (slist_iter*)_iter;
	free(iter);
}

static cr_list_iter *iter_create(slist *l) {
	slist_iter *iter = calloc(1, sizeof(slist_iter));

	iter->parent.next = iter_next;
	iter->parent.get = iter_get;
	iter->parent.past_end = iter_past_end;
	iter->parent.dtor = iter_dtor;

	iter->item = l->first;

	return (cr_list_iter*) iter;
}



cr_list *cr_slist_create(void (*elem_dtor)(void*)) {
	slist *l = calloc(1, sizeof(*l));
	/* maybe call a list constructor here */
	l->_l.dtor = (void (*)(cr_list*)) dtor;
	l->_l.add = (int (*)(cr_list*, void*)) add;
	l->_l.add_first = (int (*)(cr_list*, void*)) add_first;
	l->_l.at = (void *(*)(cr_list*, size_t)) at;
	l->_l.remove = (void *(*)(cr_list*, size_t)) remove;
	l->_l.length = (size_t (*)(cr_list*)) length;
	l->_l.find = (void *(*)(cr_list*, void*, int (*)(void*, void*))) find;
	l->_l.iter_create = (cr_list_iter *(*)(cr_list*)) iter_create;

	l->elem_dtor = elem_dtor;

	return (cr_list*) l;
}
