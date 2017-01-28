
#include "StdAfx.h"
#include "Frame.h"
#include "LandDefs.h"

AFrame::AFrame()
{
    m_origin = Vector(0, 0, 0);
    m_angles = Quaternion(1.0, 0, 0, 0);
}

BOOL AFrame::UnPack(BYTE **ppData, ULONG iSize)
{
    UNPACK_OBJ(m_origin);
    UNPACK_OBJ(m_angles);

    if (!IsValid())
        return FALSE;

    return TRUE;
}

BOOL AFrame::IsValid()
{
    if (!m_origin.IsValid())
        return FALSE;

    if (!m_angles.IsValid())
        return FALSE;

    return TRUE;
}

Frame::Frame()
{
    m_origin = Vector(0, 0, 0);
    m_angles = Quaternion(1.0, 0, 0, 0);

    cache();
}

BOOL Frame::IsValid()
{
    if (!m_origin.IsValid())
        return FALSE;

    if (!m_angles.IsValid())
        return FALSE;

    return TRUE;
}

ULONG Frame::pack_size() 
{
    return(m_origin.pack_size() + m_angles.pack_size());
}

ULONG Frame::Pack(BYTE **ppData, ULONG iSize)
{
    UNFINISHED("Frame::Pack");

    return 0;
}

BOOL Frame::UnPack(BYTE **ppData, ULONG iSize)
{
    if (iSize < pack_size())
        return FALSE;

    UNPACK_OBJ(m_origin);
    UNPACK_OBJ(m_angles);

    if (!IsValid())
        return FALSE;

    cache();

    return TRUE;
}


void EulGetOrd(int ord, int& i, int& j, int& k, int& h, int& n, int& s, int& f)
{
    #define EulFrmS         0
    #define EulFrmR         1
    #define EulFrm(ord)  ((unsigned)(ord)&1)
    #define EulRepNo     0
    #define EulRepYes    1
    #define EulRep(ord)  (((unsigned)(ord)>>1)&1)
    #define EulParEven   0
    #define EulParOdd    1
    #define EulSafe        "\000\001\002\000"
    #define EulNext        "\001\002\000\001"

    int o = ord;

    f = o & 1; o >>=1;
    s = o & 1; o >>= 1;
    n = o & 1; o >>= 1;
    i = EulSafe[ o & 3 ];
    j = EulNext[ i + n ];
    k = EulNext[ i + 1 - n ];
    h = s ? k : i;        
}

void Frame::euler_set_rotate(Vector Angles, int order)
{
    Quaternion qu;

    float a[3];
    float ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int i, j, k, h, n, s, f;
    EulGetOrd(order, i, j, k, h, n, s, f);

    if (f == EulFrmR)
    {
        // Rotate
        float t = Angles.x;
        Angles.x = Angles.z;
        Angles.z = t;
    }
    
    if (n == EulParOdd)
        Angles.y = -Angles.y;

    ti = Angles.x * 0.5f; tj = Angles.y * 0.5f; th = Angles.z * 0.5f;
    ci = cos(ti);  cj = cos(tj);  ch = cos(th);
    si = sin(ti);  sj = sin(tj);  sh = sin(th);
    cc = ci * ch; cs = ci * sh; sc = si * ch; ss = si * sh;

    if (s == EulRepYes)
    {
        a[i] = cj * (cs + sc);    /* Could speed up with */
        a[j] = sj * (cc + ss);    /* trig identities. */
        a[k] = sj * (cs - sc);
        qu.w = cj * (cc - ss);
    }
    else
    {
        a[i] = cj * sc - sj * cs;
        a[j] = cj * ss + sj * cc;
        a[k] = cj * cs - sj * sc;
        qu.w = cj * cc + sj * ss;
    }

    if (n == EulParOdd)
        a[j] = -a[j];

    qu.x = a[0];
    qu.y = a[1];
    qu.z = a[2];

    set_rotate(qu);
}

void Frame::set_rotate(Quaternion angles)
{
    m_angles = angles;
    m_angles.normalize();

    cache();
}

