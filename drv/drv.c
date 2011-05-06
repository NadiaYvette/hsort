#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sysexits.h>
#include "heap.h"
#include "str.h"

struct opt {
	unsigned numeric, reverse, field, lines, nr_delims;
	struct bounded_string *delims;
};

struct elem {
	char *line;
	union {
		double x;
		char *s;
	} key;
};

static struct bounded_string default_delims[] = { {" ", 1}, {"\t", 1}, {"\r", 1}, {"\f", 1}, {"\v", 1}, {"\n", 1}, };

static int compare_number(const void *_x, const void *_y)
{
	double x = ((struct elem *)_x)->key.x, y = ((struct elem *)_y)->key.x;

	if (isgreater(x, y))
		return 1;
	else if (isless(x, y))
		return -1;
	else
		return 0;
}

static int compare_number_rev(const void *x, const void *y)
{
	return -compare_number(x, y);
}

static int compare_string(const void *x, const void *y)
{
	return strcmp(((struct elem *)x)->key.s, ((struct elem *)y)->key.s);
}

static int compare_string_rev(const void *x, const void *y)
{
	return -strcmp(((struct elem *)x)->key.s, ((struct elem *)y)->key.s);
}

static void heap_insert(struct heap *heap, void *x, int (*compare)(const void *, const void *), struct opt *opt)
{
	void *y;

	if (heap_count(heap) == heap_size(heap)) {
		struct elem *elem;

		heap_deq(&y, heap);
		if ((*compare)(x, y) >= 0) {
			elem = (struct elem *)y;
			free(elem->line);
			if (!opt->numeric)
				free(elem->key.s);
			free(y);
		} else {
			elem = (struct elem *)x;
			free(elem->line);
			if (!opt->numeric)
				free(elem->key.s);
			free(x);
			heap_enq(heap, y);
			return;
		}
	}
	heap_enq(heap, x);
}

int main(int argc, char *argv[])
{
	int c;
	size_t i, line_size;
	ssize_t line_ret;
	char *line = NULL, *delim;
	void **array = NULL;
	int (*compare)(const void *, const void *);
	struct heap *heap = NULL;
	struct opt opt	= {	.numeric = 0,
				.reverse = 0,
				.field = 1,
				.lines = 10,
				.nr_delims = 6,
			};
	opt.delims = default_delims;
	while ((c = getopt(argc, argv, "nrd:f:l:")) >= 0) {
		switch (c) {
			case 'd':
				if (!(delim = strdup(optarg))) {
					fprintf(stderr, "Failure to allocate space for delimiter \"%s\"\n", optarg);
					return EX_OSERR;
				}
				if (opt.delims == default_delims) {
					opt.nr_delims = 1;
					if (!(opt.delims = malloc(sizeof(struct bounded_string)))) {
						fprintf(stderr, "Failure to allocate space for delimiter \"%s\"\n", optarg);
						return EX_OSERR;
					}
					opt.delims[0].length = strlen(delim);
					opt.delims[0].string = delim;
				} else {
					opt.nr_delims++;
					if (!(opt.delims = realloc(opt.delims, opt.nr_delims * sizeof(struct bounded_string)))) {
						fprintf(stderr, "Failure to allocate space for delimiter \"%s\"\n", optarg);
						return EX_OSERR;
					}
					opt.delims[opt.nr_delims - 1].length = strlen(delim);
					opt.delims[opt.nr_delims - 1].string = delim;
				}
				break;
			case 'n':
				opt.numeric = !opt.numeric;
				break;
			case 'r':
				opt.reverse = !opt.reverse;
				break;
			case 'f':
				if (sscanf(optarg, "%u", &opt.field) != 1 || !opt.field) {
					fprintf(stderr, "Invalid field number \"%s\"\n", optarg);
					return EX_USAGE;
				}
				break;
			case 'l':
				if (sscanf(optarg, "%u", &opt.lines) != 1 || !opt.lines) {
					fprintf(stderr, "Invalid line count number \"%s\"\n", optarg);
					return EX_USAGE;
				}
				break;
			default:
				fprintf(stderr, "Invalid option character \"%c\"\n", (char)c);
				return EX_USAGE;
		}
	}
	if (opt.reverse)
		compare = opt.numeric ? compare_number_rev : compare_string_rev;
	else
		compare = opt.numeric ? compare_number : compare_string;
	if (heap_ctor(&heap, opt.lines, compare)) {
		fprintf(stderr, "Failure to allocate heap\n");
		return EX_OSERR;
	}
	while ((line_ret = getline(&line, &line_size, stdin)) > 0) {
		char **token = NULL;
		size_t i, nr_tokens = 0;

		if (str_split(&token, &nr_tokens, line, opt.nr_delims, opt.delims))
			return EX_OSERR;
		for (i = 0; i < nr_tokens; ++i) {
			struct elem *elem;

			if (i != opt.field - 1) {
				free(token[i]);
				continue;
			}

			if (!(elem = malloc(sizeof(struct elem)))) {
				fprintf(stderr, "Failure to allocate heap element for %s\n", token[i]);
				return EX_OSERR;
			}
			elem->line = line;
			line = NULL;
			if (!opt.numeric)
				elem->key.s = token[i];
			else {
				if (sscanf(token[i], "%lf", &elem->key.x) != 1)
					elem->key.x = 0.0 / 0.0;
				free(token[i]);
			}
			heap_insert(heap, elem, compare, &opt);
		}
		free(token);
	}
	heap_sort(heap);
	if (heap_get(&array, heap)) {
		fprintf(stderr, "Failure to allocate heap copy array\n");
		return EX_OSERR;
	}
	for (i = heap_count(heap) ; i > 0; --i) {
		struct elem *elem = (struct elem *)array[i-1];

		fputs(elem->line, stdout);
		free(elem->line);
		if (!opt.numeric)
			free(elem->key.s);
		free(elem);
	}
	free(array);
	heap_dtor(heap);
	return EX_OK;
}
