#include "stdafx.h"
#include "CRHM_GUI.h"
#include "MacroEntryDlg.h"
#include "afxdialogex.h"
#include "GlobalDll.h"
#include "../../core/InstrumentLogger.h"

IMPLEMENT_DYNAMIC (MacroEntryDlg, CDialogEx)
MacroEntryDlg :: MacroEntryDlg (CWnd * pParent): CDialogEx (MacroEntry_DLG, pParent)
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::MacroEntryDlg(CWnd * pParent): CDialogEx (MacroEntry_DLG, pParent)@@@MacroEntryDlg.cpp>");
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::MacroEntryDlg(CWnd * pParent): CDialogEx (MacroEntry_DLG, pParent)@@@MacroEntryDlg.cpp>");
}
MacroEntryDlg :: ~MacroEntryDlg ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::~MacroEntryDlg()@@@MacroEntryDlg.cpp>");
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::~MacroEntryDlg()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: DoDataExchange (CDataExchange * pDX)
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::DoDataExchange(CDataExchange * pDX)@@@MacroEntryDlg.cpp>");
    CDialogEx :: DoDataExchange (pDX);
    DDX_Control (pDX, IDC_EDIT1, editbox);
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::DoDataExchange(CDataExchange * pDX)@@@MacroEntryDlg.cpp>");
}BEGIN_MESSAGE_MAP (MacroEntryDlg, CDialogEx)
    ON_BN_CLICKED (IDC_BUTTON1, & MacroEntryDlg :: OnBnClickedButton1)
    ON_BN_CLICKED (IDC_BUTTON3, & MacroEntryDlg :: OnBnClickedButton3)
    ON_COMMAND (ID_FILE_CREATEGROUP, & MacroEntryDlg :: OnFileCreategroup)
    ON_COMMAND (ID_FILE_SAVEAS32816, & MacroEntryDlg :: OnFileSaveas32816)
    ON_COMMAND (ID_FILE_OPEN32814, & MacroEntryDlg :: OnFileOpen32814)
    ON_BN_CLICKED (IDC_BUTTON2, & MacroEntryDlg :: OnBnClickedButton2)
END_MESSAGE_MAP ()

