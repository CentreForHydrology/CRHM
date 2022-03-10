#include "stdafx.h"
#include "CConstruct.h"
#include "CConstructCore.h"
#include "../../core/InstrumentLogger.h"

IMPLEMENT_DYNAMIC (CConstruct, CDialog)
CConstruct :: CConstruct (CWnd * pParent): CDialog (CONSTRUCT_DLG, pParent)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::CConstruct(CWnd * pParent): CDialog (CONSTRUCT_DLG, pParent)@@@CConstruct.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CConstruct::CConstruct(CWnd * pParent): CDialog (CONSTRUCT_DLG, pParent)@@@CConstruct.cpp>");
}
CConstruct :: ~CConstruct ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::~CConstruct()@@@CConstruct.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CConstruct::~CConstruct()@@@CConstruct.cpp>");
}
void CConstruct :: DoDataExchange (CDataExchange * pDX)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::DoDataExchange(CDataExchange * pDX)@@@CConstruct.cpp>");
    CDialog :: DoDataExchange (pDX);
    DDX_Control (pDX, IDC_LIST2, listbox_all_modules);
    DDX_Control (pDX, IDC_LIST1, listbox_selected_modules);
    DDX_Control (pDX, IDC_CHECK1, checkbox_basic);
    DDX_Control (pDX, IDC_CHECK2, checkbox_macro);
    DDX_Control (pDX, IDC_CHECK3, checkbox_advanced);
    DDX_Control (pDX, IDC_CHECK4, checkbox_support);
    DDX_Control (pDX, IDC_CHECK5, checkbox_custom);
    DDX_Control (pDX, IDC_CHECK6, checkbox_prototype);
    DDX_Control (pDX, IDC_CHECK7, checkbox_obsolete);
    DDX_Control (pDX, IDC_LIST3, listcontrol);
InstrumentLogger::instance()->log_instrument_log("</CConstruct::DoDataExchange(CDataExchange * pDX)@@@CConstruct.cpp>");
}BEGIN_MESSAGE_MAP (CConstruct, CDialog)
    ON_BN_CLICKED (IDC_BUTTON8, & CConstruct :: OnBnClickedButton8)
    ON_LBN_DBLCLK (IDC_LIST2, & CConstruct :: OnLbnDblclkList2)
    ON_LBN_DBLCLK (IDC_LIST1, & CConstruct :: OnLbnDblclkList1)
    ON_BN_CLICKED (IDCANCEL2, & CConstruct :: OnBnClickedCancel2)
    ON_BN_CLICKED (IDC_CHECK1, & CConstruct :: OnBnClickedCheck1)
    ON_BN_CLICKED (IDC_CHECK2, & CConstruct :: OnBnClickedCheck2)
    ON_BN_CLICKED (IDC_CHECK3, & CConstruct :: OnBnClickedCheck3)
    ON_BN_CLICKED (IDC_CHECK4, & CConstruct :: OnBnClickedCheck4)
    ON_BN_CLICKED (IDC_CHECK5, & CConstruct :: OnBnClickedCheck5)
    ON_BN_CLICKED (IDC_CHECK6, & CConstruct :: OnBnClickedCheck6)
    ON_BN_CLICKED (IDC_CHECK7, & CConstruct :: OnBnClickedCheck7)
    ON_BN_CLICKED (IDOK, & CConstruct :: OnBnClickedOk)
    ON_BN_CLICKED (IDCANCEL, & CConstruct :: OnBnClickedCancel)
    ON_LBN_SELCHANGE (IDC_LIST1, & CConstruct :: OnLbnSelchangeList1)
    ON_BN_CLICKED (IDC_BUTTON1, & CConstruct :: OnBnClickedButton1)
END_MESSAGE_MAP ()

BOOL CConstruct :: OnInitDialog ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnInitDialog()@@@CConstruct.cpp>");
    CDialog :: OnInitDialog ();
    LoadAllModules ();
    FormActivate ();

InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnInitDialog()@@@CConstruct.cpp>");
    return TRUE;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnInitDialog()@@@CConstruct.cpp>");
}
void CConstruct :: LoadAllModules ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::LoadAllModules()@@@CConstruct.cpp>");
    listbox_all_modules.ResetContent ();
    TStringList * all_modules = CRHMmain :: getInstance () -> getAllmodules ();
    for (int ii = 0; ii < all_modules -> Count; ii ++) {
        std :: string s = all_modules -> Strings [ii];
        CString c_all_modules (s.c_str ());
        listbox_all_modules.AddString (c_all_modules);
    }
