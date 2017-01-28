
#pragma once
#include "MathLib.h"

class AFrame
{
public:
    AFrame();

    BOOL UnPack(BYTE** ppData, ULONG iSize);
    BOOL IsValid();

    Vector        m_origin;
    Quaternion    m_angles;
};

class Frame : public AFrame
{
public:
    Frame();

    ULONG pack_size();
    ULONG Pack(BYTE **ppData, ULONG iSize);
    BOOL UnPack(BYTE** ppData, ULONG iSize);
    BOOL IsValid();

    BOOL is_vector_equal(const Frame& F) const;
    BOOL is_quaternion_equal(const Frame& F) const;

    void cache();
    void cache_quaternion();

    void combine(const Frame* a, const AFrame* b);
    void combine(const Frame* a, const AFrame* b, const Vector* scale);
    void rotate(const Vector& angles);
    void grotate(Vector& angles);
    void set_rotate(Quaternion angles);
    void euler_set_rotate(Vector Angles, int order);
    Vector globaltolocal(const Vector& point) const;
    Vector globaltolocalvec(const Vector& point) const;
    Vector localtoglobal(const Vector& point) const;
    Vector localtoglobalvec(const Vector& point) const;
    Vector localtolocal(const Frame& frame, const Vector& point) const;
    void subtract1(const Frame* a, const AFrame* b);

    void rotate_around_axis_to_vector(int axis, const Vector& target);
    void set_heading(float DegreeHeading);
    void set_vector_heading(const Vector& Heading);

    float get_heading(void) const;

    // Custom
    void MakeD3DMatrix(D3DXMATRIX* pOut, Vector& ObjScale);

    // Inherited:
    // Vector        m_origin;
    // Quaternion    m_angles;

    union {
        float m[3][3];

        struct
        {
            float m00, m01, m02; // 0x1C, 0x20, 0x24 0, 1, 2 | 0, 1, 2  | 0, 4, 8
            float m10, m11, m12; // 0x28, 0x2C, 0x30 3, 4, 5 | 4, 5, 6  | 1, 5, 9
            float m20, m21, m22; // 0x34, 0x38, 0x3C 6, 7, 8 | 8, 9, 10 | 2, 6, 10
        };

        struct
        {
            float _11, _12, _13;
            float _21, _22, _23;
            float _31, _32, _33;
        };
    };
};

class Position : public PackObj
{
public:
    Position(const Position& Pos);
    Position();
    ~Position();

    ULONG        pack_size();
    BOOL        UnPack(BYTE **ppData, ULONG iSize);

    ULONG        origin_pack_size();
    BOOL        UnPackOrigin(BYTE **ppData, ULONG iSize);

    Position&    operator=(const Position& Pos);
    BOOL        operator==(const Position& Pos) const;

    Vector        localtolocal(const Position& Pos, const Vector& Offset) const;
    Vector        globaltolocalvec(const Vector& point) const;

    Vector        localtoglobal(const Vector& point) const;
    Vector        localtoglobal(const Position& Pos, const Vector& point) const;

    Vector        get_offset(const Position& pos) const;
    Vector        get_offset(const Position& pos, const Vector& rotation) const;
    float        distance(const Position& pos) const;

    DWORD    m_LandCell;
    Frame    m_Frame;
};







