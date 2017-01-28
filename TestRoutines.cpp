
#include "StdAfx.h"
#include "TestRoutines.h"
#include "PartArray.h"
#include "Setup.h"
#include "Render.h"

void TestSurfaceCode()
{
    CSurface *pSurface = CSurface::Get(0x08000BEC);
}

void TestSurfaceScene()
{
    RenderScene::CreateSurfaceScene(0x080017F0);
}

void TestGfxObjScene()
{
    // lockbox: 0x2318
    // BZ: 0x1FD7 0x1FE0 0x1FDE 0x1EC 0x1FE1 0x1FDF 0x1FE2 0x1FDA 0x1FD8 0x1FDC 0x1FDB 0x1FD9 0x1FDD 0x1FE5
    // stool? 0xEB1
    
    RenderScene::CreateGfxObjScene(0x01000EB1);
}

void TestSetupScene()
{
    RenderScene::CreateSetupScene(0x02001255);
}

void TestPhysicsObjScene()
{
    // RenderScene::CreatePhysicsObjScene(0x02001255);
    RenderScene::CreatePhysicsObjScene(0x02000001);
    // RenderScene::CreatePhysicsObjScene(0x0200047B);
}

void TestDungeonScene()
{
    // Baishi meeting hall 0x0120
    // Lost City of Frore 0x0138
    // Xarabydun 0x02C6
    RenderScene::CreateDungeonScene(0x01200000);
}

void TestDungeonMapScene()
{
    // Mines of Dispair 0x0188
    RenderScene::CreateDungeonMapScene(0x01380000);
}

#include "RegionDesc.h"

void TestLandScapeScene()
{
    CRegionDesc::SetRegion(0, TRUE);

    RenderScene::CreateLandScapeScene(0x01010001);
}

void TestTestScene()
{
    RenderScene::CreateTestScene();
}

LIGHTOBJ    TestLight;
LightInfo    TestLightInfo;
Vector        TestLightOrigin;

void TestLightCode()
{
    TestLight.m_LightInfo = &TestLightInfo;
    TestLight.m_Frame.m_origin = TestLightOrigin;
    TestLight.m_Frame.m_angles = Quaternion(1.0, 0.0, 0.0, 0.0);
    TestLight.m_Flags = 0;

    TestLightInfo.m_RGBColor.m_fRed        = 0.0;
    TestLightInfo.m_RGBColor.m_fGreen    = 0.0;
    TestLightInfo.m_RGBColor.m_fBlue    = 0.0;
    TestLightInfo.m_Frame.m_origin        = Vector(0.0, 0.0, 0.0);
    TestLightInfo.m_Frame.m_angles        = Quaternion(1.0, 0.0, 0.0, 0.0);
    TestLightInfo.m_dw00    = 0;
    TestLightInfo.m_50        = 10.0;
    TestLightInfo.m_54        = 5.0;
    TestLightInfo.m_58        = 0;

    DEBUGOUT("Adding test light @ %f %f %f\r\n", TestLight.m_Frame.m_origin.x, TestLight.m_Frame.m_origin.y, TestLight.m_Frame.m_origin.z);
    // Render::pLightManager->AddLight(&TestLight);
}