InstrumentLogger::instance()->log_instrument_log("</CConstruct::LoadAllModules()@@@CConstruct.cpp>");
}
int CConstruct :: IsAlreadySelected (CString cstr)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::IsAlreadySelected(CString cstr)@@@CConstruct.cpp>");
    CString cstr1;
    for (int i = 0; i < listbox_selected_modules.GetCount (); i ++) {
        listbox_selected_modules.GetText (i, cstr1);
        if (cstr == cstr1)
        {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::IsAlreadySelected(CString cstr)@@@CConstruct.cpp>");
            return 1;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::IsAlreadySelected(CString cstr)@@@CConstruct.cpp>");
    return 0;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::IsAlreadySelected(CString cstr)@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedButton8 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedButton8()@@@CConstruct.cpp>");
    CString cstr;
    int sel = 0;
    for (int i = 0; i < AllModules -> Count; i ++) {
        sel = listbox_all_modules.GetSel (i);
        if (sel == 1)
        {
            listbox_all_modules.GetText (i, cstr);
            if (IsAlreadySelected (cstr) == 0)
            {
                listbox_selected_modules.AddString (cstr);
                SelectedModules -> AddObject (AllModules -> Strings [i], AllModules -> Objects [i]);
            }

            listbox_all_modules.SetSel (i, false);
        }

    }
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedButton8()@@@CConstruct.cpp>");
}
void CConstruct :: RemoveSelectedModule ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::RemoveSelectedModule()@@@CConstruct.cpp>");
    CString cstr;
    int c = SelectedModules -> Count;
    for (int i = 0; i < c; i ++) {
        int sel = listbox_selected_modules.GetSel (i);
        if (sel == 1)
        {
            listbox_selected_modules.GetText (i, cstr);
            listbox_selected_modules.DeleteString (i);
            CT2A s (cstr);
            string str = s;
            int indx = SelectedModules -> IndexOf (str);
            SelectedModules -> Delete (indx);
        }

    }
InstrumentLogger::instance()->log_instrument_log("</CConstruct::RemoveSelectedModule()@@@CConstruct.cpp>");
}
void CConstruct :: AddToAllModules (string str, TObject * obj)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::AddToAllModules(string str, TObject * obj)@@@CConstruct.cpp>");
    AllModules -> AddObject (str, obj);
    CString cstr (str.c_str ());
    listbox_all_modules.AddString ((LPCTSTR) cstr);
    string S = str;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::AddToAllModules(string str, TObject * obj)@@@CConstruct.cpp>");
}
void CConstruct :: AddToSelectedModules (string str, TObject * obj)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::AddToSelectedModules(string str, TObject * obj)@@@CConstruct.cpp>");
    SelectedModules -> AddObject (str, obj);
    CString cstr (str.c_str ());
    listbox_selected_modules.AddString ((LPCTSTR) cstr);
    string S = str;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::AddToSelectedModules(string str, TObject * obj)@@@CConstruct.cpp>");
}
void CConstruct :: FormActivate ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::FormActivate()@@@CConstruct.cpp>");
    listbox_all_modules.ResetContent ();
    checkbox_basic.SetCheck (0);
    checkbox_macro.SetCheck (0);
    checkbox_advanced.SetCheck (0);
    checkbox_custom.SetCheck (0);
    checkbox_obsolete.SetCheck (0);
    checkbox_prototype.SetCheck (0);
    checkbox_support.SetCheck (0);
    Mapstr2 MapAKACopy;
    MapAKACopy = Global :: MapAKA;
    AKAselectiveDelete ();
    Global :: Mapgetvar.clear ();
    Global :: Mapputvar.clear ();
    Global :: Mapreadvar.clear ();
    Global :: Mapdeclvar.clear ();
    Global :: Mapdeclstat.clear ();
    Global :: Mapdeclpar.clear ();
    Global :: Mapdeclobs.clear ();
    TBuild TempB = Global :: BuildFlag;
    Global :: BuildFlag = TBuild :: BUILD;
    for (int ii = 0; ii < Global :: AllModulesList -> Count; ++ ii) {
        if (ii == Global :: AllModulesList -> Count - 1)
        {
            int a = 10;
        }

        ClassModule * thisModule = (ClassModule *) Global :: AllModulesList -> Objects [ii];
        thisModule -> variation_max = 0;
        unsigned short Temp = thisModule -> variation;
        unsigned short Temp2 = thisModule -> variation_set;
        thisModule -> variation_set = 0;
        thisModule -> variation = 0;
        thisModule -> decl ();
        thisModule -> variation = Temp;
        thisModule -> variation_set = Temp2;
        if (thisModule -> variation > 0)
        {
            thisModule -> hh = (long) (log (thisModule -> variation) / log (2) + 1);
        }

        else
        {
            thisModule -> hh = 0;
        }

    }
    Global :: OBS_AS_IS = 0;
    Global :: WQ_prj = false;
    listbox_selected_modules.ResetContent ();
    if (Global :: OurModulesList -> Count)
    {
        ModifyBuild = true;
    }

    else
    {
        ModifyBuild = false;
        checkbox_basic.SetCheck (1);
    }

    for (int ii = 0; ii < Global :: OurModulesList -> Count; ++ ii) {
        if (((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> Lvl == LMODULE :: BASIC)
        {
            checkbox_basic.SetCheck (1);
        }

        if (((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> Lvl == LMODULE :: MACRO)
        {
            checkbox_macro.SetCheck (1);
        }

        if (((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> Lvl == LMODULE :: ADVANCE)
        {
            checkbox_advanced.SetCheck (1);
        }

        if (((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> Lvl == LMODULE :: SUPPORT)
        {
            checkbox_support.SetCheck (1);
        }

        if (((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> Lvl == LMODULE :: CUSTOM)
        {
            checkbox_custom.SetCheck (1);
        }

        if (((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> Lvl == LMODULE :: PROTO)
        {
            checkbox_prototype.SetCheck (1);
        }

        if (((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> Lvl == LMODULE :: OBSOL)
        {
            checkbox_obsolete.SetCheck (1);
        }

        AddToSelectedModules (Global :: OurModulesList -> Strings [ii], (TObject *) Global :: OurModulesList -> Objects [ii]);
        SetSuffix (ii);
        string S = Global :: OurModulesList -> Strings [ii];
    }
    Execute = false;
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::FormActivate()@@@CConstruct.cpp>");
}
void CConstruct :: SetSuffix (long Row)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::SetSuffix(long Row)@@@CConstruct.cpp>");
    ClassModule * thisModule = (ClassModule *) SelectedModules -> Objects [Row];
    string AA;
    if (thisModule -> hh > 0)
    {
        AA = "#0";
        AA [1] += (char) thisModule -> hh;
    }

    int found = SelectedModules -> Strings [Row].find ("#");
    if (found > - 1)
    {
        SelectedModules -> Strings [Row] = SelectedModules -> Strings [Row].substr (0, SelectedModules -> Strings [Row].length () - 2);
    }

    SelectedModules -> Strings [Row] = SelectedModules -> Strings [Row] + AA;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::SetSuffix(long Row)@@@CConstruct.cpp>");
}
void CConstruct :: AKAselectiveDelete ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::AKAselectiveDelete()@@@CConstruct.cpp>");
    if (! Global :: MapAKA.empty ())
    {
        Mapstr2 :: iterator it;
        for (it = Global :: MapAKA.begin (); it != Global :: MapAKA.end ();) {
            string Var = (* it).first;
            string :: size_type idx = Var.find (' ');
            string type = Var.substr (0, idx);
            TAKA Type = AKAtype (type);
            switch (Type) {
            case TAKA :: OBSR:
            case TAKA :: OBSF:
                ++ it;
                break;
            default:
                Global :: MapAKA.erase (it ++);
            }
        }
    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::AKAselectiveDelete()@@@CConstruct.cpp>");
}
void CConstruct :: AddLinesToMemory (string str)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::AddLinesToMemory(string str)@@@CConstruct.cpp>");
    CListBox * listbox = (CListBox *) GetDlgItem (IDC_LIST4);
    CString cstr (str.c_str ());
    listbox -> AddString (cstr);
InstrumentLogger::instance()->log_instrument_log("</CConstruct::AddLinesToMemory(string str)@@@CConstruct.cpp>");
}
void CConstruct :: DisplayModule (string S, ClassModule * thisModule, bool LBox1)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::DisplayModule(string S, ClassModule * thisModule, bool LBox1)@@@CConstruct.cpp>");
    Mapstr :: iterator itMap, pos, pos2, itMap2;
    string SS, S1, S2;
    MapVar :: iterator itVar;
    long Variation = 0;
    long Vindx = 0;
    int P;
    SS = S;
    if (P = S.find ("#"), P > - 1)
    {
        Vindx = S [P + 1] - char ('0');
        Variation = (long) (pow (2, S [P + 1] - char ('1')));
        S = S.substr (0, S.length () - 2);
    }

    else
    {
        Variation = 0;
    }

    if (! thisModule -> Description.empty ())
    {
        TStringList * Fred = new TStringList;
        Fred -> QuoteChar = '\'';
        Fred -> DelimitedText (thisModule -> Description);
        if (thisModule -> variation_max > 0)
        {
            ++ Vindx;
        }

        if (LBox1)
        {
            for (int ii = 0; ii < Fred -> Count; ++ ii) {
                S1 = S;
                if (ii == 0)
                {
                    S1 += " Function summary";
                }

                else
                if (ii > 1)
                {
                    S1 += "#" + to_string (ii - 1);
                }

                else
                {
                    S1 += "  ";
                }

                S1 += " -> " + Fred -> Strings [ii];
                AddLinesToMemory (S1);
            }
        }

        else
        {
            AddLinesToMemory (SS + " -> " + Fred -> Strings [Vindx]);
            if (thisModule -> DLLName == "Macro")
            {
                ClassMacro * thisMacro = dynamic_cast < ClassMacro * > (thisModule);
                if (thisMacro -> isGroup)
                {
                    AddLinesToMemory ("Group with modules:");
                    string S;
                    for (int ii = 0; ii < thisMacro -> GrpStringList -> Count; ++ ii)
                        if (ii == 0)
                        {
                            S = "  " + thisMacro -> GrpStringList -> Strings [ii];
                        }

                        else
                        {
                            S += ", " + thisMacro -> GrpStringList -> Strings [ii];
                        }

                    AddLinesToMemory (S);
                }

                else
                if (thisMacro -> isStruct)
                {
                    AddLinesToMemory ("Struct with modules:");
                    string S;
                    for (int ii = 0; ii < thisMacro -> GrpStringList -> Count; ++ ii)
                        if (ii == 0)
                        {
                            S = "  " + thisMacro -> GrpStringList -> Strings [ii];
                        }

                        else
                        {
                            S += ", " + thisMacro -> GrpStringList -> Strings [ii];
                        }

                    AddLinesToMemory (S);
                }

                else
                {
                    AddLinesToMemory ("Macro: ");
                    AddLinesToMemory ("declarations:");
                    for (int ii = Global :: MacroModulesList -> IndexOf (thisMacro -> Name.c_str ()) + 1; ii < thisMacro -> Begin - 1; ++ ii) {
                        AddLinesToMemory (" " + Global :: MacroModulesList -> Strings [ii]);
                    }
                    AddLinesToMemory ("code:");
                    for (int ii = thisMacro -> Begin; ii < thisMacro -> End; ++ ii) {
                        AddLinesToMemory (" " + Global :: MacroModulesList -> Strings [ii]);
                    }
                }

            }

        }

        AddLinesToMemory (" ");
        delete Fred;
    }

    StringGrid1 = new StringGrid ();
    for (int I = 0; I < StringGrid1 -> ColCount; ++ I)
        for (int J = 0; J < StringGrid1 -> RowCount; ++ J)
            StringGrid1 -> Cells [I] [J] = "";
    StringGrid1 -> ColCount = 6;
    StringGrid1 -> FixedRows = 0;
    StringGrid1 -> RowCount = 1;
    StringGrid1 -> Cells [0] [0] = "Observations";
    StringGrid1 -> Cells [1] [0] = "Modules";
    StringGrid1 -> Cells [2] [0] = "Inputs";
    StringGrid1 -> Cells [3] [0] = "Outputs";
    StringGrid1 -> Cells [4] [0] = "Parameters";
    StringGrid1 -> Cells [5] [0] = "New Observations";
    pair < Mapstr :: iterator, Mapstr :: iterator > range = Global :: Mapreadvar.equal_range (S);
    int ii = 0;
    for (itMap = range.first; itMap != range.second; ++ itMap) {
        SS = (itMap -> second).first;
        if (StringGrid1 -> RowCount < ii + 3)
        {
            StringGrid1 -> RowCount = ii + 3;
        }

        int Indx = StringGrid1 -> Cols [0].find (SS);
        if (Indx == - 1 && Variation_Decide (Variation, (itMap -> second).second))
        {
            StringGrid1 -> Cells [0] [ii + 2] = SS;
            ++ ii;
        }

    }
    range = Global :: Mapputvar.equal_range (S);
    ii = 0;
    for (itMap = range.first; itMap != range.second; ++ itMap) {
        SS = itMap -> second.first;
        int jj = SS.find (" ");
        S1 = SS.substr (0, jj);
        S2 = SS.substr (jj + 1, SS.length () - jj - 1);
        if (S1 != S && Variation_Decide (Variation, (itMap -> second).second))
        {
            if (StringGrid1 -> RowCount < ii + 3)
            {
                StringGrid1 -> RowCount = ii + 3;
            }

            StringGrid1 -> Cells [1] [ii + 2] = S1;
            StringGrid1 -> Cells [2] [ii + 2] = S2;
            ++ ii;
        }

    }
    range = Global :: Mapgetvar.equal_range (S);
    for (itMap = range.first; itMap != range.second; ++ itMap) {
        SS = itMap -> second.first;
        int jj = SS.find (" ");
        S1 = SS.substr (0, jj);
        S2 = SS.substr (jj + 1, SS.length () - jj - 1);
        if (S1 != S && Variation_Decide (Variation, (itMap -> second).second))
        {
            int Indx = StringGrid1 -> Cols [2].find (S2);
            if (Indx == - 1)
            {
                if (StringGrid1 -> RowCount < ii + 3)
                {
                    StringGrid1 -> RowCount = ii + 3;
                }

                StringGrid1 -> Cells [1] [ii + 2] = S1;
                StringGrid1 -> Cells [2] [ii + 2] = S2;
                ++ ii;
            }

        }

    }
    range = Global :: Mapdeclvar.equal_range (S);
    ii = 0;
    for (itMap = range.first; itMap != range.second; ++ itMap) {
        SS = itMap -> second.first;
        for (itMap2 = Global :: Mapreadvar.begin (); itMap2 != Global :: Mapreadvar.end (); ++ itMap2) {
            if (itMap2 -> second.first == SS)
            {
                break;
            }

        }
        pos = itMap2;
        for (itMap2 = Global :: Mapdeclobs.begin (); itMap2 != Global :: Mapdeclobs.end (); ++ itMap2) {
            if (itMap2 -> second.first == SS)
            {
                break;
            }

        }
        pos2 = itMap2;
        if ((pos == Global :: Mapreadvar.end () || Not_Obs (S, SS)) && pos2 == Global :: Mapdeclobs.end ())
        {
            int Indx = StringGrid1 -> Cols [3].find (SS);
            if (Indx == - 1 && Variation_Decide (Variation, (itMap -> second).second) && Visible_Decide ((itMap -> second).second))
            {
                if (StringGrid1 -> RowCount < ii + 3)
                {
                    StringGrid1 -> RowCount = ii + 3;
                }

                StringGrid1 -> Cells [3] [ii + 2] = SS;
                ++ ii;
            }

        }

    }
    range = Global :: Mapdeclpar.equal_range (S);
    ii = 0;
    for (itMap = range.first; itMap != range.second; ++ itMap) {
        if (Variation_Decide (Variation, (itMap -> second).second) && Visible_Decide ((itMap -> second).second))
        {
            SS = itMap -> second.first;
            if (StringGrid1 -> RowCount < ii + 3)
            {
                StringGrid1 -> RowCount = ii + 3;
            }

            StringGrid1 -> Cells [4] [ii + 2] = SS;
            ++ ii;
        }

    }
    range = Global :: Mapdeclobs.equal_range (S);
    ii = 0;
    for (itMap = range.first; itMap != range.second; ++ itMap) {
        if (Variation_Decide (Variation, (itMap -> second).second) && Visible_Decide ((itMap -> second).second))
        {
            SS = itMap -> second.first;
            if (StringGrid1 -> RowCount < ii + 3)
            {
                StringGrid1 -> RowCount = ii + 3;
            }

            StringGrid1 -> Cells [5] [ii + 2] = SS;
            ++ ii;
        }

    }
    if (StringGrid1 -> RowCount > 1)
    {
        StringGrid1 -> FixedRows = 1;
    }

    while (listcontrol.DeleteColumn (0));
    listcontrol.DeleteAllItems ();
    listcontrol.InsertColumn (0, _T ("Observations"), LVCFMT_LEFT, 95);
    listcontrol.InsertColumn (1, _T ("Modules"), LVCFMT_LEFT, 95);
    listcontrol.InsertColumn (2, _T ("Inputs"), LVCFMT_LEFT, 95);
    listcontrol.InsertColumn (3, _T ("Outputs"), LVCFMT_LEFT, 95);
    listcontrol.InsertColumn (4, _T ("Parameters"), LVCFMT_LEFT, 95);
    listcontrol.InsertColumn (5, _T ("New Observations"), LVCFMT_LEFT, 95);
    for (int i = 1; i < StringGrid1 -> RowCount; i ++) {
        CString cstr0 (StringGrid1 -> Cells [0] [i].c_str ());
        CString cstr1 (StringGrid1 -> Cells [1] [i].c_str ());
        CString cstr2 (StringGrid1 -> Cells [2] [i].c_str ());
        CString cstr3 (StringGrid1 -> Cells [3] [i].c_str ());
        CString cstr4 (StringGrid1 -> Cells [4] [i].c_str ());
        CString cstr5 (StringGrid1 -> Cells [5] [i].c_str ());
        int nIndex = listcontrol.InsertItem (i - 1, cstr0);
        listcontrol.SetItemText (nIndex, 1, cstr1);
        listcontrol.SetItemText (nIndex, 2, cstr2);
        listcontrol.SetItemText (nIndex, 3, cstr3);
        listcontrol.SetItemText (nIndex, 4, cstr4);
        listcontrol.SetItemText (nIndex, 5, cstr5);
    }
InstrumentLogger::instance()->log_instrument_log("</CConstruct::DisplayModule(string S, ClassModule * thisModule, bool LBox1)@@@CConstruct.cpp>");
}
bool CConstruct :: Variation_Decide (int Variation_set, long Variation)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::Variation_Decide(int Variation_set, long Variation)@@@CConstruct.cpp>");
    VandP V;
    V.Set (Variation);
    long variations = V.GetV ();
    if ((variations & 2048) != 0 && Variation_set == 0 || (variations & 4096) != 0 || (variations == 0) || (variations & Variation_set) != 0)
    {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::Variation_Decide(int Variation_set, long Variation)@@@CConstruct.cpp>");
        return true;
    }

    else
    {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::Variation_Decide(int Variation_set, long Variation)@@@CConstruct.cpp>");
        return false;
    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::Variation_Decide(int Variation_set, long Variation)@@@CConstruct.cpp>");
}
bool CConstruct :: Visible_Decide (long Variation)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::Visible_Decide(long Variation)@@@CConstruct.cpp>");
    VandP V;
    V.Set (Variation);
    long Visible = V.GetP ();

InstrumentLogger::instance()->log_instrument_log("</CConstruct::Visible_Decide(long Variation)@@@CConstruct.cpp>");
    return Visible == 1;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::Visible_Decide(long Variation)@@@CConstruct.cpp>");
}
bool CConstruct :: Not_Obs (string Mod, string Var)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::Not_Obs(string Mod, string Var)@@@CConstruct.cpp>");
    pair < Mapstr :: iterator, Mapstr :: iterator > rangeIobs = Global :: Mapreadvar.equal_range (Mod);
    for (Mapstr :: iterator itMap = rangeIobs.first; itMap != rangeIobs.second; ++ itMap)
        if (Var == itMap -> second.first)
        {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::Not_Obs(string Mod, string Var)@@@CConstruct.cpp>");
            return false;
        }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::Not_Obs(string Mod, string Var)@@@CConstruct.cpp>");
    return true;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::Not_Obs(string Mod, string Var)@@@CConstruct.cpp>");
}
void CConstruct :: OnLbnDblclkList2 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnLbnDblclkList2()@@@CConstruct.cpp>");
    int selection = listbox_all_modules.GetCurSel ();
    CString cstr;
    listbox_all_modules.GetText (selection, cstr);
    CT2A str (cstr);
    int ind = - 1;
    for (int i = 0; i < Global :: AllModulesList -> Count; i ++) {
        ind = Global :: AllModulesList -> Strings [i].find (str);
        if (ind > - 1)
        {
            ind = i;
            break;
        }

    }
    string S = str;
    ClassModule * thisModule = (ClassModule *) Global :: AllModulesList -> Objects [ind];
    DisplayModule (S, thisModule, true);
    LMODULE Lvl = thisModule -> Lvl;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnLbnDblclkList2()@@@CConstruct.cpp>");
}
void CConstruct :: OnLbnDblclkList1 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnLbnDblclkList1()@@@CConstruct.cpp>");
    int selection = listbox_selected_modules.GetCurSel ();
    CString cstr;
    listbox_selected_modules.GetText (selection, cstr);
    CT2A str (cstr);
    int ind = - 1;
    for (int i = 0; i < Global :: AllModulesList -> Count; i ++) {
        string test = Global :: AllModulesList -> array [i].Name;
        ind = test.find (str);
        if (ind > - 1)
        {
            ind = i;
            break;
        }

    }
    string S = str;
    ClassModule * thisModule = (ClassModule *) Global :: AllModulesList -> Objects [ind];
    DisplayModule (S, thisModule, false);
    LMODULE Lvl = thisModule -> Lvl;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnLbnDblclkList1()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCancel2 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCancel2()@@@CConstruct.cpp>");
    CheckModel ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCancel2()@@@CConstruct.cpp>");
}
int CConstruct :: IndexOf (TStringList * Box, string S)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::IndexOf(TStringList * Box, string S)@@@CConstruct.cpp>");
    for (int ii = 0; ii < Box -> Count; ++ ii) {
        if (Box -> Strings [ii] == S)
        {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::IndexOf(TStringList * Box, string S)@@@CConstruct.cpp>");
            return ii;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::IndexOf(TStringList * Box, string S)@@@CConstruct.cpp>");
    return - 1;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::IndexOf(TStringList * Box, string S)@@@CConstruct.cpp>");
}
string CConstruct :: StringOfChar (char c, int n)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::StringOfChar(char c, int n)@@@CConstruct.cpp>");
    string result = "";
    for (int i = 0; i < n; i ++) {
        result += c;
    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::StringOfChar(char c, int n)@@@CConstruct.cpp>");
    return result;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::StringOfChar(char c, int n)@@@CConstruct.cpp>");
}
long CConstruct :: CheckModel ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::CheckModel()@@@CConstruct.cpp>");
    long Showing = SelectedModules -> Count;
    if (Showing == 0)
    {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::CheckModel()@@@CConstruct.cpp>");
        return 0;
    }

    Mapstr :: iterator itMap, pos, pos2, itMap2;
    pair < Mapstr :: iterator, Mapstr :: iterator > rangeO;
    pair < Mapstr :: iterator, Mapstr :: iterator > rangeOobs;
    pair < Mapstr :: iterator, Mapstr :: iterator > rangeI;
    pair < Mapstr :: iterator, Mapstr :: iterator > rangeIobs;
    List = new TStringList;
    ListAll = new TStringList;
    ListUsed = new TStringList;
    ListPuts = new TStringList;
    ListStats = new TStringList;
    List -> Sorted = false;
    ListUsed -> Sorted = false;
    ListPuts -> Sorted = false;
    ListStats -> Sorted = false;
    string S, SS, S1, S2;
    int indx, Error = 0, Wild = 0;
    MapAKAVar.clear ();
    CListBox * listbox = (CListBox *) GetDlgItem (IDC_LIST4);
    listbox -> ResetContent ();
    long Variation = 0;
    long Vindx = 0;
    int P;
    Again: for (int ii = 0; ii < SelectedModules -> Count; ++ ii) {
        S = SelectedModules -> Strings [ii];
        if (P = S.find ("#"), P > - 1)
        {
            Vindx = S [P + 1] - char ('0');
            Variation = (long) pow (2, S [P + 1] - char ('1'));
            S = S.replace (S.length () - 2, 2, "");
        }

        else
        {
            Variation = 0;
        }

        rangeI = Global :: Mapgetvar.equal_range (S);
        for (itMap = rangeI.first; itMap != rangeI.second; ++ itMap) {
            SS = itMap -> second.first;
            if (Variation_Decide (Variation, (itMap -> second).second))
            {
                int jj = SS.find (" ");
                S1 = SS.substr (0, jj);
                S2 = SS.substr (jj + 1, SS.length () - jj - 1);
                if (S1 [0] != '*')
                {
                    rangeO = Global :: Mapdeclvar.equal_range (S);
                    bool Found = false;
                    for (pos = rangeO.first; pos != rangeO.second; ++ pos) {
                        if (S2 == pos -> second.first)
                        {
                            Found = true;
                            break;
                        }

                    }
                    if (! Found)
                    {
                        indx = IndexOf (SelectedModules, S1);
                        if (indx == - 1)
                        {
                            indx = IndexOf (AllModules, S1);
                            if (indx == - 1)
                            {
                                LogError ("module not found: " + S1, TExcept :: ERR);
                                AddLinesToMemory ("module not found: " + S1);
                            }

                            else
                            {
                                int result = IDYES;
                                string temp = "To satisfy module " + S + " input " + SS;
                                CString cstr (temp.c_str ());
                                string temp2 = "Add module " + S1 + " ?";
                                CString cstr2 (temp2.c_str ());
                                result = MessageBox (cstr, cstr2, MB_YESNO);
                                if (result == IDYES)
                                {
                                    SelectedModules -> InsertObject (ii, S1, AllModules -> Objects [indx]);
                                }

                            }

                        }

                    }

                }

            }

        }
    }
    for (int ii = 0; ii < SelectedModules -> Count; ++ ii) {
        S = SelectedModules -> Strings [ii];
        if (P = S.find ("#"), P > - 1)
        {
            Vindx = S [P + 1] - char ('0');
            Variation = (long) pow (2, S [P + 1] - char ('1'));
            S = S.replace (S.length () - 2, 2, "");
        }

        else
        {
            Variation = 0;
        }

        rangeI = Global :: Mapdeclstat.equal_range (S);
        for (itMap = rangeI.first; itMap != rangeI.second; ++ itMap) {
            SS = itMap -> second.first;
            if (Variation_Decide (Variation, (itMap -> second).second))
            {
                ListStats -> Add (SS);
            }

        }
        rangeO = Global :: Mapdeclvar.equal_range (S);
        rangeOobs = Global :: Mapdeclobs.equal_range (S);
        rangeI = Global :: Mapgetvar.equal_range (S);
        rangeIobs = Global :: Mapreadvar.equal_range (S);
        for (itMap = rangeOobs.first; itMap != rangeOobs.second; ++ itMap) {
            SS = itMap -> second.first;
            if (Variation_Decide (Variation, (itMap -> second).second))
            {
                List -> AddObject (S + " " + SS, (TObject *) 0);
            }

        }
        for (itMap = rangeO.first; itMap != rangeO.second; ++ itMap) {
            SS = itMap -> second.first;
            if (Variation_Decide (Variation, (itMap -> second).second))
            {
                for (itMap2 = Global :: Mapreadvar.begin (); itMap2 != Global :: Mapreadvar.end (); ++ itMap2) {
                    if (itMap2 -> second.first == SS)
                    {
                        break;
                    }

                }
                pos = itMap2;
                for (itMap2 = Global :: Mapdeclobs.begin (); itMap2 != Global :: Mapdeclobs.end (); ++ itMap2) {
                    if (itMap2 -> second.first == SS)
                    {
                        break;
                    }

                }
                pos2 = itMap2;
                if ((pos == Global :: Mapreadvar.end () || Not_Obs (S, SS)) && pos2 == Global :: Mapdeclobs.end ())
                {
                    S2 = SelectedModules -> Strings [ii] + " " + SS;
                    string S3;
                    string M3;
                    bool Twice = false;
                    int tt;
                    for (tt = 0; tt < ListAll -> Count; ++ tt) {
                        int jj = ListAll -> Strings [tt].find (" ");
                        S3 = ListAll -> Strings [tt].substr (jj + 1, ListAll -> Strings [tt].length () - jj - 1);
                        M3 = ListAll -> Strings [tt].substr (0, jj);
                        if (S3 == SS && S != M3 && S != "obs")
                        {
                            if (S3 [S3.length () - 1] != '#')
                            {
                                Twice = true;
                                break;
                            }

                        }

                    }
                    if (Twice)
                    {
                        int result = IDYES;
                        string temp = string ("YES to remove '" + S + "' or NO to remove '" + M3 + "'");
                        CString cstr (temp.c_str ());
                        string temp2 = string ("Module output generated twice: '" + S2 + "' defined earlier as '" + ListAll -> Strings [tt] + "'");
                        CString cstr2 (temp2.c_str ());
                        result = MessageBox (cstr, cstr2, MB_YESNO);
                        if (result == IDYES)
                        {
                            tt = IndexOf (SelectedModules, S);
                        }

                        else
                        if (result == IDNO)
                        {
                            tt = IndexOf (SelectedModules, M3);
                        }

                        SelectedModules -> Delete (tt);
                        Entries.clear ();
                        List -> Clear ();
                        ListAll -> Clear ();
                        ListUsed -> Clear ();
                        ListPuts -> Clear ();
                        ListStats -> Clear ();
                        goto Again;
                    }

                    ListAll -> Add (S2);
                    int indx = List -> IndexOf (S2);
                    if (indx == - 1)
                    {
                        List -> AddObject (S2, (TObject *) 0);
                    }

                }

            }

        }
        for (itMap = rangeI.first; itMap != rangeI.second; ++ itMap) {
            SS = itMap -> second.first;
            if (Variation_Decide (Variation, (itMap -> second).second))
            {
                int indx = List -> IndexOf (SS);
                if (indx == - 1 && SS [0] != '*')
                {
                    int jj = SS.find (" ");
                    S2 = SS.substr (jj + 1, SS.length () - jj - 1);
                    S1 = S + ' ' + S2;
                    indx = List -> IndexOf (S1);
                    if (indx == - 1 && S1 [S1.length () - 2] == '@')
                    {
                        indx = List -> IndexOf (SS.substr (0, SS.find ("@")));
                    }

                }

                if (indx == - 1)
                {
                    ListUsed -> AddObject (SS, (TObject *) - (ii + 1));
                }

                else
                {
                    int Cnt = (int) List -> Objects [indx];
                    List -> Objects [indx] = (TObject *) ++ Cnt;
                    ListUsed -> AddObject (SS, (TObject *) ii);
                }

            }

        }
        rangeI = Global :: Mapputvar.equal_range (S);
        for (itMap = rangeI.first; itMap != rangeI.second; ++ itMap) {
            SS = itMap -> second.first;
            if (Variation_Decide (Variation, (itMap -> second).second))
            {
                int indx = List -> IndexOf (SS);
                if (indx == - 1)
                {
                    ListPuts -> AddObject (SS, (TObject *) ListUsed -> Count);
                    ListUsed -> AddObject (SS, (TObject *) - (ii + 1));
                }

                else
                {
                    int Cnt = (int) List -> Objects [indx];
                    List -> Objects [indx] = (TObject *) ++ Cnt;
                    ListPuts -> AddObject (SS, (TObject *) ListUsed -> Count);
                    ListUsed -> AddObject (SS, (TObject *) ii);
                }

            }

        }
        for (itMap = rangeIobs.first; itMap != rangeIobs.second; ++ itMap) {
            SS = itMap -> second.first;
            if (Variation_Decide (Variation, (itMap -> second).second))
            {
                for (itMap2 = Global :: Mapdeclobs.begin (); itMap2 != Global :: Mapdeclobs.end (); ++ itMap2) {
                    if (itMap2 -> second.first == SS)
                    {
                        break;
                    }

                }
                pos = itMap2;
                if (pos == Global :: Mapdeclobs.end ())
                {
                    continue;
                }

                indx = IndexOf (SelectedModules, pos -> first);
                if (indx != ii && indx > - 1)
                {
                    S1 = pos -> first;
                    SS = pos -> second.first;
                    S2 = S1 + " " + SS;
                    indx = List -> IndexOf (S2);
                    if (indx == - 1)
                    {
                        ListUsed -> AddObject (S2, (TObject *) - (ii + 1));
                    }

                    else
                    {
                        int Cnt = (int) List -> Objects [indx];
                        List -> Objects [indx] = (TObject *) ++ Cnt;
                        ListUsed -> AddObject (S2, (TObject *) ii);
                    }

                }

            }

        }
    }
    for (int ii = 0; ii < SelectedModules -> Count; ++ ii) {
        S = SelectedModules -> Strings [ii];
        if (P = S.find ("#"), P > - 1)
        {
            Vindx = S [P + 1] - char ('0');
            Variation = (long) pow (2, S [P + 1] - char ('1'));
            S = S.replace (S.length () - 2, 2, "");
        }

        else
        {
            Vindx = 0;
            Variation = 0;
        }

        ClassModule * Module = (ClassModule *) SelectedModules -> Objects [ii];
        TStringList * Choices = new TStringList;
        Choices -> CommaText (Module -> PeerVar);
        if (Choices -> Count > Vindx)
        {
            SS = Choices -> Strings [Vindx];
        }

        else
        if (Choices -> Count)
        {
            SS = Choices -> Strings [Choices -> Count - 1];
        }

        else
        {
            SS = "";
        }

        if (SS != "")
        {
            for (int jj = 0; jj < List -> Count; ++ jj) {
                string Entry = List -> Strings [jj];
                long kk = Entry.find (' ');
                S1 = Entry.substr (kk + 1, Entry.length ());
                if (S1 == SS)
                {
                    break;
                }

            }
            if (S1 == SS)
            {
                SS = "* " + SS;
                ListUsed -> AddObject (SS, (TObject *) - (ii + 1));
                string SSS = "Ranking module '" + S + "' to '" + SS + "'";
                AddLinesToMemory (SSS);
            }

            else
            {
                S2 = "Ranking module '" + S + "' to an inaccessible variable '" + SS + "'";
                AddLinesToMemory (S2);
            }

        }

    }
    for (int ii = 0; ii < ListUsed -> Count; ++ ii) {
        if (ListUsed -> Strings [ii] [0] == '*')
        {
            string Var = ListUsed -> Strings [ii];
            Var = Var.substr (2, Var.length ());
            int jj = Best_Match (Var, List);
            if (jj > - 1)
            {
                ++ Wild;
                int thisModule = (int) ListUsed -> Objects [ii];
                thisModule = - (thisModule + 1);
                string Module = SelectedModules -> Strings [thisModule];
                ListUsed -> Strings [ii] = List -> Strings [jj];
                ListUsed -> Objects [ii] = (TObject *) abs ((int) ListUsed -> Objects [ii] + 1);
            }

        }

        else
        {
            int NotFoundIndx = (int) ListUsed -> Objects [ii];
            if (NotFoundIndx < 0)
            {
                int indx = List -> IndexOf (ListUsed -> Strings [ii]);
                if (ListPuts -> IndexOf (ListUsed -> Strings [ii]) == - 1 && indx == - 1)
                {
                    ++ Error;
                }

                if (indx > - 1)
                {
                    ListUsed -> Objects [ii] = (TObject *) abs ((int) ListUsed -> Objects [ii] + 1);
                    continue;
                }

            }

            else
            {
                continue;
            }

        }

        int NotFoundIndx = (int) ListUsed -> Objects [ii];
        if (NotFoundIndx < 0 && ListUsed -> Strings [ii] [0] == '*')
        {
            string Message;
            string Message2 (" Suggestions: ");
            ++ Error;
            LogError ("wild variable not found: " + ListUsed -> Strings [ii], TExcept :: WARNING);
            int kk = abs ((int) ListUsed -> Objects [ii] + 1);
            string Sroot = ListUsed -> Strings [ii];
            if (Sroot.find ("@") != - 1)
            {
                Sroot = Sroot.substr (0, Sroot.find ("@"));
            }

            Message = "wild variable not found: \" " + Sroot + "\" used by " + SelectedModules -> Strings [kk] + ".";
            string SW = ListUsed -> Strings [ii];
            SW = SW.substr (2, ListUsed -> Strings [ii].length ());
            bool Once = false;
            bool rootVar = (SW.find ("@") == - 1);
            if (! rootVar)
            {
                SW = SW.substr (0, SW.find ("@"));
            }

            for (itMap = Global :: Mapdeclvar.begin (); itMap != Global :: Mapdeclvar.end (); ++ itMap) {
                SS = itMap -> second.first;
                bool rootVar1 = (SS.find ("@") == - 1);
                if (! rootVar1)
                {
                    SS = SS.substr (0, SS.find ("@"));
                }

                if (SW == SS)
                {
                    Once = true;
                    string S = itMap -> first;
                    VandP V;
                    V.Set ((itMap -> second).second);
                    long Variation = V.GetV () & 2047;
                    if (Variation > 0)
                    {
                        long NN = (long) (log (Variation) / log (2) + 1);
                        string AA = "#0";
                        AA [1] += (char) NN;
                        S += AA;
                    }

                    Message2 += S + ", ";
                }

            }
            if (! Once)
            {
                AddLinesToMemory (Message + " No " + Message2);
            }

            else
            {
                AddLinesToMemory (Message + Message2);
            }

        }

    }
    if (Error)
    {
        string temp = "" + Error;
        S = "Errors: " + temp + ';';
        string Line = StringOfChar ('*', 80);
        ;
        AddLinesToMemory (Line);
    }

    else
    {
        bool Loop = true;
        int Loops = 0;
        TStringList * OrgList = new TStringList;
        for (int ii = 0; ii < SelectedModules -> Count; ++ ii) {
            OrgList -> AddObject (SelectedModules -> Strings [ii], (TObject *) ii);
            Entries.push_back (ClassEntry ());
        }
        while (Loop) {
            Loop = false;
            ++ Loops;
            for (int ii = 0; ii < SelectedModules -> Count; ++ ii) {
                Entries [ii].module = (ClassModule *) SelectedModules -> Objects [ii];
                Entries [ii].Name = SelectedModules -> Strings [ii];
                Entries [ii].rank = 0;
                Entries [ii].used = false;
                unsigned long index = OrgList -> IndexOf (SelectedModules -> Strings [ii]);
                if (index < 0 || index > Entries.size () - 1)
                {
                    LogError ("Build error: outside range", TExcept :: ERR);
                }

                Entries [ii].OrigPos = index;
            }
            for (int ii = 0; ii < ListPuts -> Count; ++ ii) {
                ListPuts -> Strings [ii] = ListUsed -> Strings [(int) ListPuts -> Objects [ii]];
            }
            for (int ii = 0; ii < ListUsed -> Count; ++ ii) {
                unsigned long thisModule = (int) ListUsed -> Objects [ii];
                for (unsigned long nn = 0; nn < Entries.size (); ++ nn) {
                    if (Entries [nn].OrigPos == thisModule)
                    {
                        thisModule = nn;
                        if (thisModule < 0 || thisModule > Entries.size () - 1)
                        {
                            LogError ("Build error: outside range", TExcept :: ERR);
                        }

                        break;
                    }

                }
                string Entry = ListUsed -> Strings [ii];
                long kk = Entry.find (' ');
                string UsesModule = Entry.substr (0, kk);
                string Var_only = Entry.substr (kk + 1, Entry.length () - kk - 1);
                unsigned long index;
                ClassModule * checkModule = NULL;
                for (index = 0; index < Entries.size (); ++ index) {
                    string Root = Entries [index].Name;
                    if (Root == UsesModule)
                    {
                        checkModule = Entries [index].module;
                        break;
                    }

                }
                if (checkModule && checkModule -> isGroup)
                {
                    Entries [index].used = true;
                    continue;
                }

                if (index >= Entries.size ())
                {
                    continue;
                }

                Entries [index].used = true;
                if (UsesModule == Entries [thisModule].Name)
                {
                    continue;
                }

                int jj = Best_Match (Var_only, ListPuts);
                if (jj > - 1 && ii == ((int) ListPuts -> Objects [jj]))
                {
                    continue;
                }

                jj = Best_Match (Var_only, ListStats);
                if (jj > - 1)
                {
                    continue;
                }

                long B = (long) powl (2, index);
                Entries [thisModule].rank |= B;
            }
            for (unsigned long ii = 0; ii < Entries.size () - 1; ++ ii) {
                for (unsigned long jj = ii + 1; jj < Entries.size (); ++ jj) {
                    if (Entries [jj].rank < Entries [ii].rank)
                    {
                        MoveDown (Entries, ii, jj);
                        Loop = true;
                    }

                }
            }
            for (long ii = 0; ii < (long) Entries.size () - 1; ++ ii) {
                ClassModule * E2 = (ClassModule *) Entries [ii].module;
                if (E2 -> PeerRank < 0)
                {
                    Entries [ii].used = true;
                }

                for (long jj = ii + 1; jj < (long) Entries.size (); ++ jj) {
                    if (Entries [jj].rank == Entries [ii].rank)
                    {
                        ClassModule * E1 = (ClassModule *) Entries [jj].module;
                        if (labs (E1 -> PeerRank) < labs (E2 -> PeerRank))
                        {
                            MoveDown (Entries, ii, jj);
                            Loop = true;
                        }

                    }

                }
            }
            SelectedModules -> Clear ();
            for (unsigned long ii = 0; ii < Entries.size (); ++ ii)
                SelectedModules -> AddObject (Entries [ii].Name, (TObject *) Entries [ii].module);
            if (Loops > 20)
            {
                Loop = false;
                Loops = - Loops;
            }

        }
        bool Removed = false;
        for (unsigned long ii = 0; ii < Entries.size () - 1; ++ ii) {
            if (! Entries [ii].used && Entries [ii].Name != "basin")
            {
                int result = IDYES;
                CString temp (Entries [ii].Name.c_str ());
                result = MessageBox (temp, _T ("Module not used!  Remove module?"), MB_YESNO);
                if (result != IDYES)
                {
                    continue;
                }

                int indx = IndexOf (SelectedModules, Entries [ii].Name);
                SelectedModules -> Delete (indx);
                Removed = true;
            }

        }
        Entries.clear ();
        List -> Clear ();
        ListAll -> Clear ();
        ListUsed -> Clear ();
        ListPuts -> Clear ();
        ListStats -> Clear ();
        if (Removed)
        {
            goto Again;
        }

        int moves = 0;
        for (unsigned long ii = 0; ii < Entries.size (); ++ ii)
            if (Entries [ii].OrigPos != ii)
            {
                ++ moves;
            }

        if (Loops)
        {
            AddLinesToMemory (" ");
            LogError ("Number of loops to build: " + to_string (Loops) + " and modules moved: " + to_string (moves), TExcept :: WARNING);
            AddLinesToMemory ("Number of loops to build: " + to_string (Loops) + " and modules moved: " + to_string (moves));
        }

        else
        {
            LogError ("Problem with modules in build. Possibly modules referencing one another!", TExcept :: WARNING);
            AddLinesToMemory ("Problem with modules in build. Possibly modules referencing one another!");
            MessageBox (_T ("Problem with modules in build."), _T ("Possibly modules referencing one another!"), MB_OK);
        }

        Entries.clear ();
        delete OrgList;
        S = "";
    }

    delete List;
    delete ListAll;
    delete ListUsed;

InstrumentLogger::instance()->log_instrument_log("</CConstruct::CheckModel()@@@CConstruct.cpp>");
    return Error;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::CheckModel()@@@CConstruct.cpp>");
}
long CConstruct :: Best_Match (string Var, TStringList * List)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::Best_Match(string Var, TStringList * List)@@@CConstruct.cpp>");
    long found = 10;
    long Indx = - 1;
    for (int jj = 0; jj < List -> Count; ++ jj) {
        string Entry = List -> Strings [jj];
        long kk = Entry.find (' ');
        string Var1 = Entry.substr (kk + 1, Entry.length ());
        bool rootVar = (Var.find ("@") == - 1);
        bool rootVar1 = (Var1.find ("@") == - 1);
        if (Var1 == Var)
        {
            found = 0;
            Indx = jj;
            break;
        }

        else
        if (rootVar && ! rootVar1 && Var == Var1.substr (0, Var1.find ("@")) && found > 1)
        {
            found = 1;
            Indx = jj;
        }

        else
        if (! rootVar && rootVar1 && Var.substr (0, Var.find ("@")) == Var1 && found > 2)
        {
            found = 2;
            Indx = jj;
        }

        else
        if (! rootVar && ! rootVar1 && Var.substr (0, Var.find ("@")) == Var1.substr (0, Var1.find ("@")) && found > 3)
        {
            found = 3;
            Indx = jj;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::Best_Match(string Var, TStringList * List)@@@CConstruct.cpp>");
    return Indx;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::Best_Match(string Var, TStringList * List)@@@CConstruct.cpp>");
}
void CConstruct :: UpdateCheckBoxes ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::UpdateCheckBoxes()@@@CConstruct.cpp>");
    AllModules -> Clear ();
    listbox_all_modules.ResetContent ();
    for (int ii = 0; ii < Global :: AllModulesList -> Count; ++ ii) {
        LMODULE str = ((ClassModule *) Global :: AllModulesList -> Objects [ii]) -> Lvl;
        if (str == LMODULE :: MACRO)
        {
            int a = 10;
        }

        if (((ClassModule *) Global :: AllModulesList -> Objects [ii]) -> Lvl == LMODULE :: BASIC)
        {
            if (checkbox_basic.GetCheck () == 1)
            {
                AddToAllModules (Global :: AllModulesList -> Strings [ii], (TObject *) Global :: AllModulesList -> Objects [ii]);
            }

        }

        if (((ClassModule *) Global :: AllModulesList -> Objects [ii]) -> Lvl == LMODULE :: ADVANCE)
        {
            if (checkbox_advanced.GetCheck () == 1)
            {
                AddToAllModules (Global :: AllModulesList -> Strings [ii], (TObject *) Global :: AllModulesList -> Objects [ii]);
            }

        }

        if (((ClassModule *) Global :: AllModulesList -> Objects [ii]) -> Lvl == LMODULE :: CUSTOM)
        {
            if (checkbox_custom.GetCheck () == 1)
            {
                AddToAllModules (Global :: AllModulesList -> Strings [ii], (TObject *) Global :: AllModulesList -> Objects [ii]);
            }

        }

        if (str == LMODULE :: MACRO)
        {
            if (checkbox_macro.GetCheck () == 1)
            {
                AddToAllModules (Global :: AllModulesList -> Strings [ii], (TObject *) Global :: AllModulesList -> Objects [ii]);
            }

        }

        if (((ClassModule *) Global :: AllModulesList -> Objects [ii]) -> Lvl == LMODULE :: OBSOL)
        {
            if (checkbox_obsolete.GetCheck () == 1)
            {
                AddToAllModules (Global :: AllModulesList -> Strings [ii], (TObject *) Global :: AllModulesList -> Objects [ii]);
            }

        }

        if (((ClassModule *) Global :: AllModulesList -> Objects [ii]) -> Lvl == LMODULE :: PROTO)
        {
            if (checkbox_prototype.GetCheck () == 1)
            {
                AddToAllModules (Global :: AllModulesList -> Strings [ii], (TObject *) Global :: AllModulesList -> Objects [ii]);
            }

        }

        if (((ClassModule *) Global :: AllModulesList -> Objects [ii]) -> Lvl == LMODULE :: SUPPORT)
        {
            if (checkbox_support.GetCheck () == 1)
            {
                AddToAllModules (Global :: AllModulesList -> Strings [ii], (TObject *) Global :: AllModulesList -> Objects [ii]);
            }

        }

    }
InstrumentLogger::instance()->log_instrument_log("</CConstruct::UpdateCheckBoxes()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCheck1 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCheck1()@@@CConstruct.cpp>");
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCheck1()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCheck2 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCheck2()@@@CConstruct.cpp>");
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCheck2()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCheck3 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCheck3()@@@CConstruct.cpp>");
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCheck3()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCheck4 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCheck4()@@@CConstruct.cpp>");
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCheck4()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCheck5 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCheck5()@@@CConstruct.cpp>");
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCheck5()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCheck6 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCheck6()@@@CConstruct.cpp>");
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCheck6()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCheck7 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCheck7()@@@CConstruct.cpp>");
    UpdateCheckBoxes ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCheck7()@@@CConstruct.cpp>");
}
long CConstruct :: BuildClick ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::BuildClick()@@@CConstruct.cpp>");
    int test = AllModules -> Count;
    long Showing = SelectedModules -> Count;
    if (Showing == 0)
    {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::BuildClick()@@@CConstruct.cpp>");
        return 1;
    }

    long Errors = CheckModel ();
    if (Errors)
    {
        string str = "Errors: " + to_string (Errors);
        CString cstr (str.c_str ());
        MessageBox (L"Model has unsatisfied module inputs or multiple definitions", cstr, MB_OK);

InstrumentLogger::instance()->log_instrument_log("</CConstruct::BuildClick()@@@CConstruct.cpp>");
        return Errors;
    }

    if (ModifyBuild)
    {
        SaveBuildParameters ();
        RemoveParameter ();
    }

    MapAKACopy = Global :: MapAKA;
    CRHMmain * crhmmain = CRHMmain :: getInstance ();
    crhmmain -> ClearModules (false);
    if (Global :: nhru > Global :: maxhru)
    {
        Update_Main_DIM (TDim :: NHRU, Global :: nhru);
    }

    for (int ii = 0; ii < SelectedModules -> Count; ++ ii) {
        ClassModule * thisModule = (ClassModule *) SelectedModules -> Objects [ii];
        if (FormatMax (thisModule -> variation_max) > 0)
        {
            thisModule -> variation = (unsigned short) pow (2, thisModule -> hh - 1);
            int found = SelectedModules -> Strings [ii].find ("#");
            if (found > - 1)
            {
                SelectedModules -> Strings [ii] = SelectedModules -> Strings [ii].replace (SelectedModules -> Strings [ii].length () - 2, 2, "");
            }

        }

    }
    Global :: OurModulesList -> Assign (SelectedModules);
    Execute = true;
    Global :: BuildFlag = TBuild :: BUILD;
    if (! MapAKAVar.empty ())
    {
        Mapstr2 :: iterator it;
        for (it = MapAKAVar.begin (); it != MapAKAVar.end (); ++ it)
            MapAKACopy.insert (* it);
    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::BuildClick()@@@CConstruct.cpp>");
    return Errors;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::BuildClick()@@@CConstruct.cpp>");
}
void CConstruct :: SaveBuildParameters ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::SaveBuildParameters()@@@CConstruct.cpp>");
    ClassPar * thisPar;
    if (ModifyBuild)
    {
        RemoveSharedParams ();
        ParaList = new TStringList;
        MapPar :: iterator itPar;
        itPar = Global :: MapPars.begin ();
        while (itPar != Global :: MapPars.end ()) {
            thisPar = (* itPar).second;
            string S = string (((* itPar).second) -> module.c_str ()) + " " + ((* itPar).second) -> param.c_str ();
            if (ParaList -> IndexOf (S) == - 1)
            {
                ClassPar * par (new ClassPar (* thisPar));
                ParaList -> AddObject (S, (TObject *) par);
            }

            ++ itPar;
        }
    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::SaveBuildParameters()@@@CConstruct.cpp>");
}
void CConstruct :: ReadBuildParameters ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::ReadBuildParameters()@@@CConstruct.cpp>");
    MapPar :: iterator itPar;
    ClassPar * thisPar;
    if (ModifyBuild)
    {
        itPar = Global :: MapPars.begin ();
        while (itPar != Global :: MapPars.end ()) {
            thisPar = (* itPar).second;
            string S = string (((* itPar).second) -> module.c_str ()) + " " + ((* itPar).second) -> param.c_str ();
            int ii = ParaList -> IndexOf (S);
            if (ii != - 1)
            {
                ClassPar * OldPar = (ClassPar *) ParaList -> Objects [ii];
                thisPar -> Change (* OldPar);
            }

            ++ itPar;
        }
        for (int ii = 0; ii < ParaList -> Count; ++ ii)
            delete (ClassPar *) ParaList -> Objects [ii];
        delete ParaList;
        ParaList = NULL;
        CRHMmain * crhmmain = CRHMmain :: getInstance ();
        crhmmain -> SqueezeParams ();
    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::ReadBuildParameters()@@@CConstruct.cpp>");
}
void CConstruct :: RemoveSharedParams ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::RemoveSharedParams()@@@CConstruct.cpp>");
    Global :: Mapgetvar.clear ();
    Global :: Mapputvar.clear ();
    Global :: Mapreadvar.clear ();
    Global :: Mapdeclvar.clear ();
    Global :: Mapdeclstat.clear ();
    Global :: Mapdeclpar.clear ();
    Global :: Mapdeclobs.clear ();
    Global :: DeclRootList -> Clear ();
    Global :: BuildFlag = TBuild :: BUILD;
    for (int ii = 0; ii < Global :: OurModulesList -> Count; ++ ii)
        ((ClassModule *) Global :: OurModulesList -> Objects [ii]) -> decl ();
    Global :: BuildFlag = TBuild :: DECL;
    ClassPar * thisPar;
    MapPar :: iterator itPar;
    long count;
    itPar = Global :: SharedMapPars.begin ();
    while (itPar != Global :: SharedMapPars.end ()) {
        thisPar = (* itPar).second;
        count = ClassParFindParBld (thisPar);
        Global :: MapPars.erase ("Shared " + thisPar -> param);
        delete thisPar;
        ++ itPar;
    }
    Global :: SharedMapPars.clear ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::RemoveSharedParams()@@@CConstruct.cpp>");
}
void CConstruct :: RemoveParameter ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::RemoveParameter()@@@CConstruct.cpp>");
    MapPar :: iterator itPar;
    ClassPar * thisPar;
    for (itPar = Global :: MapPars.begin (); itPar != Global :: MapPars.end ();) {
        thisPar = (* itPar).second;
        int kk = IndexOf (SelectedModules, thisPar -> module.c_str ());
        if (kk == - 1 && thisPar -> module != "shared")
        {
            Global :: MapPars.erase (itPar ++);
            delete thisPar;
        }

        else
        {
            ++ itPar;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</CConstruct::RemoveParameter()@@@CConstruct.cpp>");
}
long CConstruct :: FormatMax (long M)
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::FormatMax(long M)@@@CConstruct.cpp>");
    M = M & 2047;
    if (M == 0)
    {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::FormatMax(long M)@@@CConstruct.cpp>");
        return 0;
    }

    for (int ii = 1; ii < 8; ++ ii) {
        M = (M >> 1);
        if (M == 0)
        {

InstrumentLogger::instance()->log_instrument_log("</CConstruct::FormatMax(long M)@@@CConstruct.cpp>");
            return ii;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::FormatMax(long M)@@@CConstruct.cpp>");
    return - 1;
InstrumentLogger::instance()->log_instrument_log("</CConstruct::FormatMax(long M)@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedOk ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedOk()@@@CConstruct.cpp>");
    long Errors = BuildClick ();
    if (! Errors)
    {
        CDialog :: OnOK ();
    }

InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedOk()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedCancel ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedCancel()@@@CConstruct.cpp>");
    CDialog :: OnCancel ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedCancel()@@@CConstruct.cpp>");
}
void CConstruct :: OnLbnSelchangeList1 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnLbnSelchangeList1()@@@CConstruct.cpp>");
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnLbnSelchangeList1()@@@CConstruct.cpp>");
}
void CConstruct :: OnBnClickedButton1 ()
{
InstrumentLogger::instance()->log_instrument_log("<CConstruct::OnBnClickedButton1()@@@CConstruct.cpp>");
    RemoveSelectedModule ();
InstrumentLogger::instance()->log_instrument_log("</CConstruct::OnBnClickedButton1()@@@CConstruct.cpp>");
}