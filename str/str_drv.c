#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sysexits.h>
#include "str.h"

int main(int argc, char *argv[])
{
	int opt, line_nr = 1;
	ssize_t ret;
	size_t line_size = 0, tokens_size, i;
	char *line = NULL, **tokens = NULL;
	static struct bounded_string
		arg_delim[] = { {" ", 1}, {"\t", 1}, {"\r", 1}, {"\f", 1}, {"\v", 1}, {"\n", 1}, };

	while ((opt = getopt(argc, argv, "")) >= 0)
		;
	while (!feof(stdin)) {
		if ((ret = getline(&line, &line_size, stdin)) < 0)
			break;
		tokens_size = 0;
		if (str_split(&tokens, &tokens_size, line, 6, arg_delim))
			goto out_err;
		for (i = 0; i < tokens_size; ++i) {
			size_t n = strlen(tokens[i]);

			if (tokens[i][n-1] == '\n') {
				puts("newline at end of token");
				tokens[i][n-1] = '\0';
			}
			printf("line %d, token %zu: \"%s\"\n", line_nr, i, tokens[i]);
			free(tokens[i]);
		}
		free(tokens);
		line_nr++;
	}
	return EX_OK;
out_err:
	free(line);
	return EX_OSERR;
}
