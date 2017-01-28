
#pragma once

#include "RenderHUD.h"
#include "RenderScene.h"
#include "FPSTracker.h"
#include "MathLib.h"
#include "Frame.h"

class CMaterial;
class CPolygon;
class CSphere;
class CVertexArray;
class CGfxObj;
class Position;
class Frame;
class LightManager;
class CSWVertex;

class CPartCell;
class CObjCell;
class CEnvCell;
class CSortCell;
class CLandCell;
class CLandBlock;

class XFormFrame 
{
public:
    AFrame        m_LocalView;        // 0x00 View relative to Xform position
    Position    m_Position;        // 0x1C Worldspace position
    Vector        m_WorldOrigin;        // 0x64 Block Offset + Frame Offset
    float        m_ObjectScale;        // 0x70 ObjectScale
    Vector        m_ObjectScaleVec;    // 0x74 ObjectScaleVec

    D3DXMATRIX    m_Matrix;            // 0x80

    // 0x168
};

enum PassType
{
    PT_Unknown = 0,
    PT_ZPass = 1,
};

class Render
{
    friend class RenderDevice;

public:
    static BOOL Init(HWND);
    static void Cleanup();

    static void DrawFrame();

    static void GetWindowSize(ULONG *Width, ULONG *Height);
    static RECT& GetRenderRect();

    // Stats related.
    static float GetFPS();
    static DWORD GetFramePolyCount();

    // Device state related.
    static BOOL GetFogState();
    static void SetFogState(BOOL Enabled);
    static void SetFogColor(D3DCOLOR Color);
    static void SetFogStart(float Distance);
    static void SetFogEnd(float Distance);

    static void SetZCompare();
    static void SetZFill();
    static void SetZFunc(D3DCMPFUNC ZFunc);
    static void SetZWrite(BOOL ZWrite);

    static void SetAlphaBlend(BOOL Enabled);
    static void SetSrcBlend(D3DBLEND Blend);
    static void SetDestBlend(D3DBLEND Blend);

    static void SetFilling(D3DFILLMODE Fill);
    static void SetMultiSample(D3DMULTISAMPLE_TYPE MultiSample);
    static void SetCulling(D3DCULL Cull);
    static void SetLighting(BOOL Enabled);

    // Application state related.
    static void SetVertexArray(CVertexArray *pVertexArray);

    static void SetSurface(CPolygon *pPolygon, int iFace);
    static void SetSurface(CSurface *pSurface);
    static void SetSurfaceInternal(CSurface *pSurface);
    static void SetSurfaceArray(CSurface **ppSurfaces);
    static void SetMaterial(CMaterial *pMaterial);

    static void SetWorldMatrix(D3DMATRIX *pMatrix);
    static void SetViewMatrix(D3DMATRIX *pMatrix);
    static void SetProjMatrix(D3DMATRIX *pMatrix);

    static void SetTextColor(COLORREF Color);
    static void SetFillColor(COLORREF Color);

    // Poly list drawing
    static void polyListClear();
    static void polyDraw(CPolygon *Poly);
    static void polyDrawClip(CPolygon *Poly, DWORD flags);
    static void polyListFinishInternal();

    // Land poly drawing
    static void landPolyDraw(CPolygon *Poly);
    static void landPolysDraw(CPolygon **Polys, int PolyCount);

    // Object matrix stack
    static void positionPush(DWORD Ops /*Flags: 1=PUSHframe, 2=UPDATEstack*/, Position *Pos);
    static void positionPop();

    // Debugging functions.
    static void Draw3DPoint(const Vector& Center, float Size);
    static void Draw3DBox(const Vector& Center, const Vector& Dimensions);

    // Model mesh rendering function
    static int DrawMesh(CGfxObj *Obj, Position *ObjPos, BOOL bObjBuilding);

    // Environment cell rendering function.
    static void DrawEnvCell(CEnvCell *pEnvCell);

    // Short-hand primitive drawing call
    static void DrawVBDirect(D3DPRIMITIVETYPE PrimType, DWORD PrimFVF, DWORD PrimCount, DWORD VertexSize, LPDIRECT3DVERTEXBUFFER9 lpVB);
    
    // String drawing mechanisms.
    static int DrawString(long x, long y, const char* text, DWORD format=0);
    static int DrawString(RECT *prect, const char* text, DWORD format=0);
    
    // Direct interface access
    static LPD3DXFONT GetFontDrawer();
    static LPD3DXLINE GetLineDrawer();

    static RECT CreateChildRect(long x, long y, long cx, long cy);

    static D3DXMATRIX*    GetTopMatrix();
    static XFormFrame*    GetFrameCurrent();
    static Frame*        GetViewFrame();

    static int checkplanes(CSphere& sphere, DWORD* flags);
    static BOOL viewconeCheck(CSphere *pSphere);

    static void draw_neg_surf(CPolygon *Poly);
    static void draw_pneg_surf(CPolygon *Poly);
    static void draw_pos_surf(CPolygon *Poly);

    static void DrawTrianglePrim(CSWVertex **pVertices);
    static void DrawQuadPrim(CSWVertex **pVertices);
    static void DrawMultiPrim(CSWVertex **pVertices, DWORD VertexCount);

    static void update_viewpoint(const Frame& frame);
    static void update_viewpoint(const Position& pos);

    static void UpdateBoxSize();
    static BOOL SetFOV(float _fov_degree);
    static void SetFOVInternal(float _fov_radian);

    static void    SetDegradeLevelInternal(float Mult);
    static void SetObjectScale(Vector *pScale);
    static void CalcObjectMatrix();

    static BOOL        GfxObjUnderSelectionRay(CGfxObj *pObj, float *);
    static Vector    pick_ray(long SelX, long SelY);
    static void        clear_selection_cursor(void);
    static void        set_selection_cursor(long SelX, long SelY, BOOL CheckPolys);

