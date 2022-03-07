#include "ClassMacro.h"
#include "InstrumentLogger.h"

#if defined (__linux__) || defined (__APPLE__) || defined (__MINGW32__)
    #include <cxxabi.h>
#endif

double Fday ()
{
InstrumentLogger::instance()->log_instrument_log("<Fday()@@@ClassMacro.cpp>");
    int year, month, day, h, m;
    StandardConverterUtility :: DecodeDateTime (Global :: DTnow, & year, & month, & day, & h, & m);

InstrumentLogger::instance()->log_instrument_log("</Fday()@@@ClassMacro.cpp>");
    return day;
InstrumentLogger::instance()->log_instrument_log("</Fday()@@@ClassMacro.cpp>");
}
double Fmonth ()
{
InstrumentLogger::instance()->log_instrument_log("<Fmonth()@@@ClassMacro.cpp>");
    int year, month, day, h, m;
    StandardConverterUtility :: DecodeDateTime (Global :: DTnow, & year, & month, & day, & h, & m);

InstrumentLogger::instance()->log_instrument_log("</Fmonth()@@@ClassMacro.cpp>");
    return month;
InstrumentLogger::instance()->log_instrument_log("</Fmonth()@@@ClassMacro.cpp>");
}
double Fyear ()
{
InstrumentLogger::instance()->log_instrument_log("<Fyear()@@@ClassMacro.cpp>");
    int year, month, day, h, m;
    StandardConverterUtility :: DecodeDateTime (Global :: DTnow, & year, & month, & day, & h, & m);

InstrumentLogger::instance()->log_instrument_log("</Fyear()@@@ClassMacro.cpp>");
    return year;
InstrumentLogger::instance()->log_instrument_log("</Fyear()@@@ClassMacro.cpp>");
}
double Fjulian ()
{
InstrumentLogger::instance()->log_instrument_log("<Fjulian()@@@ClassMacro.cpp>");

InstrumentLogger::instance()->log_instrument_log("</Fjulian()@@@ClassMacro.cpp>");
    return Julian ("now");
InstrumentLogger::instance()->log_instrument_log("</Fjulian()@@@ClassMacro.cpp>");
}
double Fgetstep ()
{
InstrumentLogger::instance()->log_instrument_log("<Fgetstep()@@@ClassMacro.cpp>");

InstrumentLogger::instance()->log_instrument_log("</Fgetstep()@@@ClassMacro.cpp>");
    return getstep ();
InstrumentLogger::instance()->log_instrument_log("</Fgetstep()@@@ClassMacro.cpp>");
}
double Fgetfirst ()
{
InstrumentLogger::instance()->log_instrument_log("<Fgetfirst()@@@ClassMacro.cpp>");

InstrumentLogger::instance()->log_instrument_log("</Fgetfirst()@@@ClassMacro.cpp>");
    return (getstep () % Global :: Freq == 1);
InstrumentLogger::instance()->log_instrument_log("</Fgetfirst()@@@ClassMacro.cpp>");
}
double Fgetlast ()
{
InstrumentLogger::instance()->log_instrument_log("<Fgetlast()@@@ClassMacro.cpp>");

InstrumentLogger::instance()->log_instrument_log("</Fgetlast()@@@ClassMacro.cpp>");
    return (getstep () % Global :: Freq == 0);
InstrumentLogger::instance()->log_instrument_log("</Fgetlast()@@@ClassMacro.cpp>");
}
double Fgetrand ()
{
InstrumentLogger::instance()->log_instrument_log("<Fgetrand()@@@ClassMacro.cpp>");

InstrumentLogger::instance()->log_instrument_log("</Fgetrand()@@@ClassMacro.cpp>");
    return (double) rand () / RAND_MAX;
InstrumentLogger::instance()->log_instrument_log("</Fgetrand()@@@ClassMacro.cpp>");
}
ClassMacro * ClassMacro :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassMacro::klone(string name) const@@@ClassMacro.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::klone(string name) const@@@ClassMacro.cpp>");
    return new ClassMacro (name, MacroBegin, "11/13/07");
