﻿// RightOption.cpp: 구현 파일

#include "pch.h"
#include "Editor.h"
#include "RightOption.h"
#include "afxdialogex.h"
#include "EditorToolScene.h"

#include "CTAP_Collider.h"
#include "CTAP_Rigidbody.h"
#include "CTAP_Transform.h"
#include "CTAP_Animation.h"
#include "CTAP_MeshFilter.h"
#include "CTAP_Light.h"
#include "CTAP_Particle.h"

#include "FileOption.h"
#include "EditorManager.h"
#include "Loading.h"

#include <stack>
#include "EaterEngineAPI.h"
#include "GameObject.h"
#include "Transform.h"
#include "AnimationController.h"
#include "MeshFilter.h"
#include "ParticleSystem.h"
#include "Light.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "CamAnimation.h"
#include "SceneSetting.h"
#include "DialogFactory.h"


// RightOption 대화 상자

GameObject* RightOption::ChoiceObject = nullptr;

IMPLEMENT_DYNAMIC(RightOption, CustomDialog)
RightOption*  RightOption::thisPointer = nullptr;
RightOption::RightOption(CWnd* pParent /*=nullptr*/)
	: CustomDialog(IDD_RIGHT_OPTION, pParent)
	, mIsActive(FALSE)
	, mIsShadow(FALSE)
	, mIsCull(FALSE)
{

}

RightOption::~RightOption()
{
	mRigidbody	= nullptr;
	mCollider	= nullptr;
	mTransform	= nullptr;
	mAnimation	= nullptr;
	mMeshFilter = nullptr;
	mPrticle	= nullptr;
	mLight		= nullptr;
	mFileOption = nullptr;
	mCam		= nullptr;
}

BOOL RightOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	

	m_EditorManager = DialogFactory::GetFactory()->GetEditorManager();
	mFileOption		= DialogFactory::GetFactory()->GetFileOption();
	mCam			= DialogFactory::GetFactory()->GetCamAnimation();
	mSceneSetting	= DialogFactory::GetFactory()->GetSceneSetting();

	DialogFactory::GetFactory()->SetRightOption(this);
	
	//Tag 기본 리스트
	Tag_Combo.InsertString(0, L"Default");
	Tag_Combo.InsertString(1, L"MainCam");
	Tag_Combo.InsertString(2, L"Point");
	Tag_Combo.InsertString(3, L"Player");
	TagList.insert({ 0,"Default" });
	TagList.insert({ 1,"MainCam" });
	TagList.insert({ 2,"Point" });
	TagList.insert({ 3,"Player" });
	Tag_Combo.SetCurSel(0);

	//텝정보들 생성
	CRect rect;
	Component_TapList.GetWindowRect(&rect);

	mTransform = DialogFactory::GetFactory()->GetCTAP_Transform();
	mTransform->Create(IDD_TAP_TRANSFORM, &Component_TapList);
	mTransform->MoveWindow(0, 25, rect.Width() - 5, rect.Height() - 25);
	mTransform->ShowWindow(SW_HIDE);

	mAnimation = DialogFactory::GetFactory()->GetCTAP_Animation();
	mAnimation->Create(IDD_TAP_ANIMATION, &Component_TapList);
	mAnimation->MoveWindow(0, 25, rect.Width() - 5, rect.Height() - 25);
	mAnimation->ShowWindow(SW_HIDE);

	mMeshFilter = DialogFactory::GetFactory()->GetCTAP_MeshFilter();
	mMeshFilter->Create(IDD_TAP_MESHFILTER, &Component_TapList);
	mMeshFilter->MoveWindow(0, 25, rect.Width() - 5, rect.Height() - 25);
	mMeshFilter->ShowWindow(SW_HIDE);

	mPrticle = DialogFactory::GetFactory()->GetCTAP_Particle();
	mPrticle->Create(IDD_TAP_PARTICLE, &Component_TapList);
	mPrticle->MoveWindow(0, 25, rect.Width() - 5, rect.Height() - 25);
	mPrticle->ShowWindow(SW_HIDE);

	mLight = DialogFactory::GetFactory()->GetCTAP_Light();
	mLight->Create(IDD_TAP_LIGHT, &Component_TapList);
	mLight->MoveWindow(0, 25, rect.Width() - 5, rect.Height() - 25);
	mLight->ShowWindow(SW_HIDE);

	mRigidbody = DialogFactory::GetFactory()->GetCTAP_Rigidbody();
	mRigidbody->Create(IDD_TAP_RIGIDBODY, &Component_TapList);
	mRigidbody->MoveWindow(0, 25, rect.Width() - 5, rect.Height() - 25);
	mRigidbody->ShowWindow(SW_HIDE);

	mCollider = DialogFactory::GetFactory()->GetCTAP_Collider();
	mCollider->Create(IDD_TAP_COLLIDER, &Component_TapList);
	mCollider->MoveWindow(0, 25, rect.Width() - 5, rect.Height() - 25);
	mCollider->ShowWindow(SW_HIDE);

	thisPointer = this;

	HirearchyTree.InsertItem(L"DirectionLight");
	HirearchyTree.InsertItem(L"DebugCamera");

	return 0;
}

void RightOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE2, HirearchyTree);
	DDX_Control(pDX, IDC_EDIT2, HirearchyEdit);
	DDX_Control(pDX, IDC_TAB1, Component_TapList);
	DDX_Control(pDX, IDC_COMBO1, Tag_Combo);
	DDX_Control(pDX, IDC_EDIT1, AddTag_Edit);
	DDX_Control(pDX, IDC_EDIT3, ParentName_Edit);
	DDX_Control(pDX, IDC_EDIT6, ChildName_Eidt);
	DDX_Check(pDX, IDC_CHECK1, mIsActive);
	DDX_Check(pDX, IDC_CHECK2, mIsShadow);
	DDX_Check(pDX, IDC_CHECK9, mIsCull);
}

BEGIN_MESSAGE_MAP(RightOption, CDialogEx)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE2, &RightOption::OnChoice_Hirearchy_Item)
	ON_BN_CLICKED(IDC_BUTTON8, &RightOption::OnDelteObject_Button)
	ON_BN_CLICKED(IDC_BUTTON7, &RightOption::OnChange_DataFormat)
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &RightOption::OnClickTap)
	ON_WM_MOUSEHWHEEL()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON10, &RightOption::OnSceneSetting)
	ON_BN_CLICKED(IDC_BUTTON1, &RightOption::OnAddTag_Button)
	ON_CBN_SELCHANGE(IDC_COMBO1, &RightOption::OnChoiceTag)
	ON_BN_CLICKED(IDC_BUTTON2, &RightOption::OnDeleteTagButton)
	ON_BN_CLICKED(IDC_BUTTON9, &RightOption::OnCreatePrefap)
	ON_BN_CLICKED(IDC_BUTTON26, &RightOption::OnOpenOption)
	ON_BN_CLICKED(IDC_BUTTON11, &RightOption::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON13, &RightOption::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON27, &RightOption::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_CHECK1, &RightOption::OnIsActiveButton)
	ON_BN_CLICKED(IDC_CHECK2, &RightOption::OnIsShadowButton)
	ON_BN_CLICKED(IDC_CHECK9, &RightOption::OnIsCullButton)
END_MESSAGE_MAP()

RightOption* RightOption::GetThis()
{
	return thisPointer;
}

void RightOption::MouseDown()
{
	if (GetKeyDown(VK_LBUTTON))
	{
		//float X, Y;
		//X = GetMousePosX();
		//Y = GetMousePosY();

		//ChoiceObject = Picking(X,Y);

		//Demo::FindMesh(ChangeToString(MeshName), ChangeToString(ChoiceObject->Name));
	}
}

