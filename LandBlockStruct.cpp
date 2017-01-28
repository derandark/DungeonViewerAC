
#include "StdAfx.h"
#include "Polygon.h"
#include "LandDefs.h"
#include "LandCell.h"
#include "LandBlock.h"
#include "LandBlockStruct.h"
#include "RegionDesc.h"

#pragma warning(disable: 4018)

CSWVertexUV CLandBlockStruct::land_uvs[4];

CLandBlockStruct::CLandBlockStruct()
{
    m_LightCache = NULL;
    m_TransAdj = 9;
    m_VertexWidth = 0;
    m_PolygonWidth = 0;
    m_CellWidth = 0;
    m_WaterLevel = 0;

    m_Polygons = NULL;
    m_34 = 0;
    m_SurfStrips = NULL;
    m_LandCells = NULL;
    m_PolyDirs = NULL;

    int NumMapVertices = LandDefs::side_vertex_count * LandDefs::side_vertex_count;
    m_HeightMap = new BYTE[ NumMapVertices ];
    m_TerrainMap = new WORD[ NumMapVertices ];

    m_3C = -1;
}

void CLandBlockStruct::init()
{
    land_uvs[0].u = 0.0f;
    land_uvs[0].v = 1.0f;
    land_uvs[1].u = 1.0f;
    land_uvs[1].v = 1.0f;
    land_uvs[2].u = 1.0f;
    land_uvs[2].v = 0.0f;
    land_uvs[3].u = 0.0f;
    land_uvs[3].v = 0.0f;
}

CLandBlockStruct::~CLandBlockStruct()
{
    Destroy();

    delete [] m_HeightMap;
    delete [] m_TerrainMap;
}

void CLandBlockStruct::Destroy()
{
    // Set all UV array pointers to NULL (so nothing tries to delete them)
    if (m_VertexArray.m_VertexType == 1)
    {
        for (DWORD i = 0; i < m_VertexArray.m_VertexCount; i++)
            ((CSWVertex *)((BYTE *)m_VertexArray.m_pVertexBuffer + i*CVertexArray::vertex_size))->uvarray = NULL;
    }
    
    if (m_LandCells)
    {
        RemoveSurfaces();

        // Finish me

        m_LandCells = NULL;
    }

    if (m_Polygons)
    {
        delete [] m_Polygons;
        m_Polygons = NULL;
    }

    m_VertexArray.DestroyVertex();

    if (m_PolyDirs)
    {
        delete [] m_PolyDirs;
        m_PolyDirs = NULL;
    }

    if (m_SurfStrips)
    {
        UNFINISHED("CSurfaceTriStrips::Free(m_38);");
        m_SurfStrips = NULL;
    }

    if (m_LightCache)
    {
        delete [] m_LightCache;
        m_LightCache = NULL;
    }
}

void CLandBlockStruct::RemoveSurfaces()
{
    // Not done.
}

BOOL CLandBlockStruct::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    int NumMapVertices = LandDefs::side_vertex_count * LandDefs::side_vertex_count;

    // Unpack Terrain Map
    for (int i = 0; i < NumMapVertices; i++)
        UNPACK(WORD, m_TerrainMap[i]);

    // Unpack Height Map
    for (int i = 0; i < NumMapVertices; i++)
        UNPACK(BYTE, m_HeightMap[i]);

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return TRUE;
}

