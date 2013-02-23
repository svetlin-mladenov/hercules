#include "herc_rbe.h"

#include <stdlib.h>

#include "halloc.h"
#include "herc_rbe_cout.h"


struct rbe_list;

struct herc_default_rbe {
	struct herc_rbe rbe;

	struct rbe_list *wrapped;
};

struct rbe_list {
	struct rbe_list *next;
	struct herc_rbe *rbe;
};

static void make_delegates(struct herc_default_rbe *rbe);
static err_t fill_rbes(struct herc_default_rbe *rbe, struct herc *h);

/* PUBLIC API */

err_t herc_make_default_rbe(struct herc_rbe **rbe_res, struct herc *h) {
	err_t err;
	struct herc_default_rbe *def_rbe;
	*rbe_res = NULL;

	ERR_RET(halloc(def_rbe, sizeof(struct herc_default_rbe)));
	def_rbe->wrapped = NULL;
	make_delegates(def_rbe);

	/* the structure is already made i.e it's safe to call free if it fails*/
	if ( (err = fill_rbes(def_rbe, h)) ) {
		free(def_rbe);
	} else {
		*rbe_res = (struct herc_rbe*) def_rbe;
	}

	return err;
}

/* PRIVATE FUNCTION */

#define DEFINE_DELEGATE_ARG(name, arg_type, arg_name) \
	static err_t name ## _delegate(struct herc_rbe *raw_rbe, arg_type arg_name) {\
		err_t err; \
		struct herc_default_rbe *rbe = (struct herc_default_rbe*)raw_rbe; \
		struct rbe_list *wrapped = rbe->wrapped; \
		while (wrapped != NULL) { \
			ERR_RET(herc_rbe_invoke(wrapped->rbe, name, arg_name)); \
			wrapped = wrapped->next; \
		} \
		return OK; \
	}

DEFINE_DELEGATE_ARG(start, struct herc *, h);
DEFINE_DELEGATE_ARG(start_file, const char *, filename);
DEFINE_DELEGATE_ARG(start_suite, const char *, suitename);
DEFINE_DELEGATE_ARG(start_test, struct test_desc *, desc);
DEFINE_DELEGATE_ARG(end_test, struct test_desc *, desc);
DEFINE_DELEGATE_ARG(end_suite, struct herc *, h);
DEFINE_DELEGATE_ARG(end_file, struct herc *, h);
DEFINE_DELEGATE_ARG(end, struct herc *, h); //TODO free memory

#undef DEFINE_DELEGATE_ARG

static void make_delegates(struct herc_default_rbe *rbe) {
#define SET_DELEGATE(name) rbe->rbe.name = name ## _delegate

	SET_DELEGATE(start);
	SET_DELEGATE(start_file);
	SET_DELEGATE(start_suite);
	SET_DELEGATE(start_test);
	SET_DELEGATE(end_test);
	SET_DELEGATE(end_suite);
	SET_DELEGATE(end_file);
	SET_DELEGATE(end);

#undef SET_DELEGATE
}

static err_t push_rbe(struct herc_default_rbe *rbe, struct herc_rbe *rbe_to_add) {
	err_t err;
	struct rbe_list *elem;

	ERR_RET(halloc(elem, sizeof(struct rbe_list)));

	elem->rbe = rbe_to_add;
	elem->next = rbe->wrapped;
	rbe->wrapped = elem;

	return OK;
}

static err_t fill_rbes(struct herc_default_rbe *rbe, struct herc *h) {
	err_t err;
	err = 0;
	struct herc_rbe *cout_rbe;

	ERR_RET(herc_rbe_cout_make(&cout_rbe, h));
	push_rbe(rbe, cout_rbe);

	return OK;
}
