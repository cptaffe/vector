
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

void eachInt(int *i, int *j) {
	if (*i != *j) {
		printf("%d != %d!\n", *i, *j);
	}
	(*i)++;
}

int main(int argc, char *argv[]) {
	IntVector s = initIntVector();
	int i, max;
	int ret = 0;
	if (argc != 2) {
		(void) printf("Usage: %s n\n", argv[0]);
		ret = 1;
		goto Cleanup;
	}
	max = (int) strtol(argv[1], NULL, 10);
	for (i = 0; i < max; i++) {
		pushIntVector(&s, i);
	}
	i = 0;
	eachIntVector(&s, (EachIntVectorFunc*) eachInt, &i);
Cleanup:
	free(s.vector.buf);
	return ret;
}
