#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec_node *Vector;

Vector initVector(double *);

void deleteVector(Vector);

void printVector(Vector );

int compareVectors(Vector , Vector );

double *getCoords(Vector);

#endif
