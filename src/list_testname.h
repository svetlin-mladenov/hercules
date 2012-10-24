#ifndef LIST_TESTS_H
#define LIST_TESTS_H

struct list_testname {
	struct list_testname_item *first;
	struct list_testname_item *last;
};

struct list_testname_item {
	char *testname;
	struct list_testname_item *next;
};

struct list_testname *list_testname_create();

struct list_testname_item *list_testname_first(struct list_testname *list);

void list_testname_add(struct list_testname *list, char *);

void list_testname_free(struct list_testname *list);

struct list_testname_item *list_testname_next(struct list_testname_item *);


#endif
