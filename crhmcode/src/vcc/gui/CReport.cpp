#include "../stdafx.h"
#include "CRHM_GUI.h"
#include "CReport.h"
#include "afxdialogex.h"
#include "MacroEntryDlg.h"
#include "../../core/CRHMmain/CRHMmain.h"
#include "../../core/InstrumentLogger.h"

IMPLEMENT_DYNAMIC (CReport, CDialog)
CReport :: CReport (CWnd * pParent): CDialog (REPORT_DLG, pParent)
{
InstrumentLogger::instance()->log_instrument_log("<CReport::CReport(CWnd * pParent): CDialog (REPORT_DLG, pParent)@@@CReport.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CReport::CReport(CWnd * pParent): CDialog (REPORT_DLG, pParent)@@@CReport.cpp>");
}
CReport :: ~CReport ()
{
InstrumentLogger::instance()->log_instrument_log("<CReport::~CReport()@@@CReport.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CReport::~CReport()@@@CReport.cpp>");
}
void CReport :: DoDataExchange (CDataExchange * pDX)
{
InstrumentLogger::instance()->log_instrument_log("<CReport::DoDataExchange(CDataExchange * pDX)@@@CReport.cpp>");
    CDialog :: DoDataExchange (pDX);
    DDX_Control (pDX, IDC_LIST1, listbox1);
    DDX_Control (pDX, IDC_EDIT1, editbox1);
    DDX_Control (pDX, IDC_STATIC2, static2);
InstrumentLogger::instance()->log_instrument_log("</CReport::DoDataExchange(CDataExchange * pDX)@@@CReport.cpp>");
}BEGIN_MESSAGE_MAP (CReport, CDialog)
    ON_COMMAND (ID_EXTRACTGROUP, & CReport :: OnExtractgroup)
    ON_LBN_DBLCLK (IDC_LIST1, & CReport :: OnLbnDblclkList1)
    ON_COMMAND (ID_FILE_SAVE32791, & CReport :: OnFileSave32791)
    ON_WM_ACTIVATE ()
END_MESSAGE_MAP ()

