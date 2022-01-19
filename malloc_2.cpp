
extern "C"
{
#include <unistd.h>
#include <stdio.h>
}

MallocMetaData *LINK_START = NULL;
MallocMetaData *LINK_END = NULL;

class MallocMetaData
{
    bool free = false;
    size_t size;
    MallocMetaData *next;
    MallocMetaData &prev;
    MallocMetaData(size_t size) : size(size);
};

void *smalloc(size_t size)
{
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