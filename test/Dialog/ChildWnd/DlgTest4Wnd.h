#pragma once
#include "afxwin.h"

using namespace std;
class CDlgTest4Wnd : public CDialog
{
	DECLARE_DYNAMIC(CDlgTest4Wnd)
public:
	CDlgTest4Wnd(CWnd* pParent = NULL); 
	virtual ~CDlgTest4Wnd();

public:
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void					DoDataExchange(CDataExchange* pDX);    
	virtual BOOL					OnInitDialog();

	afx_msg void					OnPaint();

	afx_msg void					OnBnClickedButton1();
	afx_msg void					OnBnClickedButton2();
	afx_msg void					OnBnClickedButton3();
	afx_msg void					OnBnClickedButton4();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL							InitCtrl();
	BOOL							InitInfo();
};