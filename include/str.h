#ifndef _STR_H
#define _STR_H
struct bounded_string {
	char *string;
	size_t length;
};

char *find_sep(size_t *, const char *, const size_t, const struct bounded_string *);
char *skip_sep(const char *, const size_t, const struct bounded_string *);
int str_split(char ***, size_t *, const char *, const size_t, const struct bounded_string *);
#endif /* _STR_H */
