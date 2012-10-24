#include "list_testname.h"
#include <stdlib.h>

struct list_testname *list_testname_create() {
	struct list_testname *list = calloc(1, sizeof(struct list_testname));
	
	return list;	
}

struct list_testname_item *list_testname_first(struct list_testname *list) {
	return list->first;
}

void list_testname_add(struct list_testname *list, char *testname) {
	struct list_testname_item *item = malloc(sizeof(struct list_testname));
	item->testname = testname;
	item->next = NULL;
	if (list->last != NULL) {
		list->last->next = item;
		list->last = item;
	} else {
		list->last = item;
		list->first = item;
	}
}

void list_testname_free(struct list_testname *list) {
	struct list_testname_item *item = list->first;
	struct list_testname_item *current;
	free(list);

	while (item) {
		current = item;
		item = item->next;
		
		free(current->testname);
		free(current);
	}
}

struct list_testname_item *list_testname_next(struct list_testname_item *item) {
	return item->next;
}
