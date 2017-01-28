
#pragma once

template< typename _Kty > class HashBase;
template< typename _Kty > class HashBaseData;
template< typename _Kty > class HashBaseIter;

template< class _Mty > class LongNIValHash;
template< class _Mty > class LongNIValHashData;
template< class _Mty > class LongNIValHashIter;

template< class _Mty > class LongNIHash;
template< class _Mty > class LongNIHashData;
template< class _Mty > class LongNIHashIter;

template< typename _Kty >
class HashBaseData
{
public :
    HashBaseData() : m_pNext(NULL), m_Key(0)
    {
    }

    virtual ~HashBaseData()
    {
        m_pNext = NULL;
        m_Key = 0;
    }

    HashBaseData<_Kty> *m_pNext;
    _Kty m_Key;
};

template< typename _Kty >
class HashBaseIter
{
public:

    HashBaseIter(HashBase<_Kty> *pBase)
        : m_pBase(pBase)
    {
        
        m_iBucket    = 0;
        m_pLast        = NULL;

        if (m_pBase)
        {
            m_pNodePtr        = m_pBase->GetBucket(0);
            m_bEndReached    = FALSE;

            if (!m_pNodePtr)
                Next();
        }
        else
        {
            m_pNodePtr        = NULL;
            m_bEndReached    = TRUE;
        }
    }

    void Next()
    {
        // Grab the next node from this bucket.
        if (m_pNodePtr)
        {
            m_pLast        = m_pNodePtr;
            m_pNodePtr    = m_pNodePtr->m_pNext;
        }

        while (!m_pNodePtr)
        {
            // Dead node. Use the next bucket.
            m_iBucket++;
            m_pLast = NULL;

            if (m_iBucket >= m_pBase->GetBucketCount())
            {
                // The end has been reached.
                m_bEndReached = TRUE;
                return;
            }

            m_pNodePtr = m_pBase->GetBucket(m_iBucket);
        }
    }

    void DeleteCurrent()
    {
        if (EndReached())
            return;

        HashBaseData<_Kty> *pNextNode;

        if (m_pLast)
        {
            // This current node was not the base of a bucket.
            // To remove it from the chain, just link around it.
            m_pLast->m_pNext = m_pNodePtr->m_pNext;

            // This will be our next node.
            pNextNode = m_pLast->m_pNext;
        }
        else
        {
            // Going to have to change the bucket base.
            m_pBase->SetBucket(m_iBucket, m_pNodePtr->m_pNext);

            // This will be our next node.
            pNextNode = m_pBase->GetBucket(m_iBucket);
        }

        // This check is useless, but Turbine does it. Probably inlined.
        if (m_pNodePtr)
            delete m_pNodePtr;

        while (!pNextNode)
        {
            // Dead node. Use the next bucket.
            m_iBucket++;
            m_pLast = NULL;

            if (m_iBucket >= m_pBase->GetBucketCount())
            {
                // The end has been reached.
                m_bEndReached = TRUE;
                m_pNodePtr = NULL;
                return;
            }

            pNextNode = m_pBase->GetBucket(m_iBucket);
        }

        m_pNodePtr = pNextNode;
    }

    // -- Pea you made this one up. It was probably inlined. --
    HashBaseData<_Kty>* GetCurrent()
    {
        return m_pNodePtr;
    }

    // -- Pea you made this one up. It was probably inlined. --
    BOOL EndReached()
    {
        return m_bEndReached;
    }

private:

    HashBase<_Kty> *m_pBase; // 0x00
    size_t m_iBucket; // 0x04
    HashBaseData<_Kty> *m_pNodePtr; // 0x08
    HashBaseData<_Kty> *m_pLast; // 0x0C
    BOOL m_bEndReached; // 0x10
};

template< typename _Kty >
class HashBase
{
public:
    HashBase(size_t buckets)
    {
        preserve_table = FALSE;
        hash_table = new HashBaseData<_Kty>*[ buckets ];

        InternalInit(buckets);
    }

    virtual ~HashBase()
    {
        if (!preserve_table)
            delete [] hash_table;
    }

    void InternalInit(size_t buckets)
    {
        bucket_count = buckets;

        // 64-bit = 16-bit shift
        // 32-bit = 8-bit shift
        hash_shift = sizeof(_Kty) << 1;

        // Calculate hash mask.
        hash_mask = 0;

        for (unsigned int bit = 1; (hash_mask | bit) < bucket_count; bit = bit << 1)
            hash_mask |= bit;

        // Init the table.
        for (unsigned int i = 0; i < bucket_count; i++)
            hash_table[ i ] = NULL;
    }

