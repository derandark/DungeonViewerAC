
#pragma once
#include "Vertex.h"

class CLandCell;
class CSurfaceTriStrips;
class CPolygon;

class CLandBlockStruct
{
public:
    CLandBlockStruct();
    ~CLandBlockStruct();

    static void init();

    void Destroy();
    BOOL UnPack(BYTE **ppData, ULONG iSize);

    CPolygon *AddPolygon(int PolyIndex, int Vertex0, int Vertex1, int Vertex2);
    void AdjPlanes(void);
    void CalcCellWater(long x, long y, BOOL& CellHasWater, BOOL& CellFullyFlooded);
    void CalcWater(void);
    void ConstructPolygons(DWORD LandBlock);
    void ConstructVertices(void);
    void ConstructUVs(DWORD LandBlock);
    void GetCellRotation(DWORD LandBlock, DWORD x, DWORD y, int& uvset, int& texidx);
    void InitPVArrays(void);
    void RemoveSurfaces(void);
    void TransAdjust(void);

    void calc_lighting(void);
    BOOL generate(DWORD LandBlock, DWORD VertScale, DWORD TransAdj);

    Vector*                m_LightCache;        // 0x00 - LightCache
    DWORD                m_TransAdj;        // 0x04
    long                m_VertexWidth;        // 0x08
    long                m_PolygonWidth;    // 0x0C
    long                m_CellWidth;        // 0x10
    int                    m_WaterLevel;        // 0x14 - 0=none, 1=partial, 2=fully ?

    BYTE*                m_HeightMap;        // 0x18
    WORD*                m_TerrainMap;        // 0x1C
    CVertexArray        m_VertexArray;        // 0x20
    CPolygon*            m_Polygons;        // 0x30
    DWORD                m_34;
    CSurfaceTriStrips*    m_SurfStrips;        // 0x38
    DWORD                m_3C;
    CLandCell*            m_LandCells;        // 0x40
    DWORD*                m_PolyDirs;        // 0x44
    
    long                m_LCoordX;            // 0x48 - (0x70 from CLandBlock)
    long                m_LCoordY;            // 0x4C - (0x74 from CLandBlock)

    static CSWVertexUV    land_uvs[4];
};
