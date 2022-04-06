/* 
 * Simple, 64-bit allocator based on implicit free lists, 
 * first fit placement, and boundary tag coalescing, as described
 * in the CS:APP2e text. Blocks must be aligned to 16 byte
 * boundaries. Minimum block size is 16 bytes. 
 *
 * This version is loosely based on 
 * http://csapp.cs.cmu.edu/3e/ics3/code/vm/malloc/mm.c
 * but unlike the book's version, it does not use C preprocessor 
 * macros or explicit bit operations.
 *
 * It follows the book in counting in units of 4-byte words,
 * but note that this is a choice (my actual solution chooses
 * to count everything in bytes instead.)
 *
 * You may use this code as a starting point for your implementation
 * if you want.
 *
 * Adapted for CS3214 Summer 2020 by gback
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "mm.h"
#include "memlib.h"
#include "config.h"


struct boundary_tag {
    int inuse:1;        // inuse bit
    int size:31;        // size of block, in words
                        // block size
};

/* FENCE is used for heap prologue/epilogue. */
const struct boundary_tag FENCE = {
    .inuse = 1,
    .size = 0
};

/* FreeList struct used to store the free blocks */
struct freelist
{
    struct list list;
    size_t size;
};

/* A C struct describing the beginning of each block. 
 * For implicit lists, used and free blocks have the same 
 * structure, so one struct will suffice for this example.
 *
 * If each block is aligned at 12 mod 16, each payload will
 * be aligned at 0 mod 16.
 */
struct block {
    struct boundary_tag header; /* offset 0, at address 12 mod 16 */
    char payload[0];            /* offset 4, at address 0 mod 16 */ 
};

struct free_block {
    struct boundary_tag header; /* offset 0, at address 12 mod 16 */
    struct list_elem elem;
};

/* Basic constants and macros */
#define WSIZE       sizeof(struct boundary_tag)  /* Word and header/footer size (bytes) */
#define MIN_BLOCK_SIZE_WORDS 8  /* Minimum block size in words */
#define CHUNKSIZE  (1<<10)  /* Extend heap by this amount (words) */
#define NUM_LISTS 5 /* will start of as 1 just to make sure the free list works and will be changed later to 7 */
                    /* a helper function will be used to decide where the free block will be added */

static inline size_t max(size_t x, size_t y) {
    return x > y ? x : y;
}

static size_t align(size_t size) {
  return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

static bool is_aligned(size_t size) __attribute__((__unused__));
static bool is_aligned(size_t size) {
  return size % ALIGNMENT == 0;
}

/* Global variables */
static struct block *heap_listp = 0;  /* Pointer to first block */  
struct freelist freeblock_list[NUM_LISTS]; /* Free block list */

/**
 * @brief takes a freeblock and decides where it should be added
 * in the list according to its size
 * 
 * @param freeblock the block that is free to be added in the list
 * 
 * @return an int that represents the corresponding list
 */
static int get_freelist(size_t bsize) { //convert this to if // struct free_block* freeblock
    for (int i = 0; i < NUM_LISTS; i++)
    {
        if (bsize <= freeblock_list[i].size) {
            return i;
        }
    }
    return -1;
}

/* Function prototypes for internal helper routines */
static struct block *extend_heap(size_t words);
static struct block* place(struct block *bp, size_t asize);
static struct block *find_fit(size_t asize);
static struct block *coalesce(struct block *bp);

/* Given a block, obtain previous's block footer.
   Works for left-most block also. */
static struct boundary_tag * prev_blk_footer(struct block *blk) {
    return &blk->header - 1;
}

/* Return if block is free */
static bool blk_free(struct block *blk) { 
    return !blk->header.inuse; 
}

/* Return size of block is free */
static size_t blk_size(struct block *blk) { 
    return blk->header.size; 
}

/* Given a block, obtain pointer to previous block.
   Not meaningful for left-most block. */
static struct block *prev_blk(struct block *blk) {
    struct boundary_tag *prevfooter = prev_blk_footer(blk);
    assert(prevfooter->size != 0);
    return (struct block *)((void *)blk - WSIZE * prevfooter->size);
}

/* Given a block, obtain pointer to next block.
   Not meaningful for right-most block. */
static struct block *next_blk(struct block *blk) {
    assert(blk_size(blk) != 0);
    return (struct block *)((void *)blk + WSIZE * blk->header.size);
}

/* Given a block, obtain its footer boundary tag */
static struct boundary_tag * get_footer(struct block *blk) {
    return ((void *)blk + WSIZE * blk->header.size)
                   - sizeof(struct boundary_tag);
}

/* Set a block's size and inuse bit in header and footer */
static void set_header_and_footer(struct block *blk, int size, int inuse) {
    blk->header.inuse = inuse;
    blk->header.size = size;
    * get_footer(blk) = blk->header;    /* Copy header to footer */
}

/* Mark a block as used and set its size. */
static void mark_block_used(struct block *blk, int size) {
    set_header_and_footer(blk, size, 1);
}

/* Mark a block as free and set its size. */
static void mark_block_free(struct block *blk, int size) {
    set_header_and_footer(blk, size, 0);
}



/* 
 * mm_init - Initialize the memory manager 
 */
int mm_init(void) 
{
    assert (offsetof(struct block, payload) == 4);
    assert (sizeof(struct boundary_tag) == 4);
    /*initilize free_mem list*/
    list_init(&free_mem);
    /* Create the initial empty heap */
    struct boundary_tag * initial = mem_sbrk(4 * sizeof(struct boundary_tag));
    if (initial == (void *)-1)
        return -1;

    /* We use a slightly different strategy than suggested in the book.
     * Rather than placing a min-sized prologue block at the beginning
     * of the heap, we simply place two fences.
     * The consequence is that coalesce() must call prev_blk_footer()
     * and not prev_blk() because prev_blk() cannot be called on the
     * left-most block.
     */
    initial[2] = FENCE;                     /* Prologue footer */
    heap_listp = (struct block *)&initial[3];
    initial[3] = FENCE;                     /* Epilogue header */

    for (int i = 0; i < NUM_LISTS; i++)
    {
        list_init(&freeblock_list[i].list);
        if (i == 0)
        {
            freeblock_list->size = 50;
        }
        else if (i == 1) {
            freeblock_list->size = 100;
        }
        else if (i == 2) {
            freeblock_list->size = 500;
        }
        else if (i == 3) {
            freeblock_list->size = 1000;
        }
        else if (i == 4) {
            freeblock_list->size = 99999999;
        }
    }

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE) == NULL) 
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block with at least size bytes of payload 
 */
