﻿// CRightView.cpp: 구현 파일
//

#include "pch.h"
#include "Editor.h"
#include "RenderView.h"
#include "EaterEngineAPI.h"
#include "EditorToolScene.h"
#include "RightOption.h"

// CRightView

IMPLEMENT_DYNCREATE(RenderView, CView)

RenderView::RenderView()
{

}

RenderView::~RenderView()
{
	//EndEngine();
}

BEGIN_MESSAGE_MAP(RenderView, CView)
	ON_WM_SIZE()
	//	ON_WM_GETMINMAXINFO()
	//	ON_WM_SIZING()
	//ON_WM_MOVE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CRightView 그리기

void RenderView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}

// CRightView 진단
#ifdef _DEBUG
void RenderView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void RenderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif
#endif //_DEBUG


void RenderView::Update()
{
	if (GetFocus()->GetSafeHwnd() == hwnd)
	{
		WindowFocus(true);
	}
	else
	{
		WindowFocus(false);
	}

	//if (GetKeyDown(VK_RBUTTON))
	//{
	//	POINT p1, p2;
	//	RECT rc;
	//	GetClientRect(&rc);
	//	p1.x = rc.left;
	//	p1.y = rc.top;
	//	p2.x = rc.right;
	//	p2.y = rc.bottom;
	//
	//	ClientToScreen(&p1);
	//	ClientToScreen(&p2);
	//
	//	rc.left = p1.x;
	//	rc.top = p1.y;
	//	rc.right = p2.x;
	//	rc.bottom = p2.y;
	//	ClipCursor(&rc);
	//}
	//
	//if (GetKey(VK_RBUTTON))
	//{
	//	//중심 좌표를 이동시킨다
	//
	//	int X = MousePosX - GetMousePosX();
	//	int Y = MousePosY - GetMousePosY();
	//
	//	if (X == 0)
	//	{
	//		//DebugPrint("X축 가만히");
	//	}
	//	else
	//	{
	//		DebugPrint("%d || %d", X, Y);
	//		MousePosX = 300;
	//		MousePosY = 300;
	//		SetCursorPos(300, 300);
	//	}
	//}
	//
	//if (GetKeyUp(VK_RBUTTON))
	//{
	//	ClipCursor(nullptr);
	//}

	//POINT p;
	//GetCursorPos(&p);


	//DebugPrint("%d || %d", GetMousePosX(), GetMousePosY());

	UpdateEngine();
}

void RenderView::OnInitialUpdate()
{
	//컨퍼넌트 엔진을 초기화 시키고 씬을 적용시킨다
	hwnd = this->GetSafeHwnd();

	EngineInitialize(hwnd,false);

	CreateScene<EditorToolScene>("Demo");
	ChoiceScene("Demo");
}

void RenderView::OnSize(UINT nType, int cx, int cy)
{
	OnReSize(cx, cy);
	CView::OnSize(nType, cx, cy);
}

void RenderView::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnClose();
}


void RenderView::OnNcDestroy()
{
	CView::OnNcDestroy();
	EndEngine();
}


BOOL RenderView::PreTranslateMessage(MSG* pMsg)
{
	//Picking(LOWORD(lParam), HIWORD(lParam));
	switch (pMsg->wParam)
	{
	case VK_LBUTTON:
	{
		int x = LOWORD(pMsg->lParam);
		int y = HIWORD(pMsg->lParam);
		GameObject* Obj = Picking(x, y);
		if (Obj != nullptr)
		{
			RightOption::GetThis()->OnChoice_Hirearchy_Item(Obj);
		}
	}
	break;

	case VK_RBUTTON:
	{
		
	}
	break;
	}
	//ShowCursor(true);
	return CView::PreTranslateMessage(pMsg);
}
