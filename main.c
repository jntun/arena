#include <stdlib.h>
#include <stdio.h>

const static int MEMINCR = 10;

struct arena
{
	struct arena *next;
	char *limit;
	char *avail;
};

struct node
{
    struct node *previous;
    struct node *next;
    int value;
};

static struct arena first[] = {
	{ .next = 0, .limit = 0, .avail = 0 },
	{ .next = 0, .limit = 0, .avail = 0 },
};
static struct arena *arena[] = {
	&first[0],
	&first[1],
};

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

void add_to_linked_list(struct node *head, int lifetime)
{
	struct node *prev = head;

	for (int i = 1; i < 1000; i++)
	{
		struct node *a = NULL;
		a = allocate(sizeof(struct node), lifetime);

		a->previous    = prev;
		a->next        = 0;
		a->value       = i;
		a->previous->next = a;
		prev = a;
	}
}

int main()
{
	int lifetime = 1;
	struct node *head = allocate(sizeof(struct node), 0);
	head->previous = 0;
	head->next     = 0;
	head->value    = 0;

	add_to_linked_list(head, lifetime);
	
	struct node *iter = head;
	printf("%d, ", iter->value);
	while ((iter = iter->next))
	{
		printf("%d, ", iter->value);
	}
	puts("");

    return head->value;
}
