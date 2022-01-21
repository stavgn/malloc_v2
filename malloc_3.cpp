#include "malloc_3.h"

MemoryManager::MemoryManager() : hist(HIST_SIZE)
{
    allocated_blocks = 0;
    allocated_byte = 0;
}

void Histogram::insert(MallocMetaData *mem)
{
    size_t index = mem->size / 1024;
    hist[index].insert(mem);
}

void Histogram::remove(MallocMetaData *mem)
{
    size_t index = mem->size / 1024;
    hist[index].remove(mem);
}