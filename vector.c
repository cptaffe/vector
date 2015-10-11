
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "vector.h"

enum {
	VECTOR_INIT_SIZE = 16,
	VECTOR_SCALE_FACTOR = 2
};

static bool boundsCheckVector(Vector *s, size_t i) {
	return i < s->uspace;
}

void *getVector(Vector *s, size_t i) {
	uint8_t *e;
	assert(s);
	assert(boundsCheckVector(s, i));
	assert((e = calloc(1, s->esz)));
	assert(memcpy(e, &s->buf[i * s->esz], s->esz));
	return (void *) e;
}

void setVector(Vector *s, size_t i, void *v) {
	assert(s);
	assert(v);
	assert(boundsCheckVector(s, i));
	assert(memcpy(&s->buf[i * s->esz], v, s->esz));
}

/* Length of Vector */
size_t lengthVector(Vector *s) {
	return s->uspace;
}

static void allocationPolicyVector(Vector *s) {
	assert(s);
	if (s->buf == NULL) {
		s->aspace = VECTOR_INIT_SIZE;
		assert((s->buf = calloc(s->aspace, s->esz)));
	} else if (s->uspace * s->esz > s->aspace) {
		/* Subsequent realloc, zero new memory */
		while (s->uspace * s->esz > s->aspace) {
			size_t osize = s->aspace;
			s->aspace *= VECTOR_SCALE_FACTOR;
			assert((s->buf = realloc(s->buf, s->aspace * s->esz)));
			assert(memset(&s->buf[osize * s->esz], 0, osize));
		}
	} else if (s->uspace < s->aspace/(size_t)VECTOR_SCALE_FACTOR) {
		s->aspace = s->uspace;
		assert((s->buf = realloc(s->buf, s->aspace * s->esz)));
	}
}

/* Removes chunk from [i, j) */
void *removeChunkVector(Vector *s, size_t i, size_t j) {
	uint8_t *e;
	assert(i > j);
	assert(boundsCheckVector(s, i));
	assert(boundsCheckVector(s, j));
	/* Allocate space and copy elements */
	assert((e = calloc(j-i, s->esz)));
	assert(memcpy(e, &s->buf[i * s->esz], (j-i)*s->esz));
	/* Shift elements down */
	assert(memmove(&s->buf[i*s->esz], &s->buf[j*s->esz], (s->uspace - j) * s->esz));
	s->uspace -= j-i;
	/* Allocate/Deallocate appropriately */
	allocationPolicyVector(s);
	return (void *) e;
}

void insertChunkVector(Vector *s, size_t i, void *v, size_t len) {
	size_t osize = s->uspace;
	/* Within bounds or being directly appended */
	assert(boundsCheckVector(s, i) || i == lengthVector(s));
	s->uspace += len;
	allocationPolicyVector(s);
	if (i != osize-1) {
		/* If not last element, shift memroy up */
		assert(memmove(&s->buf[(i+len)*s->esz], &s->buf[i*s->esz], (osize-i)*s->esz));
	}
	assert(memcpy(&s->buf[i*s->esz], v, len));
}

/* Remove element index i from Vector */
void *removeVector(Vector *s, size_t i) {
	return removeChunkVector(s, i, i+1);
}

/* Insert element index i in Vector */
void insertVector(Vector *s, size_t i, void *v) {
	insertChunkVector(s, i, v, 1);
}

void pushVector(Vector *s, void *elem) {
	insertVector(s, lengthVector(s), elem);
}

void *popVector(Vector *s) {
	return removeVector(s, lengthVector(s));
}

/* Create Vector from allocated chunk */
Vector fromChunkVector(void *v, size_t len) {
	Vector s;
	assert(memset(&s, 0, sizeof(Vector)));
	s.buf = v;
	s.uspace = len;
	s.aspace = len;
	return s;
}

void eachVector(Vector *s, EachVectorFunc *func, void *state) {
	size_t i;
	for (i = 0; i < s->uspace; i++) {
		void *v = getVector(s, i);
		func(state, v);
		setVector(s, i, v);
		free(v);
	}
}

IntVector initIntVector(void) {
	IntVector s;
	assert(memset(&s, 0, sizeof(s)));
	s.vector.esz = sizeof(int);
	return s;
}

void pushIntVector(IntVector *s, int i) {
	pushVector(&s->vector, &i);
}

int popIntVector(IntVector *s) {
	int j, *i = (int *) popVector(&s->vector);
	j = *i;
	free(i);
	return j;
}

int getIntVector(IntVector *s, size_t i) {
	int j, *k = (int *) getVector(&s->vector, i);
	j = *k;
	free(k);
	return j;
}

struct IterIntVector {
	EachIntVectorFunc *func;
	void *state;
};

static EachVectorFunc iterIntVector;

static void iterIntVector(void *state, void *i) {
	struct IterIntVector *st = state;
	((EachIntVectorFunc*) st->func)(st->state, (int*) i);
}

void eachIntVector(IntVector *s, EachIntVectorFunc *func, void *state) {
	struct IterIntVector st;
	st.func = func;
	st.state = state;
	eachVector(&s->vector, iterIntVector, &st);
}