void Frame::rotate_around_axis_to_vector(int axis, const Vector& target)
{
    Vector axe = *((Vector *)(&m00 + (axis * 3)));
    Vector dir = target - (axe * axe.dot_product(target));

    int next_ord[3] = { 2, 0, 1 };

    if (!dir.normalize_check_small())
        dir = *((Vector *)(&m00 + (next_ord[axis] * 3)));

    if ((F_EPSILON * 5.0f) < abs(dir.dot_product(axe)))
        return;

    int axe_x, axe_y;
    
    axe_x = next_ord[axis];
    *((Vector *)(&m00 + axe_x)) = dir;

    axe_y = next_ord[axe_x];
    *((Vector *)(&m00 + axe_y)) = cross_product(dir, axe);
    
    // Finish me.
    cache_quaternion();
}

void Frame::set_heading(float DegreeHeading)
{
    float RadianHeading = D3DXToRadian(DegreeHeading);

    Vector vechead(sin(RadianHeading), cos(RadianHeading), m12 + (m02 + m22 * 0.0));

    set_vector_heading(vechead);
}

void Frame::set_vector_heading(const Vector& Heading)
{
    Vector NormalHeading = Heading;

    if (NormalHeading.normalize_check_small())
        return;

    double zrotate = D3DXToRadian(-fmod(450 - D3DXToDegree(atan2(NormalHeading.y, NormalHeading.x)), 360));
    double xrotate = asin(NormalHeading.z);

    euler_set_rotate(Vector(xrotate, 0, zrotate), 0);
}

void Frame::subtract1(const Frame* a, const AFrame* b)
{
    Quaternion TempQuat;
    TempQuat.w = (((a->m_angles.w * b->m_angles.w) + (a->m_angles.x * b->m_angles.x)) + (a->m_angles.y * b->m_angles.y)) + (a->m_angles.z * b->m_angles.z);
    TempQuat.x = ((a->m_angles.x * b->m_angles.w) - (a->m_angles.w * b->m_angles.x) - (a->m_angles.y * b->m_angles.z)) + (a->m_angles.z * b->m_angles.y);
    TempQuat.y = ((a->m_angles.x * b->m_angles.z) - (a->m_angles.w * b->m_angles.y)) + (a->m_angles.y * b->m_angles.w) - (a->m_angles.z * b->m_angles.x);
    TempQuat.z = (((a->m_angles.x * b->m_angles.y) + (a->m_angles.w * b->m_angles.z)) - (a->m_angles.y * b->m_angles.x)) + (a->m_angles.z * b->m_angles.w);

    set_rotate(TempQuat);

    Vector TempVec;
    TempVec.x = m_origin.x + (-(b->m_origin.x) * m00) + (-(b->m_origin.y) * m10) + (-(b->m_origin.z) * m20);
    TempVec.y = m_origin.y + (-(b->m_origin.x) * m01) + (-(b->m_origin.y) * m11) + (-(b->m_origin.z) * m21);
    TempVec.z = m_origin.z + (-(b->m_origin.x) * m02) + (-(b->m_origin.y) * m12) + (-(b->m_origin.z) * m22);

    m_origin = TempVec;
}

