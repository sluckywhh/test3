#ifndef __DLG_FILENAME_EXT_H__
#define __DLG_FILENAME_EXT_H__

class CDlgFileNameExt : public CDialog
{
	DECLARE_DYNAMIC(CDlgFileNameExt)
public:
	CDlgFileNameExt(CWnd* pParent = NULL);
	virtual ~CDlgFileNameExt();
	
public:
	enum { IDD = IDD_FILENAMEEXT_DIALOG };
	
protected:
	virtual void					DoDataExchange(CDataExchange* pDX);   
	virtual BOOL					PreTranslateMessage(MSG* pMsg);
	virtual BOOL					OnInitDialog();	
	
	afx_msg void					OnPaint();
	DECLARE_MESSAGE_MAP()
	
protected:
	BOOL							InitCtrl();
	BOOL							InitInfo();

	BOOL							InitWndSkin();
	BOOL							CreateChildWnd();
	
private:
	BOOL							m_bInited;	
};

#endif