void RightOption::Create_Hirearchy_Item(GameObject* Obj, HTREEITEM TOP)
{
	int BoneCount = Obj->GetChildBoneCount();
	int MeshCount = Obj->GetChildMeshCount();

	for (int i = 0; i < BoneCount; i++) 
	{
		GameObject* Child = Obj->GetChildBone(i);
		HTREEITEM ChildItem = HirearchyTree.InsertItem(ChangeToCString(Child->Name),TOP);
		Create_Hirearchy_Item(Child, ChildItem);
	}

	for (int i = 0; i < MeshCount; i++)
	{
		GameObject* Child = Obj->GetChildMesh(i);
		HTREEITEM ChildItem = HirearchyTree.InsertItem(ChangeToCString(Child->Name), TOP);
		Create_Hirearchy_Item(Child, ChildItem);
	}
}

void RightOption::Delete_Hirearchy_Item(HTREEITEM TOP)
{
	if (HirearchyTree.ItemHasChildren(TOP))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = HirearchyTree.GetChildItem(TOP);

		while (hChildItem != NULL)
		{
			hNextItem = HirearchyTree.GetNextItem(hChildItem, TVGN_NEXT);
			HirearchyTree.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}
}

void RightOption::ChickTapDrag(CPoint point)
{
	//Tap컨트롤에  드래그했을때 처리
	CRect rect;
	Component_TapList.GetWindowRect(&rect);
	if (DropRect(rect))
	{
		//현재 켜져있는 창을 알아온다
		int num = Component_TapList.GetCurSel();
		TCHAR buffer[256] = { 0 };
		TC_ITEM data;
		data.mask = TCIF_TEXT;
		data.pszText = buffer;
		data.cchTextMax = 256;
		Component_TapList.GetItem(num, &data);
		CString ComponentName;
		ComponentName = buffer;

		if (ComponentName == "MeshFilter")
		{
			CString* pstr = new CString(DragItemName);
			mMeshFilter->SendMessage(M_MSG_MeshFilter, 0, (LPARAM)pstr);
			delete pstr;
		}

		if (ComponentName == "Collider")
		{
			mCollider->SetPoint(DragItemName);
		}
	}
}

void RightOption::ChickHirearchyDarg(CPoint point)
{
	//하이어라키 창에 드래그했을때 처리
	CRect rect;
	HirearchyTree.GetWindowRect(&rect);

	
	//범위 안에 들어와있는지 체크
	if (DropRect(rect))
	{
		std::string Name = ChangeToString(DragItemName);
		int Type = GetFileNameType(Name);

		//드래그한 아이템 타입
		switch (Type)
		{
		case FBX:
		case EATER: 
		{
			Name = Name.substr(0, Name.rfind('.'));
			GameObject* Object = EditorToolScene::Create_Object(Name);
			HTREEITEM TopParent = HirearchyTree.InsertItem(ChangeToCString(Object->Name));
			break;
		}
		case SCENE:
		{
			HirearchyTree.DeleteAllItems();

			EditorToolScene::LoadScene(Name);
			std::map<std::string, GameObject*>::iterator Start_it	= EditorToolScene::ObjectList.begin();
			std::map<std::string, GameObject*>::iterator End_it		= EditorToolScene::ObjectList.end();
			for (Start_it; Start_it != End_it; Start_it++)
			{
				HirearchyTree.InsertItem(ChangeToCString(Start_it->first));
			}

			Tag_Combo.ResetContent();
			std::map<int,std::string>::iterator Tag_Start_it	=  EditorToolScene::TagList.begin();
			std::map<int,std::string>::iterator Tag_End_it		=  EditorToolScene::TagList.end();
			for(Tag_Start_it; Tag_Start_it != Tag_End_it; Tag_Start_it++)
			{
				Tag_Combo.InsertString( Tag_Start_it->first, ChangeToCString(Tag_Start_it->second) );
			}

			break;
		}
		case PARTICLE:
		{
			Name = Name.substr(0, Name.rfind('.'));
			GameObject* Object = EditorToolScene::Create_Particle(Name);
			HTREEITEM TopParent = HirearchyTree.InsertItem(ChangeToCString(Object->Name));
			break;
		}
		default:
			//나머지는 에러 
			AfxMessageBox(_T("Error : .Eater, .Scene 파일만 가능합니다"));
			break;
		}		
	}
}