BOOL CLandBlockStruct::generate(DWORD LandBlock, DWORD CellScale, DWORD TransAdj)
{
    long CellWidth = LandDefs::lblock_side / CellScale;

    if ((CellWidth == m_CellWidth) && (m_TransAdj == TransAdj))
        return FALSE;

    BOOL ReGenerateCells = FALSE;

    if (CellWidth != m_CellWidth)
    {
        ReGenerateCells = TRUE;

        if (m_CellWidth > 0)
            Destroy();

        m_CellWidth = CellWidth;

        m_PolygonWidth    = (m_CellWidth * LandDefs::vertex_per_cell);
        m_VertexWidth    = (m_CellWidth * LandDefs::vertex_per_cell) + 1;

        InitPVArrays();
    }

    m_TransAdj = TransAdj;
    ConstructVertices();

    if (m_TransAdj && (m_CellWidth > 1) && (m_CellWidth < LandDefs::lblock_side))
        TransAdjust();

    if (!ReGenerateCells)
    {
        AdjPlanes();
        CalcWater();
        return FALSE;
    }
    else
    {
        ConstructPolygons(LandBlock);
        ConstructUVs(LandBlock);

        //if (RenderOptions::bUseTriangleStrips)
        //  ConstructTriangleStrips();

        CalcWater();

        return TRUE;
    }
}

void CLandBlockStruct::AdjPlanes(void)
{
    for (DWORD x = 0; x < (unsigned)m_PolygonWidth; x++) {
        for (DWORD y = 0; y < (unsigned)m_PolygonWidth; y++) {
            for (DWORD poly = 0; poly < 2; poly++) {
                m_Polygons[ (((x * m_PolygonWidth) + y) * 2) + poly ].make_plane();
            }
        }
    }
}

void CLandBlockStruct::InitPVArrays(void)
{
    long NumSquares        = m_PolygonWidth * m_PolygonWidth; 
    long NumVertices    = m_VertexWidth * m_VertexWidth;
    long NumCells        = m_CellWidth * m_CellWidth;

    // Use D3DVertex check is here.
    m_VertexArray.AllocateVertex(NumVertices, 1);

    for (DWORD x = 0; x < m_VertexWidth; x++) {
        for (DWORD y = 0; y < m_VertexWidth; y++) {

            int VertIndex = ((m_VertexWidth * x) + y);

            if (m_VertexArray.m_VertexType == 2)
            {
                UNFINISHED("Port me please..");
            }
            else
            {
                CSWVertex* pVertex = (CSWVertex *)(((BYTE *)m_VertexArray.m_pVertexBuffer) + (VertIndex * CVertexArray::vertex_size));

                pVertex->index        = VertIndex;
                pVertex->uvcount    = 4;
                pVertex->uvarray    = land_uvs;
            }
        }
    }

    long NumPolygons    = NumSquares * 2;
    m_Polygons            = new CPolygon[ NumPolygons ];
    
    for (int i = 0; i < NumPolygons; i++)
    {
        m_Polygons[i].m_iVertexCount    = 3;
        m_Polygons[i].m_iPolyIndex        = i;
        m_Polygons[i].m_pVertexObjects    = new CSWVertex*[ 3 ];
        m_Polygons[i].m_pVertexIndices    = new WORD[ 3 ];
        m_Polygons[i].m_iCullType        = CullCW;

        if (m_VertexArray.m_VertexType == 1)
        {
            m_Polygons[i].m_Face1_UVIndices = new BYTE[ 3 ];
            m_Polygons[i].m_Face2_UVIndices = NULL;
            m_Polygons[i].m_pVertexUnks        = new CSWVertex*[ 3 ];

            m_Polygons[i].m_Face1_UVIndices[0] = 0;
            m_Polygons[i].m_Face1_UVIndices[1] = 1;
            m_Polygons[i].m_Face1_UVIndices[2] = 2;
        }
    }

    m_PolyDirs        = new DWORD[ NumSquares ];
    m_LandCells        = new CLandCell[ NumCells ];
    m_LightCache    = new Vector[ NumVertices ];
}

void OutputVerticeInfo(CLandBlockStruct *pStruct)
{
    for (ULONG x = 0; x < pStruct->m_VertexWidth; x++) {
        for (ULONG y = 0; y < pStruct->m_VertexWidth; y++)    {

            CSWVertex* pVertex = VERTEX_NUM(pStruct->m_VertexArray.m_pVertexBuffer, (x * pStruct->m_VertexWidth) + y);
            DEBUGOUT("CV %f %f %f\r\n", pVertex->origin.x, pVertex->origin.y, pVertex->origin.z);
        }
    }
}

