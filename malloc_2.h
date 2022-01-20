#ifndef MALLOC_2_H
#define MALLOC_2_H

#include <unistd.h>
#include <cstdio>
#include <cstdio>
#include <cstring>
#define BIG_NUM 100000000

class MallocMetaData
{
public:
    bool free = false;
    size_t size;
    MallocMetaData *next = NULL;
    MallocMetaData *prev = NULL;
    MallocMetaData(size_t size) : size(size) {}
};

class _init
{
    public:
    _init();
};

size_t __num_of_nodes(MallocMetaData *list_head);
size_t __num_of_byts_in_list(MallocMetaData *list_head);
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _size_meta_data();
size_t _num_meta_data_bytes();
MallocMetaData *_find_slot(size_t size);
void _remove_node_from_link(MallocMetaData *node);
void _insert_data_to_used_link(MallocMetaData *node);
void _insert_data_to_free_link(MallocMetaData *node);
void *smalloc(size_t size);
void *scalloc(size_t num, size_t size);
void sfree(void *p);
void *srealloc(void *oldp, size_t size);

#endif