
#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdint.h>

struct Vector {
	uint8_t *buf;
	size_t aspace, uspace, esz;
};
typedef struct Vector Vector;
typedef void(EachVectorFunc)(void *state, void *elem);
Vector fromChunkVector(void *v, size_t len);
void *getVector(Vector *s, size_t i);
void setVector(Vector *s, size_t i, void *v);
void pushVector(Vector *s, void *v);
void *popVector(Vector *s);
void insertVector(Vector *s, size_t i, void *v);
void *removeVector(Vector *s, size_t i);
void insertChunkVector(Vector *s, size_t i, void *v, size_t len);
void *removeChunkVector(Vector *s, size_t i, size_t j);
size_t lengthVector(Vector *s);
void eachVector(Vector *s, EachVectorFunc *func, void *state);

struct IntVector {
	Vector vector;
};
typedef struct IntVector IntVector;
typedef void(EachIntVectorFunc)(void *state, int *elem);
IntVector initIntVector(void);
void pushIntVector(IntVector *s, int i);
int popIntVector(IntVector *s);
int getIntVector(IntVector *s, size_t i);
void eachIntVector(IntVector *s, EachIntVectorFunc *func, void *state);

#endif /* VECTOR_H_ */