void MacroEntryDlg :: FormActivate ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::FormActivate()@@@MacroEntryDlg.cpp>");
    CString newline ("\r\n");
    CString cstr ("");
    editbox.SetWindowText (cstr);
    for (int ii = 0; ii < Global :: MacroModulesList -> Count; ++ ii) {
        CString cstr1 (Global :: MacroModulesList -> Strings [ii].c_str ());
        cstr += cstr1 + newline;
    }
    editbox.SetWindowText (cstr);
    ParaList = NULL;
    AKAList = NULL;
    GlobalModule.GrpCnt = 0;
    GlobalModule.StructCnt = 0;
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::FormActivate()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: AddStringsToEditBox (TStringList * tstr)
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::AddStringsToEditBox(TStringList * tstr)@@@MacroEntryDlg.cpp>");
    CString cstr1;
    editbox.GetWindowText (cstr1);
    CString newline ("\r\n");
    CString cstr2, cstr;
    for (int i = 0; i < tstr -> Count; i ++) {
        cstr = tstr -> Strings [i].c_str ();
        if (tstr -> Strings [i].length () != 0)
        {
            cstr2 += cstr + newline;
        }

    }
    CString finalcstr;
    if (cstr1.GetLength () > 0)
    {
        finalcstr = cstr1 + newline + cstr2;
    }

    else
    {
        finalcstr = cstr2;
    }

    editbox.SetWindowTextW (finalcstr);
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::AddStringsToEditBox(TStringList * tstr)@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: AddStringsToEditBox (string tstr)
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::AddStringsToEditBox(string tstr)@@@MacroEntryDlg.cpp>");
    CString cstr1;
    editbox.GetWindowText (cstr1);
    CString newline ("\r\n");
    CString cstr2 (tstr.c_str ());
    CString finalcstr;
    if (cstr1.GetLength () > 0)
    {
        finalcstr = cstr1 + newline + cstr2;
    }

    else
    {
        finalcstr = cstr2;
    }

    editbox.SetWindowTextW (finalcstr);
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::AddStringsToEditBox(string tstr)@@@MacroEntryDlg.cpp>");
}
TStringList * MacroEntryDlg :: GetLinesFromEditBox ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::GetLinesFromEditBox()@@@MacroEntryDlg.cpp>");
    TStringList * tstring = new TStringList ();
    CString cstr1;
    editbox.GetWindowText (cstr1);
    string str = CT2A (cstr1);
    int l = str.length ();
    string temp = "";
    for (int i = 0; i < l; i ++) {
        if (str [i] != '\r' && str [i] != '\n')
        {
            temp += str [i];
        }

        else
        {
            if (temp.length () > 0)
            {
                tstring -> AddObject (temp, 0);
                temp = "";
            }

        }

    }
    if (temp.length () > 0)
    {
        tstring -> AddObject (temp, 0);
    }

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::GetLinesFromEditBox()@@@MacroEntryDlg.cpp>");
    return tstring;
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::GetLinesFromEditBox()@@@MacroEntryDlg.cpp>");
}
TStringList * MacroEntryDlg :: GetSelectedLinesFromEditBox ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::GetSelectedLinesFromEditBox()@@@MacroEntryDlg.cpp>");
    TStringList * tstring = new TStringList ();
    CString cstr1;
    editbox.GetWindowText (cstr1);
    int start, end;
    CString cstr2;
    editbox.GetSel (start, end);
    cstr2 = cstr1.Mid (start, end - start);
    string str = CT2A (cstr2);
    int l = str.length ();
    string temp = "";
    for (int i = 0; i < l; i ++) {
        if (str [i] != '\r' && str [i] != '\n')
        {
            temp += str [i];
        }

        else
        {
            if (temp.length () > 0)
            {
                tstring -> AddObject (temp, 0);
                temp = "";
            }

        }

    }
    if (temp.length () > 0)
    {
        tstring -> AddObject (temp, 0);
    }

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::GetSelectedLinesFromEditBox()@@@MacroEntryDlg.cpp>");
    return tstring;
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::GetSelectedLinesFromEditBox()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: OnBnClickedButton1 ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OnBnClickedButton1()@@@MacroEntryDlg.cpp>");
    SaveChangesClick ();
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnBnClickedButton1()@@@MacroEntryDlg.cpp>");
}
BOOL MacroEntryDlg :: OnInitDialog ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OnInitDialog()@@@MacroEntryDlg.cpp>");
    CDialogEx :: OnInitDialog ();
    FormActivate ();

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnInitDialog()@@@MacroEntryDlg.cpp>");
    return TRUE;
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnInitDialog()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: OpenClick ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OpenClick()@@@MacroEntryDlg.cpp>");
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OpenClick()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: DoSave ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::DoSave()@@@MacroEntryDlg.cpp>");
    TStringList * Lines = new TStringList ();
    Lines = GetLinesFromEditBox ();
    DWORD dwSel = editbox.GetSel ();
    if (dwSel == 0)
    {
        Lines -> SaveToFile (macrofile -> filepath);
    }

    else
    {
        Lines = GetSelectedLinesFromEditBox ();
        Lines -> SaveToFile (macrofile -> filepath);
    }

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::DoSave()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: SaveClick ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::SaveClick()@@@MacroEntryDlg.cpp>");
    if (macrofile -> filepath.length () > 0)
    {
        DoSave ();
    }

    else
    {
        SaveAsClick ();
    }

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::SaveClick()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: SaveAsClick ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::SaveAsClick()@@@MacroEntryDlg.cpp>");
    macrofile = new FileDialog ();
    bool opened = macrofile -> Save ("mcr");
    if (opened)
    {
        DoSave ();
    }

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::SaveAsClick()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: CreateGroupClick ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::CreateGroupClick()@@@MacroEntryDlg.cpp>");
    string Txt, S, MacroName, GrpName;
    int Indx;
    openfile = new FileDialog ();
    bool opened = openfile -> Open ("prj");
    if (opened == false)
    {

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::CreateGroupClick()@@@MacroEntryDlg.cpp>");
        return;
    }

    MacroName = openfile -> filename;
    Indx = MacroName.rfind ("\\");
    if (Indx != - 1)
    {
        MacroName = MacroName.substr (Indx + 1, MacroName.length ());
    }

    Indx = MacroName.rfind (".");
    if (Indx != - 1)
    {
        string AA;
        Common :: GroupEnding (AA, ++ GlobalModule.GrpCnt);
        MacroName = MacroName.substr (0, Indx) + "_Grp" + AA.substr (1, 2);
    }

    if (! isalpha (MacroName [0]))
    {
        MessageBox (_T ("Alpha characters are upper-/lowercase characters from A through Z. Project file name must begin with an alpha character."), MB_OK);

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::CreateGroupClick()@@@MacroEntryDlg.cpp>");
        return;
    }

    GrpName = MacroName;
    if (GlobalModule.GrpCnt > 1)
    {
        GlobalModule.GrpCnt = 1;
    }

    ProjectFile = new TStringList;
    ModuleList = new TStringList;
    MacroList = new TStringList;
    DefinedList = new TStringList;
    if (! ParaList)
    {
        ParaList = new TStringList;
        ParaList -> Add ("Parameters for created Group Macro");
        ParaList -> Add ("#####");
    }

    if (! AKAList)
    {
        AKAList = new TStringList;
    }

    ProjectFile -> LoadFromFile (openfile -> filepath);
    int ThisEnd = 0;
    Module * ThisModule;
    TStringList * Lines = new TStringList ();
    Lines = GetLinesFromEditBox ();
    while (ThisEnd < Lines -> Count) {
        if (Lines -> Strings [ThisEnd].length () == 0)
        {
            continue;
        }

        while (ThisEnd < Lines -> Count && (Txt = Common :: trimleft (Lines -> Strings [ThisEnd]), Txt [0] == '/')) ++ ThisEnd;
        if (Indx = Txt.find (" "))
        {
            Txt = Txt.substr (0, Indx);
        }

        ThisModule = new Module;
        ThisModule -> head = ThisEnd;
        do S = Common :: trim (Lines -> Strings [++ ThisEnd]);
        while (S [0] == '/');
        if (S.find ("declgroup"))
        {
            ++ ThisModule -> GrpCnt;
            ThisModule -> isGroup = true;
        }

        if (S.find ("declstruct"))
        {
            ++ ThisModule -> StructCnt;
            ThisModule -> isStruct = true;
        }

        while (ThisEnd < Lines -> Count && ! (S = Common :: trim (Lines -> Strings [ThisEnd]), S.substr (0, 3) == "end" && (S.length () == 3 || S [3] == ' ' || S [3] == '/'))) ++ ThisEnd;
        ThisModule -> tail = ThisEnd ++;
        DefinedList -> AddObject (Txt, (TObject *) ThisModule);
    }
    for (int ii = 0; ii < ProjectFile -> Count; ++ ii) {
        Txt = ProjectFile -> Strings [ii];
        if (Txt == "Dimensions:")
        {
            CString cstr1 (ProjectFile -> Strings [ii + 2].substr (4, 10).c_str ());
            Global :: nhru = StrToInt (cstr1);
            CString cstr2 (ProjectFile -> Strings [ii + 3].substr (4, 10).c_str ());
            Global :: nlay = StrToInt (cstr2);
        }

        if (Txt == "Parameters:")
        {
            ++ ii;
            ++ ii;
            while (ProjectFile -> Strings [ii] [0] != '#') {
                if (ProjectFile -> Strings [ii] [0] == '\'' || ProjectFile -> Strings [ii] [0] == '-' || iswdigit (ProjectFile -> Strings [ii] [0]))
                {
                    ParaList -> Add (ProjectFile -> Strings [ii]);
                }

                else
                {
                    Indx = ProjectFile -> Strings [ii].find (" ");
                    S = ProjectFile -> Strings [ii].substr (Indx + 1, ProjectFile -> Strings [ii].length () - Indx - 1);
                    ParaList -> Add (GrpName + " " + S);
                }

                ++ ii;
            }
            continue;
        }

        if (Txt == "Macros:")
        {
            ++ ii;
            ++ ii;
            ThisModule = NULL;
            while (ProjectFile -> Strings [ii] [0] != '#') {
                while (ii < ProjectFile -> Count && (S = Common :: trimleft (ProjectFile -> Strings [ii]), S [0] == '/')) ++ ii;
                S = S.substr (0, S.length () - 2);
                if (! ThisModule)
                {
                    if (Indx = S.find (" "))
                    {
                        S = S.substr (0, Indx);
                    }

                    ThisModule = new Module;
                    ThisModule -> head = MacroList -> Count;
                    MacroList -> Add (S);
                }

                else
                {
                    if (S.find ("declgroup"))
                    {
                        ++ ThisModule -> GrpCnt;
                        ThisModule -> isGroup = true;
                    }

                    if (S.find ("declstruct"))
                    {
                        ++ ThisModule -> StructCnt;
                        ThisModule -> isStruct = true;
                    }

                    MacroList -> Add (S);
                    if (S.substr (0, 3) == "end" && (S.length () == 3 || S [3] == ' ' || S [3] == '/'))
                    {
                        ThisModule -> tail = MacroList -> Count;
                        MacroList -> Objects [ThisModule -> head] = (TObject *) (ThisModule);
                        ThisModule = NULL;
                    }

                }

                ++ ii;
            }
            continue;
        }

    }
    for (int ii = 0; ii < ProjectFile -> Count; ++ ii) {
        Txt = ProjectFile -> Strings [ii];
        if (Txt == "AKAs:")
        {
            ++ ii;
            ++ ii;
            while (ProjectFile -> Strings [ii] [0] != '#') {
                string S = Common :: trim (ProjectFile -> Strings [ii]);
                int Indx = S.find (" ");
                string type = S.substr (0, Indx);
                S = S.substr (Indx + 1, S.length () - Indx - 1);
                Indx = S.find (" ");
                string module = S.substr (0, Indx);
                S = S.substr (Indx + 1, S.length () - Indx - 1);
                Indx = S.find (" ");
                string name = S.substr (0, Indx);
                S = S.substr (Indx + 1, S.length () - Indx - 1);
                string alias;
                Indx = S.find (" ");
                if (Indx == - 1)
                {
                    alias = S;
                    ParaList -> Add (type + " " + GrpName + " " + name + " " + alias);
                }

                else
                {
                    alias = S.substr (0, Indx);
                    string source = S.substr (Indx + 1, S.length () - Indx - 1);
                    if (MacroList -> IndexOf (source) > - 1)
                    {
                        AKAList -> Add (type + " " + GrpName + " " + name + " " + alias + " " + source);
                    }

                    else
                    {
                        AKAList -> Add (type + " " + GrpName + " " + name + " " + alias + " " + GrpName);
                    }

                }

                ++ ii;
            }
        }

        if (Txt == "Modules:")
        {
            ++ ii;
            ++ ii;
            while (DefinedList -> IndexOf (S) > - 1) {
                ++ S [S.length () - 1];
            }
            ModuleList -> Add (GrpName);
            ModuleList -> Add ("declgroup " + to_string (Global :: nhru));
            while (ProjectFile -> Strings [ii] [0] != '#') {
                S = Common :: trim (ProjectFile -> Strings [ii]);
                if (S.find ("+") > - 1)
                {
                    ++ ii;
                    continue;
                }

                if (S.find (" "))
                {
                    S = S.substr (0, S.find (" "));
                }

                ModuleList -> Add (S);
                ++ ii;
            }
            ModuleList -> Add ("command");
            ModuleList -> Add ("end");
            break;
        }

    }
    ProjectFile -> Clear ();
    int EndList = ModuleList -> Count - 2;
    for (int ii = 2; ii < EndList; ++ ii) {
        AddGroup (ii);
    }
    AddStringsToEditBox (ModuleList);
    delete ProjectFile;
    delete ModuleList;
    for (int ii = 0; ii < MacroList -> Count; ++ ii)
        delete (Module *) MacroList -> Objects [ii];
    delete MacroList;
    for (int ii = 0; ii < DefinedList -> Count; ++ ii)
        delete (Module *) DefinedList -> Objects [ii];
    delete DefinedList;
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::CreateGroupClick()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: AddGroup (int ii)
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::AddGroup(int ii)@@@MacroEntryDlg.cpp>");
    int Indx;
    if (DefinedList -> IndexOf (ModuleList -> Strings [ii]) == - 1)
    {
        if (Indx = MacroList -> IndexOf (ModuleList -> Strings [ii]), Indx > - 1)
        {
            Module * ThisModule = (Module *) MacroList -> Objects [Indx];
            AddSupport (ThisModule);
            for (int jj = ThisModule -> head; jj < ThisModule -> tail; ++ jj) {
                AddStringsToEditBox (MacroList -> Strings [jj]);
            }
            ThisModule -> avail = true;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::AddGroup(int ii)@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: AddSupport (Module * ThisModule)
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::AddSupport(Module * ThisModule)@@@MacroEntryDlg.cpp>");
    int Indx;
    for (int ii = ThisModule -> head + 2; ii < ThisModule -> tail - 2; ++ ii) {
        if (DefinedList -> IndexOf (MacroList -> Strings [ii]) > - 1)
        {

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::AddSupport(Module * ThisModule)@@@MacroEntryDlg.cpp>");
            return;
        }

        if (Indx = MacroList -> IndexOf (MacroList -> Strings [ii]), Indx > - 1 && MacroList -> Objects [Indx])
        {
            Module * ThisModule = (Module *) MacroList -> Objects [Indx];
            if (ThisModule -> avail)
            {
                continue;
            }

            if (ThisModule -> isGroup || ThisModule -> isStruct)
            {
                AddSupport (ThisModule);
                ThisModule -> avail = true;
            }

            for (int jj = Indx; jj < ThisModule -> tail; ++ jj) {
                AddStringsToEditBox (MacroList -> Strings [jj]);
            }
            ThisModule -> avail = true;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::AddSupport(Module * ThisModule)@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: SaveChangesClick ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::SaveChangesClick()@@@MacroEntryDlg.cpp>");
    TStringList * Lines = new TStringList ();
    Lines = GetLinesFromEditBox ();
    Global :: MacroModulesList -> Clear ();
    for (int ii = 0; ii < Lines -> Count; ++ ii) {
        string S;
        if (Lines -> Strings [ii].length () == 0)
        {
            S = " ";
        }

        else
        {
            S = Common :: trimright (Lines -> Strings [ii]);
        }

        Global :: MacroModulesList -> Add (S);
    }
    if (ParaList)
    {
        ParaList -> Add ("#####");
        FileDialog * saveparameter = new FileDialog ();
        bool opened = saveparameter -> Save ("par");
        if (opened == true)
        {
            ParaList -> SaveToFile (saveparameter -> filepath);
            delete ParaList;
            ParaList = NULL;
        }

    }

    if (AKAList && AKAList -> Count)
    {
        string filepath = "CreateGroup.aka";
        AKAList -> SaveToFile (filepath);
        delete AKAList;
        AKAList = NULL;
    }

    this -> OnOK ();
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::SaveChangesClick()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: OnBnClickedButton3 ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OnBnClickedButton3()@@@MacroEntryDlg.cpp>");
    CreateGroupClick ();
    GetLinesFromEditBox ();
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnBnClickedButton3()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: OnFileCreategroup ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OnFileCreategroup()@@@MacroEntryDlg.cpp>");
    CreateGroupClick ();
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnFileCreategroup()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: OnFileSaveas32816 ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OnFileSaveas32816()@@@MacroEntryDlg.cpp>");
    SaveAsClick ();
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnFileSaveas32816()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: OnFileOpen32814 ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OnFileOpen32814()@@@MacroEntryDlg.cpp>");
    openfile = new FileDialog ();
    bool opened = openfile -> Open ("mcr");
    if (! opened)
    {

InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnFileOpen32814()@@@MacroEntryDlg.cpp>");
        return;
    }

    DoLoad (openfile -> filepath);
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnFileOpen32814()@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: DoLoad (string filepath)
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::DoLoad(string filepath)@@@MacroEntryDlg.cpp>");
    TStringList * slist = new TStringList ();
    slist -> LoadFromFile (filepath);
    editbox.SetWindowText (_T (""));
    AddStringsToEditBox (slist);
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::DoLoad(string filepath)@@@MacroEntryDlg.cpp>");
}
void MacroEntryDlg :: OnBnClickedButton2 ()
{
InstrumentLogger::instance()->log_instrument_log("<MacroEntryDlg::OnBnClickedButton2()@@@MacroEntryDlg.cpp>");
    editbox.SetWindowText (_T (""));
    GlobalModule.GrpCnt = 0;
    GlobalModule.StructCnt = 0;
    if (ParaList)
    {
        delete ParaList;
        ParaList = NULL;
    }

    if (AKAList)
    {
        delete AKAList;
        AKAList = NULL;
    }

    Global :: MacroModulesList -> Clear ();
    int c = Global :: AllModulesList -> Count;
    Global :: AllModulesList -> Delete (c - 1);
    this -> OnCancel ();
InstrumentLogger::instance()->log_instrument_log("</MacroEntryDlg::OnBnClickedButton2()@@@MacroEntryDlg.cpp>");
}