void RightOption::ChickObjectTap(GameObject* Obj)
{
	Transform*				TR = Obj->GetTransform();
	AnimationController*	AC = Obj->GetComponent<AnimationController>();
	MeshFilter*				MF = Obj->GetComponent<MeshFilter>();
	ParticleSystem*			PS = Obj->GetComponent<ParticleSystem>();
	Light*					LT = Obj->GetComponent<Light>();
	Rigidbody*				RI = Obj->GetComponent<Rigidbody>();
	Collider*				CL = Obj->GetComponent<Collider>();

	mIsActive = ChoiceObject->GetActive();
	mIsShadow = ChoiceObject->GetShadow();
	mIsCull = ChoiceObject->GetCull();
	UpdateData(FALSE);

	int FrontCount = 0;
	if (TR != nullptr)
	{
		Component_TapList.InsertItem(FrontCount, L"Transform");
		mTransform->ShowWindow(SW_SHOW);
		mTransform->SetGameObject(TR);
		FrontCount++;
	}

	if (AC != nullptr)
	{
		Component_TapList.InsertItem(FrontCount, L"Animation");
		mAnimation->ShowWindow(SW_HIDE);
		mAnimation->SetGameObject(AC);
		FrontCount++;
	}

	if (MF != nullptr)
	{
		Component_TapList.InsertItem(FrontCount, L"MeshFilter");
		mMeshFilter->ShowWindow(SW_HIDE);
		mMeshFilter->SetGameObject(MF);
		FrontCount++;
	}

	if (PS != nullptr)
	{
		Component_TapList.InsertItem(FrontCount, L"Particle");
		mPrticle->ShowWindow(SW_HIDE);
		mPrticle->SetGameObject(PS);
		FrontCount++;
	}

	if (LT != nullptr)
	{
		Component_TapList.InsertItem(FrontCount, L"Light");
		mLight->ShowWindow(SW_HIDE);
		mLight->SetGameObject(LT);
		FrontCount++;
	}

	if (RI != nullptr)
	{
		Component_TapList.InsertItem(FrontCount, L"Rigidbody");
		mRigidbody->ShowWindow(SW_HIDE);
		mRigidbody->SetGameObject(RI);
		FrontCount++;
	}

	if (CL != nullptr)
	{
		Component_TapList.InsertItem(FrontCount, L"Collider");
		mCollider->ShowWindow(SW_HIDE);
		mCollider->SetGameObject(CL);
		FrontCount++;
	}

	Component_TapList.SetCurSel(0);
}

void RightOption::OnChoice_Hirearchy_Item(GameObject* Obj)
{
	//에디터의 매쉬 이름을 출력
	CString Name = ChangeToCString(Obj->Name);
	HirearchyEdit.SetWindowTextW(Name);

	//컨퍼넌트를 초기화
	Component_TapList.DeleteAllItems();

	ChoiceObject = Obj;

	//만약자식오브젝트를 눌렀다면 맨위 최상위부모를 가져온다
	//ChoiceObject = FindGameObjectParent(ChoiceItem);

	//Delete_Hirearchy_Item(ChoiceItem);
	//Create_Hirearchy_Item(ChoiceObject, ChoiceItem);
	//Tag_Combo.SetCurSel(ChoiceObject->GetTag());

	//mFileOption->SetChoiceGameObjectName(MeshName, ChoiceObject);

	//오브젝트에 해당하는 컨포넌트 Tap을 보여준다
	ChickObjectTap(Obj);
}

