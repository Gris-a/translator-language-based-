#ifndef OPEN_ADDRESS_QUAD_HASH_TABLE_H
#define OPEN_ADDRESS_QUAD_HASH_TABLE_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


enum HashState
{
    FREE    = 0,
    DELETED = 1,
    TAKEN   = 2
};

struct HashMemb
{
    char *key;
    size_t val;

    enum HashState state;
};


struct QuadHashTable
{
    size_t n_memb;
    size_t size;

    struct HashMemb *data;
};


struct QuadHashTable *QuadHashTableCtr(size_t size);
struct QuadHashTable *QuadHashTableDtr(struct QuadHashTable *table);

bool QuadHashTableInsert(struct QuadHashTable *table, char *key, size_t val);
bool QuadHashTableDelete(struct QuadHashTable *table, char *key);

struct HashMemb *Find(struct QuadHashTable *table, char *key);
bool QuadHashTableExists(struct QuadHashTable *table, char *key);


#endif //OPEN_ADDRESS_QUAD_HASH_TABLE_H