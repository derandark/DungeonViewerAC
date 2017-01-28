
#pragma once

#define SMARTARRAY_DEFAULT_SIZE 8

template<class T>
class SmartArray
{
public:
    SmartArray(long initial_size);
    SmartArray();
    ~SmartArray();

    void add(const T *pdata);
    BOOL grow(long new_size);
    BOOL RemoveUnOrdered(T *pdata);

    T *array_data;
    long num_alloc;
    long num_used;
};

template<class T>
SmartArray<T>::SmartArray(long initial_size)
{
    array_data = NULL;
    num_used = 0;
    num_alloc = 0;

    grow(initial_size);
}

template<class T>
SmartArray<T>::SmartArray()
{
    array_data = NULL;
    num_used = 0;
    num_alloc = 0;
}

template<class T>
SmartArray<T>::~SmartArray()
{
    delete [] array_data;
}

template<class T>
BOOL SmartArray<T>::grow(long new_size)
{
    if (new_size < num_used)
        return FALSE;

    if (new_size > num_alloc)
    {
        if (!new_size)
        {
            num_used = 0;
            num_alloc = 0;

            delete [] array_data;
            array_data = NULL;
        }
        else
        {
            T *new_data = new T[ new_size ];

            if (!new_data)
                return FALSE;

            if (array_data)
            {
                for (int i = 0; i < num_used; i++)
                    new_data[i] = array_data[i];

                delete [] array_data;
            }

            array_data    = new_data;
            num_alloc    = new_size;
        }
    }

    return TRUE;
}

template<class T>
void SmartArray<T>::add(const T *pdata)
{
    if (num_used >= num_alloc)
    {
        if (num_alloc > 0)
        {
            // Double the size
            if (!grow(num_alloc * 2))
                return;
        }
        else
        {
            // Default size is 8
            if (!grow(SMARTARRAY_DEFAULT_SIZE))
                return;
        }
    }

    array_data[num_used] = *pdata;
    num_used++;
}

template<class T>
BOOL SmartArray<T>::RemoveUnOrdered(T *pdata)
{
    for (long i = 0; i < num_used; i++)
    {
        if (array_data[i] == *pdata)
        {
            array_data[i] = array_data[ num_used - 1 ];
            num_used--;
            return TRUE;
        }
    }

    return FALSE;
}

template<class T>
class OldSmartArray
{
public:
    OldSmartArray(long grow_step = 2);
    ~OldSmartArray();

    void Add(const T *pdata);
    void Grow(long new_size);

    T *array_data;
    long grow_size;
    long num_alloc;
    long num_used;
};

template<class T>
OldSmartArray<T>::OldSmartArray(long grow_step)
{
    array_data = NULL;
    grow_size = grow_step;
    num_used = 0;
    num_alloc = 0;
}

template<class T>
OldSmartArray<T>::~OldSmartArray()
{
    delete [] array_data;
}

template<class T>
void OldSmartArray<T>::Grow(long new_size)
{
    T *new_data = new T[ new_size ];

    if (!new_data)
        return;

    if (array_data)
    {
        for (int i = 0; i < num_used; i++)
            new_data[i] = array_data[i];

        delete [] array_data;
    }

    array_data    = new_data;
    num_alloc    = new_size;
}

template<class T>
void OldSmartArray<T>::Add(const T *pdata)
{
    if (num_used >= num_alloc)
        Grow(num_alloc + grow_size);

    array_data[num_used] = *pdata;
    num_used++;
}

