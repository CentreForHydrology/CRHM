#pragma once


// BuildModel dialog

class BuildModel : public CDialogEx
{
	DECLARE_DYNAMIC(BuildModel)

public:
	BuildModel(CWnd* pParent = nullptr);   // standard constructor
	virtual ~BuildModel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = CONSTRUCT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
