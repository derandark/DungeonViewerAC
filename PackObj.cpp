
#include "StdAfx.h"
#include "PackObj.h"

PackObj::PackObj()
{
}

PackObj::~PackObj()
{
}

ULONG PackObj::GetPackSize()
{
    return pack_size();
}

ULONG PackObj::pack_size()
{
    return 0;
}

BOOL PackObj::ALIGN_PTR(BYTE** ppData, ULONG* piSize)
{
    size_t alignBytes = 4 - (((size_t)*ppData) & 3);

    if (alignBytes != 4)
    {
        if ((*piSize) < alignBytes)
            return FALSE; // Not enough room.

        // Adjust the size.
        *piSize = (*piSize) - alignBytes;

        // Write null bytes on the padding. (ugly, matches turbine -- this can be changed later?)
        while (alignBytes > 0)
        {
            *(*ppData) = 0;
            *ppData = *ppData + 1;
            alignBytes--;
        }
    }

    return TRUE;
}