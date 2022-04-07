#include <stdio.h>
#include "list.h"

#define IMPLICIT_LIST 1
#define EXPLICIT_LIST 2
#define SEG_LIST      3

#define LIST_POLICY  EXPLICIT_LIST
extern int mm_init (void);
extern void *mm_malloc (size_t size);
extern void mm_free (void *ptr);
extern void *mm_realloc(void *ptr, size_t size);

struct list free_mem;

/* 
 * Students work in teams of one or two.  Teams enter their team name, 
 * personal names and login IDs in a struct of this
 * type in their mm.c file.
 */
typedef struct {
    char *teamname; /* ID1+ID2 or ID1 */
    char *name1;    /* full name of first member */
    char *id1;      /* login ID of first member */
    char *name2;    /* full name of second member (if any) */
    char *id2;      /* login ID of second member */
} team_t;

extern team_t team;

