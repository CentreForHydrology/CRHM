// CParameters.cpp : implementation file
//

#include "../stdafx.h"
#include "CRHM_GUI.h"
#include "CParameters.h"
#include "afxdialogex.h"


// CParameters dialog

IMPLEMENT_DYNAMIC(CParameters, CDialog)

CParameters::CParameters(CWnd* pParent /*=nullptr*/)
	: CDialog(PARAMETERS_DLG, pParent)
{

}

CParameters::~CParameters()
{
}

void CParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParameters, CDialog)
END_MESSAGE_MAP()


// CParameters message handlers