    HashBaseData<_Kty>* remove(_Kty Key)
    {
        size_t BucketIndex = (Key ^ (Key >> hash_shift)) & hash_mask;

        HashBaseData<_Kty>* pBucket = hash_table[ BucketIndex ]; 

        if (!pBucket)
            return NULL;

        if (pBucket->m_Key == Key)
        {
            // First entry of bucket was a match.
            hash_table[ BucketIndex ] = pBucket->m_pNext;
            return pBucket;
        }

        while (pBucket->m_pNext)
        {
            if (pBucket->m_pNext->m_Key == Key)
            {
                HashBaseData<_Kty> *pEntry = pBucket->m_pNext;

                // Remove from chain.
                pBucket->m_pNext = pEntry->m_pNext;

                return pEntry;
            }

            pBucket = pBucket->m_pNext;
        }
           
        // No match found.
        return NULL;
    }

    void add(HashBaseData<_Kty> *pData)
    {
        size_t BucketIndex = (pData->m_Key ^ (pData->m_Key >> hash_shift)) & hash_mask;

        pData->m_pNext = hash_table[ BucketIndex ]; 
        hash_table[ BucketIndex ] = pData;
    }

    HashBaseData<_Kty>* lookup(_Kty Key)
    {
        HashBaseData<_Kty>* pBucket = hash_table[ (Key ^ (Key >> hash_shift)) & hash_mask ];

        while (pBucket)
        {
            if (pBucket->m_Key == Key)
                return pBucket;

            pBucket = pBucket->m_pNext;
        }

        return NULL;
    }

    HashBaseData<_Kty>* clobber(HashBaseData<_Kty>* pData)
    {
        size_t BucketIndex = (pData->m_Key ^ (pData->m_Key >> hash_shift)) & hash_mask;

        HashBaseData<_Kty>* pBucket, pBaseBucket, pLastBucket;
        pBaseBucket = pBucket = hash_table[ BucketIndex ];
        pLastBucket = NULL;

        if (!pBucket)
            return NULL;

        while (pData->m_Key != pBucket->m_Key)
        {
            pLastBucket = pBucket;
            pBucket = pBucket->m_pNext;

            if (!pBucket)
            {
                // Add me, I'm a unique data entry.
                pData->m_pNext = pBaseBucket;
                hash_table[ BucketIndex ] = pData;

                // No data entry existed under my key.
                return NULL;
            }
        }

        if (pData != pBucket)
        {
            // Replace the existing data (It's using my key!).

            if (!pLastBucket)
            {
                // The duplicate is the base bucket.
                // To remove it, link the next and change the base.
                pData->m_pNext = pBucket->m_pNext;
                hash_table[ BucketIndex ] = pData;
            }
            else
            {
                // The duplicate is not the base bucket.
                // To remove it, just link around it.
                pLastBucket->m_pNext = pData;
                pData->m_pNext = pBucket->m_pNext;
            }
        }

        // Return the data that was already in the table.
        return pBucket;
    }

    // -- Pea you made this one up. It was probably inlined. --
    HashBaseData<_Kty>** GetTable()
    {
        return hash_table;
    }

    // -- Pea you made this one up. It was probably inlined. --
    size_t GetBucketIndex(_Kty Key)
    {
        return((Key ^ (Key >> hash_shift)) & hash_mask);
    }

    // -- Pea you made this one up. It was probably inlined. --
    HashBaseData<_Kty>* GetBucket(size_t index)
    {
        return hash_table[ index ];
    }

    // -- Pea you made this one up. It was probably inlined. --
    size_t GetBucketCount()
    {
        return bucket_count;
    }

    // -- Pea you made this one up. It was probably inlined. --
    void SetBucket(size_t index, HashBaseData<_Kty>* pData)
    {
        hash_table[ index ] = pData;
    }

private:
    _Kty                    hash_mask;            // 0x04 (assuming 32-bit..)
    UINT                    hash_shift;        // 0x08 
    HashBaseData<_Kty>**    hash_table;        // 0x0C
    size_t                    bucket_count;        // 0x10
    BOOL                    preserve_table;    // 0x14
};

class LongHashData : public HashBaseData<unsigned long>
{
public:
    virtual ~LongHashData() { }
};

template< class _Mty >
class LongHash : public HashBase<unsigned long>
{
public:
    typedef HashBase<unsigned long>    _Mybase; 
    typedef HashBaseData<unsigned long>    _MybaseData; 
    typedef    HashBaseIter<unsigned long>    _MybaseIter;  

