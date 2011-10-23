
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
#include "I4DefferedRenderer.h"

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
	: stopWatch(NULL)
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

	renderer = new I4DefferedRenderer;
	if (renderer->initialize(GetSafeHwnd(), width, height) == false)
	{
		MessageBox(L"renderer initalize failed.");
		return false;
	}
	
	modelInstance = renderer->createModelInstance("test.mesh.xml", "test_1");

	stopWatch = new I4StopWatch;
	stopWatch->reset();

	camera = new I4Camera;
	camera->setPerspectiveFov(mathutil::PI/4.0f, (float)width/(float)height, 1.0f, 1000.0f);
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
	delete renderer;
	delete camera;
	delete stopWatch;
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

	// update
	I4Matrix4x4 matModel;
	I4Matrix4x4 matScale;

	matModel.makeTranslation(0.0f, -3.0f, 0.0f);
	matScale.makeScale(0.15f, 0.15f, 0.15f);
	modelInstance->setModelTM(matScale*matModel);
	renderer->commitModelInstance(modelInstance);

	matModel.makeTranslation(-10.0f, -3.0f, 0.0f);
	matScale.makeScale(0.15f, 0.15f, 0.15f);
	modelInstance->setModelTM(matScale*matModel);
	renderer->commitModelInstance(modelInstance);

	matModel.makeTranslation(0.0f, -3.0f, -10.0f);
	matScale.makeScale(0.15f, 0.15f, 0.15f);
	modelInstance->setModelTM(matScale*matModel);
	renderer->commitModelInstance(modelInstance);

	// render
	renderer->preRender(camera);
	renderer->render(camera);
	renderer->postRender(camera);
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
