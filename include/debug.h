#ifndef _DEBUG
#define _DEBUG
#ifdef DEBUG
#include <stdio.h>
#define dprintf(fmt, args...)					\
	do {							\
		fprintf(stderr, "%s:%d: " fmt,			\
			__FILE__, __LINE__, ##args);		\
	} while (0)
#else /* !defined(DEBUG) */
#define dprintf(fmt, args...)					\
	do {							\
	} while (0)
#endif /* !defined(DEBUG) */
#endif /* _DEBUG */
