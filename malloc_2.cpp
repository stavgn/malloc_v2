
extern "C"
{
}
#include <unistd.h>
#include <cstdio>
#include <cstdio>
#include <cstring>
#define BIG_NUM 100000000
using namespace std;
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
    MallocMetaData *next = NULL;
    MallocMetaData *prev = NULL;
    void *data_ptr = nullptr;
    MallocMetaData(size_t size) : size(size) {}
};

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

MallocMetaData *_find_slot(size_t size)
{
    for (MallocMetaData *iter = LINK_FREE_START; iter != NULL; iter = iter->next)
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
    if (node == LINK_FREE_START)
    {
        LINK_FREE_START = node->next;
        if (LINK_FREE_START != NULL)
        {
            LINK_FREE_START->prev = NULL;
        }
    }
    if (node == LINK_USED_START)
    {
        LINK_USED_START = node->next;
        if (LINK_USED_START != NULL)
        {
            LINK_USED_START->prev = NULL;
        }
    }
    if (node == LINK_FREE_END)
    {
        LINK_FREE_END = node->prev;
        if (LINK_FREE_END != NULL)
        {
            LINK_FREE_END->next = NULL;
        }
    }
    if (node == LINK_USED_END)
    {
        LINK_USED_END = node->prev;
        if (LINK_USED_END != NULL)
        {
            LINK_USED_END->next = NULL;
        }
    }
    MallocMetaData *temp1 = node->prev;
    if (temp1 != NULL)
    {
        temp1->next = node->next;
    }
    MallocMetaData *temp2 = node->next;
    if (temp2 != NULL)
    {
        temp2->prev = node->prev;
    }
}

void _insert_data_to_used_link(MallocMetaData *node)
{
    if (LINK_USED_START == NULL)
    {
        LINK_USED_START = node;
    }
    if (LINK_USED_END == NULL)
    {
        LINK_USED_END = node;
    }
    else
    {
        LINK_USED_END->next = node;
        node->prev = LINK_USED_END;
        LINK_USED_END = node;
    }
}

void *smalloc(size_t size)
{
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
    memset(ptr, 0, size * num);
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
    MallocMetaData *new_spot = _find_slot(meta->size);
    if (new_spot == NULL)
    {
        meta->prev = LINK_FREE_END;
        if (LINK_FREE_END != NULL)
        {
            LINK_FREE_END->next = meta;
        }
        meta->next = NULL;
        LINK_FREE_END = meta;
    }

    if (new_spot == LINK_FREE_START)
    {
        meta->next = LINK_FREE_START;
        if (LINK_FREE_START != NULL)
        {
            LINK_FREE_START->prev = meta;
        }
        meta->prev = NULL;
        LINK_FREE_START = meta;
    }
    else
    {
        meta->prev = new_spot->prev;
        new_spot->prev->next = meta;
        meta->next = new_spot;
    }
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

int main()
{
    char *buf = (char *)smalloc(100);
    scanf("%s", buf);
    printf("Typed:%s\n", buf);
    sfree((void *)buf);

    char *buf2 = (char *)smalloc(10);
    if (buf == buf2)
    {
        printf("Yes!\n");
    }
    scanf("%s", buf);
    buf[strlen(buf)] = '0';
    printf("Typed:%s\n", buf);
    printf("Typed:%s\n", buf + 5);
    sfree((void *)buf);
    return 0;
}