void CLandBlockStruct::TransAdjust(void)
{
    if ((m_TransAdj == 1) || (m_TransAdj == 5) || (m_TransAdj == 7))
    {
        for (DWORD i = 1; i < m_PolygonWidth; i += 2)
        {
            CSWVertex *pV1, *pV2, *pV3;
            
            pV1 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, ((i - 1) * m_VertexWidth) + m_PolygonWidth);
            pV2 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, ((i + 1) * m_VertexWidth) + m_PolygonWidth);
            pV3 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, ((i) * m_VertexWidth) + m_PolygonWidth);

            pV3->origin.z = (pV1->origin.z + pV2->origin.z) / 2;
        }
    }

    if ((m_TransAdj == 4) || (m_TransAdj == 5) || (m_TransAdj == 6))
    {
        for (unsigned int i = 1; i < m_PolygonWidth; i += 2)
        {
            CSWVertex *pV1, *pV2, *pV3;
            
            pV1 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (i - 1));
            pV2 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (i + 1));
            pV3 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (i));

            pV3->origin.z = (pV1->origin.z + pV2->origin.z) / 2;
        }
    }

    if ((m_TransAdj == 2) || (m_TransAdj == 6) || (m_TransAdj == 8))
    {
        for (unsigned int i = 1; i < m_PolygonWidth; i += 2)
        {
            CSWVertex *pV1, *pV2, *pV3;
            
            pV1 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (i - 1) * m_VertexWidth);
            pV2 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (i + 1) * m_VertexWidth);
            pV3 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (i) * m_VertexWidth);

            pV3->origin.z = (pV1->origin.z + pV2->origin.z) / 2;
        }
    }

    if ((m_TransAdj == 3) || (m_TransAdj == 7) || (m_TransAdj == 8))
    {
        for (unsigned int i = 1; i < m_PolygonWidth; i += 2)
        {
            CSWVertex *pV1, *pV2, *pV3;
            
            DWORD dwWhatever = (m_VertexWidth * m_PolygonWidth) + i;

            pV1 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, dwWhatever - 1);
            pV2 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, dwWhatever + 1);
            pV3 = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, dwWhatever);

            pV3->origin.z = (pV1->origin.z + pV2->origin.z) / 2;
        }
    }

    if (m_CellWidth != (LandDefs::lblock_side / 2))
        return;

    if (m_TransAdj == 1)
    {
        for (unsigned int i = 1, j = 4; i < m_PolygonWidth; i += 2, j += 4)
        {
            BYTE    bHeight;
            float*    OldZ = &VERTEX_NUM(m_VertexArray.m_pVertexBuffer, i * m_VertexWidth)->origin.z;

            bHeight = m_HeightMap[ (j - 1) * LandDefs::side_vertex_count ];
            float fHeight1 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ (j) * LandDefs::side_vertex_count ];
            float fHeight2 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ (j - 3) * LandDefs::side_vertex_count ];
            float fHeight3 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ (j - 4) * LandDefs::side_vertex_count ];
            float fHeight4 = LandDefs::Land_Height_Table[ bHeight ];

            float fVariantZ;
            
            fVariantZ = (fHeight3 + fHeight3) - fHeight4;
            if (fVariantZ < (*OldZ))
                *OldZ = fVariantZ;

            fVariantZ = (fHeight1 + fHeight1) - fHeight2;
            if (fVariantZ < (*OldZ))
                *OldZ = fVariantZ;
        }
    }
    
    if (m_TransAdj == 2)
    {
        for (unsigned int i = 1, j = 4; i < m_PolygonWidth; i += 2, j += 4)
        {
            BYTE    bHeight;
            float*    OldZ = &VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (i * m_VertexWidth) + m_PolygonWidth)->origin.z;

            bHeight = m_HeightMap[ ((j - 1) * LandDefs::side_vertex_count) + LandDefs::side_vertex_count - 1];
            float fHeight1 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ ((j) * LandDefs::side_vertex_count) + LandDefs::side_vertex_count - 1];
            float fHeight2 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ ((j - 3) * LandDefs::side_vertex_count) + LandDefs::side_vertex_count - 1];
            float fHeight3 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ ((j - 4) * LandDefs::side_vertex_count) + LandDefs::side_vertex_count - 1];
            float fHeight4 = LandDefs::Land_Height_Table[ bHeight ];

            float fVariantZ1 = (fHeight3 + fHeight3) - fHeight4;
            if (fVariantZ1 < (*OldZ))
                *OldZ = fVariantZ1;

            float fVariantZ2 = (fHeight1 + fHeight1) - fHeight2;
            if (fVariantZ2 < (*OldZ))
                *OldZ = fVariantZ2;
        }
    }

    if (m_TransAdj == 3)
    {
        for (unsigned int i = 1; i < m_PolygonWidth; i += 2)
        {
            BYTE    bHeight;
            float*    OldZ = &VERTEX_NUM(m_VertexArray.m_pVertexBuffer, i)->origin.z;

            bHeight = m_HeightMap[ (i * 2) + 1 ];
            float fHeight1 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ (i * 2) + 2 ];
            float fHeight2 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ (i * 2) - 1 ];
            float fHeight3 = LandDefs::Land_Height_Table[ bHeight ];

            bHeight = m_HeightMap[ (i * 2) - 2 ];
            float fHeight4 = LandDefs::Land_Height_Table[ bHeight ];

            float fVariantZ;
            fVariantZ = (fHeight3 + fHeight3) - fHeight4;
            if (fVariantZ < (*OldZ))
                *OldZ = fVariantZ;

            fVariantZ = (fHeight1 + fHeight1) - fHeight2;
            if (fVariantZ < (*OldZ))
                *OldZ = fVariantZ;
        }
    }

    if (m_TransAdj == 4)
    {
        for (unsigned int i = 1; i < m_PolygonWidth; i += 2)
        {
            float *OldZ = &VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (m_VertexWidth * m_PolygonWidth) + i)->origin.z;
            BYTE *bTrans = &m_HeightMap[ ((LandDefs::side_vertex_count - 1) * LandDefs::side_vertex_count) + (i * 2) ];

            float fHeight1 = LandDefs::Land_Height_Table[ bTrans[1] ];
            float fHeight2 = LandDefs::Land_Height_Table[ bTrans[2] ];
            float fHeight3 = LandDefs::Land_Height_Table[ bTrans[-1] ];
            float fHeight4 = LandDefs::Land_Height_Table[ bTrans[-2] ];

            float fVariantZ;
            
            fVariantZ = (fHeight3 + fHeight3) - fHeight4;
            if (fVariantZ < (*OldZ))
                *OldZ = fVariantZ;

            fVariantZ = (fHeight1 + fHeight1) - fHeight2;
            if (fVariantZ < (*OldZ))
                *OldZ = fVariantZ;
        }
    }
}

