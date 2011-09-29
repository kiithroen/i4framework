
// I4MercyToolView.cpp : CI4MercyToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "i4mercytool.h"
#endif

#include "I4MercyToolDoc.h"
#include "I4MercyToolView.h"

#include "I4Log.h"
#include "I4VideoDriver.h"
#include "I4ShaderMgr.h"
#include "I4GeometryBuffer.h"
#include "I4MathUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CI4MercyToolView

IMPLEMENT_DYNCREATE(CI4MercyToolView, CView)

BEGIN_MESSAGE_MAP(CI4MercyToolView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CI4MercyToolView ����/�Ҹ�

CI4MercyToolView::CI4MercyToolView()
	: box_VB(NULL)
	, box_IB(NULL)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CI4MercyToolView::~CI4MercyToolView()
{
	finalize();
}

BOOL CI4MercyToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CI4MercyToolView �׸���

void CI4MercyToolView::OnDraw(CDC* /*pDC*/)
{
	CI4MercyToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}

void CI4MercyToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CI4MercyToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CI4MercyToolView ����

#ifdef _DEBUG
void CI4MercyToolView::AssertValid() const
{
	CView::AssertValid();
}

void CI4MercyToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CI4MercyToolDoc* CI4MercyToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CI4MercyToolDoc)));
	return (CI4MercyToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CI4MercyToolView �޽��� ó����



bool CI4MercyToolView::initialize()
{
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

	I4ShaderMgr::createShaderMgr(I4SHADER_MGR_TYPE_UBER);
	I4ShaderMgr* shaderMgr = I4ShaderMgr::getShaderMgr();
	if (!shaderMgr->initialize(L"render.fx"))
	{
		MessageBox(L"shader manager initialize failed.");
		return false;
	}
	
	const I4Vertex_Pos_Col box[] =
	{
		{-1,  1,  1, 0xffff0000},
		{ 1,  1,  1, 0xff000000},
		{ 1,  1, -1, 0xff0000ff},
		{-1,  1, -1, 0xffffff00},
		{-1, -1,  1, 0xff00ffff},
		{ 1, -1,  1, 0xffff00ff},
		{ 1, -1, -1, 0xff000000},
		{-1, -1, -1, 0xffffffff},
	};

	box_VB = videoDriver->createVertexBuffer();
	if (!box_VB->create(_countof(box), sizeof(I4Vertex_Pos_Col), box))
	{
		MessageBox(L"vertex buffer create failed.");
		return false;
	}

	const static unsigned long indices[] =
	{
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		0, 3, 7, 0, 7, 4,
		1, 5, 6, 1, 6, 2,
		3, 2, 6, 3, 6, 7,
		0, 4, 5, 0, 5, 1,
	};

	box_IB = videoDriver->createIndexBuffer();
	if (!box_IB->create(_countof(indices), sizeof(unsigned short), indices))
	{
		MessageBox(L"index buffer create failed.");
		return false;
	}

	return true;
}

void CI4MercyToolView::finalize()
{
	delete box_IB;
	delete box_VB;

	I4ShaderMgr::destroyShaderMgr();
	I4VideoDriver::destroyVideoDriver();
}

void CI4MercyToolView::onIdle()
{
	I4VideoDriver* videoDriver = I4VideoDriver::getVideoDriver();
	if (videoDriver)
	{
		if (videoDriver->beginScene())
		{
			videoDriver->clearScreen(0, 32, 76);
			I4ShaderMgr* shaderMgr = I4ShaderMgr::getShaderMgr();

			shaderMgr->begin(I4SHADER_MASK_VERTEX_COLOR, I4INPUT_ELEMENTS_POS_COL, _countof(I4INPUT_ELEMENTS_POS_COL));

			I4Matrix4x4 matBox;
			matBox.makeTranslation(0.0f, 0.0f, -3.0f);

			I4Matrix4x4 matProjection;
			matProjection.makePerspectiveFovLH(PI/4.0f, (float)videoDriver->getWidth()/(float)videoDriver->getHeight(), 1.0f, 1000.0f);

			I4Matrix4x4 matView;
			matView.makeCameraLookAtLH(I4Vector3(5.0f, 5.0f, -15.0f), I4Vector3(0.0f, 0.0f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));

			I4Matrix4x4 matWVP = matBox*matView*matProjection;

			shaderMgr->setMatrix(SHADER_MATRIX_WORLD, matBox.arr);
			shaderMgr->setMatrix(SHADER_MATRIX_VIEW, matView.arr);
			shaderMgr->setMatrix(SHADER_MATRIX_PROJECTION, matProjection.arr);

			box_VB->bind();
			box_IB->bind();

			box_IB->draw(I4PT_TRIANGLELIST, box_VB->getCount());

			box_IB->unbind();
			box_VB->unbind();

			shaderMgr->end();

			videoDriver->endScene();
		}
	}

}

void CI4MercyToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	finalize();
	initialize();
}
