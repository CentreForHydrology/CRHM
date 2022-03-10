#include "stdafx.h"
#include "CRHM_GUI.h"
#include "CRHMmainDlg.h"
#include "afxdialogex.h"
#include "CConstruct.h"
#include "CRHMAboutBox.h"
#include "MacroEntryDlg.h"
#include "FlowDiagramDlg.h"
#include "CReport.h"
#include "../../core/InstrumentLogger.h"

CString globFileName;
IMPLEMENT_DYNAMIC (CRHMmainDlg, CDialogEx) 
#define MAX_CFileDialog_FILE_COUNT 99
#define FILE_LIST_BUFFER_SIZE ((MAX_CFileDialog_FILE_COUNT * (MAX_PATH + 1)) + 1)

CRHMmainDlg :: CRHMmainDlg (CWnd * pParent): CDialogEx (CRHMmainDialog, pParent)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::CRHMmainDlg(CWnd * pParent): CDialogEx (CRHMmainDialog, pParent)@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    test -> FormCreate ();
    FormActivate ();
    if (ProjectFileArgument.length () > 0)
    {
        test -> DoPrjOpen (ProjectFileArgument, "");
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::CRHMmainDlg(CWnd * pParent): CDialogEx (CRHMmainDialog, pParent)@@@CRHMmainDlg.cpp>");
}
CRHMmainDlg :: CRHMmainDlg (string argumentfile)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::CRHMmainDlg(string argumentfile)@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    test -> FormCreate ();
    test -> DoPrjOpen (argumentfile, "");
    test -> RunClick ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::CRHMmainDlg(string argumentfile)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: ProcessCommandLineArgument ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::ProcessCommandLineArgument()@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    test -> FormCreate ();
    test -> DoPrjOpen (ProjectFileArgument, "");
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::ProcessCommandLineArgument()@@@CRHMmainDlg.cpp>");
}
CRHMmainDlg :: ~CRHMmainDlg ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::~CRHMmainDlg()@@@CRHMmainDlg.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::~CRHMmainDlg()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: DoDataExchange (CDataExchange * pDX)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::DoDataExchange(CDataExchange * pDX)@@@CRHMmainDlg.cpp>");
    CDialogEx :: DoDataExchange (pDX);
    DDX_Control (pDX, IDC_LIST5, listbox_varaiables_all);
    DDX_Control (pDX, IDC_LIST3, list_box_variables_selected);
    DDX_Control (pDX, IDC_LIST2, listbox_observations);
    DDX_Control (pDX, IDC_TCHART1, tchart);
    DDX_Control (pDX, IDC_DATETIMEPICKER2, DateTimePicker1);
    DDX_Control (pDX, IDC_DATETIMEPICKER1, DateTimePicker2);
    DDX_Control (pDX, IDC_LIST4, listbox_observations_selected);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::DoDataExchange(CDataExchange * pDX)@@@CRHMmainDlg.cpp>");
}BEGIN_MESSAGE_MAP (CRHMmainDlg, CDialogEx)
    ON_COMMAND (ID_FILE_OPEN, & CRHMmainDlg :: OnFileOpen)
    ON_BN_CLICKED (IDC_BUTTON5, & CRHMmainDlg :: OnBnClickedButton5)
    ON_BN_CLICKED (IDC_BUTTON6, & CRHMmainDlg :: OnBnClickedButton6)
    ON_BN_CLICKED (IDC_BUTTON7, & CRHMmainDlg :: OnBnClickedButton7)
    ON_BN_CLICKED (IDC_BUTTON8, & CRHMmainDlg :: OnBnClickedButton8)
    ON_LBN_SELCHANGE (IDC_LIST5, & CRHMmainDlg :: OnLbnSelchangeList5)
    ON_LBN_SELCHANGE (IDC_LIST2, & CRHMmainDlg :: OnLbnSelchangeList2)
    ON_COMMAND (ID_BUILD_CONSTRUCT, & CRHMmainDlg :: OnBuildConstruct)
    ON_COMMAND (ID_RUN_RUNMODEL, & CRHMmainDlg :: OnRunRunmodel)
    ON_COMMAND (ID_FILE_SAVE_AS, & CRHMmainDlg :: OnFileSaveAs)
    ON_BN_CLICKED (IDC_BUTTON9, & CRHMmainDlg :: OnBnClickedButton9)
    ON_COMMAND (ID_FILE_SAVE, & CRHMmainDlg :: OnFileSave)
    ON_COMMAND (ID_FILE_CLOSE, & CRHMmainDlg :: OnFileClose)
    ON_LBN_DBLCLK (IDC_LIST3, & CRHMmainDlg :: OnLbnDblclkList3)
    ON_LBN_DBLCLK (IDC_LIST4, & CRHMmainDlg :: OnLbnDblclkList4)
    ON_BN_CLICKED (IDC_BUTTON3, & CRHMmainDlg :: OnBnClickedButton3)
    ON_COMMAND (ID_EDIT_UNDO, & CRHMmainDlg :: OnOpenObservation)
    ON_COMMAND (ID_EDIT_CUT, & CRHMmainDlg :: OnCloseAllObservations)
    ON_COMMAND (ID_BUILD_MACRO, & CRHMmainDlg :: OnBuildMacro)
    ON_COMMAND (ID_HELP_ABOUT, & CRHMmainDlg :: OnHelpAbout)
    ON_COMMAND (ID_FLOWDIAGRAMS_SHOWDIAGRAM, & CRHMmainDlg :: OnFlowdiagramsShowdiagram)
    ON_COMMAND (ID_Report, & CRHMmainDlg :: OnReport)
    ON_COMMAND (ID_PROJECT_EXIT32832, & CRHMmainDlg :: OnProjectExit32832)
END_MESSAGE_MAP ()

