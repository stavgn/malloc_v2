
extern "C"
{
#include <unistd.h>
#include <stdio.h>
#define BIG_NUM 100000000
}

class MallocMetaData;

MallocMetaData *LINK_FREE_START = NULL;
MallocMetaData *LINK_FREE_END = NULL;

MallocMetaData *LINK_USED_START = NULL;
MallocMetaData *LINK_USED_END = NULL;

class MallocMetaData
{
public:
    bool free = false;
    size_t size;
    MallocMetaData *next;
    MallocMetaData *prev;
    MallocMetaData(size_t size) : size(size) {}
};

MallocMetaData *_find_slot(size_t size)
{
    for (MallocMetaData iter = LINK_FREE_START; i != LINK_FREE_END; iter = iter->next)
    {
        if (iter->size <= size)
        {
            return iter;
        }
    }
    return nullptr;
}

void _remove_node_from_link(MallocMetaData *node)
{
    MallocMetaData *temp1 = node->prev;
    temp1->next = node->next;
}

void _insert_data_to_used_link(MallocMetaData *node)
{
    if (LINK_USED_START == NULL)
    {
        LINK_USED_START = node;
    }

    LINK_USED_END->next = node;
    node->prev = LINK_USED_END;
    LINK_FREE_END = node;
}

smalloc(size_t size)
{
    if (size == 0 || size < BIG_NUM)
    {
        return NULL;
    }

    MallocMetaData *slot = _find_slot(size);
    if (slot == nullptr)
    {
        ptr = sbrk(_size_meta_data() + size);
        if (ptr == (void *)-1)
        {
            return NULL;
        }
        MallocMetaData data(size);
        sprintf(ptr, "%s", data);
        _insert_data_to_used_link(ptr);
        return ptr + _size_meta_data();
    }

    slot->free = false;
    _remove_node_from_link(slot);
    return slot + _size_meta_data();
}

void *scalloc(size_t num, size_t size)
{
}

void sfree(void *p)
{
}

void *srealloc(void *oldp, size_t size)
{
}

size_t __num_of_nodes(MallocMetaData *list_head)
{
    size_t counter = 0;
    MallocMetaData *iter = list_head;
    for (; iter != NULL; iter = iter->next)
    {
        counter++;
    }
    return counter;
}
size_t __num_of_byts_in_list(MallocMetaData *list_head)
{
    size_t counter = 0;
    MallocMetaData *iter = list_head;
    for (; iter != NULL; iter = iter->next)
    {
        counter += iter->size;
    }
    return counter;
}

size_t _num_free_blocks()
{
    return __num_of_nodes(LINK_FREE_START);
}

size_t _num_free_bytes()
{
    return __num_of_byts_in_list(LINK_FREE_START);
}

size_t _num_allocated_blocks()
{
    return __num_of_nodes(LINK_FREE_START) + __num_of_nodes(LINK_USED_START);
}

size_t _num_allocated_bytes()
{
    return __num_of_byts_in_list(LINK_FREE_START) + __num_of_byts_in_list(LINK_USED_START);
}


size_t _size_meta_data()
{
    static size_t size_of_block = sizeof(MallocMetaData);
    return size_of_block;
}

size_t _num_meta_data_bytes()
{
    return _num_allocated_blocks() * _size_meta_data();
}

int main()
{
    return 0;
}