    LongHash(size_t buckets)
        : _Mybase(buckets) 
    {
    }

    virtual ~LongHash()
    {
    }

    void add(_Mty* pData)
    {
        _Mybase::add(pData);
    }

    void remove(unsigned long Key)
    {
        _Mybase::remove(Key);
    }

    _Mty* lookup(unsigned long Key)
    {
        // We are converting the data pointers to their parent objects.
        _MybaseData* pData = _Mybase::lookup(Key);

        if (pData) // Safe ?
            return(static_cast< _Mty* >(pData));

        return NULL;
    }

    void destroy_contents()
    {
        _MybaseIter it(this);

        while (!it.EndReached())
            it.DeleteCurrent();
    }
};

template< class _Mty >
class LongNIValHashData : public HashBaseData<unsigned long>
{
public:
    typedef HashBaseData<unsigned long> _Mybase;

    LongNIValHashData(_Mty Data, unsigned long Key)
        : m_Data(Data)
    {
        m_Key = Key;
    }

    virtual ~LongNIValHashData()
    {
    }

    _Mty    m_Data;
};

template< typename _Mty >
class LongNIValHashIter
{
public:
    typedef LongNIValHash<_Mty>        _MyHash; 
    typedef LongNIValHashData<_Mty>    _MyData; 
    typedef    LongNIValHashIter<_Mty>    _MyIter;    

    LongNIValHashIter(_MyHash *pBase)    : m_pBase(pBase)
    {
        
        m_iBucket    = 0;
        m_pLast        = NULL;

        if (m_pBase)
        {
            m_pNodePtr        = static_cast<_MyData *>(m_pBase->GetBucket(0));
            m_bEndReached    = FALSE;

            if (!m_pNodePtr)
                Next();
        }
        else
        {
            m_pNodePtr        = NULL;
            m_bEndReached    = TRUE;
        }
    }

    void Next()
    {
        // Grab the next node from this bucket.
        if (m_pNodePtr)
        {
            m_pLast        = m_pNodePtr;
            m_pNodePtr    = static_cast<_MyData *>(m_pNodePtr->m_pNext);
        }

        while (!m_pNodePtr)
        {
            // Dead node. Use the next bucket.
            m_iBucket++;
            m_pLast = NULL;

            if (m_iBucket >= m_pBase->GetBucketCount())
            {
                // The end has been reached.
                m_bEndReached = TRUE;
                return;
            }

            m_pNodePtr = static_cast<_MyData *>(m_pBase->GetBucket(m_iBucket));
        }
    }

    void DeleteCurrent()
    {
        if (EndReached())
            return;

        _MyData *pNextNode;

        if (m_pLast)
        {
            // This current node was not the base of a bucket.
            // To remove it from the chain, just link around it.
            m_pLast->m_pNext = m_pNodePtr->m_pNext;

            // This will be our next node.
            pNextNode = static_cast<_MyData *>(m_pLast->m_pNext);
        }
        else
        {
            // Going to have to change the bucket base.
            m_pBase->SetBucket(m_iBucket, m_pNodePtr->m_pNext);

            // This will be our next node.
            pNextNode = static_cast<_MyData *>(m_pBase->GetBucket(m_iBucket));
        }

        // This check is useless, but Turbine does it. Probably inlined.
        if (m_pNodePtr)
            delete m_pNodePtr;

        while (!pNextNode)
        {
            // Dead node. Use the next bucket.
            m_iBucket++;
            m_pLast = NULL;

            if (m_iBucket >= m_pBase->GetBucketCount())
            {
                // The end has been reached.
                m_bEndReached = TRUE;
                m_pNodePtr = NULL;
                return;
            }

            pNextNode = static_cast<_MyData *>(m_pBase->GetBucket(m_iBucket));
        }

        m_pNodePtr = pNextNode;
    }

    // -- Pea you made this one up. It was probably inlined. --
    _MyData *GetCurrent()
    {
        return m_pNodePtr;
    }

    // -- Pea you made this one up. It was probably inlined. --
    BOOL EndReached()
    {
        return m_bEndReached;
    }

private:

    _MyHash *m_pBase; // 0x00
    size_t m_iBucket; // 0x04
    _MyData *m_pNodePtr; // 0x08
    _MyData *m_pLast; // 0x0C
    BOOL m_bEndReached; // 0x10
};