GameObject* RightOption::FindGameObjectParent(HTREEITEM mItem, bool FindParent)
{
	//클릭한 아이템이 부모가 있는 아이템이라면 Demo씬에서 부모안에 오브젝트를 찾아 반환
	HTREEITEM Parent	= mItem;
	CString	 MeshName	= HirearchyTree.GetItemText(mItem);
	CString ParentName;
	GameObject* Object = nullptr;
	while (true)
	{
		if (Parent == NULL)
		{
			break;
		}
		else
		{
			ParentName = HirearchyTree.GetItemText(Parent);
			Parent = HirearchyTree.GetParentItem(Parent);
		}
	}

	if (ParentName == MeshName)
	{
		Object = EditorToolScene::FindMesh(ChangeToString(MeshName));
	}
	else
	{
		if (FindParent == true)
		{
			Object = EditorToolScene::FindMeshParent(ChangeToString(MeshName), ChangeToString(ParentName));
		}
		else
		{
			Object = EditorToolScene::FindMeshChild(ChangeToString(MeshName), ChangeToString(ParentName));
		}
	}


	return Object;
}

void RightOption::OnChoice_Hirearchy_Item(NMHDR* pNMHDR, LRESULT* pResult)
{
	mTransform->ShowWindow(SW_HIDE);
	mMeshFilter->ShowWindow(SW_HIDE);
	mLight->ShowWindow(SW_HIDE);
	mPrticle->ShowWindow(SW_HIDE);
	mCollider->ShowWindow(SW_HIDE);
	mRigidbody->ShowWindow(SW_HIDE);

	//선택한 아이템의 이름을 가져온다
	HTREEITEM ChoiceItem = HirearchyTree.GetSelectedItem();

	CString Name = HirearchyTree.GetItemText(ChoiceItem);
	std::string MeshName = ChangeToString(Name);

	//에디터의 매쉬 이름을 출력
	ChoiceHirearchyName = MeshName.substr(0, MeshName.rfind('.'));
	Name = ChoiceHirearchyName.c_str();
	HirearchyEdit.SetWindowTextW(Name);

	//컨퍼넌트를 초기화
	Component_TapList.DeleteAllItems();

	//만약자식오브젝트를 눌렀다면 맨위 최상위부모를 가져온다
	ChoiceObject = FindGameObjectParent(ChoiceItem);

	mIsActive	= ChoiceObject->GetActive();
	mIsShadow	= ChoiceObject->GetShadow();
	mIsCull		= ChoiceObject->GetCull();
	UpdateData(FALSE);

	Delete_Hirearchy_Item(ChoiceItem);
	Create_Hirearchy_Item(ChoiceObject, ChoiceItem);
	Tag_Combo.SetCurSel(ChoiceObject->GetTag());

	mFileOption->SetChoiceGameObjectName(MeshName, ChoiceObject);

	//오브젝트에 해당하는 컨포넌트 Tap을 보여준다
	ChickObjectTap(ChoiceObject);
}

void RightOption::OnDelteObject_Button()
{
	std::string Name;
	Name = ChoiceHirearchyName;
	Name = Name.substr(0, Name.rfind('.'));
	bool Delete = EditorToolScene::DeleteObject(Name);
	if (Delete == true)
	{
		HTREEITEM item = HirearchyTree.GetSelectedItem();
		if (item != NULL)
		{
			HirearchyTree.DeleteItem(item);
		}

		ChoiceHirearchyName = "";
		HirearchyEdit.SetWindowTextW(_T(""));
	}
	else
	{
		AfxMessageBox(_T("Error : 삭제 실패 \n 제대로 선택하였는지 확인"));
	}
}

void RightOption::OnChange_DataFormat()
{
	//자체포멧의 값을 변경한다
	ObjectOption mObjectOption;
	mTransform->GetData(mObjectOption);
	mMeshFilter->GetData(mObjectOption);

	mObjectOption.WorldSave = false;
	m_EditorManager->OpenEaterGameObject(ChoiceObject,&mObjectOption);
	AfxMessageBox(L"변환 완료");
}

