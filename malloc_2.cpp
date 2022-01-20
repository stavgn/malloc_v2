#include "malloc_2.h"

MallocMetaData LINK_FREE_START(0);
MallocMetaData LINK_FREE_END(0);

MallocMetaData LINK_USED_START(0);
MallocMetaData LINK_USED_END(0);

bool was_init = false;
_init init_malloc;

_init::_init()
{
    if(was_init)
    {
        return;
    }
    LINK_FREE_START.prev = NULL;
    LINK_FREE_END.next = NULL;
    LINK_FREE_START.next = &LINK_FREE_END;
    LINK_FREE_END.prev = &LINK_FREE_START;

    LINK_USED_START.prev = NULL;
    LINK_USED_END.next = NULL;
    LINK_USED_START.next = &LINK_USED_END;
    LINK_USED_END.prev = &LINK_USED_START;
    was_init = true;
}

size_t __num_of_nodes(MallocMetaData *list_head)
{
    size_t counter = 0;
    MallocMetaData *iter = list_head->next;
    for (;((iter != &LINK_FREE_END) && (iter != &LINK_USED_END)); iter = iter->next)
    {
        counter++;
    }
    return counter;
}
size_t __num_of_byts_in_list(MallocMetaData *list_head)
{
    size_t counter = 0;
    MallocMetaData *iter = list_head->next;
    for (; ((iter != &LINK_FREE_END) && (iter != &LINK_USED_END)); iter = iter->next)
    {
        counter += iter->size;
    }
    return counter;
}

size_t _num_free_blocks()
{
    return __num_of_nodes(&LINK_FREE_START);
}

size_t _num_free_bytes()
{
    return __num_of_byts_in_list(&LINK_FREE_START);
}

size_t _num_allocated_blocks()
{
    return __num_of_nodes(&LINK_FREE_START) + __num_of_nodes(&LINK_USED_START);
}

size_t _num_allocated_bytes()
{
    return __num_of_byts_in_list(&LINK_FREE_START) + __num_of_byts_in_list(&LINK_USED_START);
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

MallocMetaData *_find_slot(size_t size)
{
    for (MallocMetaData *iter = &LINK_FREE_START; iter != &LINK_FREE_END; iter = iter->next)
    {
        if (size <= iter->size && iter->free == true)
        {
            return iter;
        }
    }
    return NULL;
}

void _remove_node_from_link(MallocMetaData *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void _insert_data_to_used_link(MallocMetaData *node)
{
    MallocMetaData *before_last  = LINK_USED_END.prev;
    before_last->next = node;
    LINK_USED_END.prev = node;
    node->prev = before_last;
    node->next = &LINK_USED_END;
}
void _insert_data_to_free_link(MallocMetaData *node)
{
    MallocMetaData *new_spot = _find_slot(node->size);
    if (new_spot == NULL) //list is empty 
    {
        new_spot = &LINK_FREE_END;
    }
    node->prev = new_spot->prev;
    node->next = new_spot;
    new_spot->prev->next = node;
    new_spot->prev = node;

}

void *smalloc(size_t size)
{
    _init();
    void *ptr;
    if (size == 0 || size > BIG_NUM)
    {
        return NULL;
    }

    MallocMetaData *slot = _find_slot(size);
    if (slot == NULL)
    {
        ptr = sbrk(_size_meta_data() + size);
        if (ptr == (void *)-1)
        {
            return NULL;
        }

        MallocMetaData data(size);
        memcpy(ptr, &data, _size_meta_data());
        _insert_data_to_used_link((MallocMetaData *)ptr);
        return (void *)((MallocMetaData *)ptr + 1);
    }

    slot->free = false;
    _remove_node_from_link(slot);
    _insert_data_to_used_link((MallocMetaData *)ptr);
    return slot + 1;
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
    if (meta->free)
    {
        return;
    }

    meta->free = true;
    _remove_node_from_link(meta);
    _insert_data_to_free_link(meta);
}

void *srealloc(void *oldp, size_t size)
{
    if ((oldp == NULL) || (size == 0) || (size > BIG_NUM))
    {
        return NULL;
    }
    MallocMetaData *meta = (MallocMetaData *)oldp - 1;
    if (meta->size >= size)
    {
        // now what?
        return oldp;
    }
    void *ptr = smalloc(size);
    if (ptr == NULL)
    {
        return NULL;
    };
    memcpy(ptr, oldp, size);
    sfree(oldp);
    return ptr;
}


// int  main() {return 0;}
// int main()
// {
//     char *buf = (char *)smalloc(100);
//     scanf("%s", buf);
//     printf("Typed:%s\n", buf);
//     sfree((void *)buf);

//     char *buf2 = (char *)smalloc(10);
//     if (buf == buf2)
//     {
//         printf("Yes!\n");
//     }
//     scanf("%s", buf);
//     buf[strlen(buf)] = '0';
//     printf("Typed:%s\n", buf);
//     printf("Typed:%s\n", buf + 5);
//     sfree((void *)buf);
//     return 0;
// }