void Frame::combine(const Frame* a, const AFrame* b)
{
    Vector TempVec;
    TempVec.x = a->m_origin.x + (b->m_origin.x * a->m00) + (b->m_origin.y * a->m10) + (b->m_origin.z * a->m20);
    TempVec.y = a->m_origin.y + (b->m_origin.x * a->m01) + (b->m_origin.y * a->m11) + (b->m_origin.z * a->m21);
    TempVec.z = a->m_origin.z + (b->m_origin.x * a->m02) + (b->m_origin.y * a->m12) + (b->m_origin.z * a->m22);

    m_origin = TempVec;

    Quaternion TempQuat;
    TempQuat.w = (((a->m_angles.w * b->m_angles.w) - (a->m_angles.x * b->m_angles.x)) - (a->m_angles.y * b->m_angles.y)) - (a->m_angles.z * b->m_angles.z);
    TempQuat.x = ((a->m_angles.x * b->m_angles.w) + (a->m_angles.w * b->m_angles.x) + (a->m_angles.y * b->m_angles.z)) - (a->m_angles.z * b->m_angles.y);
    TempQuat.y = ((a->m_angles.w * b->m_angles.y) - (a->m_angles.x * b->m_angles.z)) + (a->m_angles.y * b->m_angles.w) + (a->m_angles.z * b->m_angles.x);
    TempQuat.z = (((a->m_angles.x * b->m_angles.y) + (a->m_angles.w * b->m_angles.z)) - (a->m_angles.y * b->m_angles.x)) + (a->m_angles.z * b->m_angles.w);

    set_rotate(TempQuat);
}

void Frame::combine(const Frame* a, const AFrame* b, const Vector* scale)
{
    Vector ScaledVec;
    ScaledVec.x = b->m_origin.x * scale->x;
    ScaledVec.y = b->m_origin.y * scale->y;
    ScaledVec.z = b->m_origin.z * scale->z;

    Vector TempVec;
    TempVec.x = a->m_origin.x + (ScaledVec.x * a->m00) + (ScaledVec.y * a->m10) + (ScaledVec.z * a->m20);
    TempVec.y = a->m_origin.y + (ScaledVec.x * a->m01) + (ScaledVec.y * a->m11) + (ScaledVec.z * a->m21);
    TempVec.z = a->m_origin.z + (ScaledVec.x * a->m02) + (ScaledVec.y * a->m12) + (ScaledVec.z * a->m22);

    m_origin = TempVec;

    Quaternion TempQuat;
    TempQuat.w = (((a->m_angles.w * b->m_angles.w) - (a->m_angles.x * b->m_angles.x)) - (a->m_angles.y * b->m_angles.y)) - (a->m_angles.z * b->m_angles.z);
    TempQuat.x = ((a->m_angles.x * b->m_angles.w) + (a->m_angles.w * b->m_angles.x) + (a->m_angles.y * b->m_angles.z)) - (a->m_angles.z * b->m_angles.y);
    TempQuat.y = ((a->m_angles.w * b->m_angles.y) - (a->m_angles.x * b->m_angles.z)) + (a->m_angles.y * b->m_angles.w) + (a->m_angles.z * b->m_angles.x);
    TempQuat.z = (((a->m_angles.x * b->m_angles.y) + (a->m_angles.w * b->m_angles.z)) - (a->m_angles.y * b->m_angles.x)) + (a->m_angles.z * b->m_angles.w);

    set_rotate(TempQuat);
}

void Frame::rotate(const Vector& angles)
{
    Vector Rotation;

    Rotation.x = (m00 * angles.x) + (m10 * angles.y) + (m20 * angles.z);
    Rotation.y = (m01 * angles.x) + (m11 * angles.y) + (m21 * angles.z);
    Rotation.z = (m02 * angles.x) + (m12 * angles.y) + (m22 * angles.z);

    grotate(Rotation);
}

void Frame::grotate(Vector& angles)
{
    float magn = angles.magnitude();

    if (magn < F_EPSILON)
        return;

    angles.x *= 1 / magn;
    angles.y *= 1 / magn;
    angles.z *= 1 / magn;

    Quaternion qu;

    qu.w = cos(magn * 0.5f);
    qu.x = angles.x * sin(magn * 0.5f);
    qu.y = angles.y * sin(magn * 0.5f);
    qu.z = angles.z * sin(magn * 0.5f);


    Quaternion rot;
    rot.w = (((qu.w * m_angles.w) - (qu.x * m_angles.x)) - (qu.y * m_angles.y)) - (qu.z * m_angles.z);
    rot.x = ((qu.x * m_angles.w) + (qu.w * m_angles.x) + (qu.y * m_angles.z)) - (qu.z * m_angles.y);
    rot.y = ((qu.w * m_angles.y) - (qu.x * m_angles.z)) + (qu.y * m_angles.w) + (qu.z * m_angles.x);
    rot.z = (((qu.x * m_angles.y) + (qu.w * m_angles.z)) - (qu.y * m_angles.x)) + (qu.z * m_angles.w);
    
    // Normalize Rotation and Cache
    set_rotate(rot);
}