InstrumentLogger::instance()->log_instrument_log("</ClassMacro::klone(string name) const@@@ClassMacro.cpp>");
}
void ClassMacro :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassMacro::decl(void)@@@ClassMacro.cpp>");
    if (isGroup || isStruct)
    {
        if (Grpnhru > 0)
        {
            nhru = Grpnhru;
            if (Grpnhru > Global :: maxhru)
            {
                Global :: maxhru = Grpnhru;
            }

        }

        else
        if (nhru == 0)
        {
            nhru = Global :: nhru;
        }

        string S = Common :: longtoStr ((max < long > (GroupCnt, StructCnt)));
        if (isGroup)
        {
            declparam ("HRU_group", TDim :: NHRU, S, "1", "1e3", "group #", "()", & HRU_group);
        }

        if (isStruct)
        {
            std :: string Choice;
            std :: ostringstream temp;
            for (size_t it = 0; it < GrpStringList -> size (); it ++) {
                if (it)
                {
                    temp << ", ";
                }

                temp << GrpStringList -> at (it).first;
            }
            temp << endl;
            Choice = temp.str ();
            declparam ("HRU_struct", TDim :: NHRU, "[1]", "1", Common :: longtoStr (GrpStringList -> size ()), string ("select using 1/2/3 ... module/group from '" + Choice + "'"), "()", & HRU_struct);
        }

        Modules.clear ();
        unsigned short Variation;
        for (size_t ii = 0; ii < GrpStringList -> size (); ii ++) {
            std :: string S = GrpStringList -> at (ii).first;
            std :: string :: size_type idx = S.find ("#");
            if (idx != std :: string :: npos)
            {
                Variation = (unsigned short) (pow (2, S [idx + 1] - char ('1')));
                S = S.substr (0, idx);
            }

            else
            {
                Variation = 0;
            }

            int Indx = Global :: AllModulesList -> count (S);
            if (Indx == 0)
            {
                Common :: Message (string ("Module: '" + S + "' not found!"), "Macro Error");
                CRHMException Except (string (string ("module: ") + S + " not found in " + Name).c_str (), TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

            ClassModule * Mod = Global :: AllModulesList -> find (S) -> second;
            ModulePtr Test (Mod -> klone (Name));
            std :: weak_ptr < ClassModule > MG (Test);

            #if defined (_WIN32) && ! defined (__MINGW32__)
                string S1 = typeid (* Mod).name ();
                string S2 = typeid (* Test).name ();
                S2 = S2.substr (6, S2.length ());
            #endif

            #if defined (__linux__) || defined (__APPLE__) || defined (__MINGW32__)
                int demangle_status = 0;
                string S1 = abi :: __cxa_demangle (typeid (* Mod).name (), 0, 0, & demangle_status);
                auto Test_raw = Test.get ();
                string S2 = abi :: __cxa_demangle (typeid (* Test_raw).name (), 0, 0, & demangle_status);
            #endif

            if (S2 == "ClassMacro")
            {
                Test -> ID = S;
            }

            else
            {
                auto Test_raw = Test.get ();
                Test -> ID = typeid (* Test_raw).name ();
                if (S2 == "Classobs")
                {
                    this -> ObsModule = (ClassModule *) Test.get ();
                }

            }

            Test -> Name = Name;
            Test -> HRU_struct = this -> HRU_struct;
            Test -> nhru = nhru;
            Test -> GroupCnt = this -> GroupCnt;
            Test -> StructCnt = this -> StructCnt;
            Test -> variation = Variation;
            Test -> DLLName = Mod -> DLLName;
            Test -> NameRoot = Mod -> NameRoot;
            Test -> Version = this -> Version;
            if (isStruct)
            {
                Test -> ModuleIndx = ii + 1;
            }

            else
            {
                Test -> ModuleIndx = this -> ModuleIndx;
            }

            Modules.push_back (Test);
        }
        list < ModulePtr > :: iterator iterM;
        iterM = Modules.begin ();
        while (iterM != Modules.end ()) {
            ClassModule * MPP = (ClassModule *) (* iterM).get ();
            string SS = (* iterM) -> NameRoot.c_str ();
            if ((* iterM) -> variation > 0)
            {
                string AA = "#0";
                AA [1] += (char) (log ((* iterM) -> variation) / log (2) + 1);
                SS += AA;
            }

            long jj = - 1;
            for (size_t it = 0; it < GrpStringList -> size (); it ++) {
                if (GrpStringList -> at (it).first == SS)
                {
                    jj = it;
                    break;
                }

            }
            GrpStringList -> at (jj).second = MPP;
            (* iterM) -> nhru = nhru;
            (* iterM) -> decl ();
            ++ iterM;
        }

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::decl(void)@@@ClassMacro.cpp>");
        return;
    }

    list < DefCRHMPtr > :: iterator iterC;
    iterC = Calls.begin ();
    while (iterC != Calls.end ()) {
        std :: weak_ptr < DefCRHM > CP ((* iterC));
        (* iterC) -> CallDecl ();
        ++ iterC;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassMacro::decl(void)@@@ClassMacro.cpp>");
}
void ClassMacro :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassMacro::init(void)@@@ClassMacro.cpp>");
    nhru = getdim (TDim :: NHRU);
    if (isGroup || isStruct)
    {
        LogMessage (" ");
        string SS;
        if (isGroup)
        {
            SS = "Group: '" + NameRoot + "' ->";
        }

        else
        {
            SS = "Struct: '" + NameRoot + "' ->";
        }

        for (size_t ii = 0; ii < GrpStringList -> size (); ++ ii) {
            SS += " " + GrpStringList -> at (ii).first;
        }
        LogMessage (SS.c_str ());
        list < ModulePtr > :: iterator iter;
        iter = Modules.begin ();
        while (iter != Modules.end ()) {
            std :: weak_ptr < ClassModule > MP ((* iter));
            (* iter) -> init ();
            ++ iter;
        }
        if (StructCnt)
        for (hh = 0; hh < nhru; ++ hh) {
            if (HRU_struct [hh] < 1)
            {
                const_cast < long * > (HRU_struct) [hh] = 1;
            }

            if (HRU_struct [hh] > (long) GrpStringList -> size ())
            {
                const_cast < long * > (HRU_struct) [hh] = GrpStringList -> size ();
            }

        }

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::init(void)@@@ClassMacro.cpp>");
        return;
    }

    list < DefCRHMPtr > :: iterator iter;
    iter = Calls.begin ();
    while (iter != Calls.end ()) {
        DefCRHM Op = (* (* iter));
        (* iter) -> CallInit (nhru, nlay);
        ++ iter;
    }
    FP = vars.find ("hh");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("hh", VarCHRM ()));
        FP = vars.find ("hh");
    }

    FP -> second.kind = TV :: CRHMint;
    FP -> second.ivalue = const_cast < long * > (& hh);
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "hh";
    FP -> second.Me = this;
    FP = vars.find ("NHRU");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("NHRU", VarCHRM ()));
        FP = vars.find ("NHRU");
    }

    FP -> second.kind = TV :: LOCAL;
    FP -> second.val = nhru;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "NHRU";
    FP -> second.Me = this;
    FP = vars.find ("GROUP");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("GROUP", VarCHRM ()));
        FP = vars.find ("GROUP");
    }

    FP -> second.kind = TV :: LOCAL;
    FP -> second.val = GroupCnt;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "GROUP";
    FP -> second.Me = this;
    FP = vars.find ("STRUCT");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("STRUCT", VarCHRM ()));
        FP = vars.find ("STRUCT");
    }

    FP -> second.kind = TV :: LOCAL;
    FP -> second.val = StructCnt;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "STRUCT";
    FP -> second.Me = this;
    FP = vars.find ("PI");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("PI", VarCHRM ()));
        FP = vars.find ("PI");
    }

    FP -> second.kind = TV :: LOCAL;
    FP -> second.val = M_PI;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "PI";
    FP = vars.find ("NO_DISPLAY");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("NO_DISPLAY", VarCHRM ()));
        FP = vars.find ("NO_DISPLAY");
    }

    FP -> second.kind = TV :: LOCAL;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "NO_DISPLAY";
    FP = vars.find ("FREQ");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("FREQ", VarCHRM ()));
        FP = vars.find ("FREQ");
    }

    FP -> second.kind = TV :: LOCAL;
    FP -> second.val = Global :: Freq;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "FREQ";
    FP = vars.find ("RAND");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("RAND", VarCHRM ()));
        FP = vars.find ("RAND");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "RAND";
    FP -> second.pget = Fgetrand;
    FP = vars.find ("STEP");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("STEP", VarCHRM ()));
        FP = vars.find ("STEP");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "STEP";
    FP -> second.pget = Fgetstep;
    FP = vars.find ("LASTINT");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("LASTINT", VarCHRM ()));
        FP = vars.find ("LASTINT");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "LASTINT";
    FP -> second.pget = Fgetlast;
    FP = vars.find ("FIRSTINT");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("FIRSTINT", VarCHRM ()));
        FP = vars.find ("FIRSTINT");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "FIRSTINT";
    FP -> second.pget = Fgetfirst;
    FP = vars.find ("JULIAN");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("JULIAN", VarCHRM ()));
        FP = vars.find ("JULIAN");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "JULIAN";
    FP -> second.pget = Fjulian;
    FP = vars.find ("YEAR");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("YEAR", VarCHRM ()));
        FP = vars.find ("YEAR");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "YEAR";
    FP -> second.pget = Fyear;
    FP = vars.find ("MONTH");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("MONTH", VarCHRM ()));
        FP = vars.find ("MONTH");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "MONTH";
    FP -> second.pget = Fmonth;
    FP = vars.find ("DAY");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("DAY", VarCHRM ()));
        FP = vars.find ("DAY");
    }

    FP -> second.kind = TV :: FUNget;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "DAY";
    FP -> second.pget = Fday;
    FP = vars.find ("ReadAheadObs");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("ReadAheadObs", VarCHRM ()));
        FP = vars.find ("ReadAheadObs");
    }

    FP -> second.kind = TV :: FUNobs;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "ReadAheadObs";
    FP -> second.Me = this;
    FP = vars.find ("WriteAheadObs");
    if (FP == vars.end ())
    {
        vars.insert (make_pair ("WriteAheadObs", VarCHRM ()));
        FP = vars.find ("WriteAheadObs");
    }

    FP -> second.kind = TV :: FUNobs;
    FP -> second.val = 0;
    FP -> second.Ihh = 0;
    FP -> second.IndexMax = 0;
    FP -> second.name = "WriteAheadObs";
    FP -> second.Me = this;
    calculator calc (eval, vars, Operations, PCiter, LastVar, LastVas);
    Operations.clear ();
    string str (Global :: MacroModulesList -> at (Begin));
    for (int jj = Begin + 1; jj < End; ++ jj) {
        string :: size_type P = Global :: MacroModulesList -> at (jj).find ("//");
        if (P != string :: npos)
        {
            if (P - 1 > 0)
            {
                str += " \n" + Global :: MacroModulesList -> at (jj).substr (1, P - 1);
            }

        }

        else
        {
            str += " \n" + Global :: MacroModulesList -> at (jj);
        }

    }
    execbase :: Index = 0;
    string :: iterator first = str.begin ();
    parse_info < string :: iterator > info;
    do {
        info = parse (first, str.end (), calc, space_p);
        if (! info.hit || info.length == 0)
        {
            string err (first, str.end ());
            CRHMException TExcept (string ("cannot parse the command string: ") + err.c_str (), TExcept :: TERMINATE);
            LogError (TExcept);
        }

        else
        if (! info.full)
        {
            first = info.stop;
        }

    }
    while (! (info.full && info.hit));
    PCiter = Operations.begin ();
    int n = 1;
    while (PCiter != Operations.end ()) {
        (* PCiter) -> OurOrder = n ++;
        ++ PCiter;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassMacro::init(void)@@@ClassMacro.cpp>");
}
void ClassMacro :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassMacro::run(void)@@@ClassMacro.cpp>");
    if (isGroup || isStruct)
    {
        list < ModulePtr > :: iterator iter;
        iter = Modules.begin ();
        while (iter != Modules.end ()) {
            ModulePtr Op = (* iter);
            (* iter) -> pre_run ();
            (* iter) -> run ();
            ++ iter;
        }

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::run(void)@@@ClassMacro.cpp>");
        return;
    }

    for (hh = 1; chkStructOff (hh, nhru + 1); ++ hh) {
        PCiter = Operations.begin ();
        while (PCiter != Operations.end ()) {
            execbase Op = (* (* PCiter));
            double X = (* (* PCiter)) ();
            ++ PCiter;
            int Cnt = eval.size ();
        }
    }
    int Cnt = eval.size ();
    assert (eval.empty ());
    assert (LastVar.empty ());
    assert (LastVas.empty ());
