
#pragma once

class PackObj
{
public:
    PackObj();
    virtual    ~PackObj();

    // Use both names? One will call the other?
    virtual    ULONG    GetPackSize();
    virtual    ULONG    pack_size();

    // The main packing calls.
    virtual ULONG    Pack(BYTE** ppData, ULONG iSize) { return 0; }
    virtual BOOL    UnPack(BYTE** ppData, ULONG iSize) { return TRUE; };

    // Packing functions
    static BOOL ALIGN_PTR(BYTE** ppData, ULONG* piSize);

    template< typename ValType, typename OutputType >
    static BOOL UNPACK_TYPE(OutputType* pBuffer, BYTE** ppData, ULONG* piSize)
    {
        if ((*piSize) < sizeof(ValType))
            return FALSE;

        *pBuffer = (OutputType)(*((ValType *)*ppData));
        *ppData = (*ppData) + sizeof(ValType);
        *piSize = (*piSize) - sizeof(ValType);

        return TRUE;
    }

    template< typename ValType, typename InputType >
    static BOOL PACK_TYPE(const InputType Buffer, BYTE** ppData, ULONG* piSize)
    {
        if ((*piSize) < sizeof(ValType))
            return FALSE;

        *((ValType *)(*ppData)) = Buffer;
        *ppData = (*ppData) + sizeof(ValType);
        *piSize = (*piSize) - sizeof(ValType);

        return TRUE;
    }
};

// UnPack Variables
#define UNPACK(type, output_var) PackObj::UNPACK_TYPE< type >(&output_var, ppData, &iSize)
#define UNPACK_POBJ(pobj) pobj->UnPack(ppData, iSize)
#define UNPACK_OBJ(obj) obj.UnPack(ppData, iSize)
#define PACK(type, input_var) PackObj::PACK_TYPE< type >(input_var, ppData, &iSize)
#define PACK_POBJ(pobj) pobj->Pack(ppData, iSize)
#define PACK_OBJ(obj) obj.Pack(ppData, iSize)
#define PACK_ALIGN() PackObj::ALIGN_PTR(ppData, &iSize)

#define FBitSet(flags, bit) ((((DWORD)flags) >> (bit)) & 1)









