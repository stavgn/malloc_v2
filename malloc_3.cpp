#include "malloc_3.h"

MemoryManager mm;

MemoryManager::MemoryManager() : heap('l')
{
    allocated_blocks = 0;
    allocated_byte = 0;
    wilderness = NULL;
}

MallocMetaData *MemoryManager::split(MallocMetaData *block, size_t size)
{
    assert((block->size - size - _size_meta_data()) >= MIN_SPLIT);
    assert(size < block->size);
    assert(_in_heap(block));
    hist.remove(block);
    heap.remove(block);

    MallocMetaData *new_block = (MallocMetaData *)((char *)(block + 1) + size);
    *new_block = MallocMetaData(block->size - size - _size_meta_data());
    block->size = size;
    hist.insert(block);
    heap.insert(block);
    hist.insert(new_block);
    heap.insert(new_block);

    allocated_blocks++;
    allocated_byte -= _size_meta_data();
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

bool _in_heap(MallocMetaData *block)
{
    assert(block != NULL);
    return block->size < MIN_MMAP;
}

bool _shoul_split(MallocMetaData *block, size_t size)
{
    assert(block != NULL);
    return (block->size - size - _size_meta_data() >= MIN_SPLIT);
}

size_t _size_meta_data()
{
    static size_t size_of_block = sizeof(MallocMetaData);
    return size_of_block;
}

void *smalloc(size_t size)
{
    if ((size == 0) || (size > BIG_NUM))
    {
        return NULL;
    }

    MallocMetaData *block;

    if (size >= MIN_MMAP)
    {
        block = (MallocMetaData *)mmap(NULL, size + _size_meta_data(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (block == (MallocMetaData *)(-1))
        {
            return NULL;
        }
        *block = MallocMetaData(size);
        mm.mmap_list.insert(block);
        mm.allocated_blocks++;
        mm.allocated_byte += size;
        block->free = false;
        return (void *)(block + 1);
    }

    block = mm.hist.search_block(size);
    if (block != NULL)
    {
        if (_shoul_split(block, size))
        {
            block = mm.split(block, size);
        }
        mm.hist.remove(block);
        mm.heap.remove(block);
        block->free = false;
        return (void *)(block + 1);
    }

    else if ((mm.wilderness != NULL) && (mm.wilderness->free))
    {
        assert(size > mm.wilderness->size);
        size_t delta = size - mm.wilderness->size;
        if (sbrk(delta) == (void *)(-1))
        {
            return NULL;
        }
        mm.hist.remove(mm.wilderness);
        mm.wilderness->size = size;
        mm.wilderness->free = false;
        mm.allocated_byte += delta;
        return (void *)(mm.wilderness + 1);
    }
    else
    {
        block = (MallocMetaData *)sbrk(_size_meta_data() + size);
        if (block == (MallocMetaData *)(-1))
        {
            return NULL;
        }
        *block = MallocMetaData(size);
        block->free = false;
        mm.wilderness = block;
        mm.allocated_blocks++;
        mm.allocated_byte += size;
        return (void *)(block + 1);
    }
}

void *scalloc(size_t num, size_t size)
{
    if (size == 0 || size * num > BIG_NUM)
    {
        return NULL;
    }
    void *ptr = smalloc(size * num);
    if (ptr == NULL)
    {
        return NULL;
    }
    return memset(ptr, 0, size * num);
}

int main()
{
    int* data = (int*)smalloc(100);
    *data = 100;
    int* data2 = (int*)smalloc(10000000);
    *data2 = 100;
    int *data3 = (int*)scalloc(50,sizeof(int));
    printf("data:%d,data2:%d,data3[32]:%d\n",*data,*data2,data3[32]);

}