template< class _Mty >
class LongNIValHash : public HashBase<unsigned long>
{
public:
    typedef HashBase<unsigned long>    _Mybase; 
    typedef HashBaseData<unsigned long>    _MybaseData; 
    typedef    HashBaseIter<unsigned long>    _MybaseIter;  

    typedef LongNIValHashData<_Mty>    _MyData; 
    typedef    LongNIValHashIter<_Mty>    _MyIter;  

    LongNIValHash(size_t buckets = 16)
        : _Mybase(buckets) 
    {
    }

    virtual ~LongNIValHash()
    {
        destroy_contents();
    }

    void add(_Mty Data, unsigned long Key)
    {
        _MyData *pData = new _MyData(Data, Key);
        _Mybase::add(pData);
    }

    _MybaseData* remove(unsigned long Key)
    {
        return _Mybase::remove(Key);
    }

    BOOL remove(unsigned long Key, _Mty* pOutData)
    {
        // We are converting the data pointers to their parent objects.
        _MybaseData* pData = _Mybase::remove(Key);

        if (!pData)
            return FALSE;

        _MyData* pInternal = static_cast< _MyData* >(pData);
        *pOutData = pInternal->m_Data; 

        delete pInternal;

        return TRUE;
    }

    BOOL lookup(unsigned long Key, _Mty* pOutData)
    {
        // We are converting the data pointers to their parent objects.
        _MybaseData* pData = _Mybase::lookup(Key);

        if (!pData)
            return FALSE;

        _MyData* pInternal = static_cast< _MyData* >(pData);
        *pOutData = pInternal->m_Data; 

        return TRUE;
    }

    void destroy_contents()
    {
        _MyIter it(this);

        while (!it.EndReached())
            it.DeleteCurrent();
    }
};

// TODO: LongNIHash

template< class _Mty >
class LongNIHashData
{
public:
    typedef DWORD _Kty;

    LongNIHashData()
    {
        m_pNext = NULL;
        m_pData = NULL;
        m_Key = 0;
    }

    LongNIHashData<_Mty> *m_pNext;
    _Mty* m_pData;
    _Kty m_Key;
};

template< typename _Mty >
class LongNIHashIter
{
public:
    typedef LongNIHash<_Mty>        _MyHash; 
    typedef LongNIHashData<_Mty>    _MyData; 
    typedef    LongNIHashIter<_Mty>    _MyIter;    

    LongNIHashIter(_MyHash *pBase)
    {
        m_pBase            = pBase;
        m_iBucket        = -1;
        m_pNodePtr        = NULL;
        m_bEndReached    = FALSE;

        if (m_pBase)
        {
            do
            {
                if ((++m_iBucket) < m_pBase->GetBucketCount())
                {
                    m_pNodePtr = m_pBase->GetBucket(m_iBucket); 
                }
                else
                {
                    m_bEndReached = TRUE;
                    break;
                }
            }
            while (!m_pNodePtr);
        }
        else
            m_bEndReached    = TRUE;
    }

    void Next()
    {
        if (m_bEndReached)
            return;

        m_pNodePtr = m_pNodePtr->m_pNext;

        while (!m_pNodePtr)
        {
            if ((++m_iBucket) < m_pBase->GetBucketCount())
            {
                m_pNodePtr = m_pBase->GetBucket(m_iBucket); 
            }
            else
            {
                m_bEndReached = TRUE;
                break;
            }
        }
    }

    // -- Pea you made this one up. It was probably inlined. --
    _MyData *GetCurrent()
    {
        return m_pNodePtr;
    }

    // -- Pea you made this one up. It was probably inlined. --
    _Mty *GetCurrentData()
    {
        return (m_pNodePtr ? m_pNodePtr->m_pData : NULL);
    }

    // -- Pea you made this one up. It was probably inlined. --
    BOOL EndReached()
    {
        return m_bEndReached;
    }

private:

    _MyHash *m_pBase; // 0x00
    size_t m_iBucket; // 0x04
    _MyData *m_pNodePtr; // 0x08
    BOOL m_bEndReached; // 0x0C
};

template< typename _Mty >
class LongNIHash
{
public:
    typedef DWORD                    _Kty;
    typedef LongNIHashData<_Mty>    _MyData; 
    typedef LongNIHashIter<_Mty>    _MyIter; 

    LongNIHash(size_t buckets)
    {
        hash_table = NULL;
        bucket_count = buckets;

        InternalInit(buckets);
    }

