#ifndef MALLOC_3_H
#define MALLOC_3_H

#include <unistd.h>
#include <cstdio>
#include <cstdio>
#include <cstring>
#define HIST_SIZE 128

#include <unistd.h>

class MallocMetaData
{
public:
    size_t size;
    bool free;
    MallocMetaData *next_s;
    MallocMetaData *prev_s;
    MallocMetaData *next_l;
    MallocMetaData *prev_l;
    MallocMetaData(size_t size = 0);
};

class MetaDataList
{
public:
    char type;
    MallocMetaData first;
    MallocMetaData last;
    MetaDataList(char type='s');
    void insert(MallocMetaData *mem);
    void remove(MallocMetaData *mem);
    size_t number_of_elements();
    size_t total_size();
};



class Histogram
{
public:
    MetaDataList hist[HIST_SIZE];
    Histogram();
    void insert(MallocMetaData *mem);
    void remove(MallocMetaData *mem);
};

class MemoryManager
{
public:
    Histogram hist;
    MetaDataList heap;
    MetaDataList mmap_list;
    size_t allocated_byte;
    size_t allocated_blocks;
    MemoryManager();
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