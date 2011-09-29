
// I4MercyToolView.h : CI4MercyToolView Ŭ������ �������̽�
//

#pragma once

namespace i4graphics
{
	class I4VertexBuffer;
	class I4IndexBuffer;
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
	I4VertexBuffer*		box_VB;
	I4IndexBuffer*		box_IB;

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
};

#ifndef _DEBUG  // I4MercyToolView.cpp�� ����� ����
inline CI4MercyToolDoc* CI4MercyToolView::GetDocument() const
   { return reinterpret_cast<CI4MercyToolDoc*>(m_pDocument); }
#endif

