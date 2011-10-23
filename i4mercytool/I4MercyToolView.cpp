
// I4MercyToolView.cpp : CI4MercyToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "i4mercytool.h"
#endif

#include "I4MercyToolDoc.h"
#include "I4MercyToolView.h"

#include "I4Log.h"
#include "I4MathUtil.h"
#include "I4VideoDriver.h"
#include "I4ShaderMgr.h"
#include "I4ModelMgr.h"
#include "I4StaticMesh.h"
#include "I4GeometryBuffer.h"
#include "I4RenderTarget.h"
#include "I4Texture.h"
#include "I4QuadMesh.h"
#include "I4SphereMesh.h"
#include "I4StopWatch.h"
#include "I4Camera.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CI4MercyToolView

IMPLEMENT_DYNCREATE(CI4MercyToolView, CView)

BEGIN_MESSAGE_MAP(CI4MercyToolView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CI4MercyToolView 생성/소멸

CI4MercyToolView::CI4MercyToolView()
	: modelMgr(NULL)
	, modelInstance(NULL)
	, box_VB(NULL)
	, box_IB(NULL)
	, rtDiffuse(NULL)
	, rtNormal(NULL)
	, rtDepth(NULL)
	, rtLight(NULL)
	, diffuseMap(NULL)
	, specularMap(NULL)
	, normalMap(NULL)
	, quadMesh(NULL)
	, sphereMesh(NULL)
	, stopWatch(NULL)
	, camera(NULL)
	, camYaw(0)
	, camPitch(0)
	, camRoll(0)
	, frameCount(0)
	, isLButtonDown(false)
	, isMButtonDown(false)
	, isRButtonDown(false)
{
	memset(isKeyDown, 0, sizeof(bool)*256);
}

CI4MercyToolView::~CI4MercyToolView()
{
	finalize();
}

BOOL CI4MercyToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CI4MercyToolView 그리기

void CI4MercyToolView::OnDraw(CDC* /*pDC*/)
{
	CI4MercyToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

void CI4MercyToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	isRButtonDown = false;
	ShowCursor(TRUE);

//	ClientToScreen(&point);
//	OnContextMenu(this, point);
}

void CI4MercyToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CI4MercyToolView 진단

#ifdef _DEBUG
void CI4MercyToolView::AssertValid() const
{
	CView::AssertValid();
}

void CI4MercyToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CI4MercyToolDoc* CI4MercyToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CI4MercyToolDoc)));
	return (CI4MercyToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CI4MercyToolView 메시지 처리기



bool CI4MercyToolView::initialize()
{
	I4StopWatch::initialize();

	RECT rc;
	GetClientRect(&rc);

	unsigned int width = rc.right - rc.left;
	unsigned int height = rc.bottom - rc.top;

	I4VideoDriver::createVideoDriver(I4VIDEO_DRIVER_MODE_D3D10);
	I4VideoDriver* videoDriver = I4VideoDriver::getVideoDriver();
	if (!videoDriver->initialize(GetSafeHwnd(), width, height))
	{
		MessageBox(L"video driver initalize failed.");
		return false;
	}

	if (!videoDriver->setupEnvironment())
	{
		MessageBox(L"setup environment failed.");
		return false;
	}
	
	if (!I4ShaderMgr::addShaderMgr("deffered_g.fx"))
	{
		MessageBox(L"shader manager add failed.");
		return false;
	}
	
	if (!I4ShaderMgr::addShaderMgr("deffered_l_directional.fx"))
	{
		MessageBox(L"shader manager add failed.");
		return false;
	}

	if (!I4ShaderMgr::addShaderMgr("deffered_l_point.fx"))
	{
		MessageBox(L"shader manager add failed.");
		return false;
	}

	if (!I4ShaderMgr::addShaderMgr("deffered_m.fx"))
	{
		MessageBox(L"shader manager add failed.");
		return false;
	}

	const static I4Vertex_Pos_Normal_Tex box[] =
	{
		{ I4Vector3( -1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4TextureUV( 0.0f, 0.0f ) },			// 0
        { I4Vector3( 1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4TextureUV( 1.0f, 0.0f ) },			// 1
        { I4Vector3( 1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4TextureUV( 1.0f, 1.0f ) },			// 2
        { I4Vector3( -1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4TextureUV( 0.0f, 1.0f ) },			// 3

        { I4Vector3( -1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4TextureUV( 0.0f, 0.0f ) },		// 4
        { I4Vector3( 1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4TextureUV( 1.0f, 0.0f ) },		// 5
        { I4Vector3( 1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4TextureUV( 1.0f, 1.0f ) },			// 6
        { I4Vector3( -1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4TextureUV( 0.0f, 1.0f ) },		// 7

        { I4Vector3( -1.0f, -1.0f, 1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ),  I4TextureUV( 0.0f, 0.0f ) },		// 8
        { I4Vector3( -1.0f, -1.0f, -1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ), I4TextureUV( 1.0f, 0.0f ) },		// 9
        { I4Vector3( -1.0f, 1.0f, -1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ),  I4TextureUV( 1.0f, 1.0f ) },		// 10
        { I4Vector3( -1.0f, 1.0f, 1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ),  I4TextureUV( 0.0f, 1.0f ) },			// 11

        { I4Vector3( 1.0f, -1.0f, 1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),   I4TextureUV( 0.0f, 0.0f ) },			// 12
        { I4Vector3( 1.0f, -1.0f, -1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),   I4TextureUV( 1.0f, 0.0f ) },		// 13
        { I4Vector3( 1.0f, 1.0f, -1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),   I4TextureUV( 1.0f, 1.0f ) },			// 14
        { I4Vector3( 1.0f, 1.0f, 1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),	   I4TextureUV( 0.0f, 1.0f ) },			// 15

        { I4Vector3( -1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ), I4TextureUV( 0.0f, 0.0f ) },		// 16
        { I4Vector3( 1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ),  I4TextureUV( 1.0f, 0.0f ) },		// 17
        { I4Vector3( 1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ),  I4TextureUV( 1.0f, 1.0f ) },			// 18
        { I4Vector3( -1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ),  I4TextureUV( 0.0f, 1.0f ) },		// 19

        { I4Vector3( -1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),   I4TextureUV( 0.0f, 0.0f ) },		// 20
        { I4Vector3( 1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),   I4TextureUV( 1.0f, 0.0f ) },			// 21
        { I4Vector3( 1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),	   I4TextureUV( 1.0f, 1.0f ) },			// 22
        { I4Vector3( -1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),   I4TextureUV( 0.0f, 1.0f ) },			// 23
	};

	std::vector<I4Vector3> vecPosition;
	std::vector<I4Vector3> vecNormal;
	std::vector<I4TextureUV> vecTextureUV;

	for (int i = 0; i < _countof(box); ++i)
	{
		vecPosition.push_back(box[i].position);
		vecNormal.push_back(box[i].normal);
		vecTextureUV.push_back(box[i].uv);
	}

	const static I4Index16 indices[] =
	{
		I4Index16(3,1,0),
        I4Index16(2,1,3),

        I4Index16(6,4,5),
        I4Index16(7,4,6),

        I4Index16(11,9,8),
        I4Index16(10,9,11),

        I4Index16(14,12,13),
        I4Index16(15,12,14),

        I4Index16(19,17,16),
        I4Index16(18,17,19),

        I4Index16(22,20,21),
        I4Index16(23,20,22),
	};

	std::vector<I4Index16> vecIndex;
	for (int i = 0; i < _countof(indices); ++i)
	{
		vecIndex.push_back(indices[i]);
	}

	std::vector<I4Vector4> vecTangent;
	calculateTangentArray(vecPosition, vecNormal, vecTextureUV, vecIndex, vecTangent);

	box_VB = videoDriver->createVertexBuffer();
	int size = sizeof(I4Vertex_Pos_Normal_Tex_Tan);
	if (!box_VB->create(vecPosition.size(), size))
	{
		MessageBox(L"vertex buffer create failed.");
		return false;
	}

	I4Vertex_Pos_Normal_Tex_Tan* vertices = NULL;
	box_VB->lock((void**)&vertices);
	for (unsigned int i = 0; i < vecPosition.size(); ++i)
	{
		vertices[i].position = vecPosition[i];
		vertices[i].normal = vecNormal[i];
		vertices[i].uv = vecTextureUV[i];
		vertices[i].tangent = vecTangent[i];
	}
	box_VB->unlock();

	box_IB = videoDriver->createIndexBuffer();
	if (!box_IB->create(vecIndex.size()*3, sizeof(unsigned short), (void*)&vecIndex[0]))
	{
		MessageBox(L"index buffer create failed.");
		return false;
	}

	quadMesh = new I4QuadMesh;
	quadMesh->create();

	sphereMesh = new I4SphereMesh;
	sphereMesh->create();

	rtDiffuse = videoDriver->createRenderTarget();
	if (!rtDiffuse->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM))
	{
		MessageBox(L"render target create failed.");
		return false;
	}

	rtNormal = videoDriver->createRenderTarget();
	if (!rtNormal->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM))
	{
		MessageBox(L"render target create failed.");
		return false;
	}

	rtDepth = videoDriver->createRenderTarget();
	if (!rtDepth->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R32_FLOAT))
	{
		MessageBox(L"render target create failed.");
		return false;
	}

	rtLight = videoDriver->createRenderTarget();
	if (!rtLight->create(videoDriver->getWidth(), videoDriver->getHeight(), I4FORMAT_R8G8B8A8_UNORM))
	{
		MessageBox(L"render target create failed.");
		return false;
	}

	diffuseMap = videoDriver->createTexture();
	if (!diffuseMap->load("ground_diffuse.jpg"))
	{
		MessageBox(L"diffuse map create failed.");
		return false;
	}

	specularMap = videoDriver->createTexture();
	if (!specularMap->load("ground_specular.jpg"))
	{
		MessageBox(L"specular map create failed.");
		return false;
	}

	normalMap = videoDriver->createTexture();
	if (!normalMap->load("ground_normal.jpg"))
	{
		MessageBox(L"normal map create failed.");
		return false;
	}

	modelMgr = new I4ModelMgr;
	modelInstance = modelMgr->createInstance("test.mesh.xml", "test_1");

	stopWatch = new I4StopWatch;
	stopWatch->reset();

	camera = new I4Camera;
	camera->setPerspectiveFov(mathutil::PI/4.0f, (float)videoDriver->getWidth()/(float)videoDriver->getHeight(), 1.0f, 1000.0f);
	camera->setLookAt(I4Vector3(8.0f, 2.0f, -20.0f), I4Vector3(-2.0f, 0.0f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));
	
	float camYawRad;
	float camPitchRad;
	float camRollRad;
	camera->getRotation().extractYawPitchRoll(camYawRad, camPitchRad, camRollRad);

	camYaw = mathutil::radianToDegree(camYawRad);
	camPitch = mathutil::radianToDegree(camPitchRad);
	camRoll = mathutil::radianToDegree(camRollRad);

	return true;
}

void CI4MercyToolView::finalize()
{
	modelMgr->destroyInstance(modelInstance);

	delete camera;
	delete modelMgr;
	delete stopWatch;
	delete sphereMesh;
	delete quadMesh;
	delete normalMap;
	delete specularMap;
	delete diffuseMap;
	delete rtLight;
	delete rtDepth;
	delete rtNormal;
	delete rtDiffuse;
	delete box_IB;
	delete box_VB;

	I4ShaderMgr::destroyShaderMgr();
	I4VideoDriver::destroyVideoDriver();
}

void CI4MercyToolView::onIdle()
{
	++frameCount;

	static float elapsed = 0;

	float frameDeltaTime = stopWatch->getElapsedTime();	
	stopWatch->reset();

	elapsed += frameDeltaTime;

	if (elapsed >= 1.0f)
	{
		float fps = (float)frameCount/elapsed;
		I4LOG_INFO << L"fps = " << fps;

		elapsed = 0;
		frameCount = 0;
	}

	float camMoveSpeed = 100.0f*frameDeltaTime;
	I4Vector3 newCamEye = camera->getEye();
	I4Quaternion newCamRotation = camera->getRotation();

	if (isKeyDown['w'] || isKeyDown['W'])
	{
		newCamEye += camera->getDirection()*camMoveSpeed;
	}

	if (isKeyDown['s'] || isKeyDown['S'])
	{
		newCamEye -= camera->getDirection()*camMoveSpeed;
	}

	if (isKeyDown['a'] || isKeyDown['A'])
	{
		newCamEye -= camera->getRight()*camMoveSpeed;
	}

	if (isKeyDown['d'] || isKeyDown['D'])
	{
		newCamEye += camera->getRight()*camMoveSpeed;
	}

	
	if (isRButtonDown)
	{
		POINT pt;
		GetCursorPos(&pt);

		int curMouseX = pt.x;
		int curMouseY = pt.y;

		int dx = curMouseX - prevMouseX;
		int dy = curMouseY - prevMouseY;

		const float CAMERA_SENSITIVE = 0.35f;
		camYaw += dx*CAMERA_SENSITIVE;
		camPitch += dy*CAMERA_SENSITIVE;

		if (camYaw > 360)
		{
			camYaw -= 360;
		}

		if (camPitch < -90)
		{
			camPitch = -90;
		}
		
		if (camPitch > 90)
		{
			camPitch = 90;
		}

		RECT rc;	
		GetClientRect(&rc);
		pt.x = (rc.right - rc.left)/2;
		pt.y = (rc.bottom - rc.top)/2;	
		ClientToScreen(&pt);
		SetCursorPos(pt.x, pt.y);
		prevMouseX = pt.x;
		prevMouseY = pt.y;

		newCamRotation.makeRotationYawPitchRoll(mathutil::degreeToRadian(camYaw), mathutil::degreeToRadian(camPitch), mathutil::degreeToRadian(camRoll));
	}

	camera->setTransform(newCamRotation, newCamEye);

	I4VideoDriver* videoDriver = I4VideoDriver::getVideoDriver();
	if (videoDriver)
	{
		if (videoDriver->beginScene())
		{
			I4ShaderMgr* shaderMgr = NULL;

			videoDriver->clearScreen(0, 32, 76);
			
			videoDriver->setViewport(0, 0, videoDriver->getWidth(), videoDriver->getHeight());

			// -------------------------------------------------------------------------------			
			videoDriver->clearRenderTarget(rtDiffuse, 0.0f, 0.0f, 0.0f, 0.0f);
			videoDriver->clearRenderTarget(rtNormal, 0.5f, 0.5f, 0.5f, 0.0f);
			videoDriver->clearRenderTarget(rtDepth, 0.0f, 0.0f, 0.0f, 0.0f);
			videoDriver->clearRenderTarget(rtLight, 0.0f, 0.0f, 0.0f, 0.0f);
			
			// -------------------------------------------------------------------------------
			shaderMgr = I4ShaderMgr::findShaderMgr("deffered_g.fx");
			shaderMgr->begin(I4SHADER_MASK_DIFFUSEMAP|I4SHADER_MASK_SPECULARMAP|I4SHADER_MASK_NORMALMAP, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));

			I4RenderTarget*	renderTargetG[] = { rtDiffuse, rtNormal, rtDepth };
			videoDriver->setRenderTarget(_countof(renderTargetG), renderTargetG, true);
			
			shaderMgr->setMatrix(I4SHADER_MATRIX_PROJECTION, camera->getProjectionMatrix().arr);
			shaderMgr->setMatrix(I4SHADER_MATRIX_VIEW, camera->getViewMatrix().arr);
			shaderMgr->setFloat(I4SHADER_FLOAT_FAR_DISTANCE, camera->getZFar());
			shaderMgr->apply();

			box_VB->bind();
			box_IB->bind();
			
			static float boxAng = 0;
			boxAng += 0.25f;
			if (boxAng > 360)
			{
				boxAng = 0;
			}

			I4Matrix4x4 matBoxRot;
			matBoxRot.makeRotationY(mathutil::degreeToRadian(boxAng));
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_INTENSITY, 0.1f);
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_POWER, 2);
			shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, matBoxRot.arr);
			shaderMgr->setTexture(I4SHADER_TEXTURE_DIFFUSEMAP, diffuseMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_SPECULARMAP, specularMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_NORMALMAP, normalMap);
			shaderMgr->apply();
			box_IB->draw(I4PT_TRIANGLELIST);

			I4Matrix4x4 matBox;
			matBox.makeTranslation(0.0f, 5.0f, -3.0f);
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_INTENSITY, 0.3f);
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_POWER, 4);
			shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, matBox.arr);
			shaderMgr->setTexture(I4SHADER_TEXTURE_DIFFUSEMAP, diffuseMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_SPECULARMAP, specularMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_NORMALMAP, normalMap);

			shaderMgr->apply();
			box_IB->draw(I4PT_TRIANGLELIST);

			matBox.makeTranslation(-3.0f, 0.0f, -7.0f);
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_INTENSITY, 0.5f);
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_POWER, 16);
			shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, matBox.arr);
			shaderMgr->setTexture(I4SHADER_TEXTURE_DIFFUSEMAP, diffuseMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_SPECULARMAP, specularMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_NORMALMAP, normalMap);

			shaderMgr->apply();
			box_IB->draw(I4PT_TRIANGLELIST);

			matBox.makeTranslation(5.0f, 0.0f, -2.0f);
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_INTENSITY, 0.3f);
			shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_POWER, 32);
			shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, matBox.arr);
			shaderMgr->setTexture(I4SHADER_TEXTURE_DIFFUSEMAP, diffuseMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_SPECULARMAP, specularMap);
			shaderMgr->setTexture(I4SHADER_TEXTURE_NORMALMAP, normalMap);

			shaderMgr->apply();
			box_IB->draw(I4PT_TRIANGLELIST);

			box_IB->unbind();
			box_VB->unbind();

			shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX_TAN));
			shaderMgr->setMatrix(I4SHADER_MATRIX_PROJECTION, camera->getProjectionMatrix().arr);
			shaderMgr->setMatrix(I4SHADER_MATRIX_VIEW, camera->getViewMatrix().arr);
			shaderMgr->setFloat(I4SHADER_FLOAT_FAR_DISTANCE, camera->getZFar());
			shaderMgr->apply();

			I4Matrix4x4 matModel;
			matModel.makeTranslation(-2.0f, -3.0f, -3.0f);

			I4Matrix4x4 matScale;
			matScale.makeScale(0.15f, 0.15f, 0.15f);

			modelInstance->setModelTM(matScale*matModel);
			for (unsigned int i = 0; i < modelInstance->getSubCount(); ++i)
			{
				I4MeshInstance& meshInstance = modelInstance->getSubMeshInstance(i);
				I4StaticMesh* mesh = modelMgr->findMesh(meshInstance.meshID);
				mesh->bind();
				
				I4Texture* diffuse = modelMgr->findTexture(meshInstance.diffuseMapID);
				I4Texture* specular = modelMgr->findTexture(meshInstance.specularMapID);
				I4Texture* normal = modelMgr->findTexture(meshInstance.normalMapID);

				I4Matrix4x4::multiply(meshInstance.resultTM, meshInstance.meshLocalTM, modelInstance->getModelTM());
				shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, meshInstance.resultTM.arr);
				shaderMgr->setTexture(I4SHADER_TEXTURE_DIFFUSEMAP, diffuse);
				shaderMgr->setTexture(I4SHADER_TEXTURE_SPECULARMAP, specular);
				shaderMgr->setTexture(I4SHADER_TEXTURE_NORMALMAP, normal);

				shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_INTENSITY, meshInstance.specularInensity);
				shaderMgr->setFloat(I4SHADER_FLOAT_SPECULAR_POWER, meshInstance.specularPower);

				shaderMgr->apply();
				mesh->draw();

				mesh->unbind();
			}

			shaderMgr->end();
			

			// -------------------------------------------------------------------------------
			videoDriver->setRenderTarget(1, &rtLight, false);
			{
				shaderMgr = I4ShaderMgr::findShaderMgr("deffered_l_directional.fx");
				shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));			

				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, rtNormal);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, rtDepth);
				
				shaderMgr->setVector(I4SHADER_VECTOR_FAR_TOP_RIGHT, camera->getFarTopRight().xyz);
				shaderMgr->apply();

				I4Vector3 lightDirection[] =
				{					
					I4Vector3(-1.0f, -1.0f, 0.0f),
					I4Vector3(1.0f, 0.5f, 0.5f),
				};

				I4Vector3 lightColor[] =
				{										
					I4Vector3(1.0f, 1.0f, 1.0f),
					I4Vector3(0.6f, 0.725f, 1.0f)*0.9f,
				};

				quadMesh->bind();

				I4Matrix4x4 matLight;
				for (int i = 0; i < 2; ++i)
				{
					if (i == 0)
					{						
						matLight.makeIdentity();
					}
					else
					{
						static float lightAng = 0;
						lightAng += 0.2f;
						if (lightAng > 360)
						{
							lightAng -= 360;
						}

						matLight.makeRotationY(mathutil::degreeToRadian(lightAng));
					}

					I4Matrix4x4 matLightView;
					I4Matrix4x4::multiply(matLightView, matLight, camera->getViewMatrix());
					I4Vector3 lightDir = matLightView.transformVector(lightDirection[i]);
					shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_DIRECTION, lightDir.xyz);
					shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_COLOR, lightColor[i].xyz);			

					shaderMgr->apply();
					quadMesh->draw();
				}
				quadMesh->unbind();

				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, NULL);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, NULL);
				shaderMgr->apply();

				shaderMgr->end();
			}

			{
				shaderMgr = I4ShaderMgr::findShaderMgr("deffered_l_point.fx");
				shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS, _countof(I4INPUT_ELEMENTS_POS));

				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, rtNormal);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, rtDepth);
		
				shaderMgr->setMatrix(I4SHADER_MATRIX_PROJECTION, camera->getProjectionMatrix().arr);
				shaderMgr->setMatrix(I4SHADER_MATRIX_VIEW, camera->getViewMatrix().arr);
				shaderMgr->setVector(I4SHADER_VECTOR_FAR_TOP_RIGHT, camera->getFarTopRight().xyz);
				shaderMgr->apply();

				I4Vector4 lightPointRadius[] =
				{
					I4Vector4(0.0f, 3.0f, -2.0f, 7.0f),
					I4Vector4(-1.0f, 3.0f, -5.0f, 7.0f),
					I4Vector4(3.5f, 6.0f, -4.5f, 7.0f),
					I4Vector4(1.0f, 5.5f, -7.0f, 7.0),
					I4Vector4(2.0f, 3.0f, -4.5f, 7.0),
				};

				I4Vector3 lightColor[] =
				{
					I4Vector3(1.0f, 0.125f, 0.93f),
					I4Vector3(1.0f, 0.0f, 0.0f),
					I4Vector3(0.0f, 0.8f, 0.8f),
					I4Vector3(1.0f, 1.0f, 0.0f),
					I4Vector3(1.0f, 0.5f, 0.25f),
				};

				sphereMesh->bind();
				for (int i = 0; i < 5; ++i)
				{
					I4Matrix4x4 matScale;
					matScale.makeScale(lightPointRadius[i].w, lightPointRadius[i].w, lightPointRadius[i].w);

					
					I4Matrix4x4 matRot;
					if (i%2 ==  0)
					{
						static float lightAng = 0;
						lightAng += 0.1f;
						matRot.makeRotationAxis(I4Vector3(0.0f, 0.2f, 1.0f), mathutil::degreeToRadian(lightAng));
						if (lightAng > 360)
						{
							lightAng -= 360;
						}
					}
					else
					{
						static float lightAng = 0;
						lightAng += 0.2f;
						matRot.makeRotationAxis(I4Vector3(1.0f, 0.5f, 0.5f), mathutil::degreeToRadian(-lightAng));
						if (lightAng > 360)
						{
							lightAng -= 360;
						}
					}

					I4Matrix4x4 matTrans;
					matTrans.makeTranslation(lightPointRadius[i].x, lightPointRadius[i].y, lightPointRadius[i].z);

					I4Matrix4x4 matLight = matScale*matTrans*matRot;	

					I4Vector3 lightWorldPos;
					matLight.extractTranslation(lightWorldPos);

					if (camera->isVisibleSphere(lightWorldPos, lightPointRadius[i].w) == false)
						continue;

					shaderMgr->setMatrix(I4SHADER_MATRIX_WORLD, matLight.arr);
					
					I4Matrix4x4 matLightView = camera->getViewMatrix();
					I4Vector3 lightViewPos = matLightView.transformCoord(lightWorldPos);
					I4Vector4 light;
					light.x = lightViewPos.x;
					light.y = lightViewPos.y;
					light.z = lightViewPos.z;
					light.w = lightPointRadius[i].w;

					
					shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_POINT_RADIUS, light.xyzw);
					shaderMgr->setVector(I4SHADER_VECTOR_LIGHT_COLOR, lightColor[i].xyz);			

					shaderMgr->apply();
					sphereMesh->draw();
				}
				sphereMesh->unbind();

				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_NORMAL, NULL);
				shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DEPTH, NULL);
				shaderMgr->apply();

				shaderMgr->end();
			}
			
			// -------------------------------------------------------------------------------
			shaderMgr = I4ShaderMgr::findShaderMgr("deffered_m.fx");
			shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_TEX, _countof(I4INPUT_ELEMENTS_POS_TEX));

			videoDriver->resetRenderTarget();

			shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, rtDiffuse);
			shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_LIGHT, rtLight);
			
			shaderMgr->apply();
			quadMesh->bind();
			quadMesh->draw();
			quadMesh->unbind();

			shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_DIFFUSE, NULL);
			shaderMgr->setRenderTarget(I4SHADER_RENDER_TARGET_LIGHT, NULL);
			shaderMgr->apply();
			
			videoDriver->endScene();
		}
	}
}

