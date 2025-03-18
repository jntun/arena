#include <stdlib.h>
#include <stdio.h>

const static int MEMINCR = 10;

struct arena
{
	struct arena *next;
	char *limit;
	char *avail;
};

extern struct arena first[];
extern struct arena *arena[]; 

char *arena_alloc(int n, struct arena **p)
{   
    struct arena *ap;

    for (ap = *p; ap->avail + n > ap->limit; *p = ap)
        if (ap->next) {
            ap = ap->next;
			ap->avail = (char *)ap + sizeof *ap;
		} else {
			int m = ((n + 3)&3) + MEMINCR*1024 + sizeof *ap;
			ap->next  = (struct arena *) malloc(m);
			ap = ap->next;
			ap->limit = (char *)ap + m;
			ap->avail = (char *)ap + sizeof *ap;
			ap->next  = 0;
		}
	ap->avail += n;
	return ap->avail - n;
}

void deallocate(int t)
{
	if (arena[t] == first[t].next)
		arena[t]->avail = (char *)arena[t] + sizeof *arena[t];
	else
		arena[t] = &first[t];
}

void *allocate(int k, int t)
{
	char *p = arena[t]->avail;
	if ((arena[t]->avail += k) > arena[t]->limit)
		p = arena_alloc(k, &arena[t]);

	return (void *)p;
}