void CLandBlockStruct::ConstructVertices(void)
{
    DWORD CellScale = LandDefs::lblock_side / m_CellWidth;
    float PolyGridWidth = LandDefs::block_length / m_PolygonWidth;

    for (ULONG x = 0; x < m_VertexWidth; x++) {
        float PolyGridX = (float)x * PolyGridWidth;

        for (ULONG y = 0; y < m_VertexWidth; y++)    {
            float PolyGridY = (float)y * PolyGridWidth;

            CSWVertex* pVertex = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, (x * m_VertexWidth) + y);

            BYTE HeightTableIndex
                = m_HeightMap[ ((x * LandDefs::side_vertex_count) + y) * CellScale ];

            float ZHeight = LandDefs::Land_Height_Table[HeightTableIndex];

            pVertex->origin = Vector(PolyGridX, PolyGridY, ZHeight);
        }
    }
}

void CLandBlockStruct::ConstructPolygons(DWORD LandBlock)
{
    // if (LandBlock == 0x0000FFFF)
    //    __asm int 3;

    long LCoordX, LCoordY;
    LandDefs::blockid_to_lcoord(LandBlock, LCoordX, LCoordY);

    long var_4 = 214614067 * LandDefs::vertex_per_cell;
    long var_8 = 1109124029 * LandDefs::vertex_per_cell;
    long var_20 = 1;
    long var_24 = 214614067 * (LCoordX * LandDefs::vertex_per_cell);
    long var_28 = 1109124029 * (LCoordX * LandDefs::vertex_per_cell);
    long var_2C = 0;
    long var_10 = (LCoordY * LandDefs::vertex_per_cell);
    long var_3C = LandDefs::vertex_per_cell;
    
    for (int x = 0; x < m_CellWidth; x++) // x = var_58
    {
        long var_38 = LCoordX + x;
        long var_30 = 1;
        long var_34 = 0;
        float var_C = var_20;

        for (int y = 0; y < m_CellWidth; y++) // y = arg_0
        {
            long var_14 = LCoordY + y;
            long var_40 = var_28;
            long var_44 = var_24 + 1813693831;
            
            for (int i = 0; i < LandDefs::vertex_per_cell; i++) // i = var_4C
            {
                long var_ebx = var_2C + i;
                long var_edi = var_34;

                for (int j = 0; j < LandDefs::vertex_per_cell; j++) // j = var_48
                {
                    DWORD var_54 = (((var_10 + var_edi) * var_44) - var_40) - 1369149221;
                    DWORD var_50 = ((LandDefs::vertex_per_cell * i) + j) * 2;

                    if (((float)var_54 * 2.3283064e-10) < 0.5)
                    {
                        m_PolyDirs[ (m_PolygonWidth * var_ebx) + var_edi ] = 0;
                        
                        long unk = (m_VertexWidth * var_ebx) + var_edi;
                        var_54 = var_ebx + 1;

                        m_LandCells[(m_CellWidth * x) + y].m_Polys[var_50] =
                            AddPolygon(
                                ((m_PolygonWidth * var_ebx) + var_edi) * 2, unk,
                                (m_VertexWidth * var_54) + var_edi, unk+1);

                        long unk2 = (m_VertexWidth * var_54) + var_edi;

                        m_LandCells[(m_CellWidth * x) + y].m_Polys[var_50+1] =
                            AddPolygon(
                                (((m_PolygonWidth * var_ebx) + var_edi) * 2) + 1, unk2+1,
                                (m_VertexWidth * var_ebx) + var_edi + 1, unk2);
                    }
                    else
                    {
                        m_PolyDirs[ (m_PolygonWidth * var_ebx) + var_edi ] = 1;

                        long unk = (m_VertexWidth * (var_ebx + 1)) + var_edi;
                        var_54 = var_ebx + 1;

                        m_LandCells[(m_CellWidth * x) + y].m_Polys[var_50] =
                            AddPolygon(
                                ((m_PolygonWidth * var_ebx) + var_edi) * 2,
                                (m_VertexWidth * var_ebx) + var_edi,
                                unk, unk+1);

                        long unk2 = (m_VertexWidth * var_ebx) + var_edi;

                        m_LandCells[(m_CellWidth * x) + y].m_Polys[var_50+1] =
                            AddPolygon(
                                (((m_PolygonWidth * var_ebx) + var_edi) * 2) + 1, unk2,
                                (m_VertexWidth * var_54) + var_edi + 1, unk2+1);
                    }

                    var_edi++;
                }

                var_44 += 214614067;
                var_40 += 1109124029;
            }

            DWORD CellID = LandDefs::get_block_gid(var_38, var_14);

            m_LandCells[(m_CellWidth * x) + y].m_Key = CellID;
            m_LandCells[(m_CellWidth * x) + y].m_Position.m_LandCell = CellID;

            m_LandCells[(m_CellWidth * x) + y].m_Position.m_Frame.m_origin.x =
                (var_C * LandDefs::half_square_length);
            m_LandCells[(m_CellWidth * x) + y].m_Position.m_Frame.m_origin.y =
                ((float)var_30 * LandDefs::half_square_length);

            var_30 += 2;
            var_34 += LandDefs::vertex_per_cell; // double check this
        }

        var_2C += LandDefs::vertex_per_cell;
        var_28 += var_8;
        var_24 += var_4;
        var_20 += 2;
    }
}

