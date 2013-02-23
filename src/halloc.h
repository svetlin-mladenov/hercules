#ifndef HALLOC_H_
#define HALLOC_H_

#include "herr.h"
#include <stdlib.h>

#define halloc(res, size) ((res=calloc(1,size)) != NULL) ? OK : err_nomem()

#endif /* HALLOC_H_ */
