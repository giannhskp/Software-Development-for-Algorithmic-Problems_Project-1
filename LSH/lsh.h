#ifndef LSH_H
#define LSH_H

typedef struct lsh_n * LSH;

LSH initializeLSH(int );
void insertToLSH(LSH ,Vector );
void printLSH(LSH );
void destroyLSH(LSH );

void nearestNeigbor(LSH ,Vector );
#endif