Vector Frame::globaltolocal(const Vector& point) const
{
    Vector point_offset = point - m_origin;

    float x = (point_offset.x * m00) + (point_offset.y * m01) + (point_offset.z * m02);
    float y = (point_offset.x * m10) + (point_offset.y * m11) + (point_offset.z * m12);
    float z = (point_offset.x * m20) + (point_offset.y * m21) + (point_offset.z * m22);

    return Vector(x, y, z);
}

Vector Frame::globaltolocalvec(const Vector& point) const
{
    float x = (point.x * m00) + (point.y * m01) + (point.z * m02);
    float y = (point.x * m10) + (point.y * m11) + (point.z * m12);
    float z = (point.x * m20) + (point.y * m21) + (point.z * m22);

    return Vector(x, y, z);
}

Vector Frame::localtoglobal(const Vector& point) const
{
    float x = m_origin.x + (point.x * m00) + (point.y * m10) + (point.z * m20);
    float y = m_origin.y + (point.x * m01) + (point.y * m11) + (point.z * m21);
    float z = m_origin.z + (point.x * m02) + (point.y * m12) + (point.z * m22);

    return Vector(x, y, z);
}

Vector Frame::localtoglobalvec(const Vector& point) const
{
    float x = (point.x * m00) + (point.y * m10) + (point.z * m20);
    float y = (point.x * m01) + (point.y * m11) + (point.z * m21);
    float z = (point.x * m02) + (point.y * m12) + (point.z * m22);

    return Vector(x, y, z);
}

Vector Frame::localtolocal(const Frame& frame, const Vector& point) const
{
    Vector global_point = frame.localtoglobal(point);
    return globaltolocal(global_point);
}

void Frame::MakeD3DMatrix(D3DXMATRIX* pOut, Vector& ObjScale)
{
    D3DXMATRIX ObjectMatrix;

    ObjectMatrix._11 = ObjScale.x * m[0][0];
    ObjectMatrix._21 = ObjScale.y * m[1][0];
    ObjectMatrix._31 = ObjScale.z * m[2][0];
    ObjectMatrix._41 = m_origin.x;

    ObjectMatrix._12 = ObjScale.x * m[0][2];
    ObjectMatrix._22 = ObjScale.y * m[1][2];
    ObjectMatrix._32 = ObjScale.z * m[2][2];
    ObjectMatrix._42 = m_origin.z;

    ObjectMatrix._13 = ObjScale.x * m[0][1];
    ObjectMatrix._23 = ObjScale.y * m[1][1];
    ObjectMatrix._33 = ObjScale.z * m[2][1];
    ObjectMatrix._43 = m_origin.y;    

    ObjectMatrix._14 = 0.0f;
    ObjectMatrix._24 = 0.0f;
    ObjectMatrix._34 = 0.0f;
    ObjectMatrix._44 = 1.0f;    

    *pOut = ObjectMatrix;
}

void Frame::cache_quaternion()
{
    float W, X, Y, Z, S, T;
    T = m00 + m11 + m22 + 1.0f;

    if (T > F_EPSILON)
    {
        S = sqrt(T) * 2;
        X = (m12 - m21) / S;
        Y = (m20 - m02) / S;
        Z = (m01 - m10) / S;
        W = 0.25 * S;
    }
    else
    {
        // I'd rather not reverse this.
        // Ripped from http://skal.planet-d.net/demo/matrixfaq.htm#Q55
        // This matrix is a flipped 3x3 from the documentation's 4x4 :P

        if (m00 > m11 && m00 > m22) 
        { // Column 0: 
            S  = sqrt(1.0 + m00 - m11 - m22) * 2;
            X = 0.25 * S;
            Y = (m01 + m10) / S;
            Z = (m20 + m02) / S;
            W = (m12 - m21) / S;
        } else if (m11 > m22)
        { // Column 1: 
            S  = sqrt(1.0 + m11 - m00 - m22) * 2;
            X = (m01 + m10) / S;
            Y = 0.25 * S;
            Z = (m12 + m21) / S;
            W = (m20 - m02) / S;
        }
        else
        { // Column 2:
            S  = sqrt(1.0 + m22 - m00 - m11) * 2;
            X = (m20 + m02) / S;
            Y = (m12 + m21) / S;
            Z = 0.25 * S;
            W = (m01 - m10) / S;
        }
    }

    set_rotate(Quaternion(W, X, Y, Z));
}