void RightOption::OnClickTap(NMHDR* pNMHDR, LRESULT* pResult)
{
	int num = Component_TapList.GetCurSel();

	TCHAR buffer[256] = { 0 };
	TC_ITEM data;
	data.mask = TCIF_TEXT;
	data.pszText = buffer;
	data.cchTextMax = 256;
	Component_TapList.GetItem(num, &data);

	CString ComponentName;
	ComponentName = buffer;

	if (ComponentName == L"Transform")
	{
		mAnimation->ShowWindow(SW_HIDE);
		mMeshFilter->ShowWindow(SW_HIDE);
		mPrticle->ShowWindow(SW_HIDE);
		mLight->ShowWindow(SW_HIDE);
		mRigidbody->ShowWindow(SW_HIDE);
		mCollider->ShowWindow(SW_HIDE);
		mTransform->ShowWindow(SW_SHOW);
	}
	else if (ComponentName == L"Animation")
	{
		mTransform->ShowWindow(SW_HIDE);
		mMeshFilter->ShowWindow(SW_HIDE);
		mPrticle->ShowWindow(SW_HIDE);
		mLight->ShowWindow(SW_HIDE);
		mRigidbody->ShowWindow(SW_HIDE);
		mCollider->ShowWindow(SW_HIDE);
		mAnimation->ShowWindow(SW_SHOW);
	}
	else if (ComponentName == L"MeshFilter")
	{
		mTransform->ShowWindow(SW_HIDE);
		mAnimation->ShowWindow(SW_HIDE);
		mMeshFilter->ShowWindow(SW_SHOW);
		mPrticle->ShowWindow(SW_HIDE);
		mLight->ShowWindow(SW_HIDE);
		mRigidbody->ShowWindow(SW_HIDE);
		mCollider->ShowWindow(SW_HIDE);
	}
	else if (ComponentName == L"Particle")
	{
		mTransform->ShowWindow(SW_HIDE);
		mAnimation->ShowWindow(SW_HIDE);
		mMeshFilter->ShowWindow(SW_HIDE);
		mPrticle->ShowWindow(SW_SHOW);
		mLight->ShowWindow(SW_HIDE);
		mRigidbody->ShowWindow(SW_HIDE);
		mCollider->ShowWindow(SW_HIDE);
	}
	else if (ComponentName == L"Light")
	{
		mTransform->ShowWindow(SW_HIDE);
		mAnimation->ShowWindow(SW_HIDE);
		mMeshFilter->ShowWindow(SW_HIDE);
		mPrticle->ShowWindow(SW_HIDE);
		mLight->ShowWindow(SW_SHOW);
		mRigidbody->ShowWindow(SW_HIDE);
		mCollider->ShowWindow(SW_HIDE);
	}
	else if (ComponentName == L"Camera")
	{
		//mTransform->ShowWindow(SW_HIDE);
		//mAnimation->ShowWindow(SW_HIDE);
		//mMeshFilter->ShowWindow(SW_HIDE);
		//mPrticle->ShowWindow(SW_HIDE);
		//mLight->ShowWindow(SW_HIDE);
		//mRigidbody->ShowWindow(SW_HIDE);
		//mCollider->ShowWindow(SW_HIDE);
	}
	else if (ComponentName == L"Collider")
	{
		mTransform->ShowWindow(SW_HIDE);
		mAnimation->ShowWindow(SW_HIDE);
		mMeshFilter->ShowWindow(SW_HIDE);
		mPrticle->ShowWindow(SW_HIDE);
		mLight->ShowWindow(SW_HIDE);
		mRigidbody->ShowWindow(SW_HIDE);
		mCollider->ShowWindow(SW_SHOW);
	}
	else if (ComponentName == L"Rigidbody")
	{
		mTransform->ShowWindow(SW_HIDE);
		mAnimation->ShowWindow(SW_HIDE);
		mMeshFilter->ShowWindow(SW_HIDE);
		mPrticle->ShowWindow(SW_HIDE);
		mLight->ShowWindow(SW_HIDE);
		mRigidbody->ShowWindow(SW_SHOW);
		mCollider->ShowWindow(SW_HIDE);
	}
}

