#include "malloc_3.h"

MemoryManager::MemoryManager() : heap('l')
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

MetaDataList::MetaDataList(char type) : type(type)
{
    if (type == 's')
    {
        first.next_s = &last;
        first.prev_s = NULL;
        last.next_s = NULL;
        last.prev_s = &first;
    }
    else if (type == 'l')
    {
        first.next_l = &last;
        first.prev_l = NULL;
        last.next_l = NULL;
        last.prev_l = &first;
    }

}

void MetaDataList::insert(MallocMetaData *mem)
{
    MallocMetaData *slot;
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
    }
}

void MetaDataList::remove(MallocMetaData *mem)
{
    if (type == 's')
    {
        mem->next_s->prev_s = mem->prev_s;
        mem->prev_s->next_s = mem->next_s;
    }
    else if (type == 'l')
    {
        mem->next_l->prev_l = mem->prev_l;
        mem->prev_l->next_l = mem->next_l;
    }
}

MallocMetaData::MallocMetaData(size_t size) : size(size)
{
}

Histogram::Histogram() {}
