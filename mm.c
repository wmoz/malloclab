#include "mm.h"
#include "memlib.h"

/**
 * @brief Team info
 * 
 */
team_t team = {
    /* ID1+ID2 or ID1 */
    "omarelgeoushywmoz",
    /* full name of first member */
    "Omar Elgeoushy",
    /* login ID of first member */
    "omarelgeoushy",
    /* full name of second member (if any) */
    "Walid Zeineldin",
    /* login ID of second member */
    "wmoz"
};

//points to the start of the heap
static char* heap; 

/**
 * @brief Before calling mm malloc mm realloc or mm free, the application program
 * (i.e., the trace-driven driver program that you will use to evaluate your implementation)
 * calls mm init to perform any necessary initializations, such as allocating the initial heap
 * area. 
 * 
 * @return int should be -1 if there was a problem in performing the initialization, 0 otherwise.
 */
extern int mm_init (void)
{
    mem_init(); //what value should be used??
    heap = mem_sbrk(4); //initializing the heap with size 4
    if (heap == NULL) 
    {
        return -1;
    }
    return 0;
}

/**
 * @brief The mm malloc routine returns a pointer to an allocated block payload of at
 * least size bytes. The entire allocated block should lie within the heap region and should
 * not overlap with any other allocated chunk.
 * 
 * Just like the standard C library (libc), whose malloc always returns payload pointers that
 * are aligned to 16 bytes, your malloc implementation should do likewise and always return 16-byte aligned pointers. 
 * The ALIGNMENT value of 16 bytes is encoded in the macro ALIGNMENT defined in config.h which you may include.
 * 
 * @param size to be malloced
 * @return void* 
 */
extern void *mm_malloc (size_t size)
{

}

/**
 * @brief The mm free routine frees the block pointed to by ptr. It returns nothing. This
 * routine is only guaranteed to work when the passed pointer (ptr) was returned by an earlier
 * call to mm malloc or mm realloc and has not yet been freed.
 * 
 * @param ptr what to be freed
 */
extern void mm_free (void *ptr)
{

}

/**
 * @brief The mm realloc routine returns a pointer to an allocated region of at least
 * size bytes with the following constraints.
 *      – if ptr is NULL, the call is equivalent to mm malloc(size);
 *      – if size is equal to zero, the call is equivalent to mm free(ptr);
 *      – if ptr is not NULL, it must have been returned by an earlier call to mm malloc or
 *        mm realloc. The call to mm realloc changes the size of the memory block pointed
 *        to by ptr (the old block) to size bytes and returns the address of the new block. Notice
 *        that the address of the new block might be the same as the old block, or it might be
 *        different, depending on your implementation, the amount of internal fragmentation in
 *        the old block, and the size of the realloc request.
 *        The contents of the new block are the same as those of the old ptr block, up to the
 *        minimum of the old and new sizes. Note that the new size may be smaller than the old
 *        size. Everything else is uninitialized. For example, if the old block is 8 bytes and the
 *        new block is 12 bytes, then the first 8 bytes of the new block are identical to the first 8
 *        bytes of the old block and the last 4 bytes are uninitialized. Similarly, if the old block is
 *        8 bytes and the new block is 4 bytes, then the contents of the new block are identical to
 *        the first 4 bytes of the old block.
 * 
 * @param ptr what to be reallocated
 * @param size the new size
 * @return void* 
 */
extern void *mm_realloc(void *ptr, size_t size)
{

}