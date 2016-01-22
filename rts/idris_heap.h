#ifndef _IDRIS_HEAP_H
#define _IDRIS_HEAP_H

#include <stdbool.h>
#include <stddef.h>

/* *** C heap ***
 * Objects with finalizers. Mark&sweep-collected.
 */

typedef void CDataFinalizer_t(void *);

typedef struct CHeapItem {
    void * data;
    CDataFinalizer_t * finalizer;  // should free() the passed data pointer, too
    bool is_used;
    struct CHeapItem * next;
    struct CHeapItem ** prev_next;
} CHeapItem;

typedef struct CHeap {
    CHeapItem * first;
} CHeap;

void c_heap_init(CHeap * c_heap);
void c_heap_destroy(CHeap * c_heap);  // will call finalizers on everything
void c_heap_insert_if_needed(CHeap * c_heap, CHeapItem * item);
void c_heap_mark_item(CHeapItem * item);
void c_heap_sweep(CHeap * c_heap);
CHeapItem * c_heap_create_item(void * data, CDataFinalizer_t * finalizer);

// this is the function to use in C binding code
CHeapItem * c_heap_allocate(size_t size, CDataFinalizer_t * finalizer);
void c_heap_free(CHeapItem * item);

/* *** Idris heap **
 * Objects without finalizers. Cheney-collected.
 */

typedef struct {
    char*  next;   // Next allocated chunk. Should always (heap <= next < end).
    char*  heap;   // Point to bottom of heap
    char*  end;    // Point to top of heap
    size_t size;   // Size of _next_ heap. Size of current heap is /end - heap/.
    size_t growth; // Quantity of heap growth in bytes.

    char* old;
} Heap;


void alloc_heap(Heap * heap, size_t heap_size, size_t growth, char * old);
void free_heap(Heap * heap);


#ifdef IDRIS_DEBUG
void heap_check_all(Heap * heap);
// Should be used _between_ gc's.
#define HEAP_CHECK(vm) heap_check_all(&(vm->heap));
#else
#define HEAP_CHECK(vm)
#endif // IDRIS_DEBUG

#endif // _IDRIS_HEAP_H