CPolygon* CLandBlockStruct::AddPolygon(int PolyIndex, int Vertex0, int Vertex1, int Vertex2)
{
    m_Polygons[PolyIndex].m_pVertexObjects[0] = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, Vertex0);
    m_Polygons[PolyIndex].m_pVertexIndices[0] = Vertex0;

    m_Polygons[PolyIndex].m_pVertexObjects[1] = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, Vertex1);
    m_Polygons[PolyIndex].m_pVertexIndices[1] = Vertex1;

    m_Polygons[PolyIndex].m_pVertexObjects[2] = VERTEX_NUM(m_VertexArray.m_pVertexBuffer, Vertex2);
    m_Polygons[PolyIndex].m_pVertexIndices[2] = Vertex2;

    m_Polygons[PolyIndex].make_plane();

    m_Polygons[PolyIndex].m_Face1_TexIndex = 0;

    if (m_Polygons[PolyIndex].m_pVertexObjects[0]->origin.z == 0 && 
        m_Polygons[PolyIndex].m_pVertexObjects[1]->origin.z == 0 &&
        m_Polygons[PolyIndex].m_pVertexObjects[2]->origin.z == 0)
    {
        m_Polygons[PolyIndex].m_Face1_TexIndex = 0;
    }
    else
        m_Polygons[PolyIndex].m_Face1_TexIndex = 1;

    return &m_Polygons[PolyIndex];
}

