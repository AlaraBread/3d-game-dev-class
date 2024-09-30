#ifndef __ASSERTS_H__
#define __ASSERTS_H__

#include "simple_logger.h"

#define ASSERT(x)                                                                                                      \
	if(!(x)) {                                                                                                         \
		slog("assertion failed");                                                                                      \
		exit(1);                                                                                                       \
	}
#define ASSERT_EQ_I(a, b)                                                                                              \
	if(!((a) == (b))) {                                                                                                \
		slog("assertion failed: %d != %d", (a), (b));                                                                  \
		exit(1);                                                                                                       \
	}

#endif
