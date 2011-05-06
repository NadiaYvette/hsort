#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "debug.h"

struct heap {
	int (*compare)(const void *, const void *);
	size_t size, count;
	void **heap;
};

int heap_ctor(struct heap **heap, const size_t size, int (*compare)(const void *, const void *))
{
	if (!(*heap = malloc(sizeof(struct heap))))
		return -1;
	(*heap)->compare = compare;
	(*heap)->size = size;
	(*heap)->count = 0;
	if (!((*heap)->heap = calloc(size, sizeof(void *)))) {
		free(*heap);
		*heap = NULL;
		return -1;
	}
	return 0;
}

void heap_dtor(struct heap *heap)
{
	free(heap->heap);
	free(heap);
}

int heap_hd(void **head, const struct heap *heap)
{
	if (!heap->count)
		return -1;
	*head = heap->heap[0];
	return 0;
}

size_t heap_size(const struct heap *heap)
{
	return heap->size;
}

size_t heap_count(const struct heap *heap)
{
	return heap->count;
}

int heap_get(void ***array, const struct heap *heap)
{
	if (!heap->count) {
		*array = NULL;
		return 0;
	}
	if (!(*array = calloc(heap->count, sizeof(void *))))
		return -1;
	memcpy(*array, heap->heap, heap->count * sizeof(void *));
	return 0;
}

int heap_enq(struct heap *heap, const void *item)
{
	size_t i, j;

	if (heap->count >= heap->size)
		return -1;
	heap->heap[heap->count] = (void *)item;
	i = heap->count;
	while (i > 0) {
		j = ((i & 1) ? i : (i - 1)) / 2;
		if ((*heap->compare)(heap->heap[i], heap->heap[j]) >= 0)
			break;
		else {
			void *tmp = heap->heap[i];

			heap->heap[i] = heap->heap[j];
			heap->heap[j] = tmp;
		}
		i = j;
	}
	heap->count++;
	return 0;
}

int heap_deq(void **head, struct heap *heap)
{
	size_t i, j;

	if (!heap->count)
		return -1;
	*head = heap->heap[0];
	heap->count--;
	heap->heap[0] = heap->heap[heap->count];
	i = 0;
	j = 1;
	while (j < heap->count) {
		if (j + 1 < heap->count && (*heap->compare)(heap->heap[j], heap->heap[j + 1]) > 0)
			j++;
		if ((*heap->compare)(heap->heap[i], heap->heap[j]) <= 0)
			break;
		else {
			void *tmp = heap->heap[i];

			heap->heap[i] = heap->heap[j];
			heap->heap[j] = tmp;
		}
		i = j;
		j = 2 * i + 1;
	}
	return 0;
}

void heap_sort(struct heap *heap)
{
	void *tmp;
	size_t count = heap->count;

	if (!count)
		return;
	while (count) {
		tmp = heap->heap[0];
		size_t i, j;

		--count;
		/* XXX: Bad code duplication. Abstract somehow. */
		heap->heap[0] = heap->heap[count];
		heap->heap[count] = tmp;
		i = 0;
		j = 1;
		while (j < count) {
			if (j + 1 < count && (*heap->compare)(heap->heap[j], heap->heap[j + 1]) > 0)
				j++;
			if ((*heap->compare)(heap->heap[i], heap->heap[j]) <= 0)
				break;
			else {
				tmp = heap->heap[i];
				heap->heap[i] = heap->heap[j];
				heap->heap[j] = tmp;
			}
			i = j;
			j = 2 * i + 1;
		}
	}
	/* reversing the resulting order's required to keep heaps valid */
	for (count = 0; count < heap->count - count - 1; ++count) {
		tmp = heap->heap[count];
		heap->heap[count] = heap->heap[heap->count - count - 1];
		heap->heap[heap->count - count - 1] = tmp;
	}
}