void CLandBlockStruct::ConstructUVs(DWORD LandBlock)
{
    // Missing RenderOptions::bSingleSurfaceLScape code

    for (int x = 0; x < m_PolygonWidth; x++) {
        for (int y = 0; y < m_PolygonWidth; y++) {

            int uvset, texidx;
            GetCellRotation(LandBlock, x, y, uvset, texidx);

            if (m_PolyDirs[ (x * m_PolygonWidth) + y ] != 0)
            {
                if (m_VertexArray.m_VertexType == 1)
                {
                }
            }
            else
            {
                if (m_VertexArray.m_VertexType == 1)
                {
                }
            }
        }
    }

    // Missing RenderOptions::bSingleSurfaceLScape code
}

void CLandBlockStruct::GetCellRotation(DWORD LandBlock, DWORD x, DWORD y, int& uvset, int& texidx)
{
    return;
#if 0
    long lcx, lcy;
    LandDefs::blockid_to_lcoord(LandBlock, lcx, lcy);
    lcx += x;
    lcy += y;

    long var_18 = LandDefs::lblock_side / m_CellWidth;

    int PalShifted;

    if (CRegionDesc::current_region->IsPalShifted())
        PalShifted = 1;
    else
        PalShifted = ((var_18 > 1) ? 4 : 0);
    
    long var_1C = (LandDefs::side_vertex_count * x) + y;
    DWORD Terrain = m_TerrainMap[var_1C * var_18];
    long var_esi = (Terrain & 0x7F) >> LandDefs::terrain_byte_offset;
    long var_30 = Terrain & (LandDefs::num_road - 1);
    long var_28 = (LandDefs::side_vertex_count * (x + 1)) + y;
    DWORD Terrain2 = m_TerrainMap[var_28 * var_18];
    long var_edi = (Terrain2 & 0x7F) >> LandDefs::terrain_byte_offset;
    long var_34 = Terrain2 & (LandDefs::num_road - 1);
    DWORD Terrain3 = m_TerrainMap[(var_28 + 1) * var_18];
    long var_ebx = (Terrain3 & 0x7F) >> LandDefs::terrain_byte_offset;
    long var_38 = Terrain3 & (LandDefs::num_road - 1);
    DWORD Terrain4 = m_TerrainMap[(var_1C + 1) * var_18];
    long var_ebp = (Terrain4 & 0x7F) >> LandDefs::terrain_byte_offset;
    long var_28 = Terrain4 & (LandDefs::num_road - 1);

    DWORD Palettes[4]; // var_10, var_C, var_8, var_4

    Palettes[0] = GetPalCode(PalShifted, var_30, var_34, var_38, var_28, var_esi, var_edi, var_ebx, var_ebp);
    Palettes[1] = GetPalCode(PalShifted, var_34, var_38, var_28, var_30, var_edi, var_ebx, var_ebp, var_esi);
    Palettes[2] = GetPalCode(PalShifted, var_38, var_28, var_30, var_34, var_ebx, var_ebp, var_esi, var_edi);
    Palettes[3] = GetPalCode(PalShifted, var_28, var_30, var_34, var_38, var_ebp, var_esi, var_edi, var_ebx);

    // Missing RenderOptions::bSingleSurfaceLScape code

    CRegionDesc::current_region->m_TerrainDesc->GetLandSurf()->
        SelectTerrain(lcx, lcy, uvset, texid, Palettes, PalShifted, CRegionDesc::current_region->m_34);
#endif
}

