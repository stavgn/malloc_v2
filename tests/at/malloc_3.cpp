#include "malloc_3.h"

MemoryManager mm;

MemoryManager::MemoryManager() : heap('l')
{
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
    new_block->free = true;
    block->size = size;
    hist.insert(block);
    heap.insert(block);
    hist.insert(new_block);
    heap.insert(new_block);

    if (mm.wilderness == block)
    {
        mm.wilderness = new_block;
    }
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
    first.free = false;
    last.free = false;
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
    return ((size < block->size) && (block->size - size - _size_meta_data() >= MIN_SPLIT));
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
        // mm.heap.remove(block);
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
        mm.heap.insert(block);
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

void sfree(void *p)
{
    if (p == NULL)
    {
        return;
    }
    MallocMetaData *meta = (MallocMetaData *)p;
    meta--; // now points to metadata

    if (_in_heap(meta))
    {
        mm.attempt_merge(meta);
    }
    else // mmeap
    {
        mm.mmap_list.remove(meta);
        if (munmap((void *)meta, meta->size + _size_meta_data()) == 1)
        {
            perror("munmap faild:");
        }
    }
}

MallocMetaData *MemoryManager::attempt_merge(MallocMetaData *meta, bool mark_free)
{
    MallocMetaData *merged = attempt_merge_right(attempt_merge_left(meta, false), false);
    return mark_free ? _make_free(merged) : merged;
}

MallocMetaData *MemoryManager::attempt_merge_left(MallocMetaData *meta, bool mark_free)
{
    MallocMetaData *merged = meta;
    if (merged->prev_l->free && _validate_neighbors(merged->prev_l, meta))
    {
        merged = _merge_left(meta, mark_free);
    }
    return merged;
}

MallocMetaData *MemoryManager::attempt_merge_right(MallocMetaData *meta, bool mark_free)
{
    MallocMetaData *merged = meta;
    if (merged->next_l->free && _validate_neighbors(merged, merged->next_l))
    {
        merged = _merge_right(merged, mark_free);
    }
    return merged;
}

MallocMetaData *_merge_left(MallocMetaData *meta, bool mark_free)
{
    assert(meta->free == false);
    mm.heap.remove(meta);
    mm.heap.remove(meta->prev_l);
    mm.hist.remove(meta->prev_l);
    meta->prev_l->size += meta->size + _size_meta_data();
    meta->prev_l->free = false;
    mm.heap.insert(meta->prev_l);
    if (mm.wilderness == meta)
    {
        mm.wilderness = meta->prev_l;
    }
    return mark_free ? _make_free(meta->prev_l) : meta->prev_l;
}

MallocMetaData *_merge_right(MallocMetaData *meta, bool mark_free)
{
    assert(meta->free == false);
    mm.heap.remove(meta);
    mm.heap.remove(meta->next_l);
    mm.hist.remove(meta->next_l);
    meta->size += meta->next_l->size + _size_meta_data();
    mm.heap.insert(meta);
    if (mm.wilderness == meta->next_l)
    {
        mm.wilderness = meta;
    }
    return mark_free ? _make_free(meta) : meta;
}

bool _validate_neighbors(MallocMetaData *m1, MallocMetaData *m2)
{
    assert(m1->size + (char *)(m1 + 1) == (char *)m2);
    return (m1->size + (char *)(m1 + 1) == (char *)m2);
}

void *srealloc(void *oldp, size_t size)
{
    if ((size == 0) || (size > BIG_NUM))
    {
        return NULL;
    }
    if (oldp == NULL)
    {
        return smalloc(size);
    }
    MallocMetaData *meta = (MallocMetaData *)oldp - 1;
    if (meta->size >= size)
    {
        return oldp;
    }

    if (!_in_heap(meta))
    {
        assert(size > MIN_MMAP);
        void *ptr = smalloc(size); // should mmap
        if (ptr == NULL)
        {
            return NULL;
        }
        size_t min = size < meta->size ? size : meta->size;
        memcpy(ptr, oldp, min);
        sfree(oldp);
        return ptr;
    }

    // attempt merge section start
    bool w_l = is_wilderness_in_reach_left(meta);
    bool w_r = is_wilderness_in_reach_right(meta);
    MallocMetaData *merged = meta;
    if (((meta->prev_l->size + meta->size + _size_meta_data()) >= size && meta->prev_l->free) || w_l)
    {
        merged = mm.attempt_merge_left(merged, true);
    }
    else if ((meta->next_l->size + meta->size + _size_meta_data()) >= size && meta->next_l->free)
    {
        merged = mm.attempt_merge_right(merged, true);
    }
    else if (((meta->next_l->size + meta->prev_l->size + meta->size + 2 * _size_meta_data()) >= size && meta->prev_l->free && meta->next_l->free) || w_r)
    {
        merged = mm.attempt_merge(merged, true);
    }
    // attempt merge section end

    if (merged->size >= size)
    {
        if (_shoul_split(merged, size))
        {
            merged = mm.split(merged, size);
        }
        merged->free = false;
        mm.hist.remove(merged);
        merged++;
        memcpy((void *)merged, oldp, size);
        return (void *)merged;
    }
    else if (mm.wilderness == merged)
    {
        size_t delta = size - mm.wilderness->size;
        if (sbrk(delta) == (void *)(-1))
        {
            return NULL;
        }
        merged->free = false;
        mm.wilderness->size += delta;
        mm.hist.remove(merged);
        merged++;
        memcpy((void *)merged, oldp, size);
        return (void *)merged;
    }

    void *ptr = smalloc(size);
    if (ptr == NULL)
    {
        return NULL;
    }
    memcpy(ptr, oldp, size);
    sfree(oldp);
    return ptr;
}

MallocMetaData *_make_free(MallocMetaData *meta)
{
    assert(meta->free == false);
    meta->free = true;
    mm.hist.insert(meta);
    return meta;
}

bool is_wilderness_in_reach_left(MallocMetaData *meta)
{
    return mm.wilderness == meta && meta->prev_l->free;
}

bool is_wilderness_in_reach_right(MallocMetaData *meta)
{
    return mm.wilderness == meta->next_l && meta->next_l->free;
}

size_t _num_free_blocks()
{
    size_t counter = 0;
    for (MallocMetaData *iter = mm.heap.first.next_l; iter != &mm.heap.last; iter = iter->next_l)
    {
        if (iter->free)
        {
            counter++;
        }
    }
    return counter;
}
size_t _num_free_bytes()
{
    size_t counter = 0;
    for (MallocMetaData *iter = mm.heap.first.next_l; iter != &mm.heap.last; iter = iter->next_l)
    {
        if (iter->free)
        {
            counter += iter->size;
        }
    }
    return counter;
}
size_t _num_allocated_blocks()
{
    return mm.heap.number_of_elements() + mm.mmap_list.number_of_elements();
}
size_t _num_allocated_bytes()
{
    return mm.heap.number_of_bytes() + mm.mmap_list.number_of_bytes();
}
size_t _num_meta_data_bytes()
{
    return _size_meta_data() * _num_allocated_blocks();
}

// int main()
// {
//     int *data = (int *)smalloc(101);
//     *data = 100;
//     int *data2 = (int *)smalloc(102);
//     *data2 = 100;
//     int *data3 = (int *)smalloc(103);
//     sfree(data2);
//     sfree(data3);
//     srealloc(data, 150);
//     //  int *data3 = (int *)scalloc(50, sizeof(int));
//     printf("data:%d,data2:%d,data3[32]:%d\n", *data, *data2, data3[32]);
// }
