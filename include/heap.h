#ifndef _HEAP_H
#define _HEAP_H
struct heap;

int heap_ctor(struct heap **, const size_t, int (*)(const void *, const void *));
void heap_dtor(struct heap *);
int heap_enq(struct heap *, const void *);
int heap_deq(void **, struct heap *);
int heap_hd(void **, const struct heap *);
size_t heap_size(const struct heap *);
size_t heap_count(const struct heap *);
void heap_sort(struct heap *);
int heap_get(void ***, const struct heap *);
#endif /* _HEAP_H */
