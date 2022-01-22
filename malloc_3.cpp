#include "malloc_3.h"

MemoryManager::MemoryManager() : heap('l')
{
    allocated_blocks = 0;
    allocated_byte = 0;
    wilderness = NULL;
}

MallocMetaData* MemoryManager::split(MallocMetaData *block,size_t size)
{
    assert((block->size-size-_size_meta_data()) >= MIN_SPLIT);
    assert(size < block->size);
    assert(in_heap(block));
    hist.remove(block);
    heap.remove(block);

    MallocMetaData *new_block = (MallocMetaData*)((char*)(block + 1) + size); 
    *new_block = MallocMetaData(block->size-size-_size_meta_data());
    block->size = size;
    hist.insert(new_block);
    heap.insert(new_block);
    return block;
}

void Histogram::insert(MallocMetaData *mem)
{
    assert(mem->size < MIN_MMAP);

    size_t index = mem->size / 1024;
    hist[index].insert(mem);
}

void Histogram::remove(MallocMetaData *mem)
{
    assert(mem->size < MIN_MMAP);

    size_t index = mem->size / 1024;
    hist[index].remove(mem);
}

MallocMetaData *Histogram::search_block(size_t size)
{
    assert(size < MIN_MMAP);
    size_t index = size / 1024;
    for (; index < HIST_SIZE; index++)
    {
        for (MallocMetaData *iter = hist[index].first.next_s; iter != &hist[index].last; iter = iter->next_s)
        {
            if (iter->size >= size)
            {
                return iter;
            }
        }
    }
    return NULL;
}

MetaDataList::MetaDataList(char type) : type(type)
{
    if (type == 's')
    {
        first.next_s = &last;
        first.prev_s = NULL;
        last.next_s = NULL;
        last.prev_s = &first;
        return;
    }
    else if (type == 'l')
    {
        first.next_l = &last;
        first.prev_l = NULL;
        last.next_l = NULL;
        last.prev_l = &first;
        return;
    }
    assert(0);
}

void MetaDataList::insert(MallocMetaData *mem)
{
    MallocMetaData *slot = &last;
    if (type == 's')
    {
        for (MallocMetaData *iter = first.next_s; (iter != &last); iter = iter->next_s)
        {
            if (iter->size > mem->size)
            {
                slot = iter;
                break;
            }
        }
        mem->prev_s = slot->prev_s;
        mem->prev_s->next_s = mem;
        slot->prev_s = mem;
        mem->next_s = slot;
        return;
    }
    else if (type == 'l')
    {
        for (MallocMetaData *iter = first.next_l; iter != &last; iter = iter->next_l)
        {
            if (iter > mem)
            {
                slot = iter;
                break;
            }
        }
        mem->prev_l = slot->prev_l;
        mem->prev_l->next_l = mem;
        slot->prev_l = mem;
        mem->next_l = slot;
        return;
    }
    assert(0);
}

void MetaDataList::remove(MallocMetaData *mem)
{
    if (type == 's')
    {
        mem->next_s->prev_s = mem->prev_s;
        mem->prev_s->next_s = mem->next_s;
        return;
    }
    else if (type == 'l')
    {
        mem->next_l->prev_l = mem->prev_l;
        mem->prev_l->next_l = mem->next_l;
        return;
    }
    assert(0);
}

MallocMetaData::MallocMetaData(size_t size) : size(size)
{
}

size_t MetaDataList::number_of_elements()
{
    size_t counter = 0;
    if (type == 's')
    {
        for (MallocMetaData *iter = first.next_s; iter != &last; iter = iter->next_s)
        {
            counter++;
        }
    }
    else if (type == 'l')
    {
        for (MallocMetaData *iter = first.next_l; iter != &last; iter = iter->next_l)
        {
            counter++;
        }
    }
    return counter;
}
size_t MetaDataList::number_of_bytes()
{
    size_t counter = 0;
    if (type == 's')
    {
        for (MallocMetaData *iter = first.next_s; iter != &last; iter = iter->next_s)
        {
            counter += iter->size;
        }
    }
    else if (type == 'l')
    {
        for (MallocMetaData *iter = first.next_l; iter != &last; iter = iter->next_l)
        {
            counter += iter->size;
        }
    }
    return counter;
}

bool in_heap(MallocMetaData *block)
{
    assert(block != NULL);
    return block->size < MIN_MMAP;
}

size_t _size_meta_data()
{
    static size_t size_of_block = sizeof(MallocMetaData);
    return size_of_block;
}


int main()
{
    MemoryManager mem;
    assert(0);
}