void CLandBlockStruct::CalcWater(void)
{
    BOOL WaterBlock = TRUE; // ebp
    BOOL HasWater = FALSE; // var_C

    if (LandDefs::lblock_side == m_CellWidth) 
    {
        for (int x = 0; x < m_CellWidth; x++) {
            for (int y = 0; y < m_CellWidth; y++) {

                BOOL CellHasWater, CellFullyFlooded;
                CalcCellWater(x, y, CellHasWater, CellFullyFlooded);

                if (CellHasWater)
                {
                    CellHasWater = TRUE;

                    if (CellFullyFlooded)
                        m_LandCells[ (m_CellWidth * x) + y ].m_Water40 = 2;
                    else
                    {
                        m_LandCells[ (m_CellWidth * x) + y ].m_Water40 = 1;
                        WaterBlock = FALSE;
                    }
                }
                else
                {
                    m_LandCells[ (m_CellWidth * x) + y ].m_Water40 = 0;
                    WaterBlock = FALSE;
                }
            }
        }
    }

    m_WaterLevel = HasWater ? (WaterBlock ? 2 : 1) : 0;
}

void CLandBlockStruct::CalcCellWater(long x, long y, BOOL& CellHasWater, BOOL& CellFullyFlooded)
{
    CellHasWater = FALSE;
    CellFullyFlooded = TRUE;

    for (int vx = (x * LandDefs::vertex_per_cell); vx < ((x + 1) * LandDefs::vertex_per_cell); vx++) {
        for (int vy = (y * LandDefs::vertex_per_cell); vy < ((y + 1) * LandDefs::vertex_per_cell); vy++)
        {
            static BOOL terrain_is_water[32] = {
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            };

            if (1 == terrain_is_water[ ((m_TerrainMap[ (LandDefs::side_vertex_count * vx) + vy ] & 0x7F) >> LandDefs::terrain_byte_offset) ])
                CellHasWater = TRUE;
            else
                CellFullyFlooded = FALSE;
        }
    }
}

void CLandBlockStruct::calc_lighting(void)
{
    // Not done.
}









