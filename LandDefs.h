
#pragma once
#include "MathLib.h"

class LandDefs
{
public:
    static DWORD blockid_mask;
    static DWORD lbi_cell_id;
    static DWORD block_cell_id;
    static DWORD first_envcell_id;
    static DWORD last_envcell_id;
    static DWORD first_lcell_id;
    static DWORD last_lcell_id;
    static long max_block_width;
    static DWORD max_block_shift;
    static DWORD blockx_mask;
    static DWORD blocky_mask;
    static DWORD block_part_shift;
    static DWORD cellid_mask;
    static DWORD terrain_byte_offset;

    /*
    static float Land_Height_Table[256];
    static float inside_val;
    static long num_block_length;
    static long num_block_width;
    static long num_blocks;
    static float square_length;
    static float half_square_length;
    static long lblock_side;
    static long lblock_shift;
    static long lblock_mask;
    static long land_width;
    static long land_height;
    static float block_length;
    static long last_lcell_id;
    */

    static long    side_vertex_count;
    static float half_square_length;
    static float square_length;
    static float block_length;
    static long lblock_shift;
    static long lblock_side;
    static long lblock_mask;
    static long land_width;
    static long land_length;
    static long num_block_length;
    static long num_block_width;
    static long num_blocks;
    static float inside_val;
    static float outside_val;
    static float max_object_height;
    static float road_width;
    static float sky_height;
    static long vertex_per_cell;
    static long polys_per_landcell;
    static float Land_Height_Table[256];

    static Vector    get_block_offset(DWORD LandBlock1, DWORD LandBlock2);
    static DWORD    get_block_gid(long X, long Y);
    static BOOL        blockid_to_lcoord(DWORD LandBlock, long& X, long& Y);
    static BOOL        gid_to_lcoord(DWORD LandCell, long& X, long& Y);
    static DWORD    lcoord_to_gid(long X, long Y);
    static BOOL        inbound_valid_cellid(DWORD LandCell);
    static BOOL        in_bounds(long GidX, long GidY);
    static BOOL        set_height_table(const float *HeightTable);

    static BOOL        init(long NumBlockLength, long NumBlockWidth, float SquareLength, long LBlockLength,
                            long VertexPerCell, float MaxObjHeight, float SkyHeight, float RoadWidth);
};


