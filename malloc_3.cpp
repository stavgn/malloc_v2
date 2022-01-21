#include "malloc_3.h"

MetaDataListNode::MetaDataListNode(MallocMetaData *data) : data(data)
{
}

MetaDataList::MetaDataList(char type) : type(type)
{
}

MetaDataList::insert(MallocMetaData *mem)
{
    MetaDataListNode node(mem);
    MetaDataListNode slot;
    for (MetaDataListNode iter = first.next; iter != last; iter = iter.next)
    {
        if (type == 's')
        {
            if (iter.data->size > mem->size)
            {
                slot = iter;
                break;
            }
        }
        else
        {
            //if ()
        }
    }

    node.prev = slot.prev;
    node.prev.next = node;
    slot.prev = node;
    node.next = slot;
}

MetaDataList::remove(MallocMetaData *mem)
{
}

MallocMetaData::MallocMetaData(size_t size) : size(size)
{
}

Histogram::Histogram() {}