void CI4MercyToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	initialize();
}


void CI4MercyToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	isLButtonDown = true;

	CView::OnLButtonDown(nFlags, point);
}


void CI4MercyToolView::OnMButtonDown(UINT nFlags, CPoint point)
{
	isMButtonDown = true;

	CView::OnMButtonDown(nFlags, point);
}


void CI4MercyToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	isRButtonDown = true;
	/*
	POINT pt;
	RECT rc;	
	GetClientRect(&rc);
	pt.x = (rc.right - rc.left)/2;
	pt.y = (rc.bottom - rc.top)/2;	
	ClientToScreen(&pt);
	SetCursorPos(pt.x, pt.y);
	*/
	ClientToScreen(&point);
	prevMouseX = point.x;
	prevMouseY = point.y;
	
	ShowCursor(FALSE);

	CView::OnRButtonDown(nFlags, point);
}


BOOL CI4MercyToolView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	I4Vector3 newEye = camera->getEye() + camera->getDirection()*zDelta*0.01F;
	camera->setTransform(camera->getRotation(), newEye);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CI4MercyToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);
}


void CI4MercyToolView::OnMButtonUp(UINT nFlags, CPoint point)
{
	isLButtonDown = false;

	CView::OnMButtonUp(nFlags, point);
}


void CI4MercyToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	isKeyDown[nChar] = true;

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CI4MercyToolView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	isKeyDown[nChar] = false;

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CI4MercyToolView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonUp(nFlags, point);
}
