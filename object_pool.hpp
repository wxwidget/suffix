#ifndef SUFFIX_MEMORY_POOL_H
#define SUFFIX_MEMORY_POOL_H
template <typename T>
class MemoryPool
{
public:
    MemoryPool(int blockSize = 512);
    ~MemoryPool();
public:
    static MemoryPool<T>& instance()
    {
        static MemoryPool<T> inst;
        return inst;
    }
    inline void * alloc();
    inline void free(void *mem);
private:
    //make sure allocated memory can hold a pointer
    union AllocateElement
    {
        char mPlacehodler[sizeof(T)];
        AllocateElement *mNext;
    };
    void*  alloc_block();
    int mBlockSize;
    AllocateElement *mHeadOfFreeList;
    std::vector<void *> mBlockMem;
};

template <typename T>
MemoryPool<T>::MemoryPool(int blockSize)
{
    mBlockSize = blockSize;
    mHeadOfFreeList = NULL;
    mBlockMem.resize(100);
}

template <typename T>
MemoryPool<T>::~MemoryPool()
{
    std::vector<void *>::iterator it = mBlockMem.begin();
    for(; it != mBlockMem.end(); it++)
    {
        //::operator delete(*it);
        free(*it);
    }
}

template <typename T>
void* MemoryPool<T>::alloc_block()
{
    AllocateElement *p = mHeadOfFreeList;
    //void *blockMem = ::operator new(mBlockSize * sizeof(AllocateElement));
    void *blockMem = malloc(mBlockSize * sizeof(AllocateElement));
    AllocateElement *newMem = static_cast<AllocateElement *>(blockMem);
    //record begin address of block
    mBlockMem.push_back(blockMem);
    //create free list
    for(int i = 1; i < (mBlockSize - 1); i++)
    {
        newMem[i].mNext = newMem + i + 1;
    }
    newMem[mBlockSize - 1].mNext = NULL;
    p = newMem;
    mHeadOfFreeList = newMem + 1;
    return p;
}
template <typename T>
inline void * MemoryPool<T>::alloc()
{
    AllocateElement *p = mHeadOfFreeList;
    if(p != NULL)
    {
        mHeadOfFreeList = p->mNext;
    }
    else
    {
        p = (AllocateElement*)alloc_block();
    }
    return static_cast<void *>(p);
}
template <typename T>
inline void MemoryPool<T>::free(void *mem)
{
    if(mem != NULL)
    {
        AllocateElement *objMem = static_cast<AllocateElement *>(mem);
        objMem->mNext = mHeadOfFreeList;
        mHeadOfFreeList = objMem;
    }
}
#define DECLARE_MEMORY_POOL(Cls) \
  public: \
  static void* operator new(size_t s) \
  { \
      assert(s == sizeof(Cls)); \
      return MemoryPool<Cls>::instance().alloc(); \
  } \
  static void operator delete(void* p, size_t t) \
  { \
     if (p != NULL) \
     MemoryPool<Cls>::instance().free(p); \
  }
#endif
