
extern "C"
{
#include <unistd.h>
#include <stdio.h>
}
class MallocMetaData;
MallocMetaData *LINK_START = NULL;
MallocMetaData *LINK_END = NULL;

class MallocMetaData
{
    public:
    bool free = false;
    size_t size;
    MallocMetaData *next;
    MallocMetaData *prev;
    MallocMetaData(size_t size) : size(size){}
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
    size_t counter = 0;
    MallocMetaData *iter = LINK_START;
    if (iter == NULL)
    {
        return 0;
    }
    for(;iter != NULL; iter = iter->next)
    {
        counter++;
    }
    return counter;
}

size_t _num_free_bytes()
{
     size_t counter = 0;
    MallocMetaData *iter = LINK_START;
    if (iter == NULL)
    {
        return 0;
    }
    for(;iter != NULL; iter = iter->next)
    {
        counter += iter->size;
    }
    return counter;
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
    static size_t size_of_block = sizeof(MallocMetaData);
    return size_of_block;
}

int main()
{
    return 0;
}