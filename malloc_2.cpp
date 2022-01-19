
extern "C"
{
#include <unistd.h>
#include <stdio.h>
}

MallocMetaData *LINK_FREE_START = NULL;
MallocMetaData *LINK_FREE_END = NULL;

MallocMetaData *LINK_USED_START = NULL;
MallocMetaData *LINK_USED_END = NULL;

class MallocMetaData
{
    bool free = false;
    size_t size;
    MallocMetaData *next;
    MallocMetaData *prev;
    void *data_ptr = nullptr;
    MallocMetaData(size_t size) : size(size);
};

MallocMetaData *find_slot(size_t size)
{
    for (MallocMetaData iter = LINK_START; i != LINK_END; iter = iter->next)
    {
        if (iter->size <= size)
        {
            return iter;
        }
    }
    return nullptr;
}

void *smalloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    MallocMetaData slot * = find_slot(size);
    if (slot == nullptr)
    {
        ptr = sbrk(_size_meta_data() + size);
        if (ptr == (void *)-1)
        {
            return NULL;
        }
        MallocMetaData data(size);
        sprintf(ptr, "%s", ) return ptr;
    }
    insert_data_to_link()
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

size_t _num_free_blocks()
{
}

size_t _num_free_bytes()
{
}

size_t _num_allocated_blocks()
{
}

size_t _num_allocated_bytes()
{
}

size_t _num_meta_data_bytes()
{
}

size_t _size_meta_data()
{
    return sizeof(MallocMetaData);
}

int main()
{
    return 0;
}