
extern "C"
{
#include <unistd.h>
#include <stdio.h>
#define BIG_NUM 100000000
}

void *smalloc(size_t size)
{
    void *ptr;
    if (size == 0 || size > BIG_NUM)
    {
        return NULL;
    }
    ptr = sbrk(size);
    if (ptr == (void *)-1)
    {
        return NULL;
    }
    return ptr;
}

int main()
{
    void *data = smalloc(-1234);
    if (data == NULL)
    {
        printf("DOnt fuck with me");
        return 0;
    }
    printf("Type", (char *)data);
    scanf("%s", (char *)data);
    printf("%s", (char *)data);
    return 0;
}