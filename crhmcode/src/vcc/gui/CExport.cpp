#include "../stdafx.h"
#include "CRHM_GUI.h"
#include "CExport.h"
#include "afxdialogex.h"
#include "../../core/InstrumentLogger.h"

IMPLEMENT_DYNAMIC (CExport, CDialog)
CExport :: CExport (CWnd * pParent): CDialog (EXPORT_DLG, pParent)
{
InstrumentLogger::instance()->log_instrument_log("<CExport::CExport(CWnd * pParent): CDialog (EXPORT_DLG, pParent)@@@CExport.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CExport::CExport(CWnd * pParent): CDialog (EXPORT_DLG, pParent)@@@CExport.cpp>");
}
CExport :: ~CExport ()
{
InstrumentLogger::instance()->log_instrument_log("<CExport::~CExport()@@@CExport.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CExport::~CExport()@@@CExport.cpp>");
}
void CExport :: DoDataExchange (CDataExchange * pDX)
{
InstrumentLogger::instance()->log_instrument_log("<CExport::DoDataExchange(CDataExchange * pDX)@@@CExport.cpp>");
    CDialog :: DoDataExchange (pDX);
InstrumentLogger::instance()->log_instrument_log("</CExport::DoDataExchange(CDataExchange * pDX)@@@CExport.cpp>");
}BEGIN_MESSAGE_MAP (CExport, CDialog)
END_MESSAGE_MAP ()
