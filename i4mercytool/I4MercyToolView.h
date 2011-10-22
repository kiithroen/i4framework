
// I4MercyToolView.h : CI4MercyToolView Ŭ������ �������̽�
//

#pragma once

namespace i4core
{
	class I4StopWatch;
	class I4Camera;
}
using namespace i4core;

namespace i4graphics
{
	class I4ModelMgr;
	class I4ModelInstance;
	class I4VertexBuffer;
	class I4IndexBuffer;
	class I4RenderTarget;
	class I4Texture;
	class I4QuadMesh;
	class I4SphereMesh;
}

using namespace i4graphics;

class CI4MercyToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CI4MercyToolView();
	DECLARE_DYNCREATE(CI4MercyToolView)

// Ư���Դϴ�.
public:
	CI4MercyToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	bool	initialize();
	void	finalize();
	void	onIdle();

private:
	I4ModelMgr*			modelMgr;
	I4ModelInstance*	modelInstance;
	I4VertexBuffer*		box_VB;
	I4IndexBuffer*		box_IB;
	I4RenderTarget*		rtDiffuse;
	I4RenderTarget*		rtNormal;
	I4RenderTarget*		rtDepth;
	I4RenderTarget*		rtLight;
	I4Texture*			diffuseMap;
	I4Texture*			specularMap;
	I4Texture*			normalMap;
	I4QuadMesh*			quadMesh;
	I4SphereMesh*		sphereMesh;
	I4StopWatch*		stopWatch;
	I4Camera*			camera;
	float				camYaw;
	float				camPitch;
	float				camRoll;

	unsigned int		frameCount;	
	
	int					prevMouseX;
	int					prevMouseY;

	bool				isLButtonDown;
	bool				isRButtonDown;
	bool				isMButtonDown;

	bool				isKeyDown[256];

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CI4MercyToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // I4MercyToolView.cpp�� ����� ����
inline CI4MercyToolDoc* CI4MercyToolView::GetDocument() const
   { return reinterpret_cast<CI4MercyToolDoc*>(m_pDocument); }
#endif

