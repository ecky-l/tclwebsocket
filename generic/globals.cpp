
#include <tcl.h>

void* operator new(size_t size)
{
    void* p = ckalloc(size);
    return p;
}

void operator delete(void* p)
{
    ckfree(p);
}