    static Position        ViewerPos;
    static Vector        ViewOrigin, Xaxis, Yaxis, Zaxis;
    static Plane        ViewPlane;
    static D3DMATRIX    ViewMatrix;

    static BOOL            m_CheckSelection;
    static BOOL            m_CheckCurrObjectPolys;
    static float        m_CurrDepth;
    static Vector        m_SelectionRay;
    static long            m_SelectionX;
    static long            m_SelectionY;

    static float        m_DegMul;
    static float        particle_distance_2dsq;
    static float        object_distance_2dsq;

    static float        m_FOV;
    static float        m_ZNear;
    static float        m_ZFar;
    static float        m_Scale;
    static float        m_XInvScale;
    static float        m_YInvScale;
    static float        m_TX;
    static float        m_TY;
    static float        m_VDst;
    static float        m_BoxWidth;
    static float        m_BoxHeight;
    static D3DMATRIX    ProjMatrix;

    static Vector        LocalObjectCenter;
    static float        LocalObjectRadius;

    static Vector        ObjectScaleVec;
    static float        ObjectScale;
    static D3DMATRIX    ObjectMatrix;

    static XFormFrame    FrameStack[10];
    static XFormFrame*    FrameCurrent;
    static DWORD        FramePushCount;

    static LightManager *pLightManager;

    static D3DPRESENT_PARAMETERS m_PParams;

	static bool FullBrightOverride;

private:

    static void DrawPoly(CPolygon *Poly);

    static BOOL InitDirect3D();
    static BOOL InitDirect3DDevice(HWND);
    static BOOL InitDirect3DFont();
    static BOOL InitDirect3DLine();
    static void InitFrameStack();

    static void ReleaseDirect3D();
    static void ReleaseDirect3DDevice();

    static void SetupD3DDevice();
    static void UpdateViewpointInternal();

    //
    static FPSTracker    CurrFPS;
    static RECT            CurrRenderRect;

    // Current options.
    static D3DCOLOR        CurrTextColor;
    static D3DCOLOR        CurrFillColor;

    // Current Device states.
    static BOOL            CurrFogState;
    static D3DCOLOR        CurrFogColor;
    static float        CurrFogStart;
    static float        CurrFogEnd;
    static BOOL            CurrAlphaBlend;
    static D3DBLEND        CurrSrcBlend;
    static D3DBLEND        CurrDestBlend;
    static D3DCMPFUNC    CurrZFunc;
    static BOOL            CurrZWrite;
    static D3DFILLMODE    CurrFill;
    static D3DCULL        CurrCull;
    static BOOL            CurrLighting;
    static int            CurrPass;

    // Attributes set by surfaces.
    static D3DCOLORVALUE CurrSurfaceColor;
    static long            CurrAlpha;
    static float        CurrOffsetU;
    static float        CurrOffsetV;
    static float        CurrScaleU;
    static float        CurrScaleV;
    static D3DCOLOR        CurrColor;
    static D3DCOLORVALUE CurrDiffuse;
    static D3DCOLORVALUE CurrLuminosity;
    static D3DCMPFUNC    TransparentZFuncMode;
    static D3DCMPFUNC    NormalZFuncMode;
    static BOOL            NormalZWriteMode;

    // Current Texture -- especially important.
    static LPD3DXMATRIXSTACK MatrixStack;
    static LPDIRECT3DTEXTURE9 CurrD3DTexture;
    static D3DMATERIAL9 DefaultD3DMaterial;

    // Polygon draw list
    static CPolygon*    PolyList[ 1024 ];
    static CPolygon**    PolyNext;
    static DWORD        FramePolyCount;

    // Current application states.
    static CVertexArray* CurrVertexArray;
    static DWORD        CurrVertexType;
    static LPVOID        CurrVertices;
    static DWORD        CurrNumVertices;
    static BYTE*        CurrUVIndices;
    static CSurface**    CurrSurfaceArray;
    static CSurface*    CurrSurface;
    static DWORD        CurrSurfaceType;
    static ImgTex*        CurrTexture;
    static Palette*        CurrPalette;
    static CMaterial*    CurrMaterial;
    static BOOL            CurrMaterialAlphaBlend;

    // Rendering interfaces.
    static LPD3DXFONT    DXFont;
    static LPD3DXLINE    DXLine;

    // Generic Stats Display
    static PrimaryHUD    SharedHUD;
};

class RenderDevice
{
    friend class Render;

public:
    static void UpdateObjCell(CObjCell *pCell);
    static void DrawPartCell(CPartCell *pCell, DWORD ClipPlaneIndex);
    static void DrawBlock(CLandBlock *pLandBlock);
    static void DrawLandCell(CLandCell *pLandCell);
};

#include "DArray.h"
#define MAX_ACTIVE_LIGHTS 256

// Given by CObjCell
class LIGHTOBJ;

class LightManager
{
public:
    LightManager();
    virtual ~LightManager();

    void AddLight(LIGHTOBJ *, Position *pPos);
    void RemoveLight(LIGHTOBJ *);

    struct LightData
    {
        Position*    m_CellPosition;
        LIGHTOBJ*    m_LightObj;
        D3DLIGHT9    m_D3DLight9;
        Frame        m_Composite;
        Vector        m_Origin;
    };

    void DisableAll();
    void EnableAll();
    void EnableAllInCell(DWORD Cell);
    void EnableLight(LightData *);

    void Init();

    DWORD                m_LightCount;
    DArray<LightData *>    m_LightData;

    DWORD                m_EnabledLights;
};

extern LPDIRECT3D9            g_pD3D;
extern LPDIRECT3DDEVICE9    g_pD3DDevice;
