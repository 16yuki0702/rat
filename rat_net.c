#include "rat_net.h"

void
free_buf(buf *b)
{
	free(b->d);
	free(b);
}
