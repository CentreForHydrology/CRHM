#pragma once


// CParameters dialog

class CParameters : public CDialog
{
	DECLARE_DYNAMIC(CParameters)

public:
	CParameters(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CParameters();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = PARAMETERS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