void CRHMmainDlg :: OnFileOpen ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnFileOpen()@@@CRHMmainDlg.cpp>");
    CFile theFile;
    TCHAR szFilters [] = _T ("MyType Files (*.prj)|*.prj|All Files (*.*)|*.*||");
    CString fileName;
    wchar_t * p = fileName.GetBuffer (FILE_LIST_BUFFER_SIZE);
    CFileDialog fileDlg (TRUE, _T ("prj"), _T ("*.prj"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
    CRHMmain * test = CRHMmain :: getInstance ();
    if (fileDlg.DoModal () == IDOK)
    {
        CString filepath = fileDlg.GetPathName ();
        CString filename = fileDlg.GetFileName ();
        globFileName = filename;
        string file_p = CT2A (filepath.GetString ());
        string file_n = CT2A (filename.GetString ());
        OpenProject (file_p, file_n);
        AddObservationsOnOpenProject ();
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnFileOpen()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OpenProject (string filepath, string filename)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OpenProject(string filepath, string filename)@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    test -> DoPrjOpen (filepath, filename);
    loadGuiComponents ();
    std :: string str1 = "The Cold Regions Hydrological Model - ";
    str1.append (filepath);
    CString cFilePath (str1.c_str ());
    if (filepath == defaultprojectpath)
    {
        SetWindowText (L"The Cold Regions Hydrological Model");
    }

    else
    {
        SetWindowText (cFilePath);
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OpenProject(string filepath, string filename)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: loadGuiComponents ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::loadGuiComponents()@@@CRHMmainDlg.cpp>");
    listbox_varaiables_all.ResetContent ();
    list_box_variables_selected.ResetContent ();
    listbox_observations.ResetContent ();
    listbox_observations_selected.ResetContent ();
    std :: string s = "Hello";
    LPSTR pst = & s [0];
    LPCSTR pcstr = s.c_str ();
    CRHMmain * crhm_core = CRHMmain :: getInstance ();
    TStringList * variables = CRHMmain :: getInstance () -> getVariables ();
    for (int ii = 0; ii < variables -> Count; ii ++) {
        std :: string s = variables -> Strings [ii];
        CString cvariables (s.c_str ());
        if (s != "t")
        {
            listbox_varaiables_all.AddString (cvariables);
        }

    }
    TStringList * observations = CRHMmain :: getInstance () -> getObservations ();
    for (int ii = 0; ii < observations -> Count; ii ++) {
        std :: string s = observations -> Strings [ii];
        CString observations (s.c_str ());
        listbox_observations.AddString (observations);
    }
    TStringList * sel_variables = CRHMmain :: getInstance () -> getSelectedVariables ();
    for (int ii = 0; ii < sel_variables -> Count; ii ++) {
        std :: string s = sel_variables -> Strings [ii];
        CString observations (s.c_str ());
        list_box_variables_selected.AddString (observations);
    }
    TStringList * sel_observations = CRHMmain :: getInstance () -> getSelectedObservations ();
    for (int ii = 0; ii < sel_observations -> Count; ii ++) {
        std :: string s = sel_observations -> Strings [ii];
        CString observations (s.c_str ());
        listbox_observations_selected.AddString (observations);
    }
    CRHMmain * test = CRHMmain :: getInstance ();
    SetTime (test -> GetStartDate (), test -> GetEndDate ());
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::loadGuiComponents()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: SetTime (double startDate, double endDate)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::SetTime(double startDate, double endDate)@@@CRHMmainDlg.cpp>");
    COleDateTime oletimeTime (startDate);
    VERIFY (DateTimePicker1.SetTime (oletimeTime));
    COleDateTime oletimeTime2 (endDate);
    VERIFY (DateTimePicker2.SetTime (oletimeTime2));
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::SetTime(double startDate, double endDate)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: FormActivate ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::FormActivate()@@@CRHMmainDlg.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::FormActivate()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBnClickedButton5 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBnClickedButton5()@@@CRHMmainDlg.cpp>");
    CString str, str2;
    int hru_value = 0;
    GetDlgItemText (IDC_EDIT1, str);
    hru_value = _ttoi (str);
    if (str.Trim ().GetLength () > 0)
    {
        if (hru_value > 0)
        {
            hru_value = _ttoi (str) - 1;
            str2.Format (_T ("%d"), hru_value);
            SetDlgItemText (IDC_EDIT1, str2);
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBnClickedButton5()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBnClickedButton6 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBnClickedButton6()@@@CRHMmainDlg.cpp>");
    CString str, str2;
    int hru_value = 1;
    GetDlgItemText (IDC_EDIT1, str);
    if (str.Trim ().GetLength () > 0)
    {
        hru_value = _ttoi (str) + 1;
    }

    str2.Format (_T ("%d"), hru_value);
    SetDlgItemText (IDC_EDIT1, str2);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBnClickedButton6()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBnClickedButton7 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBnClickedButton7()@@@CRHMmainDlg.cpp>");
    CString str, str2;
    int hru_value = 0;
    GetDlgItemText (IDC_EDIT2, str);
    hru_value = _ttoi (str);
    if (str.Trim ().GetLength () > 0)
    {
        if (hru_value > 0)
        {
            hru_value = _ttoi (str) - 1;
            str2.Format (_T ("%d"), hru_value);
            SetDlgItemText (IDC_EDIT2, str2);
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBnClickedButton7()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBnClickedButton8 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBnClickedButton8()@@@CRHMmainDlg.cpp>");
    CString str, str2;
    int hru_value = 1;
    GetDlgItemText (IDC_EDIT2, str);
    if (str.Trim ().GetLength () > 0)
    {
        hru_value = _ttoi (str) + 1;
    }

    str2.Format (_T ("%d"), hru_value);
    SetDlgItemText (IDC_EDIT2, str2);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBnClickedButton8()@@@CRHMmainDlg.cpp>");
}
BOOL CRHMmainDlg :: OnInitDialog ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnInitDialog()@@@CRHMmainDlg.cpp>");
    CDialogEx :: OnInitDialog ();
    std :: string str1 = "The Cold Regions Hydrological Model";
    CString cStr (str1.c_str ());
    SetDlgItemText (IDC_EDIT1, _T ("1"));
    SetDlgItemText (IDC_EDIT2, _T ("3"));

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnInitDialog()@@@CRHMmainDlg.cpp>");
    return TRUE;
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnInitDialog()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnLbnSelchangeList5 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnLbnSelchangeList5()@@@CRHMmainDlg.cpp>");
    CString str;
    CString cstr;
    int index = listbox_varaiables_all.GetCurSel ();
    listbox_varaiables_all.GetText (index, str);
    CT2A vstr4 (str);
    string vstr = vstr4;
    int count = 0;
    for (int i = 0; i < list_box_variables_selected.GetCount (); i ++) {
        list_box_variables_selected.GetText (i, cstr);
        CT2A s (cstr);
        string str1 = s;
        int ind = str1.find (vstr + "(");
        if (ind > - 1)
        {
            count ++;
        }

    }
    count ++;
    string vstr2 (vstr);
    string vstr1 = vstr2 + "(" + to_string (count) + ")";
    CString cstr4 (vstr1.c_str ());
    list_box_variables_selected.AddString (cstr4);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnLbnSelchangeList5()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: AddObservationsOnOpenProject ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::AddObservationsOnOpenProject()@@@CRHMmainDlg.cpp>");
    CString cstr;
    int c = listbox_observations_selected.GetCount ();
    for (int i = 0; i < c; i ++) {
        listbox_observations_selected.GetText (i, cstr);
        CT2A str (cstr);
        string str2 = str;
        AddObservation (str2);
    }
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddObservationsOnOpenProject()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: AddObservation (string obsstr)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::AddObservation(string obsstr)@@@CRHMmainDlg.cpp>");
    CString str (obsstr.c_str ());
    int got = 0;
    for (int i = 0; i < observationsFilesCount; i ++) {
        for (int j = 0; j < observationsFiles [i] -> Count; j ++) {
            CString str1 (observationsFiles [i] -> Strings [j].c_str ());
            if (str1 == str)
            {
                CRHMmain * test = CRHMmain :: getInstance ();
                char * filepath = const_cast < char * > (observationsFiles [i] -> FilePath.c_str ());
                char * obsname = const_cast < char * > (observationsFiles [i] -> Strings [j].c_str ());
                AddListBox2Click ();
                got = 1;
                break;
            }

        }
        if (got == 1)
        {
            break;
        }

    }
    if (got == 0)
    {
        CRHMmain * test = CRHMmain :: getInstance ();
        CT2A ss (str);
        string ss2 = ss;
        char * t = const_cast < char * > (ss2.c_str ());
        char * t2 = const_cast < char * > (test -> OpenNameObs.c_str ());
        CString cstr;
        int count = listbox_observations.GetCount ();
        int selindex = 0;
        for (int i = 0; i < count; i ++) {
            listbox_observations.GetText (i, cstr);
            CT2A obsname (cstr);
            string obsname2 = obsname;
            obsname2 = obsname2 + "(1)";
            if (obsname2 == obsstr)
            {
                listbox_observations.SetCurSel (i);
                selindex = i;
                break;
            }

        }
        AddListBox2Click ();
        listbox_observations.SetSel (selindex, false);
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddObservation(string obsstr)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnLbnSelchangeList2 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnLbnSelchangeList2()@@@CRHMmainDlg.cpp>");
    CString str;
    CString cstr;
    CString extension ("(1)");
    int index = listbox_observations.GetCurSel ();
    listbox_observations.GetText (index, str);
    str.Append (extension);
    CT2A vstr4 (str);
    string vstr = vstr4;
    int count = 0;
    for (int i = 0; i < listbox_observations_selected.GetCount (); i ++) {
        listbox_observations_selected.GetText (i, cstr);
        CT2A s (cstr);
        string str1 = s;
        if (str1 == vstr)
        {
            count = 1;
            break;
        }

    }
    if (count == 0)
    {
        listbox_observations_selected.AddString (str);
        int got = 0;
        for (int i = 0; i < observationsFilesCount; i ++) {
            for (int j = 0; j < observationsFiles [i] -> Count; j ++) {
                CString str1 (observationsFiles [i] -> Strings [j].c_str ());
                if (str1 == str)
                {
                    CRHMmain * test = CRHMmain :: getInstance ();
                    char * filepath = const_cast < char * > (observationsFiles [i] -> FilePath.c_str ());
                    char * obsname = const_cast < char * > (observationsFiles [i] -> Strings [j].c_str ());
                    AddListBox2Click ();
                    got = 1;
                    break;
                }

            }
            if (got == 1)
            {
                break;
            }

        }
        if (got == 0)
        {
            CRHMmain * test = CRHMmain :: getInstance ();
            CT2A ss (str);
            string ss2 = ss;
            char * t = const_cast < char * > (ss2.c_str ());
            char * t2 = const_cast < char * > (test -> OpenNameObs.c_str ());
            AddListBox2Click ();
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnLbnSelchangeList2()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBuildConstruct ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBuildConstruct()@@@CRHMmainDlg.cpp>");
    CConstruct * build_form;
    TStringList * all_module_list = CRHMmain :: getInstance () -> getAllmodules ();
    build_form = new CConstruct ();
    INT_PTR nRet = build_form -> DoModal ();
    if (build_form -> Execute)
    {
        InitModules ();
        build_form -> ReadBuildParameters ();
        dirty = true;
        loadGuiComponents ();
        OpenObservation (defaultobservationpath);
    }

    globFileName = defaultprojectpath.c_str ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBuildConstruct()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: InitModules ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::InitModules()@@@CRHMmainDlg.cpp>");
    Global :: BuildFlag = TBuild :: DECL;
    Box1Disply = (int) TVISIBLE :: OUTPUT;
    for (Global :: CurrentModuleRun = 0; Global :: CurrentModuleRun < Global :: OurModulesList -> Count; Global :: CurrentModuleRun ++) {
        ((ClassModule *) Global :: OurModulesList -> Objects [Global :: CurrentModuleRun]) -> nhru = Global :: nhru;
        ((ClassModule *) Global :: OurModulesList -> Objects [Global :: CurrentModuleRun]) -> decl ();
    }
    GetAllVariables ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::InitModules()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: GetAllVariables ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::GetAllVariables()@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    CConstruct * buildform = new CConstruct ();
    ClassVar * thisVar;
    MapVar :: iterator itVar;
    string Labels [] = {"Variables", "Variables by Module", "Diagnostic Variables", "Private Variables"};
    string Newname;
    ++ Box1Disply;
    if (Box1Disply > (int) TVISIBLE :: PRIVATE)
    {
        Box1Disply = (int) TVISIBLE :: OUTPUT;
    }

    test -> AllVariables -> Clear ();
    if (Box1Disply == (int) TVISIBLE :: OUTPUT)
    {
        test -> AllVariables -> Sorted = true;
        for (itVar = Global :: MapVars.begin (); itVar != Global :: MapVars.end (); itVar ++) {
            thisVar = (* itVar).second;
            if (thisVar -> varType < TVar :: Read && thisVar -> visibility == TVISIBLE :: USUAL && thisVar -> dimen != TDim :: NREB && (thisVar -> values || thisVar -> ivalues) && ! thisVar -> FileData)
            {
                Newname = DeclObsName (thisVar);
                if (buildform -> IndexOf (test -> AllVariables, Newname) == - 1)
                {
                    test -> AllVariables -> AddObject (Newname, (TObject *) thisVar);
                }

            }

        }
    }

    else
    {
        test -> AllVariables -> Sorted = false;
        string S0;
        for (int ii = 0; ii < Global :: OurModulesList -> Count; ii ++) {
            string S = Global :: OurModulesList -> Strings [ii];
            ClassModule * thisModule = (ClassModule *) Global :: OurModulesList -> Objects [ii];
            if (S0 != S)
            {
                if (! (S0.length () == 0))
                {
                    test -> AllVariables -> Add (" ");
                }

                S0 = S;
                if (thisModule -> variation != 0)
                {
                    string AA ("#0");
                    AA [1] += (char) (log (thisModule -> variation) / log (2) + 1);
                    S0 += AA;
                }

                S0 = "   --- " + S0;
                test -> AllVariables -> AddObject (S0 + " ---", Global :: OurModulesList -> Objects [ii]);
            }

            switch (Box1Disply) {
            case (int) TVISIBLE :: USUAL:
                for (itVar = Global :: MapVars.begin (); itVar != Global :: MapVars.end (); itVar ++) {
                    thisVar = (* itVar).second;
                    if (S == thisVar -> module.c_str () && thisVar -> visibility == TVISIBLE :: USUAL && Variation_Decide (thisVar -> variation_set, thisModule -> variation) && thisVar -> dimen != TDim :: NREB && (thisVar -> values || thisVar -> ivalues) && ! thisVar -> FileData)
                    {
                        Newname = DeclObsName (thisVar);
                        if (buildform -> IndexOf (test -> AllVariables, Newname) == - 1)
                        {
                            test -> AllVariables -> AddObject (Newname, (TObject *) thisVar);
                        }

                    }

                }
                break;
            case (int) TVISIBLE :: DIAGNOSTIC:
                for (itVar = Global :: MapVars.begin (); itVar != Global :: MapVars.end (); itVar ++) {
                    thisVar = (* itVar).second;
                    if (S == thisVar -> module.c_str () && thisVar -> visibility == TVISIBLE :: DIAGNOSTIC && thisVar -> dimen != TDim :: NREB && (thisVar -> values || thisVar -> ivalues) && ! thisVar -> FileData)
                    {
                        Newname = DeclObsName (thisVar);
                        if (buildform -> IndexOf (test -> AllVariables, Newname) == - 1)
                        {
                            test -> AllVariables -> AddObject (Newname, (TObject *) thisVar);
                        }

                    }

                }
                break;
            case (int) TVISIBLE :: PRIVATE:
                for (itVar = Global :: MapVars.begin (); itVar != Global :: MapVars.end (); itVar ++) {
                    thisVar = (* itVar).second;
                    if (S == thisVar -> module.c_str () && thisVar -> visibility == TVISIBLE :: PRIVATE && thisVar -> dimen != TDim :: NREB && (thisVar -> values || thisVar -> ivalues) && ! thisVar -> FileData)
                    {
                        Newname = DeclObsName (thisVar);
                        if (buildform -> IndexOf (test -> AllVariables, Newname) == - 1)
                        {
                            test -> AllVariables -> AddObject (Newname, (TObject *) thisVar);
                        }

                    }

                }
                break;
            default:
                break;
            }
        }
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::GetAllVariables()@@@CRHMmainDlg.cpp>");
}
string CRHMmainDlg :: DeclObsName (ClassVar * thisVar)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::DeclObsName(ClassVar * thisVar)@@@CRHMmainDlg.cpp>");
    string Newname = thisVar -> name.c_str ();
    int jj = Global :: OurModulesList -> IndexOf (thisVar -> module.c_str ());
    int found = Newname.find ("#");
    if (jj > - 1 && found > 0)
    {
        ClassModule * thisModule = (ClassModule *) Global :: OurModulesList -> Objects [jj];
        if (thisModule -> isGroup)
        {
            string AA;
            Common :: GroupEnding (AA, thisModule -> GroupCnt);
            Newname += AA;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::DeclObsName(ClassVar * thisVar)@@@CRHMmainDlg.cpp>");
    return Newname;
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::DeclObsName(ClassVar * thisVar)@@@CRHMmainDlg.cpp>");
}
bool CRHMmainDlg :: Variation_Decide (int Variation_set, long Variation)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::Variation_Decide(int Variation_set, long Variation)@@@CRHMmainDlg.cpp>");
    VandP V;
    V.Set (Variation);
    long variations = V.GetV ();
    if ((Variation_set & 2048) != 0 && variations == 0 || (Variation_set & 4096) != 0 || (Variation_set == 0) || (variations & Variation_set) != 0)
    {

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::Variation_Decide(int Variation_set, long Variation)@@@CRHMmainDlg.cpp>");
        return true;
    }

    else
    {

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::Variation_Decide(int Variation_set, long Variation)@@@CRHMmainDlg.cpp>");
        return false;
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::Variation_Decide(int Variation_set, long Variation)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnRunRunmodel ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnRunRunmodel()@@@CRHMmainDlg.cpp>");
    CRHMmain * crhmmain = CRHMmain :: getInstance ();
    TStringList * tslist = crhmmain -> getSelectedObservations ();
    SaveProject ();
    RunClickFunction ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnRunRunmodel()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: RunClickFunction ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::RunClickFunction()@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    if (test -> ObsFilesList -> Count == 0)
    {
        MessageBox (_T ("Please open an observation file."));

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::RunClickFunction()@@@CRHMmainDlg.cpp>");
        return;
    }

    MMSData * mmsdata = test -> RunClick2Start ();
    int seriesCount = test -> SeriesCnt;
    if (seriesCount == 0)
    {

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::RunClickFunction()@@@CRHMmainDlg.cpp>");
        return;
    }

    CSeries series [1000];
    for (int i = 0; i < seriesCount; i ++) {
        string s = test -> getSelectedVariables () -> Strings [i];
        CString cs (s.c_str ());
        int removed = 0;
        int cnt = tchart.get_SeriesCount ();
        for (int j = 0; j < cnt; j ++) {
            if (tchart.SeriesTitleLegend (j) == cs)
            {
                tchart.RemoveSeries (j);
                removed = 1;
                break;
            }

        }
        tchart.AddSeries (0);
        if (removed == 1)
        {
            series [i] = tchart.Series (cnt - 1);
        }

        else
        {
            series [i] = tchart.Series (cnt);
        }

        series [i].put_LegendTitle (cs);
    }
    string values = "";
    int seriesIndex = 0;
    int TotalCount = 0;
    int pcount = 0;
    int n = 0;
    for (int indx = Global :: DTmin; indx < Global :: DTmax; indx = indx + 500) {
        int next = indx + 500;
        if (next >= Global :: DTmax)
        {
            test -> RunClick2Middle (mmsdata, indx, Global :: DTmax);
        }

        else
        {
            test -> RunClick2Middle (mmsdata, indx, indx + 500);
        }

        TotalCount = test -> cdSeries [0] -> Count ();
        for (int i = pcount; i < TotalCount; i ++) {
            for (int j = 0; j < seriesCount; j ++) {
                int y, m, d, h, mi;
                StandardConverterUtility :: GetDateTimeElements (test -> cdSeries [j] -> XValues [i], & y, & m, & d, & h, & mi);
                string dt = to_string (m) + "/" + to_string (d) + "/" + to_string (y);
                CString str (dt.c_str ());
                LPCTSTR dtstr = (LPCTSTR) str;
                series [j].AddXY (test -> cdSeries [j] -> XValues [i], test -> cdSeries [j] -> YValues [i], dtstr, series [j].get_Color ());
            }
            n ++;
            if (n % 500 == 0)
            {
                tchart.Repaint ();
            }

        }
        pcount = TotalCount;
    }
    tchart.Repaint ();
    test -> RunClick2End (mmsdata);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::RunClickFunction()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: AddSeriesToTChart (TSeries * tseries)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::AddSeriesToTChart(TSeries * tseries)@@@CRHMmainDlg.cpp>");
    CSeries series;
    tchart.AddSeries (0);
    int count = tchart.get_SeriesCount ();
    series = tchart.Series (count - 1);
    CString cstr (tseries -> Title.c_str ());
    series.put_LegendTitle (cstr);
    for (int i = 0; i < tseries -> Used; i ++) {
        int y, m, d, h, mi;
        StandardConverterUtility :: GetDateTimeElements (tseries -> XValues [i], & y, & m, & d, & h, & mi);
        string dt = to_string (m) + "/" + to_string (d) + "/" + to_string (y);
        CString str (dt.c_str ());
        LPCTSTR dtstr = (LPCTSTR) str;
        series.AddXY (tseries -> XValues [i], tseries -> YValues [i], dtstr, series.get_Color ());
    }
    tchart.Repaint ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddSeriesToTChart(TSeries * tseries)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: AddSeriesToTChart ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::AddSeriesToTChart()@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    CSeries series;
    tchart.AddSeries (0);
    int count = tchart.get_SeriesCount ();
    series = tchart.Series (count - 1);
    CString cstr (test -> observationseries -> Title.c_str ());
    series.put_LegendTitle (cstr);
    for (int i = 0; i < test -> observationseries -> Count (); i ++) {
        int y, m, d, h, mi;
        StandardConverterUtility :: GetDateTimeElements (test -> observationseries -> XValues [i], & y, & m, & d, & h, & mi);
        string dt = to_string (m) + "/" + to_string (d) + "/" + to_string (y);
        CString str (dt.c_str ());
        LPCTSTR dtstr = (LPCTSTR) str;
        series.AddXY (test -> observationseries -> XValues [i], test -> observationseries -> YValues [i], dtstr, series.get_Color ());
    }
    tchart.Repaint ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddSeriesToTChart()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnFileSaveAs ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnFileSaveAs()@@@CRHMmainDlg.cpp>");
    TCHAR szFilters [] = _T ("MyType Files (*.prj)|*.prj|All Files (*.*)|*.*||");
    CRHMAboutBox * test;
    test = new CRHMAboutBox ();
    CString fileName;
    wchar_t * p = fileName.GetBuffer (FILE_LIST_BUFFER_SIZE);
    CFileDialog fileDlg (FALSE, _T ("prj"), _T ("*.prj"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
    INT_PTR result = fileDlg.DoModal ();
    if (result == IDOK)
    {
        CRHMmain * crhmmain = CRHMmain :: getInstance ();
        COleDateTime time1;
        DateTimePicker1.GetTime (time1);
        crhmmain -> setStartDate (time1);
        COleDateTime time2;
        DateTimePicker2.GetTime (time2);
        crhmmain -> setEndDate (time2);
        TStringList * selected_variables = GetSelectedVariablesFromListBox (IDC_LIST3);
        TStringList * selected_observations = GetSelectedObservationsFromListBox (IDC_LIST4);
        crhmmain -> setSelectedVariables (selected_variables);
        crhmmain -> setSelectedObservatoions (selected_observations);
        fileName = fileDlg.GetPathName ();
        string filepath = CT2A (fileName.GetString ());
        crhmmain -> SaveProject ("Description - to be added", filepath);
        if (result == IDOK)
        {
            MessageBox (_T ("Your project has been saved."));
        }

        globFileName = "";
        CloseProject ();
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnFileSaveAs()@@@CRHMmainDlg.cpp>");
}
TStringList * CRHMmainDlg :: GetSelectedVariablesFromListBox (int listboxid)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::GetSelectedVariablesFromListBox(int listboxid)@@@CRHMmainDlg.cpp>");
    CRHMmain * t = CRHMmain :: getInstance ();
    TStringList * list = new TStringList ();
    CListBox * listbox = (CListBox *) GetDlgItem (listboxid);
    int itemcount = listbox -> GetCount ();
    for (int i = 0; i < itemcount; i ++) {
        CString cstr;
        listbox -> GetText (i, cstr);
        CT2A str (cstr);
        string s = str;
        TObject * obj = t -> GetObjectOfVariable (s);
        list -> AddObject (s, obj);
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::GetSelectedVariablesFromListBox(int listboxid)@@@CRHMmainDlg.cpp>");
    return list;
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::GetSelectedVariablesFromListBox(int listboxid)@@@CRHMmainDlg.cpp>");
}
TStringList * CRHMmainDlg :: GetSelectedObservationsFromListBox (int listboxid)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::GetSelectedObservationsFromListBox(int listboxid)@@@CRHMmainDlg.cpp>");
    CRHMmain * t = CRHMmain :: getInstance ();
    TStringList * list = new TStringList ();
    CListBox * listbox = (CListBox *) GetDlgItem (listboxid);
    int itemcount = listbox -> GetCount ();
    for (int i = 0; i < itemcount; i ++) {
        CString cstr;
        listbox -> GetText (i, cstr);
        CT2A str (cstr);
        string s = str;
        TObject * obj = t -> GetObjectOfObservation (s);
        list -> AddObject (s, obj);
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::GetSelectedObservationsFromListBox(int listboxid)@@@CRHMmainDlg.cpp>");
    return list;
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::GetSelectedObservationsFromListBox(int listboxid)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: SetItemsToListBox (int listboxid, TStringList * list)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::SetItemsToListBox(int listboxid, TStringList * list)@@@CRHMmainDlg.cpp>");
    CListBox * listbox = (CListBox *) GetDlgItem (listboxid);
    int count = list -> Count;
    for (int i = 0; i < count; i ++) {
        string str = list -> array [i].get_Name ();
        CString cstr (str.c_str ());
        listbox -> AddString (cstr);
    }
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::SetItemsToListBox(int listboxid, TStringList * list)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBnClickedButton9 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBnClickedButton9()@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    test -> GetObservationData ("Badlake73_76.obs", "rh");
    AddSeriesToTChart ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBnClickedButton9()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnFileSave ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnFileSave()@@@CRHMmainDlg.cpp>");
    SaveProject ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnFileSave()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: SaveProject ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::SaveProject()@@@CRHMmainDlg.cpp>");
    CRHMmain * crhmmain = CRHMmain :: getInstance ();
    COleDateTime time1;
    DateTimePicker1.GetTime (time1);
    crhmmain -> setStartDate (time1);
    COleDateTime time2;
    DateTimePicker2.GetTime (time2);
    crhmmain -> setEndDate (time2);
    TStringList * selected_variables = GetSelectedVariablesFromListBox (IDC_LIST3);
    TStringList * selected_observations = GetSelectedObservationsFromListBox (IDC_LIST4);
    crhmmain -> setSelectedVariables (selected_variables);
    crhmmain -> setSelectedObservatoions (selected_observations);
    if (crhmmain -> OpenProjectPath.length () == 0)
    {
        crhmmain -> OpenProjectPath = defaultprojectpath;
    }

    crhmmain -> SaveProject ("Description - to be added", crhmmain -> OpenProjectPath);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::SaveProject()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnFileClose ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnFileClose()@@@CRHMmainDlg.cpp>");
    CString filename = globFileName;
    CRHMmain * t = CRHMmain :: getInstance ();
    if (listbox_varaiables_all.GetCount () == 0 && list_box_variables_selected.GetCount () == 0 && listbox_observations.GetCount () == 0 && listbox_observations_selected.GetCount () == 0)
    {
    }

    else
    {
        if (t -> OpenProjectPath == defaultprojectname)
        {
            const int result = MessageBox (L"Do you want to save this project?", L"CRHM_GUI", MB_YESNOCANCEL);
            if (result == IDYES)
            {
                OnFileSaveAs ();
            }

            if (result == IDCANCEL)
            {

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnFileClose()@@@CRHMmainDlg.cpp>");
                return;
            }

        }

    }

    CloseProject ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnFileClose()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: CloseProject ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::CloseProject()@@@CRHMmainDlg.cpp>");
    CRHMmain * t = CRHMmain :: getInstance ();
    listbox_varaiables_all.ResetContent ();
    ASSERT (listbox_varaiables_all.GetCount () == 0);
    t -> AllVariables -> Clear ();
    list_box_variables_selected.ResetContent ();
    ASSERT (list_box_variables_selected.GetCount () == 0);
    t -> SelectedVariables -> Clear ();
    listbox_observations.ResetContent ();
    ASSERT (listbox_observations.GetCount () == 0);
    t -> AllObservations -> Clear ();
    listbox_observations_selected.ResetContent ();
    ASSERT (listbox_observations_selected.GetCount () == 0);
    t -> SelectedObservations -> Clear ();
    COleDateTime oletimeTime (StandardConverterUtility :: GetCurrentDateTime ());
    VERIFY (DateTimePicker1.SetTime (oletimeTime));
    VERIFY (DateTimePicker2.SetTime (oletimeTime));
    t -> FormDestroy ();
    t -> FormCreate ();
    this -> EndDialog (1);
    globFileName = "";
    t -> OpenProjectPath = "";
    CRHMmainDlg maindlg = new CRHMmainDlg ();
    maindlg.DoModal ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::CloseProject()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: ReopenProject ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::ReopenProject()@@@CRHMmainDlg.cpp>");
    SaveProject ();
    CRHMmain * t = CRHMmain :: getInstance ();
    if (t -> OpenProjectPath.length () == 0)
    {
        OpenProject (defaultprojectpath, defaultprojectname);
    }

    else
    {
        OpenProject (t -> OpenProjectPath, t -> OpenProjectPath);
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::ReopenProject()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnLbnDblclkList3 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnLbnDblclkList3()@@@CRHMmainDlg.cpp>");
    CString str;
    CString cstr;
    string svariable;
    int index = list_box_variables_selected.GetCurSel ();
    list_box_variables_selected.GetText (index, str);
    CT2A vstr4 (str);
    string vstr = vstr4;
    int scount = tchart.get_SeriesCount ();
    for (int i = 0; i < scount; i ++) {
        CString title = tchart.SeriesTitleLegend (i);
        if (title == str)
        {
            tchart.RemoveSeries (i);
            break;
        }

    }
    int c = list_box_variables_selected.GetCount ();
    for (int i = c - 1; i >= 0; i --) {
        list_box_variables_selected.GetText (i, cstr);
        CT2A s (cstr);
        string str1 = s;
        if (str1.c_str () == str)
        {
            list_box_variables_selected.DeleteString (i);
            break;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnLbnDblclkList3()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnLbnDblclkList4 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnLbnDblclkList4()@@@CRHMmainDlg.cpp>");
    CString str;
    CString cstr;
    string svariable;
    int index = listbox_observations_selected.GetCurSel ();
    listbox_observations_selected.GetText (index, str);
    CT2A vstr4 (str);
    string vstr = vstr4;
    int scount = tchart.get_SeriesCount ();
    for (int i = 0; i < scount; i ++) {
        CString title = tchart.SeriesTitleLegend (i);
        if (title == str)
        {
            tchart.RemoveSeries (i);
            break;
        }

    }
    int c = listbox_observations_selected.GetCount ();
    for (int i = c - 1; i >= 0; i --) {
        listbox_observations_selected.GetText (i, cstr);
        CT2A s (cstr);
        string str1 = s;
        if (str1.c_str () == str)
        {
            listbox_observations_selected.DeleteString (i);
            break;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnLbnDblclkList4()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBnClickedButton3 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBnClickedButton3()@@@CRHMmainDlg.cpp>");
    CSeries series;
    bool active;
    int count = tchart.get_SeriesCount ();
    for (int i = 0; i < count; i ++) {
        series = tchart.Series (i);
        active = series.get_Active ();
        series.put_Active (! active);
    }
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBnClickedButton3()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnOpenObservation ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnOpenObservation()@@@CRHMmainDlg.cpp>");
    CFile theFile;
    TCHAR szFilters [] = _T ("MyType Files (*.obs)|*.obs|All Files (*.*)|*.*||");
    CString fileName;
    wchar_t * p = fileName.GetBuffer (FILE_LIST_BUFFER_SIZE);
    CFileDialog fileDlg (TRUE, _T ("obs"), _T ("*.obs"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
    if (fileDlg.DoModal () == IDOK)
    {
        CString filepath = fileDlg.GetPathName ();
        CString filename = fileDlg.GetFileName ();
        string file_p = CT2A (filepath.GetString ());
        string file_n = CT2A (filename.GetString ());
        Box1Disply = (int) TVISIBLE :: PRIVATE;
        GetAllVariables ();
        OpenObservation (file_p);
    }

    Box1Disply = (int) TVISIBLE :: OUTPUT;
    GetAllVariables ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnOpenObservation()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OpenObservation (string obsfilepath)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OpenObservation(string obsfilepath)@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    test -> OpenObsFile (obsfilepath);
    TStringList * observations = test -> getObservations ();
    for (int i = 0; i < observations -> Count; i ++) {
        CString cstr (observations -> Strings [i].c_str ());
        listbox_observations.AddString (cstr);
    }
    SetTime (test -> GetStartDate (), test -> GetEndDate ());
    SaveProject ();
    ReopenProject ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OpenObservation(string obsfilepath)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnCloseAllObservations ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnCloseAllObservations()@@@CRHMmainDlg.cpp>");
    listbox_observations.ResetContent ();
    listbox_observations_selected.ResetContent ();
    tchart.RemoveAllSeries ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnCloseAllObservations()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: AddListBox2Click ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::AddListBox2Click()@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    int StartCnt = listbox_observations.GetCount ();
    ;
    for (int ii = 0; ii < StartCnt; ++ ii) {
        int selection = listbox_observations.GetCurSel ();
        if (selection > - 1 && selection == ii)
        {
            int Indx = 1;
            int IndxMax = ((ClassVar *) test -> AllObservations -> Objects [ii]) -> cnt;
            if (Indx > IndxMax)
            {
                Indx = IndxMax;
            }

            string S = test -> AllObservations -> Strings [ii] + "(" + to_string (Indx) + ")" + test -> Sstrings [(int) test -> Funct];
            string seriesTitle = S;
            int iii = test -> SelectedObservations -> IndexOf (test -> AllObservations -> Strings [ii]);
            if (iii == - 1)
            {
                TSeries * cdSeries = NULL;
                if (((ClassVar *) test -> AllObservations -> Objects [ii]) -> FileData -> Times == NULL)
                {
                    cdSeries = new TSeries (50000);
                }

                else
                {
                    cdSeries = new TSeries (50000);
                }

                ClassVar * thisVar = (ClassVar *) test -> AllObservations -> Objects [ii];
                cdSeries -> Tag = thisVar;
                cdSeries -> Title = seriesTitle;
                test -> SelectedObservations -> AddObject (S, (TObject *) cdSeries);
                AddObsPlot ((ClassVar *) test -> AllObservations -> Objects [ii], cdSeries, S, test -> Funct);
            }

        }

    }
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddListBox2Click()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: AddDataToSeries (CSeries series, double xvalue, double yvalue)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::AddDataToSeries(CSeries series, double xvalue, double yvalue)@@@CRHMmainDlg.cpp>");
    int y, m, d, h, mi;
    StandardConverterUtility :: GetDateTimeElements (xvalue, & y, & m, & d, & h, & mi);
    string dt = to_string (m) + "/" + to_string (d) + "/" + to_string (y);
    CString str (dt.c_str ());
    LPCTSTR dtstr = (LPCTSTR) str;
    series.AddXY (xvalue, yvalue, dtstr, series.get_Color ());
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddDataToSeries(CSeries series, double xvalue, double yvalue)@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: AddObsPlot (ClassVar * thisVar, TSeries * cdSeries, string S, TFun Funct)
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::AddObsPlot(ClassVar * thisVar, TSeries * cdSeries, string S, TFun Funct)@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    ClassVar * newVar;
    Global :: HRU_OBS = Global :: HRU_OBS_DIRECT;
    double ** Data = thisVar -> FileData -> Data;
    double xx;
    double DTstartR = test -> GetStartDate ();
    double DTendR = test -> GetEndDate ();
    SetTime (DTstartR, DTendR);
    if (DTstartR >= DTendR)
    {

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddObsPlot(ClassVar * thisVar, TSeries * cdSeries, string S, TFun Funct)@@@CRHMmainDlg.cpp>");
        return;
    }

    TDateTime Save_DTnow = Global :: DTnow;
    double MyInterval = thisVar -> FileData -> Interval;
    long DTmin = INT ((DTstartR - Global :: DTstart) * thisVar -> FileData -> Freq) * Global :: Freq / thisVar -> FileData -> Freq;
    long DTmax = INT ((DTendR - Global :: DTstart) * thisVar -> FileData -> Freq) * Global :: Freq / thisVar -> FileData -> Freq;
    long jj1 = S.rfind ("(");
    long jj2 = S.rfind (")");
    long Indx;
    string :: size_type pp;
    pp = thisVar -> name.rfind ('(');
    bool AlreadyIndex = (pp != string :: npos);
    if (test -> ListHruNames && thisVar -> varType < TVar :: Read)
    {
        Indx = test -> ListHruNames -> IndexOf (S.substr (jj1 + 1, jj2 - jj1 - 1)) - 1;
    }

    else
    {
        if (thisVar -> root != "" || AlreadyIndex)
        {
            Indx = 0;
        }

        else
        {
            Indx = stoi (S.substr (jj1 + 1, jj2 - jj1 - 1)) - 1;
        }

    }

    long IndxMin = thisVar -> FileData -> IndxMin;
    long IndxMax = thisVar -> FileData -> IndxMax;
    if (thisVar -> FileData -> Times != NULL)
    {
        if (Global :: Freq == 1)
        {
            -- DTendR;
        }

        double Sum = 0.0;
        for (long ii = 0; ii < thisVar -> FileData -> Lines; ++ ii) {
            if (thisVar -> FileData -> Times [ii] < DTstartR)
            {
                continue;
            }

            if (thisVar -> FileData -> Times [ii] > DTendR)
            {
                continue;
            }

            xx = Data [thisVar -> offset + Indx] [ii];
            if (Funct == TFun :: TOT)
            {
                Sum += xx;
                xx = Sum;
            }

            cdSeries -> AddXY (thisVar -> FileData -> Times [ii], xx);
        }
    }

    else
    if (Funct <= TFun :: MJ_W)
    {
        for (Global :: DTindx = DTmin; Global :: DTindx < DTmax; Global :: DTindx ++) {
            Global :: DTnow = Global :: DTstart + Global :: Interval * Global :: DTindx + Global :: Interval;
            if (MyInterval >= 1)
            {
                -- Global :: DTnow;
            }

            if (Global :: DTindx * thisVar -> FileData -> Freq / Global :: Freq >= IndxMin && Global :: DTindx * thisVar -> FileData -> Freq / Global :: Freq <= IndxMax)
            {
                xx = Data [thisVar -> offset + Indx] [(Global :: DTindx * thisVar -> FileData -> Freq / Global :: Freq - IndxMin)];
                if (xx > 0)
                {
                    int a = 10;
                }

                if (Funct == TFun :: FOBS)
                {
                    ;
                }

                else
                if (Funct == TFun :: VP_SAT)
                {
                    if (xx > 0.0)
                    {
                        xx = 0.611 * exp (17.27 * xx / (xx + 237.3));
                    }

                    else
                    {
                        xx = 0.611 * exp (21.88 * xx / (xx + 265.5));
                    }

                }

                else
                if (Funct == TFun :: W_MJ)
                {
                    xx *= thisVar -> FileData -> Interval * 86400 / 1.0E6;
                }

                else
                if (Funct == TFun :: MJ_W)
                {
                    xx *= 1.0E6 / 86400 / thisVar -> FileData -> Interval;
                }

                cdSeries -> AddXY (Global :: DTnow, xx);
            }

        }
    }

    else
    {
        newVar = new ClassVar (* thisVar);
        newVar -> name = S.c_str ();
        newVar -> FileData -> DataFileName = "Copy";
        string :: size_type pp = thisVar -> units.find_last_of (")");
        if (thisVar -> FileData -> Freq > 1 && (thisVar -> units [pp - 1] == 'd'))
        {
            thisVar -> Daily = TRUE;
        }

        else
        {
            thisVar -> Daily = FALSE;
        }

        if (newVar -> root == "")
        {
            if (thisVar -> FileData -> Freq == 1)
            {
                newVar -> LoopFunct = & ClassVar :: LoopFirst;
            }

            else
            if (thisVar -> Daily)
            {
                newVar -> LoopFunct = & ClassVar :: LoopFirst;
            }

            else
            {
                newVar -> LoopFunct = & ClassVar :: LoopRange;
            }

        }

        else
        {
            if (thisVar -> Daily)
            {
                newVar -> LoopFunct = & ClassVar :: LoopLast;
            }

            else
            {
                newVar -> LoopFunct = & ClassVar :: LoopRange;
            }

        }

        newVar -> FunctVar = thisVar;
        switch (Funct) {
        case TFun :: AVG:
            newVar -> UserFunct_ = & ClassVar :: Tot_;
            newVar -> FunKind = TFun :: AVG;
            break;
        case TFun :: MIN:
            newVar -> UserFunct_ = & ClassVar :: Min_;
            newVar -> FunKind = TFun :: MIN;
            break;
        case TFun :: MAX:
            newVar -> UserFunct_ = & ClassVar :: Max_;
            newVar -> FunKind = TFun :: MAX;
            break;
        case TFun :: TOT:
            newVar -> UserFunct_ = & ClassVar :: Tot_;
            newVar -> FunKind = TFun :: TOT;
            break;
        case TFun :: POS:
            newVar -> UserFunct_ = & ClassVar :: Pos_;
            newVar -> FunKind = TFun :: POS;
            break;
        case TFun :: FIRST:
            newVar -> UserFunct_ = & ClassVar :: First_;
            newVar -> FunKind = TFun :: FIRST;
            newVar -> LoopFunct = & ClassVar :: LoopFirst;
            break;
        case TFun :: LAST:
            newVar -> UserFunct_ = & ClassVar :: Last_;
            newVar -> FunKind = TFun :: LAST;
            newVar -> LoopFunct = & ClassVar :: LoopLast;
            break;
        case TFun :: CNT:
            newVar -> UserFunct_ = & ClassVar :: Count_;
            newVar -> FunKind = TFun :: CNT;
            break;
        case TFun :: CNT0:
            newVar -> UserFunct_ = & ClassVar :: Count0_;
            newVar -> FunKind = TFun :: CNT0;
            break;
        case TFun :: DLTA:
            newVar -> UserFunct_ = & ClassVar :: First_;
            newVar -> LoopFunct = & ClassVar :: LoopFirst;
            newVar -> FunKind = TFun :: DLTA;
            break;
        default:
            break;
        }
        bool First = false;
        long Next = - 1;
        long Days = 0;
        long LastDays = 0;
        long Lastkk = 0;
        long CurrentIndx = - 1;
        long LastIndex = - 1;
        long itime [6];
        long Greatest;
        long DTminX = DTmin;
        if (IndxMin > 0)
        {
            DTminX = IndxMin;
        }

        double Delta0 = 0.0;
        double First0;
        double Temp;
        dattim ("now", itime);
        for (Global :: DTindx = DTmin; Global :: DTindx < DTmax; Global :: DTindx += Global :: Freq) {
            Global :: DTnow = Global :: DTstart + Global :: Interval * Global :: DTindx + Global :: Interval;
            if (Global :: DTindx * Global :: Freq / thisVar -> FileData -> Freq >= IndxMin)
            if (Global :: DTindx * thisVar -> FileData -> Freq / Global :: Freq > IndxMax)
            {
                break;
            }

            else
            {
                if (Global :: Interval >= 1)
                {
                    -- Global :: DTnow;
                }

                dattim ("now", itime);
                switch (TBase) {
                case 0:
                    if (Next == - 1 || Next != itime [2])
                    {
                        Next = itime [2];
                        First = TRUE;
                    }

                    break;
                case 1:
                    if (Next == - 1 || itime [0] == Next && itime [1] == water_year_month)
                    {
                        if (Next == - 1 && itime [1] < water_year_month)
                        {
                            Next = itime [0];
                        }

                        else
                        {
                            Next = itime [0] + 1;
                        }

                        First = TRUE;
                    }

                    break;
                case 2:
                    if (Next == - 1 || itime [0] == Next && itime [1] == 1)
                    {
                        Next = itime [0] + 1;
                        First = TRUE;
                    }

                    break;
                case 3:
                    if (Next == - 1 || Next == itime [1])
                    {
                        Next = (itime [1]) % 12 + 1;
                        First = TRUE;
                    }

                    break;
                case 4:
                    if (Next == - 1)
                    {
                        Next = 0;
                        First = TRUE;
                    }

                default:
                    break;
                }
                CurrentIndx = (Global :: DTindx - DTminX) / thisVar -> FileData -> Freq - 1;
                if (First)
                {
                    if (Global :: DTindx > DTmin && Global :: DTindx > IndxMin)
                    {
                        switch (Funct) {
                        case TFun :: DLTA:
                            Temp = cdSeries -> YValue ((Global :: DTindx - DTmin) / thisVar -> FileData -> Freq - 1);
                            cdSeries -> YValues [CurrentIndx] -= Delta0;
                            Delta0 = Temp;
                        case TFun :: AVG:
                        case TFun :: MIN:
                        case TFun :: MAX:
                        case TFun :: TOT:
                        case TFun :: POS:
                        case TFun :: LAST:
                        case TFun :: CNT:
                        case TFun :: CNT0:
                            if (ObsFunct_Toggle == 1)
                            {
                                for (long jj = LastIndex + 1; jj <= CurrentIndx; ++ jj)
                                    cdSeries -> YValues [jj] = cdSeries -> YValues [CurrentIndx];
                            }

                            break;
                        case TFun :: FIRST:
                            for (long jj = LastIndex + 1; jj <= CurrentIndx; ++ jj)
                                cdSeries -> YValues [jj] = First0;
                            break;
                        default:
                            break;
                        }
                    }

                    else
                    if (Funct == TFun :: DLTA && TBase)
                    {
                        (newVar ->* (newVar -> LoopFunct)) (Indx);
                        Delta0 = newVar -> values [Indx];
                        newVar -> UserFunct_ = & ClassVar :: Last_;
                        newVar -> FunKind = TFun :: LAST;
                        newVar -> LoopFunct = & ClassVar :: LoopLast;
                    }

                    Lastkk = Global :: DTindx;
                    if (CurrentIndx > - 1)
                    {
                        LastIndex = CurrentIndx;
                    }

                    switch (Funct) {
                    case TFun :: MAX:
                        newVar -> values [Indx] = - 1000000.0;
                        break;
                    case TFun :: MIN:
                        newVar -> values [Indx] = 1000000.0;
                        break;
                    case TFun :: AVG:
                    case TFun :: TOT:
                    case TFun :: CNT:
                    case TFun :: CNT0:
                    case TFun :: DLTA:
                    case TFun :: POS:
                        newVar -> values [Indx] = 0.0;
                    default:
                        break;
                    }
                    LastDays = Days;
                    Days = 0;
                }

                (newVar ->* (newVar -> LoopFunct)) (Indx);
                xx = newVar -> values [Indx];
                cdSeries -> AddXY (Global :: DTnow, xx);
                if (First)
                {
                    First0 = xx;
                }

                if (Global :: DTindx > DTmin && Global :: DTindx > IndxMin)
                {
                    switch (Funct) {
                    case TFun :: AVG:
                        Greatest = Days;
                        if (LastDays > Days)
                        {
                            Greatest = LastDays;
                        }

                        cdSeries -> YValues [CurrentIndx] /= ((long long) Global :: Freq * (long long) Greatest);
                        LastDays = 0;
                        break;
                    case TFun :: DLTA:
                        if (! First)
                        {
                            cdSeries -> YValues [CurrentIndx] -= Delta0;
                        }

                        break;
                    default:
                        break;
                    }
                }

                ++ Days;
                First = FALSE;
            }

        }
        if (Global :: DTindx > DTmin && Global :: DTindx > IndxMin)
        {
            CurrentIndx = (Global :: DTindx - DTminX) / thisVar -> FileData -> Freq - 1;
            switch (Funct) {
            case TFun :: AVG:
                Greatest = Days;
                if (LastDays > Days)
                {
                    Greatest = LastDays;
                }

                cdSeries -> YValues [CurrentIndx] /= ((long long) Global :: Freq * (long long) Greatest);
                if (ObsFunct_Toggle == 1)
                {
                    for (long jj = LastIndex + 1; jj <= CurrentIndx; ++ jj)
                        cdSeries -> YValues [jj] = cdSeries -> YValues [CurrentIndx];
                }

                break;
            case TFun :: DLTA:
                cdSeries -> YValues [CurrentIndx] -= Delta0;
            case TFun :: MIN:
            case TFun :: MAX:
            case TFun :: TOT:
            case TFun :: POS:
            case TFun :: LAST:
            case TFun :: CNT:
            case TFun :: CNT0:
                if (ObsFunct_Toggle == 1)
                {
                    for (long jj = LastIndex + 1; jj <= CurrentIndx; ++ jj)
                        cdSeries -> YValues [jj] = cdSeries -> YValues [CurrentIndx];
                }

                break;
            case TFun :: FIRST:
                for (long jj = LastIndex + 1; jj <= CurrentIndx; ++ jj)
                    cdSeries -> YValues [jj] = First0;
                break;
            default:
                break;
            }
        }

        delete newVar -> FileData;
        delete newVar;
        cdSeries -> Tag = NULL;
    }

    Global :: DTnow = Save_DTnow;
    AddSeriesToTChart (cdSeries);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::AddObsPlot(ClassVar * thisVar, TSeries * cdSeries, string S, TFun Funct)@@@CRHMmainDlg.cpp>");
}
void HruNameClick ()
{
InstrumentLogger::instance()->log_instrument_log("<HruNameClick()@@@CRHMmainDlg.cpp>");
    CRHMmain * test = CRHMmain :: getInstance ();
    MapPar :: iterator itPar;
    ClassPar * newPar;
    long Hru, Lay;
    test -> SelectedVariables -> Sorted = false;
    if (! test -> ListHruNames)
    {
        test -> ListHruNames = new TStringList;
        test -> MapGrpNames = new MapstrSS;
        if ((itPar = Global :: MapPars.find ("basin basin_name")) != Global :: MapPars.end ())
        {
            newPar = (* itPar).second;
            test -> ListHruNames -> Add (newPar -> Strings -> Strings [0]);
        }

        else
        {
            test -> ListHruNames -> Add ("Basin");
        }

        if ((itPar = Global :: MapPars.find ("basin hru_names")) != Global :: MapPars.end ())
        {
            newPar = (* itPar).second;
            for (int ii = 0; ii < newPar -> Strings -> Count; ++ ii)
                test -> ListHruNames -> Add (newPar -> Strings -> Strings [ii]);
        }

        else
        {
            for (int ii = 0; ii < Global :: nhru; ++ ii)
                test -> ListHruNames -> Add ("hru" + to_string (ii));
        }

        for (int ii = 0; ii < Global :: OurModulesList -> Count; ++ ii) {
            ClassModule * Mod = (ClassModule *) Global :: OurModulesList -> Objects [ii];
            string S = Global :: OurModulesList -> Strings [ii];
            if (Mod -> GroupCnt > 0 || Mod -> StructCnt > 0)
            {
                if (Mod -> GroupCnt > 0)
                {
                    string AA;
                    Common :: GroupEnding (AA, Mod -> GroupCnt);
                    Pairstr Item = Pairstr (AA.c_str (), ('@' + Mod -> Name).c_str ());
                    test -> MapGrpNames -> insert (Item);
                }

                else
                if (Mod -> StructCnt > 0)
                {
                    string AA;
                    Common :: GroupEnding (AA, Mod -> GroupCnt);
                    Pairstr Item = Pairstr (AA.c_str (), ('@' + Mod -> Name).c_str ());
                    test -> MapGrpNames -> insert (Item);
                }

            }

        }
        for (int ii = 0; test -> SelectedVariables -> Count > ii; ++ ii) {
            string S = test -> SelectedVariables -> Strings [ii];
            ClassVar * thisVar = (ClassVar *) test -> SelectedVariables -> Objects [ii];
            test -> HruNames = false;
            S = test -> ExtractHruLay (S, Hru, Lay);
            test -> HruNames = true;
            S = test -> BuildHru (S, Hru, thisVar -> dimen);
            if (Lay)
            {
                S = test -> BuildLay (S, Lay);
            }

            int Indx = S.find ("@");
            if (Indx)
            {
                MapstrSS :: iterator it;
                if ((it = test -> MapGrpNames -> find (S.substr (Indx, 2))) != test -> MapGrpNames -> end ())
                {
                    string NewName = (* it).second;
                    S.replace (Indx, 2, "");
                    S.insert (Indx, NewName);
                }

            }

            test -> SelectedVariables -> Strings [ii] = S;
        }
    }

    else
    {
        for (int ii = 0; test -> SelectedVariables -> Count > ii; ++ ii) {
            string S = test -> SelectedVariables -> Strings [ii];
            test -> HruNames = true;
            S = test -> ExtractHruLay (S, Hru, Lay);
            test -> HruNames = false;
            S = test -> BuildHru (S, Hru, TDim :: NHRU);
            if (Lay)
            {
                S = test -> BuildLay (S, Lay);
            }

            int Indx = S.find ("@");
            if (Indx)
            {
                MapstrSS :: iterator it;
                for (it = test -> MapGrpNames -> begin (); it != test -> MapGrpNames -> end (); ++ it) {
                    int pos = S.find ((* it).second);
                    if (pos)
                    {
                        string NewName = (* it).first;
                        string Name = (* it).second;
                        int Len = Name.length ();
                        int End = S.find ("(");
                        if (End - pos == Len)
                        {
                            S.replace (pos, Len, "");
                            S.insert (pos, NewName);
                            break;
                        }

                    }

                }
            }

            test -> SelectedVariables -> Strings [ii] = S;
        }
        delete test -> ListHruNames;
        test -> ListHruNames = NULL;
        delete test -> MapGrpNames;
        test -> MapGrpNames = NULL;
    }

    test -> SelectedVariables -> Sorted = false;
InstrumentLogger::instance()->log_instrument_log("</HruNameClick()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnBuildMacro ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnBuildMacro()@@@CRHMmainDlg.cpp>");
    MacroEntryDlg * macroentry;
    macroentry = new MacroEntryDlg ();
    if (macroentry -> DoModal ())
    {
        CRHMmain * t = CRHMmain :: getInstance ();
        t -> MacroClick ();
        loadGuiComponents ();
        tchart.RemoveAllSeries ();
        t -> OpenProjectPath = "";
    }

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnBuildMacro()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnHelpAbout ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnHelpAbout()@@@CRHMmainDlg.cpp>");
    CRHMAboutBox aboutbox;
    aboutbox.DoModal ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnHelpAbout()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnFlowdiagramsShowdiagram ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnFlowdiagramsShowdiagram()@@@CRHMmainDlg.cpp>");
    FlowDiagramDlg flowdiagram;
    flowdiagram.DoModal ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnFlowdiagramsShowdiagram()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnReport ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnReport()@@@CRHMmainDlg.cpp>");
    CReport report;
    report.DoModal ();
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnReport()@@@CRHMmainDlg.cpp>");
}
void CRHMmainDlg :: OnProjectExit32832 ()
{
InstrumentLogger::instance()->log_instrument_log("<CRHMmainDlg::OnProjectExit32832()@@@CRHMmainDlg.cpp>");
    CString filename = globFileName;
    CRHMmain * t = CRHMmain :: getInstance ();
    if (listbox_varaiables_all.GetCount () == 0 && list_box_variables_selected.GetCount () == 0 && listbox_observations.GetCount () == 0 && listbox_observations_selected.GetCount () == 0)
    {
    }

    else
    {
        if (t -> OpenProjectPath == defaultprojectname)
        {
            const int result = MessageBox (L"Do you want to save this project?", L"CRHM_GUI", MB_YESNOCANCEL);
            if (result == IDYES)
            {
                OnFileSaveAs ();
            }

            if (result == IDCANCEL)
            {

InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnProjectExit32832()@@@CRHMmainDlg.cpp>");
                return;
            }

        }

    }

    exit (0);
InstrumentLogger::instance()->log_instrument_log("</CRHMmainDlg::OnProjectExit32832()@@@CRHMmainDlg.cpp>");
}