    virtual ~LongNIHash()
    {
        DeleteAll();

        delete [] hash_table;
        hash_table = NULL;
    }

    void InternalInit(size_t buckets)
    {
        hash_table = new _MyData *[ bucket_count ];

        ZeroMemory(hash_table, sizeof(_MyData *) * bucket_count);
    }

    void DeleteAll()
    {
        _MyIter it(this);

        while (!it.EndReached())
        {
            _Mty *pNodeData = it.GetCurrentData();

            it.Next();

            if (pNodeData)
                delete pNodeData;
        }

        flush();
    }

    void flush()
    {
        for (DWORD i = 0; i < bucket_count; i++)
        {
            _MyData *pData = hash_table[i];

            while (pData)
            {
                _MyData *pNextData = pData->m_pNext; // Next entry in chain.
                delete pData;

                pData = pNextData;
            }

            hash_table[i] = NULL;
        }
    }
    
    _Mty* remove(_Kty Key)
    {
        size_t BucketIndex = (Key ^ (Key >> 16)) % bucket_count;
        _MyData** BucketOffset = &hash_table[ BucketIndex ]; 

        _MyData* RootEntry = BucketOffset[0];

        if (!RootEntry)
            return NULL;

        if (RootEntry->m_Key == Key)
        {
            // First entry of bucket was a match.
            hash_table[ BucketIndex ] = RootEntry->m_pNext;
            _Mty *pData = RootEntry->m_pData;
            delete RootEntry;
            
            return pData;
        }

        _MyData* CurrentEntry = RootEntry;

        // Iterate through the chain in this bucket.
        while (CurrentEntry->m_pNext)
        {
            _MyData* NextEntry = CurrentEntry->m_pNext;

            if (NextEntry->m_Key == Key)
            {
                // Next entry is the one we want, so remove from chain.

                // This is a dumb check, but for whatever reason the client compiled this way.
                if (CurrentEntry)
                {
                    CurrentEntry->m_pNext = NextEntry->m_pNext;
                    _Mty *pData = NextEntry->m_pData;
                    delete NextEntry;

                    return pData;
                }
                else
                {
                    hash_table[ BucketIndex ] = NextEntry->m_pNext;
                    _Mty *pData = NextEntry->m_pData;
                    delete NextEntry;
                    
                    return pData;
                }
            }

            CurrentEntry = NextEntry;
        }
           
        // No match found.
        return NULL;
    }

    BOOL add(_Mty *pData, _Kty Key)
    {
        size_t BucketIndex = (Key ^ (Key >> 16)) % bucket_count;
        _MyData **BucketOffset = &hash_table[ BucketIndex ]; 

        _MyData *pHashData = new _MyData;

        if (!pHashData)
            return FALSE;

        pHashData->m_Key = Key;
        pHashData->m_pData = pData;
        pHashData->m_pNext = *BucketOffset;
        *BucketOffset = pHashData;

        return TRUE;
    }

    _Mty *lookup(_Kty Key)
    {
        _Mty *pEntry = hash_table[ (Key ^ (Key >> 16)) % bucket_count ];

        while (pEntry)
        {
            if (pEntry->m_Key == Key)
                return pEntry;

            pEntry = pEntry->m_pNext;
        }

        return NULL;
    }

    // -- Pea you made this one up. It was probably inlined. --
    _MyData* GetTable()
    {
        return hash_table;
    }

    // -- Pea you made this one up. It was probably inlined. --
    size_t GetBucketIndex(_Kty Key)
    {
        return((Key ^ (Key >> 16)) % bucket_count);
    }

    // -- Pea you made this one up. It was probably inlined. --
    _MyData* GetBucket(size_t index)
    {
        return hash_table[ index ];
    }

    // -- Pea you made this one up. It was probably inlined. --
    size_t GetBucketCount()
    {
        return bucket_count;
    }

    // -- Pea you made this one up. It was probably inlined. --
    void SetBucket(size_t index, _MyData* pData)
    {
        hash_table[ index ] = pData;
    }

private:
    _MyData**    hash_table;        // 0x00
    size_t        bucket_count;        // 0x04
};

template< typename _Kty, class _Dty >
class PackableHashTable
{
public:
    PackableHashTable()
    {
        // ...
    } 

    virtual ~PackableHashTable()
    {
        Destroy();
    }

    void Destroy()
    {
        // ...
    }

    void EmptyContents()
    {
        // ...
    }

    virtual BOOL UnPack(BYTE **ppData, ULONG iSize)
    {
        // ...
        return TRUE;
    }
};