BOOL RightOption::PreTranslateMessage(MSG* pMsg)
{
	//키눌린 메시지가 들어올때 esc이거나 return  값이면
   //걍 리턴 시켜준다.
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
		default:
			break;
		}
	}

	if (pMsg->message == WM_COMMAND)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
		default:
			break;
		}
	}



	return CDialog::PreTranslateMessage(pMsg);
}

void RightOption::OnSceneSetting()
{
	mSceneSetting->ShowWindow(SW_SHOW);
	mSceneSetting->Setting();
}

void RightOption::OnAddTag_Button()
{
	CString AddTagName;
	int MaxIndex = 0;
	AddTag_Edit.GetWindowTextW(AddTagName);
	int Count  = EditorToolScene::AddTag(ChangeToString(AddTagName));

	if (Count != -1)
	{
		AfxMessageBox(L"Add Tag");
	}
	else
	{
		AfxMessageBox(L"Error : Delete Tag");
	}

	Tag_Combo.InsertString(Count, AddTagName);
	AddTag_Edit.SetWindowTextW(L"");
}

void RightOption::OnChoiceTag()
{
	GameObject* Object = EditorToolScene::FindMesh(ChoiceHirearchyName);
	if (Object == nullptr) 
	{
		AfxMessageBox(L"Error : 선택한 오브젝트가 없습니다");
		return;
	}

	int number = Tag_Combo.GetCurSel();
	Object->SetTag(number);
	AfxMessageBox(L"Object Tag 설정 완료");
}

void RightOption::OnDeleteTagButton()
{
	CString AddTagName;
	AddTag_Edit.GetWindowTextW(AddTagName);
	int Count = EditorToolScene::DeleteTag(ChangeToString(AddTagName));

	if (Count == -1)
	{
		AfxMessageBox(L"Error : 삭제할 태그의 이름을 입력해주세요");
		return;
	}
	else
	{
		Tag_Combo.DeleteString(Count);
		AfxMessageBox(L"삭제 성공");
		AddTag_Edit.SetWindowTextW(L"");
	}
}

void RightOption::OnCreatePrefap()
{
	EditorToolScene::SavePrefap("../Assets/Model/Prefap/","Test", ChoiceHirearchyName);
}


void RightOption::OnOpenOption()
{
	mFileOption->ShowWindow(SW_SHOW);
}


void RightOption::OnBnClickedButton11()
{
	//부모 설정
	ParentName = ChoiceHirearchyName;
	ParentName_Edit.SetWindowTextW(ChangeToCString(ParentName));
	ParentItem = HirearchyTree.GetSelectedItem();
}


void RightOption::OnBnClickedButton13()
{
	//자식 설정
	ChildName = ChoiceHirearchyName;
	ChildName_Eidt.SetWindowTextW(ChangeToCString(ChildName));
	ChildItem = HirearchyTree.GetSelectedItem();
}


void RightOption::OnBnClickedButton27()
{
	//결합
	//선택한 부모로 설정한 오브젝트와 자식오브젝트
	GameObject* ParentObject	= FindGameObjectParent(ParentItem);
	GameObject* ChildTopObject	= FindGameObjectParent(ChildItem);


	ParentObject->ChoiceChild(ChildTopObject);
	ChildTopObject->ChoiceParent(ParentObject);
}


void RightOption::OnIsActiveButton()
{
	UpdateData(true);
	ChoiceObject->SetActive(mIsActive);
}


void RightOption::OnIsShadowButton()
{
	UpdateData(true);
	ChoiceObject->SetShadow(mIsShadow);
}


void RightOption::OnIsCullButton()
{
	UpdateData(true);
	ChoiceObject->SetCull(mIsCull);
}