void *mm_malloc(size_t size)
{
    struct block *bp;      

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    #if(LIST_POLICY == IMPLICIT_LIST)
    size += 2 * sizeof(struct boundary_tag);    /* account for tags */
    #endif
    #if(LIST_POLICY == EXPLICIT_LIST)
    size += 2 * sizeof(struct boundary_tag);    /* account for tags  */ //no need list_elem??
    #endif
    /* Adjusted block size in words */
    size_t awords = max(MIN_BLOCK_SIZE_WORDS, align(size)/WSIZE); /* respect minimum size */

    /* Search the free list for a fit */
    if ((bp = find_fit(awords)) != NULL) 
    {
        bp = place(bp, awords);
        return bp->payload;
    }

    /* No fit found. Get more memory and place the block */
    size_t extendwords = max(awords,CHUNKSIZE); /* Amount to extend heap if no fit */
    if ((bp = extend_heap(extendwords)) == NULL)  
        return NULL;

    bp = place(bp, awords);
    return bp->payload;
} 

/* 
 * mm_free - Free a block 
 */
void mm_free(void *bp)
{
    assert (heap_listp != 0);       // assert that mm_init was called
    if (bp == 0) 
        return;

    /* Find block from user pointer */
    struct block *blk = bp - offsetof(struct block, payload);

    mark_block_free(blk, blk_size(blk));
    #if(LIST_POLICY == EXPLICIT_LIST)
    // list_push_back(&free_mem, &((struct free_block*)blk)->elem);
    struct free_block * newblk = ((struct free_block*)blk);
    int list_index = get_freelist(newblk->header.size);
    list_push_back(&freeblock_list[list_index].list, &newblk->elem); //get_freelist(newblk)].list
    #endif
    coalesce(blk);
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static struct block *coalesce(struct block *bp) 
{
    bool prev_alloc = prev_blk_footer(bp)->inuse;   /* is previous block allocated? */
    bool next_alloc = ! blk_free(next_blk(bp));     /* is next block allocated? */
    size_t size = blk_size(bp);

    if (prev_alloc && next_alloc) {            /* Case 1 */
        // both are allocated, nothing to coalesce
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        // combine this block and next block by extending it
        #if(LIST_POLICY ==EXPLICIT_LIST)
        list_remove(&((struct free_block*)next_blk(bp))->elem);
        #endif
        mark_block_free(bp, size + blk_size(next_blk(bp)));
        
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        // combine previous and this block by extending previous
         #if(LIST_POLICY ==EXPLICIT_LIST)
        list_remove(&((struct free_block*)bp)->elem);
        #endif
        bp = prev_blk(bp);
        mark_block_free(bp, size + blk_size(bp));

       
    }

    else {                                     /* Case 4 */
        // combine all previous, this, and next block into one
        mark_block_free(prev_blk(bp), 
                        size + blk_size(next_blk(bp)) + blk_size(prev_blk(bp)));

        #if(LIST_POLICY ==EXPLICIT_LIST)
        list_remove(&((struct free_block*)bp)->elem);
        list_remove(&((struct free_block*)next_blk(bp))->elem);
        #endif

        bp = prev_blk(bp);
        
    }
    return bp;
}

/*
 * mm_realloc - Naive implementation of realloc
 */
void *mm_realloc(void *ptr, size_t size)
{
    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL) {
        return mm_malloc(size);
    }

    void *newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if (!newptr) {
        return 0;
    }

    /* Copy the old data. */
    struct block *oldblock = ptr - offsetof(struct block, payload);
    size_t oldsize = blk_size(oldblock) * WSIZE;
    if (size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    return newptr;
}

/* 
 * checkheap - We don't check anything right now. 
 */
void mm_checkheap(int verbose)
{ 
}

/* 
 * The remaining routines are internal helper routines 
 */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
static struct block *extend_heap(size_t words) 
{
    
    void *bp = mem_sbrk(words * WSIZE);

    if ((intptr_t) bp == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header.
     * Note that we overwrite the previous epilogue here. */
    struct block * blk = bp - sizeof(FENCE);
    mark_block_free(blk, words);
    #if(LIST_POLICY == EXPLICIT_LIST)
    // list_push_back(&free_mem,&((struct free_block*)blk)->elem);
    struct free_block * newblk = ((struct free_block*)blk);
    int list_index = get_freelist(newblk->header.size);
    list_push_back(&freeblock_list[list_index].list, &newblk->elem);
    #endif


    next_blk(blk)->header = FENCE;

    /* Coalesce if the previous block was free */
    return coalesce(blk);
}


/* 
 * place - Place block of asize words at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static struct block* place(struct block *bp, size_t asize)
{
    size_t csize = blk_size(bp);
    #if(LIST_POLICY == EXPLICIT_LIST )
    
    if ((csize - asize) >= MIN_BLOCK_SIZE_WORDS) { 
        mark_block_free(bp, csize-asize);
        bp = next_blk(bp);
        mark_block_used(bp, asize);
        //list_push_back(&free_mem, &((struct free_block*)bp_next)->elem);
        
    }
    else 
    {
        list_remove(&((struct free_block*)bp)->elem);
        mark_block_used(bp, csize);
    }
    #endif

    #if(LIST_POLICY == IMPLICIT_LIST )
    if ((csize - asize) >= MIN_BLOCK_SIZE_WORDS) { 
        mark_block_used(bp, asize);
        bp = next_blk(bp);
        mark_block_free(bp, csize-asize);
    }
    else 
    { 
        mark_block_used(bp, csize);
    }
    #endif
    return bp;
    
}




/* 
 * find_fit - Find a fit for a block with asize words 
 */
static struct block *find_fit(size_t asize)
{
    /* First fit search */
    #if(LIST_POLICY == IMPLICIT_LIST)
    for (struct block * bp = heap_listp; blk_size(bp) > 0; bp = next_blk(bp)) {
        if (blk_free(bp) && asize <= blk_size(bp)) {
            return bp;
        }
    }
    #endif
    #if(LIST_POLICY == EXPLICIT_LIST)

    int list_index = get_freelist(asize);
    struct list_elem *e = list_begin(&freeblock_list[list_index].list);
    for (; e != list_end(&freeblock_list[list_index].list); e = list_next(e))
    {
        struct free_block *bp = list_entry(e,struct free_block,elem);
        if (blk_free((struct block*)bp) && asize <= blk_size((struct block*)bp)) {
            return (struct block*)bp;
        }
    }
    #endif
    return NULL;
     /* No fit */
}

team_t team = {
    /* Team name */
    "A7A?",
    /* First member's full name */
    "Omar Elgeoushy",
    "omarelgeoushy@vt.edu",
    /* Second member's full name (leave blank if none) */
    "Walid Zeineldin",
    "wmoz@vt.edu",
};