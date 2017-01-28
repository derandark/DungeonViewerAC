
#pragma once

class DLListNode
{
public:
    DLListNode()
    {
        m_pNext = NULL;
        m_pPrev = NULL;
    }

    virtual ~DLListNode() {
    }

    DLListNode *m_pNext;
    DLListNode *m_pPrev;
};

class DLListBase
{
public:

    DLListBase()
    {
        m_pListBegin = NULL;
        m_pListEnd = NULL;
    }

    virtual ~DLListBase() {
    }

    void InsertAfter(DLListNode *pNode, DLListNode *pPlacement)
    {
        if (pPlacement)
        {
            pNode->m_pNext = pPlacement->m_pNext;

            if (pPlacement->m_pNext)
                pPlacement->m_pNext->m_pPrev = pNode;
            else
                m_pListEnd = pNode;

            pPlacement->m_pNext = pNode;
            pNode->m_pPrev = pPlacement;
        }
        else
        {
            // Place at beginning.
            pNode->m_pNext = m_pListBegin;

            if (m_pListBegin)
                m_pListBegin->m_pPrev = pNode;
            else
                m_pListEnd = pNode;

            m_pListBegin = pNode;
            pNode->m_pPrev = NULL;
        }
    }

    void Remove(DLListNode *pNode)
    {

        if (pNode->m_pPrev)
            pNode->m_pPrev->m_pNext = pNode->m_pNext;
        else
        {
            m_pListBegin = m_pListBegin->m_pNext;

            if (m_pListBegin)
                m_pListBegin->m_pPrev = NULL;
        }

        if (pNode->m_pNext)
            pNode->m_pNext->m_pPrev = pNode->m_pPrev;
        else
        {
            m_pListEnd = m_pListEnd->m_pPrev;

            if (m_pListEnd)
                m_pListEnd->m_pNext = NULL;
        }

        pNode->m_pNext = NULL;
        pNode->m_pPrev = NULL;
    }

    // Made up
    BOOL Empty()
    {
        return(!m_pListBegin ? TRUE : FALSE);
    }

    // Made up
    void DestroyContents()
    {
        DLListNode *pNode;

        while (pNode = m_pListBegin)
        {
            Remove(pNode);

            delete pNode;
        }
    }

    DLListNode *m_pListBegin;
    DLListNode *m_pListEnd;
};

class LListNode
{
public:
    LListNode() {
        m_pNext = NULL;
    }

    LListNode *m_pNext;
};

class LListBase
{
public:

    LListBase()
    {
        m_pListBegin = NULL;
        m_pListEnd = NULL;
    }

    ~LListBase() {    }

    // Made up
    void InsertAtEnd(LListNode *pNode)
    {
        if (m_pListEnd)
        {
            m_pListEnd->m_pNext = pNode;
            m_pListEnd = pNode;
        }
        else
        {
            m_pListBegin = pNode;
            m_pListEnd = pNode;
        }
    }

    // Made up
    LListNode* RemoveHead()
    {
        LListNode *pNode = m_pListBegin;
        m_pListBegin = pNode->m_pNext;

        if (m_pListBegin == NULL)
            m_pListEnd = NULL;

        pNode->m_pNext = NULL;
    }

    // Made up
    BOOL Empty()
    {
        return(!m_pListBegin ? TRUE : FALSE);
    }

    // Made up
    void DestroyContents()
    {
        while (m_pListBegin)
        {
            LListNode *pNode = RemoveHead();
            delete pNode; 
        }
    }

    LListNode *m_pListBegin;
    LListNode *m_pListEnd;
};