InstrumentLogger::instance()->log_instrument_log("</ClassMacro::run(void)@@@ClassMacro.cpp>");
}
void ClassMacro :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassMacro::finish(bool good)@@@ClassMacro.cpp>");
    if (isGroup || isStruct)
    {
        list < ModulePtr > :: iterator iter;
        iter = Modules.begin ();
        while (iter != Modules.end ()) {
            ModulePtr Op = (* iter);
            Op -> finish (false);
            ++ iter;
        }

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::finish(bool good)@@@ClassMacro.cpp>");
        return;
    }

    Operations.clear ();
    symtab_t :: iterator pos;
    for (pos = vars.begin (); pos != vars.end ();) {
        if (pos -> second.kind == TV :: LOCAL)
        {
            vars.erase (pos ++);
        }

        else
        {
            ++ pos;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassMacro::finish(bool good)@@@ClassMacro.cpp>");
}
ClassMacro :: ClassMacro (string Name, int ThisBegin, string Version, string Desc): ClassModule (Name, Version, LMODULE :: MACRO)
{
InstrumentLogger::instance()->log_instrument_log("<ClassMacro::ClassMacro(string Name, int ThisBegin, string Version, string Desc): ClassModule (Name, Version, LMODULE :: MACRO)@@@ClassMacro.cpp>");
    size_t ThisEnd = ThisBegin;
    MacroBegin = ThisBegin;
    Description = Desc;
    string S, SS;
    ID = Name;
    Grpnhru = 0;
    S = Global :: MacroModulesList -> at (ThisEnd).c_str ();
    S = Common :: trim (S);
    SS = S.substr (0, 3);
    while (! (SS == "end" && (S.length () == 3 || S.find_first_of (" /") != string :: npos)) && Global :: MacroModulesList -> size () > ThisEnd) {
        ThisEnd ++;
        S = Global :: MacroModulesList -> at (ThisEnd).c_str ();
        S = Common :: trim (S);
        SS = S.substr (0, 3);
    }
    if (! DefCRHM :: DefStringList)
    {
        DefCRHM :: DefStringList = new std :: vector < std :: string > ();
    }

    isGroup = false;
    isStruct = false;
    for (size_t ii = ThisBegin + 1; ii < ThisEnd - 1; ++ ii) {
        string :: size_type Indx;
        Indx = Global :: MacroModulesList -> at (ii).find ("//");
        if (Indx != string :: npos)
        {
            string temp = Global :: MacroModulesList -> at (ii).substr (0, Indx - 1);
            temp = Common :: trimright (temp);
            Common :: tokenizeString (Add_Quote (temp), DefCRHM :: DefStringList);
        }

        else
        {
            Common :: tokenizeString (Add_Quote (Global :: MacroModulesList -> at (ii)), DefCRHM :: DefStringList);
        }

        long Pcnt = (long) DefCRHM :: DefStringList -> size ();
        if (Pcnt < 1)
        {
            continue;
        }

        DefCRHM :: DefStringList -> at (0) = Common :: trimleft (DefCRHM :: DefStringList -> at (0));
        if (! DefCRHM :: DefStringList -> at (0).compare ("//") || DefCRHM :: DefStringList -> at (0).empty ())
        {
            continue;
        }

        if (DefCRHM :: DefStringList -> at (0).find ("command") == 0)
        {
            Begin = ii + 1;
            End = ThisEnd;
            DefCRHMPtr Call (new Defcommand (this));
            break;
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declgroup"))
        {
            if (! isGroup)
            {
                isGroup = true;
                GroupCnt = ++ Global :: GroupCntTrk;
                GrpStringList = new std :: vector < std :: pair < std :: string, ClassModule * >> ();
                if (DefCRHM :: DefStringList -> size () > 1 && (DefCRHM :: DefStringList -> at (1).find ("//") == string :: npos))
                {
                    Grpnhru = Strtolong (DefCRHM :: DefStringList -> at (1));
                }

                else
                {
                    Grpnhru = nhru;
                }

            }

        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declstruct"))
        {
            if (! isStruct)
            {
                isStruct = true;
                StructCnt = ++ Global :: StructCntTrk;
                GrpStringList = new std :: vector < std :: pair < std :: string, ClassModule * >> ();
                if (DefCRHM :: DefStringList -> size () > 1 && ! DefCRHM :: DefStringList -> at (1).find ("//"))
                {
                    Grpnhru = Strtolong (DefCRHM :: DefStringList -> at (1));
                }

                else
                {
                    Grpnhru = nhru;
                }

            }

        }

        else
        if (isGroup || isStruct)
        {
            string S = DefCRHM :: DefStringList -> at (0);
            string V;
            string :: size_type N = S.find ("#");
            if (N != string :: npos)
            {
                V = S.substr (N, 2);
                S = S.substr (1, N - 1);
            }

            long jj = - 1;
            if (Global :: OldModuleName != NULL)
            {
                for (size_t i = 0; i < Global :: OldModuleName -> size (); i ++) {
                    if (Global :: OldModuleName -> at (i) == S)
                    {
                        jj = i;
                    }

                }
            }

            if (jj == - 1)
            {
                GrpStringList -> push_back (std :: pair < std :: string, ClassModule * > (DefCRHM :: DefStringList -> at (0), NULL));
            }

            else
            {
                GrpStringList -> push_back (std :: pair < std :: string, ClassModule * > ((Global :: NewModuleName -> at (jj) + V), NULL));
                string Message = "Converting module " + Global :: OldModuleName -> at (jj) + V + " to " + Global :: NewModuleName -> at (jj) + V + " in macro " + Name.c_str ();
                LogMessage (Message.c_str ());
            }

        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declparam") && Pcnt >= 8)
        {
            DefCRHMPtr Call (new Defdeclparam (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("decldiagparam") && Pcnt >= 8)
        {
            DefCRHMPtr Call (new Defdeclparam (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declreadobs") && Pcnt >= 5)
        {
            DefCRHMPtr Call (new Defdeclreadobs (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declobsfunc") && Pcnt >= 4)
        {
            DefCRHMPtr Call (new Defdeclobsfunc (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declvar") && Pcnt >= 5)
        {
            DefCRHMPtr Call (new Defdeclvar (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("decldiag") && Pcnt >= 5)
        {
            DefCRHMPtr Call (new Defdecldiag (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declstatvar") && Pcnt >= 5)
        {
            DefCRHMPtr Call (new Defdeclstatvar (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("decllocal") && Pcnt >= 5)
        {
            DefCRHMPtr Call (new Defdecllocal (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declgetvar") && Pcnt >= 4)
        {
            DefCRHMPtr Call (new Defdeclgetvar (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declputvar") && Pcnt >= 4)
        {
            DefCRHMPtr Call (new Defdeclputvar (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declputparam") && Pcnt >= 4)
        {
            DefCRHMPtr Call (new Defdeclputparam (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("declobs") && Pcnt >= 5)
        {
            DefCRHMPtr Call (new Defdeclobs (this));
            Calls.push_back (Call);
        }

        else
        if (! DefCRHM :: DefStringList -> at (0).compare ("setpeer"))
        {
            if (DefCRHM :: DefStringList -> size () > 2)
            {
                PeerVar = DefCRHM :: DefStringList -> at (1);
                PeerRank = Strtolong (DefCRHM :: DefStringList -> at (2));
            }

        }

        else
        {
            Common :: Message (string (string ("function not recognised or too few parameters: ") + "\"" + Global :: MacroModulesList -> at (ii) + "\""), "Macro Error");
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassMacro::ClassMacro(string Name, int ThisBegin, string Version, string Desc): ClassModule (Name, Version, LMODULE :: MACRO)@@@ClassMacro.cpp>");
}
ClassMacro :: ~ClassMacro ()
{
    Calls.clear ();
}
;

long ClassMacro :: declobs (string module, string name, TDim dimen, string help, string units, double ** value)
{
InstrumentLogger::instance()->log_instrument_log("<ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    ClassVar * thisVar;
    int cnt = getdim (dimen);
    Convert convert;
    convert.CheckUnitsString (Name, name, units);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (name, variation_set);
            PairstrI Item = PairstrI (module, Item2);
            Global :: Mapdeclvar.insert (Item);
            Global :: Mapdeclobs.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
            return (- 1);
        }
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + name)) != Global :: MapVars.end ())
            {
                thisVar = (* itVar).second;
                if (thisVar -> dim < cnt)
                {
                    thisVar -> ReleaseM ();
                    thisVar -> dim = cnt;
                    if (thisVar -> lay > 0)
                    {
                        thisVar -> layvalues = new double * [thisVar -> lay];
                        for (int ii = 0; ii < thisVar -> lay; ii ++)
                            thisVar -> layvalues [ii] = new double [cnt];
                    }

                    thisVar -> values = new double [cnt];
                }

                thisVar -> dimen = dimen;
                thisVar -> module = module;
                thisVar -> DLLName = DLLName;
                thisVar -> root = ID;

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
                return (- 1);
            }

            newVar = new ClassVar ("obs", name, cnt, 0, NULL);
            newVar -> varType = TVar :: Float;
            newVar -> help = help;
            newVar -> units = units;
            newVar -> module = module;
            newVar -> DLLName = DLLName;
            newVar -> root = ID;
            newVar -> values = new double [cnt];
            PairVar Item = PairVar ("obs " + name, newVar);
            Global :: MapVars.insert (Item);
            Item = PairVar (Name + " " + name, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
            return (- 1);
        }
    case TBuild :: INIT:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + name)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if (newVar -> FileData)
                {
                    * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
                    return (- (newVar -> cnt - 1));
                }

                else
                {
                    * value = newVar -> values;
                    newVar -> cnt = cnt;
                    LogError (CRHMException (" macro module '" + module + "' creating declared observation: '" + name + "'", TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
                    return (newVar -> cnt - 1);
                }

            }

            else
            {
                CRHMException Except ("observation not found: " + module + " " + name, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
    return 0;
InstrumentLogger::instance()->log_instrument_log("</ClassMacro::declobs(string module, string name, TDim dimen, string help, string units, double ** value)@@@ClassMacro.cpp>");
}
string Add_Quote (string s)
{
InstrumentLogger::instance()->log_instrument_log("<Add_Quote(string s)@@@ClassMacro.cpp>");
    string :: size_type pos, here;
    for (pos = 0; pos < s.size (); ++ pos) {
        here = s.find_first_of ('\"', pos);
        if (here == string :: npos)
        {
            break;
        }

        else
        {
            if (s [++ here] != '\'')
            {
                s.insert (here, "'");
            }

            here = s.find_first_of ('\"', here);
            if (s [here - 1] != '\'')
            {
                s.insert (here, "'");
            }

            pos = here + 1;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</Add_Quote(string s)@@@ClassMacro.cpp>");
    return s;
InstrumentLogger::instance()->log_instrument_log("</Add_Quote(string s)@@@ClassMacro.cpp>");
}