void Frame::cache()
{
    m00 = (1 - (m_angles.y * (m_angles.y * 2))) - (m_angles.z * (m_angles.z * 2));
    m01 = (m_angles.x * (m_angles.y * 2)) + (m_angles.w * (m_angles.z * 2));
    m02 = (m_angles.x * (m_angles.z * 2)) - (m_angles.w * (m_angles.y * 2));
    m10 = (m_angles.x * (m_angles.y * 2)) - (m_angles.w * (m_angles.z * 2));
    m11 = (1 - (m_angles.x * (m_angles.x * 2))) - (m_angles.z * (m_angles.z * 2));
    m12 = (m_angles.y * (m_angles.z * 2)) + (m_angles.w * (m_angles.x * 2));
    m20 = (m_angles.x * (m_angles.z * 2)) + (m_angles.w * (m_angles.y * 2));
    m21 = (m_angles.y * (m_angles.z * 2)) - (m_angles.w * (m_angles.x * 2));
    m22 = (1 - (m_angles.x * (m_angles.x * 2))) - (m_angles.y * (m_angles.y * 2));
}

BOOL Frame::is_vector_equal(const Frame& F) const
{
    if (fabs(m_origin.x - F.m_origin.x) >= F_EPSILON)
        return FALSE;
    if (fabs(m_origin.y - F.m_origin.y) >= F_EPSILON)
        return FALSE;
    if (fabs(m_origin.z - F.m_origin.z) >= F_EPSILON)
        return FALSE;

    return TRUE;
}

BOOL Frame::is_quaternion_equal(const Frame& F) const
{
    if (fabs(m_angles.w - F.m_angles.w) >= F_EPSILON)
        return FALSE;
    if (fabs(m_angles.x - F.m_angles.x) >= F_EPSILON)
        return FALSE;
    if (fabs(m_angles.y - F.m_angles.y) >= F_EPSILON)
        return FALSE;
    if (fabs(m_angles.z - F.m_angles.z) >= F_EPSILON)
        return FALSE;

    return TRUE;
}

float Frame::get_heading(void) const
{
    // Untested?
    Vector Heading;
    Heading.x = ((m00 + m20) * 0.0f) + m10;
    Heading.y = ((m01 + m21) * 0.0f) + m11;

    return fmod(450 - D3DXToDegree(atan2(Heading.y, Heading.x)), 360);
}

Position::Position(const Position& Pos)
{
    *this = Pos;
}

Position::Position()
{
    m_LandCell = 0;
}

Position::~Position()
{
}

Position& Position::operator=(const Position& Pos)
{
    m_LandCell = Pos.m_LandCell;
    m_Frame = Pos.m_Frame;

    return *this;
}

ULONG Position::pack_size()
{
    // Actually: sizeof(DWORD) + m_Frame.Pack(.., 0);
    return(sizeof(DWORD) + m_Frame.pack_size());
}

BOOL Position::UnPack(BYTE **ppData, ULONG iSize)
{
    if (iSize < pack_size())
        return FALSE;

    UNPACK(DWORD, m_LandCell);
    
    return UNPACK_OBJ(m_Frame);
}

ULONG Position::origin_pack_size()
{
    // This function was made up.
    return(sizeof(DWORD) + m_Frame.m_origin.pack_size());
}

