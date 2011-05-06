#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"

static int at_sep(size_t *which, const char *s, size_t nr_sep, const struct bounded_string *sep)
{
	size_t i;

	for (i = 0; i < nr_sep; ++i) {
		if (!strncmp(s, sep[i].string, sep[i].length)) {
			*which = i;
			return 1;
		}
	}
	return 0;
}

char *find_sep(size_t *which, const char *buf, const size_t nr_sep, const struct bounded_string *sep)
{
	const char *s;

	for (s = buf; *s; ++s) {
		if (at_sep(which, s, nr_sep, sep)) {
			return (char *)s;
		}
	}
	return NULL;
}

char *skip_sep(const char *s, const size_t nr_sep, const struct bounded_string *sep)
{
	size_t which;

	while (at_sep(&which, s, nr_sep, sep)) {
		s = &s[sep[which].length];
		if (!*s)
			return NULL;
	}
	return (char *)s;
}

int str_split(char ***strings, size_t *size, const char *s, const size_t nr_sep, const struct bounded_string *sep)
{
	int i, count = 0;
	const char *start, *end, **tmp;

	*strings = NULL;
	end = s;
	for (;;) {
		size_t which;

		start = skip_sep(end, nr_sep, sep);
		if (!start)
			break;
		if (!(end = find_sep(&which, start, nr_sep, sep))) {
			for (end = start; *end; ++end)
				;
		}
		++count;
		if (!(tmp = realloc(*strings, count * sizeof(char **)))) {
			--count;
			goto out_err;
		}
		*strings = (char **)tmp;
		if (!((*strings)[count - 1] = strndup(start, end - start)))
			goto out_err;
	}
	*size = count;
	return 0;
out_err:
	for (i = 0; i < count; ++i)
		free((*strings)[i]);
	free(*strings);
	*strings = NULL;
	return -1;
}
