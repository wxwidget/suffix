#ifndef SUFFIX_HASH_TABLE_H
#define SUFFIX_HASH_TABLE_H
#include "object_pool.hpp"
#define HT_ITEM_POOL 1;
template<typename Key, typename Value>
class HashTable
{
public:
    struct BucketItem
    {
#ifdef HT_ITEM_POOL
        DECLARE_MEMORY_POOL(BucketItem);
#endif
        BucketItem(const Key& key = 0, const Value& v = 0): 
            first(key), second(v), next(NULL), rnext(NULL)
        {

        }
        Key first;
        Value second;
        struct BucketItem* next;
        struct BucketItem* rnext;
    };
    class iterator
    {
    public:
        iterator(BucketItem* p):item(p)
        {
        }
        BucketItem* operator-> ()
        {
            return item;
        }
        iterator operator++ (int)//post ++
        {
            BucketItem* v = item;
            item = item->rnext;
            return v; 
        }
        iterator operator++ ()//prefix++
        {
            item = item->rnext;
            return item;
        }
        friend inline bool operator == (const iterator& i1, const iterator& i2)
        {
            return i1.item == i2.item;
        }
        friend inline bool operator != (const iterator& i1, const iterator& i2)
        {
            return i1.item != i2.item;
        }
        BucketItem* item;
    };
    int size() const
    {
        return count;
    }
    bool empty() const
    {
        return 0 == count;
    }
    typedef unsigned(*HashFunc)(const Key& key);
    const static int DEFFAULT_BUCKET_SIZE = 17;
    HashTable(unsigned b = 17, HashFunc fun = NULL):buckets(NULL),
        count(0), bsize(b), head(NULL), tail(NULL), hashFun(fun)
    {
    }
    ~HashTable()
    {
#ifndef  HT_ITEM_POOL
        BucketItem* p = head;
        while(p != NULL)
        {
            delete p;
            p = p->rnext;
        }
#endif
        if (buckets)
        {
            delete []buckets;
        }
    }
    BucketItem** buckets;
    int count;			/* number if items in this table */
    unsigned bsize;
    BucketItem* head;
    BucketItem* tail;
    HashFunc hashFun;
    void setHash(HashFunc h)
    {
        hashFun = h;
    }
    /* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
    inline unsigned hash(const std::string& key)
    {
        //unsigned v = hashFun(key);
        //return v % bsize;
        const char* str = key.c_str();
        register unsigned int h = 5381;
        while(*str != 0)
            h = ((h << 5) + h) + *str++; /* hash * 33 + c */
        return h % bsize;
    }
    inline unsigned hash(const char key)
    {
        return (unsigned)key % bsize;
    }
    inline unsigned hash(const unsigned int key)
    {
        return key % bsize;
    }
    void add(Key& key, Value& value)
    {
        if (NULL == buckets)
        {
            buckets = new BucketItem*[bsize];
            memset(buckets, 0, sizeof(BucketItem*) * bsize);
        }
        int k = hash(key);
        BucketItem* item = buckets[k];
        BucketItem* newItem = new BucketItem(key, value);
        count++;
        if (head == NULL)
        {
            head = newItem;
            tail = head;
        }
        else
        {
            tail->rnext = newItem;
        }
        if (item == NULL)
        {
            item = newItem;
        }
        else
        {
            newItem->next = item->next; 
            item->next = newItem;
        }
    }
    iterator end()
    {
        return iterator(NULL);
    }
    iterator begin()
    {
        return iterator(head);
    }
    Value& operator [](const Key& key)
    {
        if (NULL == buckets)
        {
            buckets = new BucketItem*[bsize];
            memset(buckets, 0, sizeof(BucketItem*) * bsize);
        }
        int k = hash(key);
        BucketItem*& p = buckets[k];
        BucketItem* item = p;
        while(item != NULL)
        {
            if (item->first == key)
            {
                break;
            }
            item = item->next;
        }
        if (item == NULL)
        {
            BucketItem* newItem = new BucketItem(key, 0);
            count++;
            if (head == NULL)
            {
                head = newItem;
                tail = head;
            }
            else
            {
                tail->rnext = newItem;
                tail = newItem;
            }
            if (p == NULL)
            {
                p = newItem;
            }
            else
            {
                newItem->next = p->next; 
                p->next = newItem;
            }
            return newItem->second;
        }
        return item->second;
    }
    iterator find(const Key& key)
    {
        if (buckets == NULL)
            return iterator(0);
        int k = hash(key);
        BucketItem* p = buckets[k];
        while(p != NULL)
        {
            if (p->first == key)
            {
                return iterator(p);
            }
            p = p->next;
        }
        return iterator(0);
    }
};
#endif
