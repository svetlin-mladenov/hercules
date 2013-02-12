#include "cryad/list.h"

void cr_list_free(cr_list *l) {
	l->dtor(l);
}

int cr_list_add(cr_list *l, void *element) {
	return l->add(l, element);
}

int cr_list_add_first(cr_list *l, void *element) {
	return l->add_first(l, element);
}

void *cr_list_at(cr_list *l, size_t index) {
	return l->at(l, index);
}

void *cr_list_remove(cr_list *l, size_t index) {
	return l->remove(l, index);
}

size_t cr_list_length(cr_list *l) {
	return l->length(l);
}

void *cr_list_find(cr_list *l, void *trait, int (*comparator) (void *, void *)) {
	return l->find(l, trait, comparator);
}

cr_list_iter *cr_list_iter_create(cr_list *l) {
	return l->iter_create(l);
}

void cr_list_iter_next(cr_list_iter *iter) {
	return iter->next(iter);
}

void *cr_list_iter_get(cr_list_iter *iter) {
	return iter->get(iter);
}

bool cr_list_iter_past_end(cr_list_iter *iter) {
	return iter->past_end(iter);
}

void cr_list_iter_free(cr_list_iter *iter) {
	iter->dtor(iter);
}
