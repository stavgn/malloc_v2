#ifndef MALLOC_3_H
#define MALLOC_3_H
#define HIST_SIZE 128

class MetaDataListNode
{
    MallocMetaData *data;
    MetaDataListNode *prev;
    MetaDataListNode *next;
}

class MetaDataList
{
public:
    MetaDataListNode first;
    MetaDataListNode last;
    List();
    insert(MallocMetaData *mem);
    remove(MallocMetaData *mem);
}

class MallocMetaData
{
public:
    size_t size;
    bool free;
    MallocMetaData(size_t size = 0);
}

class Histogram
{
public:
    MallocMetaData hist[HIST_SIZE];
    Histogram(int size);
    insert(MallocMetaData *mem);
    remove(MallocMetaData *mem);
}

class MemoryManager
{
public:
    Histogram hist;
    MetaDataList heap;
    MallocMetaData *mmap_list;
    MemoryManager();
}

void
__print_free_list();
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