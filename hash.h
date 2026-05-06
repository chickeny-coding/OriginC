#ifndef _HASH_C_
#define _HASH_C_

#include "errors.h"
#include <string.h>

typedef const char *HashKeyType;
typedef unsigned long long HashValueType;

#define FNV1A_OFFSET    14695981039346656037ULL
#define FNV1A_PRIME     1099511628211ULL
#define HASH_INIT_CAP   16

typedef struct _ListNode
{
    HashKeyType key;
    HashValueType value;
    struct _ListNode *next;
} ListNode;

typedef struct _HashTable
{
    size_t size;
    size_t cap;
    ListNode **bucket;
} HashTable;

bool hash_init(HashTable *const ht)
{
    ht->cap = HASH_INIT_CAP;
    ht->bucket = (ListNode **)(alloc(ht->cap, sizeof(ListNode *), true));
    return !ht->bucket;
}

/*
    String key must be null-termined.
*/
static HashValueType fnv1a(HashKeyType key)
{
    HashValueType r = FNV1A_OFFSET;
    while (*key)
    {
        r ^= *key;
        r *= FNV1A_PRIME;
        ++key;
    }
    return r;
}

static void hash_rehash(HashTable *const ht)
{
    ht->cap <<= 1;
    ListNode **const bkt = (ListNode **)(alloc(ht->cap, sizeof(ListNode *), true));
    for (size_t i = 0; i < ht->cap >> 1; ++i)
    {
        for (ListNode *j = ht->bucket[i], *t; j; j = t)
        {
            const size_t idx = fnv1a(j->key) % ht->cap;
            t = j->next;
            j->next = bkt[idx];
            bkt[idx] = j;
        }
    }
    free(ht->bucket);
    ht->bucket = bkt;
}

bool hash_insert(HashTable *const ht, const HashKeyType key, const HashValueType value)
{
    const size_t idx = fnv1a(key) % ht->cap;
    ListNode *const node = (ListNode *)(1, sizeof(ListNode));
    if (!node)
    {
        return true;
    }
    node->key = (HashKeyType)(strdup(key));
    if (!node->key)
    {
        free(node);
        err(ERR_MSG_STRDUP, key);
        return true;
    }
    node->value = value;
    node->next = ht->bucket[idx];
    ht->bucket[idx] = node;
    ++ht->size;
    if (ht->size << 2 > ht->cap + (ht->cap << 1))
    {
        hash_rehash(ht);
    }
    return false;
}

bool hash_query(const HashTable *const ht, const HashKeyType key, HashValueType *const value)
{
    const size_t idx = fnv1a(key) % ht->cap;
    for (const ListNode *i = ht->bucket[idx]; i; i = i->next)
    {
        if (!strcmp(i->key, key))
        {
            *value = i->value;
            return true;
        }
    }
    return false;
}

#endif