BOOL Position::UnPackOrigin(BYTE **ppData, ULONG iSize)
{
    if (iSize < origin_pack_size())
        return FALSE;

    UNPACK(DWORD, m_LandCell);
    
    return UNPACK_OBJ(m_Frame);
}

BOOL Position::operator==(const Position& Pos) const
{
    if (m_LandCell != Pos.m_LandCell)
        return FALSE;

    if (!m_Frame.is_vector_equal(Pos.m_Frame))
        return FALSE;

    if (!m_Frame.is_quaternion_equal(Pos.m_Frame))
        return FALSE;

    return TRUE;
}

Vector Position::localtolocal(const Position& Pos, const Vector& Offset) const
{
    Vector cell_offset = Pos.m_Frame.localtoglobal(Offset);

    Vector block_offset =
        LandDefs::get_block_offset(m_LandCell, Pos.m_LandCell);
    
    return m_Frame.globaltolocal(block_offset + cell_offset);
}

Vector Position::globaltolocalvec(const Vector& point) const
{
    return m_Frame.globaltolocalvec(point);
}

Vector Position::localtoglobal(const Position& Pos, const Vector& point) const
{
    Vector cell_offset = Pos.m_Frame.localtoglobal(point);

    Vector block_offset =
        LandDefs::get_block_offset(m_LandCell, Pos.m_LandCell);

    return(block_offset + cell_offset);
}

Vector Position::localtoglobal(const Vector& point) const
{
    // DOES NOT factor in block offsets
    return m_Frame.localtoglobal(point);
}

// Duplicate function name? sub_42A3C0
Vector Position::get_offset(const Position& pos) const
{
    Vector block_offset =
        LandDefs::get_block_offset(m_LandCell, pos.m_LandCell);

    Vector global_offset = block_offset + pos.m_Frame.m_origin - m_Frame.m_origin;

    return global_offset;
}

Vector Position::get_offset(const Position& pos, const Vector& rotation) const
{
    Vector block_offset =
        LandDefs::get_block_offset(m_LandCell, pos.m_LandCell);

    Vector rotated_origin =    pos.localtoglobal(rotation);

    return(block_offset + rotated_origin - m_Frame.m_origin);
}

float Position::distance(const Position& pos) const
{
    return get_offset(pos).magnitude();
}

/*
void Render::CalcObjectMatrix()
{
    D3DXMATRIX ObjectMatrix;

    ObjectMatrix._11 = object_scale_vec.x * FrameCurrent.m[0][0]; // 0x40
    ObjectMatrix._21 = object_scale_vec.y * FrameCurrent.m[1][0]; // 0x4C
    ObjectMatrix._31 = object_scale_vec.z * FrameCurrent.m[2][0]; // 0x58
    ObjectMatrix._41 = FrameCurrent.m_origin.x;// FrameCurrent.m_fScale[0]; // 0x64

    ObjectMatrix._12 = object_scale_vec.x * FrameCurrent.m[0][2];
    ObjectMatrix._22 = object_scale_vec.y * FrameCurrent.m[1][2];
    ObjectMatrix._32 = object_scale_vec.z * FrameCurrent.m[2][2];
    ObjectMatrix._42 = FrameCurrent.m_origin.z;// FrameCurrent.m_fScale[2]; // 0x6C

    ObjectMatrix._13 = object_scale_vec.x * FrameCurrent.m[0][1];
    ObjectMatrix._23 = object_scale_vec.y * FrameCurrent.m[1][1];
    ObjectMatrix._33 = object_scale_vec.z * FrameCurrent.m[2][1];
    ObjectMatrix._43 = FrameCurrent.m_origin.y;// FrameCurrent.m_fScale[1]; // 0x68

    ObjectMatrix._14 = 0.0f;
    ObjectMatrix._24 = 0.0f;
    ObjectMatrix._34 = 0.0f;
    ObjectMatrix._44 = 1.0f;

    SetWorldMatrix(&ObjectMatrix);
}
*/




