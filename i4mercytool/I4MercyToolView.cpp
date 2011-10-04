
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
#include "I4MathUtil.h"
#include "I4VideoDriver.h"
#include "I4ShaderMgr.h"
#include "I4GeometryBuffer.h"
#include "I4RenderTarget.h"
#include "I4Texture.h"

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

	if (!I4ShaderMgr::addShaderMgr(L"deffered_render_g.fx"))
	{
		MessageBox(L"shader manager add failed.");
		return false;
	}
	
	const I4Vertex_Pos_Normal_Tex box[] =
	{
		{ I4Vector3( -1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4Vector2( 0.0f, 0.0f ) },
        { I4Vector3( 1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4Vector2( 1.0f, 0.0f ) },
        { I4Vector3( 1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4Vector2( 1.0f, 1.0f ) },
        { I4Vector3( -1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 1.0f, 0.0f ), I4Vector2( 0.0f, 1.0f ) },

        { I4Vector3( -1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4Vector2( 0.0f, 0.0f ) },
        { I4Vector3( 1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4Vector2( 1.0f, 0.0f ) },
        { I4Vector3( 1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4Vector2( 1.0f, 1.0f ) },
        { I4Vector3( -1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, -1.0f, 0.0f ),  I4Vector2( 0.0f, 1.0f ) },

        { I4Vector3( -1.0f, -1.0f, 1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ),  I4Vector2( 0.0f, 0.0f ) },
        { I4Vector3( -1.0f, -1.0f, -1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ), I4Vector2( 1.0f, 0.0f ) },
        { I4Vector3( -1.0f, 1.0f, -1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ),  I4Vector2( 1.0f, 1.0f ) },
        { I4Vector3( -1.0f, 1.0f, 1.0f ), I4Vector3( -1.0f, 0.0f, 0.0f ),  I4Vector2( 0.0f, 1.0f ) },

        { I4Vector3( 1.0f, -1.0f, 1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),   I4Vector2( 0.0f, 0.0f ) },
        { I4Vector3( 1.0f, -1.0f, -1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),   I4Vector2( 1.0f, 0.0f ) },
        { I4Vector3( 1.0f, 1.0f, -1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),   I4Vector2( 1.0f, 1.0f ) },
        { I4Vector3( 1.0f, 1.0f, 1.0f ), I4Vector3( 1.0f, 0.0f, 0.0f ),	   I4Vector2( 0.0f, 1.0f ) },

        { I4Vector3( -1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ), I4Vector2( 0.0f, 0.0f ) },
        { I4Vector3( 1.0f, -1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ),  I4Vector2( 1.0f, 0.0f ) },
        { I4Vector3( 1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ),  I4Vector2( 1.0f, 1.0f ) },
        { I4Vector3( -1.0f, 1.0f, -1.0f ), I4Vector3( 0.0f, 0.0f, -1.0f ),  I4Vector2( 0.0f, 1.0f ) },

        { I4Vector3( -1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),   I4Vector2( 0.0f, 0.0f ) },
        { I4Vector3( 1.0f, -1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),   I4Vector2( 1.0f, 0.0f ) },
        { I4Vector3( 1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),	   I4Vector2( 1.0f, 1.0f ) },
        { I4Vector3( -1.0f, 1.0f, 1.0f ), I4Vector3( 0.0f, 0.0f, 1.0f ),   I4Vector2( 0.0f, 1.0f ) },
	};

	box_VB = videoDriver->createVertexBuffer();
	if (!box_VB->create(_countof(box), sizeof(I4Vertex_Pos_Normal_Tex), box))
	{
		MessageBox(L"vertex buffer create failed.");
		return false;
	}

	const static unsigned long indices[] =
	{
		3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
	};

	box_IB = videoDriver->createIndexBuffer();
	if (!box_IB->create(_countof(indices), sizeof(unsigned short), indices))
	{
		MessageBox(L"index buffer create failed.");
		return false;
	}

	renderTarget[0] = videoDriver->createRenderTarget();
	if (!renderTarget[0]->create(800, 600, I4FORMAT_R8G8B8A8_UNORM))
	{
		MessageBox(L"render target create failed.");
		return false;
	}

	renderTarget[1] = videoDriver->createRenderTarget();
	if (!renderTarget[1]->create(800, 600, I4FORMAT_R8G8B8A8_UNORM))
	{
		MessageBox(L"render target create failed.");
		return false;
	}

	renderTarget[2] = videoDriver->createRenderTarget();
	if (!renderTarget[2]->create(800, 600, I4FORMAT_R32_FLOAT))
	{
		MessageBox(L"render target create failed.");
		return false;
	}

	diffuseMap = videoDriver->createTexture();
	if (!diffuseMap->load("seafloor.dds"))
	{
		MessageBox(L"diffuse map create failed.");
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

			unsigned int numRT = _countof(renderTarget);
			for (unsigned int i = 0; i < numRT; ++i)
			{
				videoDriver->clearRenderTarget(renderTarget[i], 255, 0, 255);
			}

			videoDriver->setViewport(0, 0, 800, 600);
			videoDriver->setRenderTarget(numRT, renderTarget);

			I4ShaderMgr* shaderMgr = I4ShaderMgr::findShaderMgr(L"deffered_render_g.fx");

			shaderMgr->begin(I4SHADER_MASK_NONE, I4INPUT_ELEMENTS_POS_NORMAL_TEX, _countof(I4INPUT_ELEMENTS_POS_NORMAL_TEX));

			I4Matrix4x4 matBox;
			matBox.makeTranslation(0.0f, 0.0f, -3.0f);

			I4Matrix4x4 matProjection;
			matProjection.makePerspectiveFovLH(PI/4.0f, (float)videoDriver->getWidth()/(float)videoDriver->getHeight(), 1.0f, 1000.0f);

			I4Matrix4x4 matView;
			matView.makeCameraLookAtLH(I4Vector3(3.0f, 5.0f, -15.0f), I4Vector3(0.0f, 0.0f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));

			I4Matrix4x4 matWVP = matBox*matView*matProjection;

			shaderMgr->setMatrix(SHADER_MATRIX_WORLD, matBox.arr);
			shaderMgr->setMatrix(SHADER_MATRIX_VIEW, matView.arr);
			shaderMgr->setMatrix(SHADER_MATRIX_PROJECTION, matProjection.arr);
			shaderMgr->setTexture(SHADER_TEXTURE_DIFFUSEMAP, diffuseMap);

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