void CReport :: OnExtractgroup ()
{
InstrumentLogger::instance()->log_instrument_log("<CReport::OnExtractgroup()@@@CReport.cpp>");
    ClassModule * thisMod;
    listbox1.ResetContent ();
    editbox1.SetWindowText (_T (""));
    static2.SetWindowTextW (_T ("Macros Used"));
    for (int ii = 0; ii < Global :: OurModulesList -> Count; ++ ii) {
        string S = Global :: OurModulesList -> Strings [ii];
        long Indx = Global :: AllModulesList -> IndexOf (S);
        thisMod = (ClassModule *) Global :: AllModulesList -> Objects [Indx];
        if (thisMod -> isGroup)
        {
            CString cstr (S.c_str ());
            listbox1.AddString (cstr);
        }

    }
InstrumentLogger::instance()->log_instrument_log("</CReport::OnExtractgroup()@@@CReport.cpp>");
}
void CReport :: OnLbnDblclkList1 ()
{
InstrumentLogger::instance()->log_instrument_log("<CReport::OnLbnDblclkList1()@@@CReport.cpp>");
    CString cstr;
    int index = listbox1.GetCurSel ();
    listbox1.GetText (index, cstr);
    CT2A str (cstr);
    string groupname = str;
    ClassModule * thisMod;
    long Indx = Global :: AllModulesList -> IndexOf (groupname);
    thisMod = (ClassModule *) Global :: AllModulesList -> Objects [Indx];
    ExtractGroupFile (thisMod);
InstrumentLogger::instance()->log_instrument_log("</CReport::OnLbnDblclkList1()@@@CReport.cpp>");
}
void CReport :: ExtractGroupFile (ClassModule * thisModule)
{
InstrumentLogger::instance()->log_instrument_log("<CReport::ExtractGroupFile(ClassModule * thisModule)@@@CReport.cpp>");
    CString newline ("\r\n");
    CString cstr ("");
    ClassMacro * thisMacro;
    string S, SS;
    MapPar :: iterator itPar;
    ClassPar * thisPar;
    if (thisModule -> isGroup)
    {
        thisMacro = dynamic_cast < ClassMacro * > (thisModule);
    }

    else
    {

InstrumentLogger::instance()->log_instrument_log("</CReport::ExtractGroupFile(ClassModule * thisModule)@@@CReport.cpp>");
        return;
    }

    editbox1.SetWindowTextW (_T (""));
    TStringList * stringlist;
    stringlist = new TStringList ();
    CRHMmain * test = CRHMmain :: getInstance ();
    S = thisMacro -> Name.c_str ();
    stringlist -> Add ("*** extracted group: '" + S + "' from '" + test -> OpenProjectPath + "' ***");
    stringlist -> Add ("######");
    stringlist -> Add ("Dimensions:");
    stringlist -> Add ("######");
    stringlist -> Add (string ("nhru " + Common :: longtoStr (Add_NHRU (S, Global :: MacroModulesList))));
    stringlist -> Add (string ("nlay " + Common :: longtoStr (Global :: nlay)));
    stringlist -> Add (string ("nobs " + Common :: longtoStr (1)));
    stringlist -> Add ("######");
    stringlist -> Add ("Macros:");
    stringlist -> Add ("######");
    long MacroPos = stringlist -> Count;
    stringlist -> Add ("######");
    stringlist -> Add ("Modules:");
    stringlist -> Add ("######");
    list < ModulePtr > :: iterator iterM;
    iterM = ((ClassMacro *) thisModule) -> Modules.begin ();
    while (iterM != ((ClassMacro *) thisModule) -> Modules.end ()) {
        std :: weak_ptr < ClassModule > MP ((* iterM));
        S = (* iterM) -> NameRoot.c_str ();
        if ((* iterM) -> DLLName == "Macro")
        {
            AddMacroCode (S, Global :: MacroModulesList, stringlist, MacroPos);
        }

        if ((* iterM) -> variation > 0)
        {
            string AA ("#0 ");
            AA [1] += (char) (log ((* iterM) -> variation) / log (2) + 1);
            S = S + AA.c_str ();
        }

        else
        {
            S = S + " ";
        }

        S = S + (* iterM) -> DLLName + " ";
        S = S + (* iterM) -> Version;
        stringlist -> Add (S);
        ++ iterM;
    }
    stringlist -> Add ("######");
    stringlist -> Add ("Parameters:");
    stringlist -> Add ("######");
    S = thisMacro -> Name.c_str ();
    for (itPar = Global :: MapPars.begin (); itPar != Global :: MapPars.end (); itPar ++) {
        thisPar = (* itPar).second;
        if (S == thisPar -> module.c_str ())
        {
            if (thisPar -> param == "HRU_group")
            {
                continue;
            }

            string S = string ("Shared " + string (thisPar -> param.c_str ()));
            if (thisPar -> varType != TVar :: Txt)
            {
                S += "\t<\t" + FloatToStrF (thisPar -> minVal, TFloatFormat :: ffGeneral, 4, 0) + "\t" + FloatToStrF (thisPar -> maxVal, TFloatFormat :: ffGeneral, 4, 0) + "\t>";
            }

            stringlist -> Add (S);
            for (int jj = 0; jj < thisPar -> lay; jj ++) {
                S = "";
                for (int ii = 0; ii < thisPar -> dim; ii ++) {
                    if (thisPar -> varType == TVar :: Float)
                    {
                        S = S + FloatToStrF (thisPar -> layvalues [jj] [ii], TFloatFormat :: ffGeneral, 4, 0) + "\t";
                    }

                    else
                    if (thisPar -> varType == TVar :: Int)
                    {
                        S = S + FloatToStrF (thisPar -> ilayvalues [jj] [ii], TFloatFormat :: ffFixed, 8, 0) + "\t";
                    }

                    else
                    if (thisPar -> varType == TVar :: Txt)
                    {
                        if (thisPar -> Strings -> Count > ii)
                        {
                            S = S + "'" + thisPar -> Strings -> Strings [ii] + "'\t";
                        }

                        else
                        {
                            S = "''\t";
                        }

                    }

                    if (ii % 16 == 15)
                    {
                        stringlist -> Add (S);
                        S = "";
                    }

                }
                if (S.length () > 0)
                {
                    stringlist -> Add (S);
                    S = "";
                }

            }
            if (S.length () > 0)
            {
                stringlist -> Add (S);
            }

        }

    }
    stringlist -> Add ("######");
    CString cstrtemp, text;
    for (int i = 0; i < stringlist -> Count; i ++) {
        string strtemp = stringlist -> Strings [i];
        cstrtemp = strtemp.c_str ();
        text += newline + cstrtemp;
    }
    editbox1.SetWindowTextW (text);
InstrumentLogger::instance()->log_instrument_log("</CReport::ExtractGroupFile(ClassModule * thisModule)@@@CReport.cpp>");
}
long CReport :: Add_NHRU (string S, TStringList * MacroModulesList)
{
InstrumentLogger::instance()->log_instrument_log("<CReport::Add_NHRU(string S, TStringList * MacroModulesList)@@@CReport.cpp>");
    for (int ii = 0; ii < MacroModulesList -> Count; ++ ii) {
        int found = MacroModulesList -> Strings [ii].find (S);
        if (found >= 0)
        {
            string Trim = Common :: trimleft (MacroModulesList -> Strings [++ ii]);
            long len = Trim.length ();
            if (len < 11)
            {

InstrumentLogger::instance()->log_instrument_log("</CReport::Add_NHRU(string S, TStringList * MacroModulesList)@@@CReport.cpp>");
                return Global :: nhru;
            }

            else
            {
                string temp = Trim.substr (10, len - 10);
                CString ctemp (temp.c_str ());
                long n = StrToInt (ctemp);
                if (n <= 0)
                {

InstrumentLogger::instance()->log_instrument_log("</CReport::Add_NHRU(string S, TStringList * MacroModulesList)@@@CReport.cpp>");
                    return Global :: nhru;
                }

                else
                {

InstrumentLogger::instance()->log_instrument_log("</CReport::Add_NHRU(string S, TStringList * MacroModulesList)@@@CReport.cpp>");
                    return n;
                }

            }

        }

    }

InstrumentLogger::instance()->log_instrument_log("</CReport::Add_NHRU(string S, TStringList * MacroModulesList)@@@CReport.cpp>");
    return 0;
InstrumentLogger::instance()->log_instrument_log("</CReport::Add_NHRU(string S, TStringList * MacroModulesList)@@@CReport.cpp>");
}
void CReport :: AddMacroCode (string S, TStringList * MacroModulesList, TStringList * Memo1, long & MacroPos)
{
InstrumentLogger::instance()->log_instrument_log("<CReport::AddMacroCode(string S, TStringList * MacroModulesList, TStringList * Memo1, long & MacroPos)@@@CReport.cpp>");
    int endpos = 0;
    for (int ii = 0; ii < MacroModulesList -> Count; ++ ii) {
        int found = MacroModulesList -> Strings [ii].find (S);
        if (found >= 0)
        {
            Memo1 -> Add ("'" + MacroModulesList -> Strings [ii] + "'");
            MacroPos ++;
            do {
                ++ ii;
                Memo1 -> Add ("'" + MacroModulesList -> Strings [ii] + "'");
                MacroPos ++;
                endpos = MacroModulesList -> Strings [ii].find ("end");
            }
            while (! (endpos >= 0 && MacroModulesList -> Strings [ii].length () == 3) && ii < MacroModulesList -> Count - 1);
            break;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</CReport::AddMacroCode(string S, TStringList * MacroModulesList, TStringList * Memo1, long & MacroPos)@@@CReport.cpp>");
}
void CReport :: OnFileSave32791 ()
{
InstrumentLogger::instance()->log_instrument_log("<CReport::OnFileSave32791()@@@CReport.cpp>");
    CString cstr;
    editbox1.GetWindowText (cstr);
    CT2A str (cstr);
    TStringList * strings = new TStringList ();
    string temp = str;
    int l = temp.length ();
    for (int i = 0; i < l; i ++) {
        if (i < l - 1)
        {
            if (temp [i] == '\r')
            {
                temp [i] = ' ';
            }

        }

    }
    strings -> Add (temp);
    FileDialog * filedialog;
    filedialog = new FileDialog ();
    if (filedialog -> Save ("rpt") == true)
    {
        strings -> SaveToFile (filedialog -> filepath);
        MessageBox (_T ("Group Saved"));
    }

InstrumentLogger::instance()->log_instrument_log("</CReport::OnFileSave32791()@@@CReport.cpp>");
}
void CReport :: OnActivate (UINT nState, CWnd * pWndOther, BOOL bMinimized)
{
InstrumentLogger::instance()->log_instrument_log("<CReport::OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized)@@@CReport.cpp>");
    CDialog :: OnActivate (nState, pWndOther, bMinimized);
    OnExtractgroup ();
InstrumentLogger::instance()->log_instrument_log("</CReport::OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinimized)@@@CReport.cpp>");
}