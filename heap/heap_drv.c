#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sysexits.h>
#include "heap.h"
#include "str.h"

static int drv_compare(const void *_x, const void *_y)
{
	long x = (long)_x, y = (long)_y;

	if (x < y)
		return -1;
	else if (x > y)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	size_t heap_size = 10, buf_size = 0;
	char *buf = NULL;
	struct heap *heap = NULL;
	int opt;

	while ((opt = getopt(argc, argv, "s:")) >= 0) {
		switch (opt) {
			case 's':
				if (sscanf(optarg, "%zu", &heap_size) != 1) {
					fprintf(stderr, "Erroneous size argument \"%s\"\n", optarg);
					return EX_USAGE;
				}
				break;
			default:
				fprintf(stderr, "Invalid option '%c'\n", (char)opt);
				return EX_USAGE;
		}
	}
	if (heap_ctor(&heap, heap_size, drv_compare)) {
		fprintf(stderr, "Heap allocation failure\n");
		return EX_OSERR;
	}
	while (!feof(stdin)) {
		ssize_t ret;
		size_t which;
		char *cmd_arg, *cmd_str;
		static struct bounded_string
			arg_delim[] = { {" ", 1}, {"\t", 1}, {"\r", 1}, {"\f", 1}, {"\v", 1}, {"\n", 1}, },
			cmd_chars[] = { {"c", 1}, {"d", 1}, {"e", 1}, {"g", 1}, {"h", 1}, {"q", 1}, {"s", 1}, };

		if (isatty(STDIN_FILENO)) {
			fputs(">> ", stdout);
			fflush(stdout);
		}
		if ((ret = getline(&buf, &buf_size, stdin)) < 0)
			break;
		cmd_arg = find_sep(&which, buf, 6, arg_delim);
		cmd_str = strndup(buf, cmd_arg - buf);
		if (strlen(cmd_str) > 1 || !find_sep(&which, cmd_str, 7, cmd_chars))
			fprintf(stderr, "Unrecognized command \"%s\"\n", cmd_str);
		else {
			void *item, **array;
			long elem;
			size_t i, count;

			switch (*cmd_str) {
				case 'c':
					printf("%zu\n", heap_count(heap));
					break;
				case 'd':
					if (heap_deq(&item, heap))
						fprintf(stderr, "Empty heap, unable to dequeue\n");
					else
						printf("%ld\n", (long)item);
					break;
				case 'e':
					if (sscanf(cmd_arg, "%ld", &elem) != 1)
						fprintf(stderr, "Invalid argument \"%s\"\n", cmd_arg);
					else if (heap_enq(heap, (void *)elem))
						fprintf(stderr, "Unable to enqueue %ld\n", elem);
					break;
				case 'g':
					if (heap_get(&array, heap))
						fprintf(stderr, "Unable to get heap contents\n");
					else {
						count = heap_count(heap);
						for (i = 0; i < count; ++i)
							printf("%zu:\t%ld\n", i, (long)array[i]);
						free(array);
					}
					break;
				case 'h':
					if (heap_hd(&item, heap))
						fprintf(stderr, "Empty heap, unable to determine head\n");
					else
						printf("%ld\n", (long)item);
					break;
				case 'q':
					free(cmd_str);
					goto loop_break;
				case 's':
					heap_sort(heap);
					break;
				default:
					assert(0);
					break;
			}
		}
		free(cmd_str);
	}
	if (isatty(STDIN_FILENO)) {
		putchar('\n');
		fflush(stdout);
	}
loop_break:
	heap_dtor(heap);
	return EX_OK;
}
