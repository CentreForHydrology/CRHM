#pragma once


// FlowDiagram dialog

class FlowDiagram : public CDialogEx
{
	DECLARE_DYNAMIC(FlowDiagram)

public:
	FlowDiagram(CWnd* pParent = nullptr);   // standard constructor
	virtual ~FlowDiagram();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = FlowDiagram };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
