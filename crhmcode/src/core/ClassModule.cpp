#include <stdio.h>
#include "assert.h"
#include "ClassModule.h"
#include "Common.h"
#include "InstrumentLogger.h"

using namespace std; std :: vector < std :: pair < std :: string, ClassVar * >> * ReadListN = new std :: vector < std :: pair < std :: string, ClassVar * >> ();
std :: vector < std :: pair < std :: string, ClassVar * >> * FunctListN = new std :: vector < std :: pair < std :: string, ClassVar * >> ();
string Fstrings [] = {"Observation", "VP_saturated", "W_to_MJ/Int", "MJ/Int_to_W", "Average", "Minimum", "Maximum", "Daily Sum", "Positive", "Total", "First", "Last", "Peak", "Count", "Count0", "Intvl"};
string AKAstrings [] = {"VARG", "OBSR", "VARD", "OBSD", "PARD", "OBSF"};

double ClassModule :: Now (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::Now(void)@@@ClassModule.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassModule::Now(void)@@@ClassModule.cpp>");
    return Global :: DTnow;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::Now(void)@@@ClassModule.cpp>");
}
string ClassModule :: Var_name (ClassModule * thisModule, string S)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::Var_name(ClassModule * thisModule, string S)@@@ClassModule.cpp>");
    if (thisModule -> variation != 0)
    {
        string AA ("#0");
        AA [2] += (char) (log (thisModule -> variation) / log (2) + 1);
        S += AA;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::Var_name(ClassModule * thisModule, string S)@@@ClassModule.cpp>");
    return S;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::Var_name(ClassModule * thisModule, string S)@@@ClassModule.cpp>");
}
void ClassModule :: initbase (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::initbase(void)@@@ClassModule.cpp>");
    Global :: BuildFlag = TBuild :: INIT;
    MapPar :: iterator itPar;
    if (Var_NDEFN_cnt)
    {
        Var_loop_lay_table = new double ** [Var_NDEFN_cnt];
        for (long jj = 0; jj < Var_NDEFN_cnt; ++ jj) {
            Var_loop_lay_table [jj] = new double * [nlay];
        }
        Var_loop_lay_value = new double * [Var_NDEFN_cnt];
        for (long jj = 0; jj < Var_NDEFN_cnt; ++ jj)
            Var_loop_lay_value [jj] = new double [nhru];
    }

    else
    {
        Var_loop_lay_table = NULL;
        Var_loop_lay_value = NULL;
    }

    if (Par_NDEFN_cnt)
    {
        Par_loop_lay_table = new double ** [Par_NDEFN_cnt];
        for (long jj = 0; jj < Par_NDEFN_cnt; ++ jj) {
            Par_loop_lay_table [jj] = new double * [nlay];
            for (long ll = 0; ll < nlay; ++ ll)
                Par_loop_lay_table [ll] = new double * [nhru];
        }
        Par_loop_lay_value = new double * [Par_NDEFN_cnt];
        for (long jj = 0; jj < Par_NDEFN_cnt; ++ jj) {
            Par_loop_lay_value [jj] = new double [nlay];
            for (long ll = 0; ll < nlay; ++ ll)
                Par_loop_lay_value [ll] = new double [nhru];
        }
    }

    else
    {
        Par_loop_lay_table = NULL;
        Par_loop_lay_value = NULL;
    }

    Var_NDEFN_cnt = 0;
    Par_NDEFN_cnt = 0;
    decl ();
    init ();
InstrumentLogger::instance()->log_instrument_log("</ClassModule::initbase(void)@@@ClassModule.cpp>");
}
int ClassModule :: declgrpvar (string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
        return (- 1);
    }

    Convert convert;
    convert.CheckUnitsString (Name, variable, units);
    AKAhook (TAKA :: VARD, Name, variable, variable);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (variable.c_str (), variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclvar.insert (Item);
            PeerVar = queryvar.c_str ();

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
                return 0;
            }

            if (layvalue == NULL)
            {
                LogError (CRHMException ("Layer Array not defined for " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
                return - 1;
            }

            if (! Global :: GroupCntTrk)
            {
                LogError (CRHMException ("No groups defined " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
                return - 1;
            }

            newVar = new ClassVar (Name, variable, TDim :: NREB, help, units, TVar :: Float, PointPlot, nhru);
            newVar -> varType = TVar :: Float;
            newVar -> variation_set = variation_set;
            newVar -> InGroup = GroupCnt;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            PairVar Item = PairVar (Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: INIT:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                * value = newVar -> values;
                for (int ii = 0; ii < nhru; ++ ii)
                    newVar -> layvalues [ii] = NULL;
                if (! Global :: GroupCntTrk)
                {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
                    return 0;
                }

                long querycnt = 0;
                for (itVar = Global :: MapVars.begin (); itVar != Global :: MapVars.end (); itVar ++) {
                    ClassVar * foundVar = (* itVar).second;
                    if (newVar != NULL)
                    {
                        if (foundVar -> FileData)
                        {
                            continue;
                        }

                        string S = foundVar -> name;
                        string :: size_type indx = S.find ('@');
                        if (indx != string :: npos)
                        {
                            string N = S.substr (indx + 1);
                            S = S.erase (indx);
                            if (S == queryvar)
                            {
                                newVar -> layvalues [querycnt] = foundVar -> values;
                                newVar -> values [querycnt] = foundVar -> dim;
                                ++ querycnt;
                            }

                        }

                    }

                }
                if (newVar != NULL)
                {
                    * layvalue = newVar -> layvalues;
                }

                else
                {
                    CRHMException Except ("variable was null found: " + Name + ' ' + variable, TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
                return querycnt;
            }

            else
            {
                CRHMException Except ("variable not found: " + Name + ' ' + variable, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
    return - 1;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgrpvar(string variable, string queryvar, string help, string units, double ** value, double ** * layvalue, bool PointPlot)@@@ClassModule.cpp>");
}
void ClassModule :: decldiag (string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decldiag(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, TVISIBLE Local)@@@ClassModule.cpp>");
    declvar (variable, dimen, help, units, value, layvalue, dim, PointPlot, false, Local);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decldiag(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: decldiag (string variable, TDim dimen, string help, string units, long ** value, long ** * ilayvalue, const int dim, bool PointPlot, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decldiag(string variable, TDim dimen, string help, string units, long ** value, long ** * ilayvalue, const int dim, bool PointPlot, TVISIBLE Local)@@@ClassModule.cpp>");
    declvar (variable, dimen, help, units, value, ilayvalue, dim, PointPlot, false, Local);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decldiag(string variable, TDim dimen, string help, string units, long ** value, long ** * ilayvalue, const int dim, bool PointPlot, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: declstatdiag (string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, TVISIBLE Local)
{
    declvar (variable, dimen, help, units, value, layvalue, dim, PointPlot, true, Local);
    if (Global :: BuildFlag == TBuild :: BUILD)
    {
        string s = string (Name.c_str ()) + " " + variable.c_str ();
        VandP VP;
        VP.PutV (variation_set);
        VP.PutP ((int) Local);
        PairstrV Item2 = PairstrV (s, VP.both);
        PairstrI Item = PairstrI (Name.c_str (), Item2);
        Global :: Mapdeclstat.insert (Item);
    }

}
;

void ClassModule :: declstatdiag (string variable, TDim dimen, string help, string units, long ** value, long ** * ilayvalue, const int dim, bool PointPlot, TVISIBLE Local)
{
    declvar (variable, dimen, help, units, value, ilayvalue, dim, PointPlot, true, Local);
    if (Global :: BuildFlag == TBuild :: BUILD)
    {
        string s = string (Name.c_str ()) + " " + variable.c_str ();
        VandP VP;
        VP.PutV (variation_set);
        VP.PutP ((int) Local);
        PairstrV Item2 = PairstrV (s, VP.both);
        PairstrI Item = PairstrI (Name.c_str (), Item2);
        Global :: Mapdeclstat.insert (Item);
    }

}
;

void ClassModule :: declstatvar (string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, TVISIBLE Local)
{
    declvar (variable, dimen, help, units, value, layvalue, dim, PointPlot, true, Local);
    if (Global :: BuildFlag == TBuild :: BUILD)
    {
        string s = string (Name.c_str ()) + " " + variable.c_str ();
        VandP VP;
        VP.PutV (variation_set);
        VP.PutP ((int) Local);
        PairstrV Item2 = PairstrV (s, VP.both);
        PairstrI Item = PairstrI (Name.c_str (), Item2);
        Global :: Mapdeclstat.insert (Item);
    }

}
;

void ClassModule :: declstatvar (string variable, TDim dimen, string help, string units, long ** value, long ** * ilayvalue, const int dim, bool PointPlot, TVISIBLE Local)
{
    declvar (variable, dimen, help, units, value, ilayvalue, dim, PointPlot, true, Local);
    if (Global :: BuildFlag == TBuild :: BUILD)
    {
        string s = string (Name.c_str ()) + " " + variable.c_str ();
        VandP VP;
        VP.PutV (variation_set);
        VP.PutP ((int) Local);
        PairstrV Item2 = PairstrV (s, VP.both);
        PairstrI Item = PairstrI (Name.c_str (), Item2);
        Global :: Mapdeclstat.insert (Item);
    }

}
;

void ClassModule :: declvar (string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    string Orgvariable = variable;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
        return;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, variable, units);
    AKAhook (TAKA :: VARD, Name, variable, variable);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) Local);
            PairstrV Item2 = PairstrV (variable.c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclvar.insert (Item);
            Global :: DeclRootList -> push_back (string (ID.c_str ()) + " " + Orgvariable.c_str ());

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            if (dimen == TDim :: NLAY && layvalue == NULL)
            {
                LogError (CRHMException ("Layer Array not defined for " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            if (dimen == TDim :: NDEFN)
            {
                ++ Var_NDEFN_cnt;
            }

            newVar = new ClassVar (Name, variable, dimen, help, units, TVar :: Float, PointPlot, nhru, dim);
            newVar -> varType = TVar :: Float;
            newVar -> StatVar = StatVar;
            newVar -> InGroup = GroupCnt;
            newVar -> visibility = Local;
            newVar -> variation_set = variation_set;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            PairVar Item = PairVar (Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: INIT:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if (newVar -> layvalues != NULL)
                {
                    if (dimen == TDim :: NFREQ && newVar -> lay != Global :: Freq)
                    {
                        newVar -> nfreq = true;
                        newVar -> lay = Global :: Freq;
                        newVar -> ReleaseM (true);
                        newVar -> layvalues = new double * [newVar -> lay];
                        for (int ii = 0; ii < newVar -> lay; ii ++)
                            newVar -> layvalues [ii] = new double [newVar -> dim];
                    }

                    if ((dimen == TDim :: NLAY || dimen == TDim :: NDEFN) && newVar -> lay != dim)
                    {
                        long JJ = newVar -> lay;
                    }

                }

                if (layvalue)
                {
                    * layvalue = newVar -> layvalues;
                }

                * value = newVar -> values;
                if (dimen == TDim :: NDEF)
                {
                    Var_loop_lay_table [Var_NDEFN_cnt] = newVar -> layvalues;
                    Var_loop_lay_value [Var_NDEFN_cnt ++] = newVar -> values;
                }

                Global :: DeclRootList -> push_back (string (ID.c_str ()) + " " + Orgvariable.c_str ());

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            else
            {
                CRHMException Except ("variable not found: " + Name + ' ' + variable, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: declvar (string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    string Orgvariable = variable;
    if (Variation_Skip ())
    {
        * ivalue = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
        return;
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: VARD, Name, variable, variable);
    Convert convert;
    convert.CheckUnitsString (Name, variable, units);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) Local);
            PairstrV Item2 = PairstrV (variable.c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclvar.insert (Item);
            Global :: DeclRootList -> push_back (string (ID.c_str ()) + " " + Orgvariable.c_str ());

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            if (dimen == TDim :: NLAY && ilayvalue == NULL)
            {
                LogError (CRHMException ("Layer Array not defined for " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            newVar = new ClassVar (Name, variable, dimen, help, units, TVar :: Int, PointPlot, nhru, dim);
            newVar -> varType = TVar :: Int;
            newVar -> StatVar = StatVar;
            newVar -> InGroup = GroupCnt;
            newVar -> visibility = Local;
            newVar -> variation_set = variation_set;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            PairVar Item = PairVar (Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: INIT:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if ((dimen == TDim :: NLAY || dimen == TDim :: NFREQ) && ilayvalue != NULL)
                {
                    if (newVar -> nfreq && newVar -> lay != Global :: Freq)
                    {
                        newVar -> ReleaseM (true);
                        newVar -> lay = Global :: Freq;
                        newVar -> nfreq = true;
                        newVar -> ilayvalues = new long * [newVar -> lay];
                        for (int ii = 0; ii < newVar -> lay; ii ++)
                            newVar -> ilayvalues [ii] = new long [newVar -> dim];
                    }

                }

                if (ilayvalue)
                {
                    * ilayvalue = newVar -> ilayvalues;
                }

                * ivalue = newVar -> ivalues;
                Global :: DeclRootList -> push_back (string (ID.c_str ()) + " " + Orgvariable.c_str ());

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            else
            {
                CRHMException Except ("Variable not found: " + Name + ' ' + variable, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declvar(string variable, TDim dimen, string help, string units, long ** ivalue, long ** * ilayvalue, const int dim, bool PointPlot, bool StatVar, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: decllocal (string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decllocal(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)@@@ClassModule.cpp>");
        return;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, variable, units);
    AKAhook (TAKA :: VARD, Name, variable, variable);
    switch (Global :: BuildFlag) {
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find ("#" + Name + " " + variable)) != Global :: MapVars.end ())
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)@@@ClassModule.cpp>");
                return;
            }

            if ((dimen == TDim :: NLAY || dimen == TDim :: NFREQ) && layvalue == NULL)
            {
                LogError (CRHMException ("Layer Array not defined for " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)@@@ClassModule.cpp>");
                return;
            }

            newVar = new ClassVar (Name, variable, dimen, help, units, TVar :: Float, false, nhru, dim);
            newVar -> varType = TVar :: Float;
            newVar -> InGroup = GroupCnt;
            newVar -> visibility = TVISIBLE :: PRIVATE;
            newVar -> variation_set = variation_set;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            PairVar Item = PairVar ("#" + Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: INIT:
        {
            string S = "#" + Name + " " + variable;
            if ((itVar = Global :: MapVars.find (S)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if ((dimen == TDim :: NLAY || dimen == TDim :: NFREQ) && layvalue != NULL)
                {
                    if (newVar -> nfreq && newVar -> lay != Global :: Freq)
                    {
                        newVar -> ReleaseM (true);
                        newVar -> lay = Global :: Freq;
                        newVar -> nfreq = true;
                        newVar -> layvalues = new double * [newVar -> lay];
                        for (int ii = 0; ii < newVar -> lay; ii ++)
                            newVar -> layvalues [ii] = new double [newVar -> dim];
                    }

                    * layvalue = newVar -> layvalues;
                }

                * value = newVar -> values;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)@@@ClassModule.cpp>");
                return;
            }

            else
            {
                CRHMException Except ("variable not found: " + S, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, double ** value, double ** * layvalue, const int dim)@@@ClassModule.cpp>");
}
void ClassModule :: decllocal (string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decllocal(string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)@@@ClassModule.cpp>");
        return;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, variable, units);
    AKAhook (TAKA :: VARD, Name, variable, variable);
    switch (Global :: BuildFlag) {
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find ("#" + Name + " " + variable)) != Global :: MapVars.end ())
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)@@@ClassModule.cpp>");
                return;
            }

            if (dimen == TDim :: NLAY && layvalue == NULL)
            {
                LogError (CRHMException ("Layer Array not defined for " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)@@@ClassModule.cpp>");
                return;
            }

            newVar = new ClassVar (Name, variable, dimen, help, units, TVar :: Int, false, nhru, dim);
            newVar -> varType = TVar :: Int;
            newVar -> InGroup = GroupCnt;
            newVar -> visibility = TVISIBLE :: PRIVATE;
            newVar -> variation_set = variation_set;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            PairVar Item = PairVar ("#" + Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: INIT:
        {
            string S = "#" + Name + " " + variable;
            if ((itVar = Global :: MapVars.find (S)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if ((dimen == TDim :: NLAY || dimen == TDim :: NFREQ) && layvalue != NULL)
                {
                    if (newVar -> nfreq && newVar -> lay != Global :: Freq)
                    {
                        newVar -> ReleaseM (true);
                        newVar -> lay = Global :: Freq;
                        newVar -> nfreq = true;
                        newVar -> ilayvalues = new long * [newVar -> lay];
                        for (int ii = 0; ii < newVar -> lay; ii ++)
                            newVar -> ilayvalues [ii] = new long [newVar -> dim];
                    }

                    * layvalue = newVar -> ilayvalues;
                }

                * value = newVar -> ivalues;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)@@@ClassModule.cpp>");
                return;
            }

            else
            {
                CRHMException Except ("variable not found: " + S, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocal(string variable, TDim dimen, string help, string units, long ** value, long ** * layvalue, const int dim)@@@ClassModule.cpp>");
}
std :: vector < std :: string > * ClassModule :: decldiagparam (string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decldiagparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
    std :: vector < std :: string > * fix = declparam (param, dimen, Texts, help, stringsList, Local);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decldiagparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
    return fix;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decldiagparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
}
std :: vector < std :: string > * ClassModule :: decllocalparam (string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decllocalparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
    std :: vector < std :: string > * fix = declparam (param, dimen, Texts, help, stringsList, Local);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocalparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
    return fix;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocalparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
}
std :: vector < std :: string > * ClassModule :: declparam (string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar;
    if (Variation_Skip ())
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
        return NULL;
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: PARD, Name, param, param);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) Local);
            PairstrV Item2 = PairstrV (param.c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclpar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
            return NULL;
        }
    case TBuild :: DECL:
        {
            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                if ((* itPar).second -> dim == this -> nhru || dimen == TDim :: BASIN)
                {
                    newPar = (* itPar).second;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
                    return newPar -> Strings;
                }

                else
                {
                    (* itPar).second -> ExpandShrink (this -> nhru);
                    newPar = (* itPar).second;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
                    return newPar -> Strings;
                }

            }

            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
                return newPar -> Strings;
            }

            newPar = new ClassPar (string (Name.c_str ()), string (param), dimen, Texts, help, TVar :: Txt, nhru);
            newPar -> basemodule = this -> NameRoot;
            newPar -> variation_set = variation_set;
            newPar -> visibility = Local;
            PairPar Item = PairPar (Name + " " + param, newPar);
            Global :: MapPars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
            return newPar -> Strings;
        }
    case TBuild :: INIT:
        {
            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                stringsList = newPar -> Strings;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
                return stringsList;
            }

            else
            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                stringsList = newPar -> Strings;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
                return stringsList;
            }

            else
            {
                CRHMException Except ("Parameter not found: " + Name + param, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
    return NULL;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string Texts, string help, std :: vector < std :: string > * stringsList, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: decldiagparam (string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decldiagparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
    declparam (param, dimen, valstr, minstr, maxstr, help, units, value, layvalue, dim, Local);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decldiagparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: decllocalparam (string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decllocalparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
    declparam (param, dimen, valstr, minstr, maxstr, help, units, value, layvalue, dim, Local);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocalparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: declparam (string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
        return;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, param, units);
    AKAhook (TAKA :: PARD, Name, param, param);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) Local);
            PairstrV Item2 = PairstrV (param.c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclpar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: DECL:
        {
            if (dim <= 0)
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            if (dimen == TDim :: NDEFN)
            {
                ++ Par_NDEFN_cnt;
            }

            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                if ((* itPar).second -> dim == this -> nhru || dimen == TDim :: BASIN)
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share == 2)
                    {
                        newPar -> basemodule = this -> NameRoot;
                        newPar -> variation_set = variation_set;
                        newPar -> visibility = Local;
                        newPar -> Inhibit_share = 1;
                        newPar -> valstr = valstr;
                        newPar -> minVal = atof (minstr.c_str ());
                        newPar -> maxVal = atof (maxstr.c_str ());
                        newPar -> help = help;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                    return;
                }

                else
                {
                    (* itPar).second -> ExpandShrink (this -> nhru);
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share == 2)
                    {
                        newPar -> Inhibit_share = 1;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                    return;
                }

            }

            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                if ((* itPar).second -> dim == this -> nhru || dimen == TDim :: BASIN)
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share == 2)
                    {
                        newPar -> Inhibit_share = 1;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                    return;
                }

            }

            if ((dimen == TDim :: NLAY && layvalue == NULL) || (dimen == TDim :: NDEF && layvalue == NULL) || (dimen == TDim :: NDEFN && layvalue == NULL))
            {
                LogError (CRHMException ("Layer Array not defined for " + Name + " " + param, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            double minval = atof (minstr.c_str ());
            double maxval = atof (maxstr.c_str ());
            newPar = new ClassPar (string (Name.c_str ()), string (param), dimen, valstr, minval, maxval, help, units, TVar :: Float, dim, nhru);
            newPar -> basemodule = this -> NameRoot;
            newPar -> variation_set = variation_set;
            newPar -> visibility = Local;
            Myparser ob;
            ob.eval_exp (newPar);
            PairPar Item = PairPar (Name + " " + param, newPar);
            Global :: MapPars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: INIT:
        {
            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                * value = newPar -> values;
            }

            else
            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                * value = newPar -> values;
            }

            else
            if ((newPar = ClassParFindPar (param)))
            {
                * value = newPar -> values;
            }

            else
            {
                CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

            if ((dimen == TDim :: NLAY && layvalue != NULL) || (dimen == TDim :: NDEF && layvalue != NULL) || (dimen == TDim :: NDEFN && layvalue != NULL))
            {
                * layvalue = (const double **) newPar -> layvalues;
            }

            if (dimen == TDim :: NDEF)
            {
                Par_loop_lay_table [Par_NDEFN_cnt] = newPar -> layvalues;
                Par_loop_lay_value [Par_NDEFN_cnt ++] = newPar -> values;
            }

        }
    default:
        break;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const double ** value, const double ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: decldiagparam (string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** value, const long ** * layvalue, const int dim, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decldiagparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** value, const long ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
    declparam (param, dimen, valstr, minstr, maxstr, help, units, value, layvalue, dim, Local);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decldiagparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** value, const long ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: decllocalparam (string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** value, const long ** * layvalue, const int dim, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::decllocalparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** value, const long ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
    declparam (param, dimen, valstr, minstr, maxstr, help, units, value, layvalue, dim, Local);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::decllocalparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** value, const long ** * layvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
}
void ClassModule :: declparam (string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar;
    if (Variation_Skip ())
    {
        * ivalue = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
        return;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, param, units);
    AKAhook (TAKA :: PARD, Name, param, param);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) Local);
            PairstrV Item2 = PairstrV (param.c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclpar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: DECL:
        {
            if (dim <= 0)
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            * ivalue = & Dummy;
            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                if ((* itPar).second -> dim == this -> nhru || dimen == TDim :: BASIN)
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share == 2)
                    {
                        newPar -> basemodule = this -> NameRoot;
                        newPar -> variation_set = variation_set;
                        newPar -> visibility = Local;
                        newPar -> Inhibit_share = 1;
                        newPar -> valstr = valstr;
                        newPar -> minVal = atof (minstr.c_str ());
                        newPar -> maxVal = atof (maxstr.c_str ());
                        newPar -> help = help;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                    return;
                }

                else
                {
                    (* itPar).second -> ExpandShrink (this -> nhru);
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share == 2)
                    {
                        assert (0);
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                    return;
                }

            }

            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                if ((* itPar).second -> dim == this -> nhru || dimen == TDim :: BASIN)
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share == 2)
                    {
                        assert (0);
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                    return;
                }

            }

            if ((dimen == TDim :: NLAY && ilayvalue == NULL) || (dimen == TDim :: NDEF && ilayvalue == NULL) || (dimen == TDim :: NDEFN && ilayvalue == NULL))
            {
                LogError (CRHMException ("Layer Array not defined for " + Name + " " + param, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            double minval = atof (minstr.c_str ());
            double maxval = atof (maxstr.c_str ());
            newPar = new ClassPar (string (Name.c_str ()), string (param), dimen, valstr, minval, maxval, help, units, TVar :: Int, dim, nhru);
            newPar -> basemodule = this -> NameRoot;
            newPar -> variation_set = variation_set;
            newPar -> visibility = Local;
            Myparser ob;
            ob.eval_exp (newPar);
            PairPar Item = PairPar (Name + " " + param, newPar);
            Global :: MapPars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: INIT:
        {
            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                * ivalue = newPar -> ivalues;
                if ((dimen == TDim :: NLAY && ilayvalue != NULL) || (dimen == TDim :: NDEF && ilayvalue != NULL) || (dimen == TDim :: NDEFN && ilayvalue != NULL))
                {
                    * ilayvalue = (const long **) newPar -> ilayvalues;
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            else
            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                * ivalue = newPar -> ivalues;
                if ((dimen == TDim :: NLAY && ilayvalue != NULL) || (dimen == TDim :: NDEF && ilayvalue != NULL) || (dimen == TDim :: NDEFN && ilayvalue != NULL))
                {
                    * ilayvalue = (const long **) newPar -> ilayvalues;
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            else
            if ((newPar = ClassParFindPar (param)))
            {
                * ivalue = newPar -> ivalues;
                if ((dimen == TDim :: NLAY && ilayvalue != NULL) || (dimen == TDim :: NDEF && ilayvalue != NULL) || (dimen == TDim :: NDEFN && ilayvalue != NULL))
                {
                    * ilayvalue = (const long **) newPar -> ilayvalues;
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
                return;
            }

            else
            {
                CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declparam(string param, TDim dimen, string valstr, string minstr, string maxstr, string help, string units, const long ** ivalue, const long ** * ilayvalue, const int dim, TVISIBLE Local)@@@ClassModule.cpp>");
}
long ClassModule :: FindWildVarFloat (string name, ClassVar * & newVar, bool OnceFlag)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::FindWildVarFloat(string name, ClassVar * & newVar, bool OnceFlag)@@@ClassModule.cpp>");
    string spacename = " " + name;
    ;
    string Var;
    MapVar :: iterator itVar;
    for (itVar = Global :: MapVars.begin (); itVar != Global :: MapVars.end (); itVar ++) {
        Var = (* itVar).first;
        if (Var [0] == '#')
        {
            continue;
        }

        string :: size_type indx = Var.rfind (spacename);
        if (indx == string :: npos)
        {
            continue;
        }

        newVar = (* itVar).second;
        if (newVar -> cnt > 0)
        {
            continue;
        }

        if (spacename.substr (1) != newVar -> name)
        {
            continue;
        }

        Global :: thisVar = newVar;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildVarFloat(string name, ClassVar * & newVar, bool OnceFlag)@@@ClassModule.cpp>");
        return newVar -> GetUnit ();
    }
    if (OnceFlag == false)
    {
        OnceFlag = true;
        string :: size_type indx = name.find ('@');
        if (indx != string :: npos)
        {
            spacename = name.substr (0, indx);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildVarFloat(string name, ClassVar * & newVar, bool OnceFlag)@@@ClassModule.cpp>");
            return FindWildVarFloat (spacename, newVar, true);
        }

    }

    Global :: thisVar = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildVarFloat(string name, ClassVar * & newVar, bool OnceFlag)@@@ClassModule.cpp>");
    return - 1;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildVarFloat(string name, ClassVar * & newVar, bool OnceFlag)@@@ClassModule.cpp>");
}
long ClassModule :: FindWildParFloat (string name, ClassPar * & newPar, bool Trunc = false, bool Root = true)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::FindWildParFloat(string name, ClassPar * & newPar, bool Trunc = false, bool Root = true)@@@ClassModule.cpp>");
    string spacename;
    if (! Trunc)
    {
        spacename = " " + name;
    }

    else
    {
        string :: size_type indx2 = name.find ('@');
        if (indx2 == string :: npos)
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildParFloat(string name, ClassPar * & newPar, bool Trunc = false, bool Root = true)@@@ClassModule.cpp>");
            return - 1;
        }

        else
        {
            spacename = " " + name.substr (0, indx2);
        }

    }

    MapPar :: iterator itPar;
    for (itPar = Global :: MapPars.begin (); itPar != Global :: MapPars.end (); itPar ++) {
        string Var = (* itPar).first;
        if (Var [0] == '#')
        {
            continue;
        }

        string :: size_type indx = Var.rfind (spacename);
        newPar = (* itPar).second;
        if (indx == string :: npos)
        {
            continue;
        }

        string :: size_type indx2 = newPar -> param.find ('@');
        if (! Root || indx2 == string :: npos)
        {
            if (spacename.substr (1) != newPar -> param)
            {
                continue;
            }

        }

        else
        if (spacename.substr (1) != newPar -> param.substr (0, indx2))
        {
            continue;
        }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildParFloat(string name, ClassPar * & newPar, bool Trunc = false, bool Root = true)@@@ClassModule.cpp>");
        return newPar -> GetUnit ();
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildParFloat(string name, ClassPar * & newPar, bool Trunc = false, bool Root = true)@@@ClassModule.cpp>");
    return - 1;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindWildParFloat(string name, ClassPar * & newPar, bool Trunc = false, bool Root = true)@@@ClassModule.cpp>");
}
long ClassModule :: declgetvar (string source, string name, string units, const double ** value, const double ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: VARG, Name, name, name, source, ID);
    MapPar :: iterator itPar;
    long GetUnit;
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            string s = string (source.c_str ()) + " " + name.c_str ();
            PairstrV Item2 = PairstrV (s, variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapgetvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: DECL:
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: INIT:
        {
            string :: size_type indx2 = name.find ('@');
            if ((itVar = Global :: MapVars.find (source + " " + name)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                * value = newVar -> values;
                if (layvalue != NULL)
                {
                    * layvalue = (const double **) newVar -> layvalues;
                }

                PairVar Item = PairVar (Name + " " + name, newVar);
                Global :: MapVarsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
                return newVar -> GetUnit ();
            }

            else
            if (indx2 != string :: npos && (itVar = Global :: MapVars.find (source + " " + name.substr (0, indx2))) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                * value = newVar -> values;
                if (layvalue != NULL)
                {
                    * layvalue = (const double **) newVar -> layvalues;
                }

                PairVar Item = PairVar (Name + " " + name, newVar);
                Global :: MapVarsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
                return newVar -> GetUnit ();
            }

            else
            if (source [0] == '*')
            {
                GetUnit = FindWildVarFloat (name, newVar);
                if (GetUnit > - 1)
                {
                    * value = newVar -> values;
                    if (layvalue != NULL)
                    {
                        * layvalue = (const double **) newVar -> layvalues;
                    }

                    PairVar Item = PairVar (Name + " " + name, newVar);
                    Global :: MapVarsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
                    return GetUnit;
                }

            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
}
long ClassModule :: declgetvar (string source, string name, string units, const long ** value, const long ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: VARG, Name, name, name, source, ID);
    MapPar :: iterator itPar;
    long GetUnit;
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            string s = string (source.c_str ()) + " " + name.c_str ();
            PairstrV Item2 = PairstrV (s, variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapgetvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: DECL:
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: INIT:
        {
            if (name == "Not_Used")
            {
                name = "Not_Used_int";
            }

            string :: size_type indx2 = name.find ('@');
            if ((itVar = Global :: MapVars.find (source + " " + name)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                * value = newVar -> ivalues;
                if (layvalue != NULL)
                {
                    * layvalue = (const long **) newVar -> ilayvalues;
                }

                PairVar Item = PairVar (Name + " " + name, newVar);
                Global :: MapVarsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
                return newVar -> GetUnit ();
            }

            else
            if (indx2 != string :: npos && (itVar = Global :: MapVars.find (source + " " + name.substr (0, indx2))) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                * value = newVar -> ivalues;
                if (layvalue != NULL)
                {
                    * layvalue = (const long **) newVar -> ilayvalues;
                }

                PairVar Item = PairVar (Name + " " + name, newVar);
                Global :: MapVarsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
                return newVar -> GetUnit ();
            }

            else
            if (source [0] == '*')
            {
                GetUnit = FindWildVarFloat (name, newVar);
                if (GetUnit > - 1)
                {
                    * value = newVar -> ivalues;
                    if (layvalue != NULL)
                    {
                        * layvalue = (const long **) newVar -> ilayvalues;
                    }

                    PairVar Item = PairVar (Name + " " + name, newVar);
                    Global :: MapVarsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
                    return GetUnit;
                }

            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetvar(string source, string name, string units, const long ** value, const long ** * layvalue)@@@ClassModule.cpp>");
}
long ClassModule :: declobs (string name, TDim dimen, string help, string units, double ** value)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    ClassVar * thisVar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, name, units);
    AKAhook (TAKA :: OBSD, Name, name, name);
    int cnt = getdim (dimen);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) TVISIBLE :: USUAL);
            PairstrV Item2 = PairstrV ((name + "#").c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclvar.insert (Item);
            Global :: Mapdeclobs.insert (Item);
            Global :: DeclRootList -> push_back (string (ID.c_str ()) + " " + (name + "#").c_str ());

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + name + "#")) != Global :: MapVars.end ())
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
                thisVar -> DLLName = DLLName.c_str ();
                thisVar -> root = ID.c_str ();

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
                return (- 1);
            }

            newVar = new ClassVar (Name, name + "#", cnt, 0, NULL);
            newVar -> varType = TVar :: Float;
            newVar -> help = help;
            newVar -> units = units;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            newVar -> values = new double [cnt];
            newVar -> variation_set = variation_set;
            PairVar Item = PairVar (Name + " " + name + "#", newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: INIT:
        {
            if ((itVar = Global :: MapVars.find (Name + " " + name + "#")) != Global :: MapVars.end ())
            {
                Global :: DeclRootList -> push_back (string (ID.c_str ()) + " " + (name + "#").c_str ());
                newVar = (* itVar).second;
                if (newVar -> FileData)
                {
                    * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
                    return (- (newVar -> cnt - 1));
                }

                else
                {
                    * value = newVar -> values;
                    newVar -> cnt = cnt;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
                    return (newVar -> cnt - 1);
                }

            }

            else
            {
                CRHMException Except ("Observation not found: " + Name + " " + name + "#", TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobs(string name, TDim dimen, string help, string units, double ** value)@@@ClassModule.cpp>");
}
long ClassModule :: getdimObs (string variable)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::getdimObs(string variable)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    if ((itVar = Global :: MapVars.find ("obs " + variable)) != Global :: MapVars.end ())
    {
        newVar = (* itVar).second;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdimObs(string variable)@@@ClassModule.cpp>");
        return (newVar -> cnt);
    }

    else
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdimObs(string variable)@@@ClassModule.cpp>");
        return (- 1);
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdimObs(string variable)@@@ClassModule.cpp>");
}
long ClassModule :: declreadobs (string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    PairVar Item;
    string declModule = "obs ";
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: OBSR, Name, variable, variable, declModule, ID);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (variable.c_str (), variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapreadvar.insert (Item);
            Global :: Mapdeclvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: DECL:
        {
            if (dimen == TDim :: NFREQ && layvalue == NULL)
            {
                LogError (CRHMException ("NFREQ Array not defined for " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
                return (- 1);
            }

            if ((itVar = Global :: MapVars.find (declModule + variable)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if (newVar -> varType == TVar :: Read)
                {
                    Convert convert;
                    convert.CheckUnitsObs (newVar -> units, units, variable);
                    newVar -> values = NULL;
                    newVar -> varType = TVar :: ReadF;
                    newVar -> dimen = dimen;
                    newVar -> help = help;
                    newVar -> units = units;
                    Item = PairVar (Name + " " + variable, newVar);
                    Global :: MapVars.insert (Item);
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
                return (0);
            }

            if (optional)
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
                return (- 1);
            }

            newVar = new ClassVar ("obs", variable, dimen, help, units, TVar :: Float);
            newVar -> varType = TVar :: Float;
            Item = PairVar (declModule + variable, newVar);
            Global :: MapVars.insert (Item);
            Item = PairVar (Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: INIT:
        {
            newVar = NULL;
            if ((itVar = Global :: MapVars.find (declModule + variable)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if ((newVar -> varType == TVar :: ReadF || newVar -> varType == TVar :: Read) && newVar -> values == NULL)
                {
                    Convert convert;
                    convert.CheckUnitsObs (newVar -> units, units, variable);
                    newVar -> varType = TVar :: ReadF;
                    newVar -> dimMax = Global :: maxhru;
                    newVar -> values = new double [newVar -> dimMax];
                    newVar -> help = help;
                    newVar -> units = units;
                    Item = PairVar (Name + " " + variable, newVar);
                    Global :: MapVars.insert (Item);
                }

            }

            string :: size_type indx = 0;
            if (itVar == Global :: MapVars.end () || (! newVar -> cnt && newVar -> DLLName.empty ()))
            {
                string variable2;
                indx = variable.find ('#');
                if (indx == string :: npos)
                {
                    variable2 = variable + '#';
                }

                else
                {
                    variable2 = variable.erase (indx);
                }

                if (GroupCnt)
                {
                    if ((itVar = Global :: MapVars.find (Name + ' ' + variable2)) != Global :: MapVars.end ())
                    {
                        newVar = (* itVar).second;
                    }

                }

                else
                {
                    if ((itVar = Global :: MapVars.find (declModule + variable2)) != Global :: MapVars.end ())
                    {
                        newVar = (* itVar).second;
                    }

                }

            }

            if (newVar == NULL)
            {
                if (optional)
                {
                    * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
                    return (- 1);
                }

                CRHMException Except ("Observation not found " + declModule + ": " + variable, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

            if (! newVar -> cnt && newVar -> DLLName.empty () && indx == string :: npos)
            {
                if (optional)
                {
                    * value = NULL;
                    PairVar Item = PairVar (Name + " " + variable, newVar);
                    Global :: MapObsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
                    return (- 1);
                }

                else
                {
                    CRHMException Except ("Observation: '" + variable + "', not in Data file. Requested by '" + Name.c_str () + "'", TExcept :: TERMINATE);
                    LogError (Except);
                    string msgstring = "Observation: '" + variable + "', not in Data file. Requested by '" + Name.c_str () + "'";
                    throw Except;
                }

            }

            if (newVar -> FileData)
            {
                if (dimen == TDim :: NHRU)
                {
                    newVar -> dim = nhru;
                }

                else
                {
                    newVar -> dim = newVar -> cnt;
                }

                addtoreadlist (newVar);
            }

            * value = newVar -> values;
            PairVar Item = PairVar (Name + " " + variable, newVar);
            Global :: MapObsGet.insert (Item);
            if (HRU_index > 0)
            {
                newVar -> HRU_OBS_indexed = HRU_index - 1;
            }

            if (newVar -> cnt > 0)
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
                return (newVar -> cnt - 1);
            }

            else
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
                return (newVar -> dim - 1);
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const double ** value, long HRU_index, bool optional, const double ** * layvalue)@@@ClassModule.cpp>");
}
long ClassModule :: declreadobs (string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    PairVar Item;
    string declModule = "obs ";
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: OBSR, Name, variable, variable, declModule, ID);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (variable.c_str (), variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapreadvar.insert (Item);
            Global :: Mapdeclvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: DECL:
        {
            if (dimen == TDim :: NFREQ && layvalue == NULL)
            {
                LogError (CRHMException ("NFREQ Array not defined for " + Name + " " + variable, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
                return (- 1);
            }

            if ((itVar = Global :: MapVars.find (declModule + variable)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if (newVar -> varType == TVar :: Read || newVar -> varType == TVar :: ReadI)
                {
                    if (dimen == TDim :: NFREQ && (newVar -> lay == 0 || nhru > newVar -> dim))
                    {
                        newVar -> ReleaseM (true);
                        newVar -> dim = nhru;
                        newVar -> dimMax = newVar -> dim;
                        newVar -> lay = Global :: Freq;
                        newVar -> nfreq = true;
                        newVar -> ilayvalues = new long * [newVar -> lay];
                        for (int ii = 0; ii < newVar -> lay; ++ ii)
                            newVar -> ilayvalues [ii] = new long [newVar -> dimMax];
                    }

                    if (newVar -> varType == TVar :: Read || (newVar -> varType == TVar :: ReadF && nhru > newVar -> dimMax))
                    {
                        if (newVar -> ivalues != NULL)
                        {
                            delete [] newVar -> ivalues;
                        }

                        else
                        {
                            Convert convert;
                            convert.CheckUnitsObs (newVar -> units, units, variable);
                        }

                        newVar -> dimMax = max < long > (nhru, newVar -> cnt);
                        newVar -> ivalues = new long [newVar -> dimMax];
                        newVar -> varType = TVar :: ReadI;
                        newVar -> help = help;
                        newVar -> units = units;
                        Item = PairVar (Name + " " + variable, newVar);
                        Global :: MapVars.insert (Item);
                    }

                    else
                    {
                        Item = PairVar (Name + " " + variable, newVar);
                        Global :: MapVars.insert (Item);
                    }

                }

                else
                if (newVar -> dimen != dimen && dimen == TDim :: NFREQ)
                {
                    newVar -> ReleaseM (true);
                    newVar -> lay = Global :: Freq;
                    newVar -> nfreq = true;
                    newVar -> ilayvalues = new long * [newVar -> lay];
                    for (int ii = 0; ii < newVar -> lay; ii ++)
                        newVar -> ilayvalues [ii] = new long [newVar -> dim];
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
                return (- 1);
            }

            newVar = new ClassVar (declModule, variable, dimen, help, units, TVar :: Int);
            newVar -> varType = TVar :: Int;
            Item = PairVar (declModule + variable, newVar);
            Global :: MapVars.insert (Item);
            Item = PairVar (Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: INIT:
        {
            newVar = NULL;
            if ((itVar = Global :: MapVars.find (declModule + variable)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                if (itVar == Global :: MapVars.end () || (! newVar -> cnt && newVar -> DLLName.empty ()))
                {
                    string variable2;
                    string :: size_type indx = variable.find ('#');
                    if (indx == string :: npos)
                    {
                        variable2 = variable + '#';
                    }

                    else
                    {
                        variable2 = variable.erase (indx);
                    }

                    if (GroupCnt)
                    {
                        if ((itVar = Global :: MapVars.find (Name + ' ' + variable2)) != Global :: MapVars.end ())
                        {
                            newVar = (* itVar).second;
                        }

                    }

                    else
                    {
                        if ((itVar = Global :: MapVars.find (declModule + variable2)) != Global :: MapVars.end ())
                        {
                            newVar = (* itVar).second;
                        }

                    }

                }

            }

            if (newVar == NULL)
            {
                CRHMException Except ("Observation not found " + declModule + ": " + variable, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

            if (! newVar -> cnt && newVar -> DLLName.empty ())
            {
                if (optional)
                {
                    * value = NULL;
                    PairVar Item = PairVar (Name + " " + variable, newVar);
                    Global :: MapObsGet.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
                    return (- 1);
                }

                else
                {
                    CRHMException Except ("Observation: '" + variable + "', not in Data file. Requested by '" + Name.c_str () + "'", TExcept :: TERMINATE);
                    LogError (Except);
                    string msgstring = "Observation: '" + variable + "', not in Data file. Requested by '" + Name.c_str () + "'";
                    throw Except;
                }

            }

            newVar -> No_ReadVar = 0;
            if (layvalue != NULL)
            {
                if (newVar -> nfreq && newVar -> lay != Global :: Freq)
                {
                    newVar -> ReleaseM (true);
                    newVar -> lay = Global :: Freq;
                    newVar -> nfreq = true;
                    newVar -> ilayvalues = new long * [newVar -> lay];
                    for (int ii = 0; ii < newVar -> lay; ii ++)
                        newVar -> ilayvalues [ii] = new long [newVar -> dim];
                }

                * layvalue = (const long **) newVar -> ilayvalues;
                newVar -> UserFunct = & ClassVar :: Intvl;
                newVar -> FunKind = TFun :: INTVL;
                newVar -> FunctVar = newVar;
                addtofunctlist (newVar);
            }

            if (newVar -> FileData)
            {
                newVar -> dim = nhru;
                addtoreadlist (newVar);
            }

            * value = newVar -> ivalues;
            PairVar Item = PairVar (Name + " " + variable, newVar);
            Global :: MapObsGet.insert (Item);
            if (HRU_index > 0)
            {
                newVar -> HRU_OBS_indexed = HRU_index - 1;
            }

            if (newVar -> cnt > 0)
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
                return (newVar -> cnt - 1);
            }

            else
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
                return (newVar -> dim - 1);
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declreadobs(string variable, TDim dimen, string help, string units, const long ** value, long HRU_index, bool optional, const long ** * layvalue)@@@ClassModule.cpp>");
}
long ClassModule :: declobsfunc (string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * obsVar, * newVar;
    PairVar Item;
    string declModule = "obs ";
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: OBSF, Name, obs, obs, declModule, ID);
    AKAhook (TAKA :: VARD, Name, variable, variable);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            pair < Mapstr :: iterator, Mapstr :: iterator > range = Global :: Mapreadvar.equal_range (Name.c_str ());
            for (Mapstr :: iterator itMap = range.first; itMap != range.second; ++ itMap) {
                if (itMap -> second.first == obs.c_str ())
                {
                    break;
                }

            }
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) TVISIBLE :: DIAGNOSTIC);
            PairstrV Item2 = PairstrV (obs.c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapreadvar.insert (Item);
            Global :: Mapdeclvar.insert (Item);
            Item2 = PairstrV (variable.c_str (), VP.both);
            Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find (declModule + obs)) == Global :: MapVars.end ())
            {
                LogError (CRHMException ("function obs variable not declared: " + Name + " " + obs, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
                return (- 1);
            }

            obsVar = (* itVar).second;
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
                return (- 1);
            }

            string help = obsVar -> help + " <" + "> ";
            newVar = new ClassVar (Name, variable, nhru, help, obsVar -> units, TVar :: Float);
            newVar -> dimMax = nhru;
            newVar -> varType = TVar :: Float;
            newVar -> visibility = TVISIBLE :: DIAGNOSTIC;
            newVar -> variation_set = variation_set;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            PairVar Item = PairVar (Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
            return (0);
        }
    case TBuild :: INIT:
        {
            itVar = Global :: MapVars.find (Name + " " + variable);
            newVar = (itVar == Global :: MapVars.end ()) ? NULL : (* itVar).second;
            if ((itVar = Global :: MapVars.find (declModule + obs)) != Global :: MapVars.end ())
            {
                obsVar = (* itVar).second;
                if (! newVar)
                {
                    newVar = new ClassVar (Name, variable, nhru, obsVar -> help + " <" + Fstrings [(int) typeFun] + "> ", obsVar -> units, TVar :: Float);
                    newVar -> dimMax = nhru;
                    newVar -> varType = TVar :: Float;
                    newVar -> visibility = TVISIBLE :: DIAGNOSTIC;
                    newVar -> variation_set = variation_set;
                    newVar -> DLLName = DLLName.c_str ();
                    newVar -> root = ID.c_str ();
                    PairVar Item = PairVar (Name + " " + variable, newVar);
                    Global :: MapVars.insert (Item);
                }

                if (obsVar -> FileData == NULL)
                {
                    if (optional)
                    {
                        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
                        return (- 1);
                    }

                    else
                    {
                        CRHMException Except ("Function observation: '" + variable + "', cannot be generated from the declared observation, '" + obs.c_str () + "'. Requested by '" + Name.c_str () + "'", TExcept :: TERMINATE);
                        LogError (Except);
                        throw Except;
                    }

                }

                if (obsVar -> FileData -> Times != NULL)
                {
                    CRHMException Except ("Function observation: '" + variable + "', uses a 'sparse' Data file. Requested by '" + Name.c_str () + "'", TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

                if (typeFun == TFun :: INTVL)
                {
                    newVar -> ReleaseM (false);
                    newVar -> dim = max < long > (nhru, newVar -> dim);
                    newVar -> dimMax = max < long > (newVar -> dim, newVar -> dimMax);
                    newVar -> lay = Global :: Freq;
                    newVar -> layvalues = new double * [newVar -> lay];
                    * layvalue = newVar -> layvalues;
                    for (int ii = 0; ii < newVar -> lay; ++ ii)
                        newVar -> layvalues [ii] = new double [newVar -> dimMax];
                    newVar -> values = newVar -> layvalues [0];
                    newVar -> FileData = obsVar -> FileData;
                }

                else
                if (nhru > newVar -> dim)
                {
                    delete [] newVar -> values;
                    newVar -> dimMax = nhru;
                    newVar -> values = new double [newVar -> dimMax];
                    newVar -> dim = newVar -> dimMax;
                }

                if (GroupCnt)
                {
                    newVar -> CustomFunct = NULL;
                }

                switch (typeFun) {
                case TFun :: FOBS:
                    newVar -> FunctVar = obsVar;
                    newVar -> FunKind = TFun :: FOBS;
                    newVar -> FileData = obsVar -> FileData;
                    if (GroupCnt)
                    {
                        newVar -> No_ReadVar = 1;
                    }

                    break;
                case TFun :: AVG:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Avg;
                    newVar -> FunKind = TFun :: AVG;
                    break;
                case TFun :: MIN:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Min;
                    newVar -> FunKind = TFun :: MIN;
                    break;
                case TFun :: MAX:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Max;
                    newVar -> FunKind = TFun :: MAX;
                    break;
                case TFun :: DTOT:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Dtot;
                    newVar -> FunKind = TFun :: DTOT;
                    break;
                case TFun :: TOT:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Tot;
                    newVar -> FunKind = TFun :: TOT;
                    break;
                case TFun :: FIRST:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: First;
                    newVar -> FunKind = TFun :: FIRST;
                    break;
                case TFun :: LAST:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Last;
                    newVar -> FunKind = TFun :: LAST;
                    break;
                case TFun :: POS:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Pos;
                    newVar -> FunKind = TFun :: POS;
                    break;
                case TFun :: INTVL:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Intvl;
                    newVar -> FunKind = TFun :: INTVL;
                    newVar -> offset = newVar -> FunctVar -> offset;
                    * layvalue = newVar -> layvalues;
                    break;
                default:
                    break;
                }
                * value = newVar -> values;
                newVar -> HRU_OBS_indexed = obsVar -> HRU_OBS_indexed;
                if (newVar -> FunKind == TFun :: FOBS)
                {
                    addtoreadlist (newVar);
                }

                else
                if ((Global :: RH_EA_obs == - 1 && obs == "rh") || (Global :: RH_EA_obs == - 1 && obs == "ea") || Global :: OBS_AS_IS || ! (obs == "ea" || obs == "rh"))
                {
                    addtofunctlist (newVar);
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
                return (obsVar -> cnt - 1);
            }

            else
            {
                if (! optional)
                {
                    CRHMException Except ("Observation not found obs: " + obs, TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

                else
                {
                    * value = NULL;
                }

            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, double ** value, TFun typeFun, double ** * layvalue, bool optional)@@@ClassModule.cpp>");
}
long ClassModule :: declobsfunc (string obs, string variable, long ** value, TFun typeFun, bool optional)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * obsVar, * newVar;
    PairVar Item;
    string declModule = "obs ";
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: OBSF, Name, obs, obs, declModule, ID);
    AKAhook (TAKA :: VARD, Name, variable, variable);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            pair < Mapstr :: iterator, Mapstr :: iterator > range = Global :: Mapreadvar.equal_range (Name.c_str ());
            for (Mapstr :: iterator itMap = range.first; itMap != range.second; ++ itMap) {
                if (itMap -> second.first == obs.c_str ())
                {
                    break;
                }

            }
            VandP VP;
            VP.PutV (variation_set);
            VP.PutP ((int) TVISIBLE :: DIAGNOSTIC);
            PairstrV Item2 = PairstrV (obs.c_str (), VP.both);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapreadvar.insert (Item);
            Global :: Mapdeclvar.insert (Item);
            Item2 = PairstrV (variable.c_str (), VP.both);
            Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: DECL:
        {
            if ((itVar = Global :: MapVars.find (declModule + obs)) == Global :: MapVars.end ())
            {
                LogError (CRHMException ("function obs variable not declared: " + Name + " " + obs, TExcept :: WARNING));

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
                return (- 1);
            }

            obsVar = (* itVar).second;
            if ((itVar = Global :: MapVars.find (Name + " " + variable)) != Global :: MapVars.end ())
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
                return (- 1);
            }

            if (! obsVar -> ivalues)
            {
                obsVar -> ivalues = new long [obsVar -> dim];
                obsVar -> varType = TVar :: ReadI;
            }

            newVar = new ClassVar (Name, variable, obsVar -> dim, obsVar -> help + " <" + Fstrings [(int) typeFun] + "> ", obsVar -> units, TVar :: Float);
            newVar -> varType = TVar :: Float;
            newVar -> visibility = TVISIBLE :: DIAGNOSTIC;
            newVar -> variation_set = variation_set;
            newVar -> DLLName = DLLName.c_str ();
            newVar -> root = ID.c_str ();
            PairVar Item = PairVar (Name + " " + variable, newVar);
            Global :: MapVars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
            return (- 1);
        }
    case TBuild :: INIT:
        {
            itVar = Global :: MapVars.find (Name + " " + variable);
            newVar = (* itVar).second;
            if ((itVar = Global :: MapVars.find (declModule + obs)) != Global :: MapVars.end ())
            {
                obsVar = (* itVar).second;
                if (obsVar -> FileData == NULL)
                {
                    if (optional)
                    {
                        LogError (CRHMException ("Function observation: '" + variable + "', cannot be generated from the declared observation, '" + obs.c_str () + "'. Requested by '" + Name.c_str () + "' (optional).", TExcept :: WARNING));
                        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
                        return (- 1);
                    }

                    else
                    {
                        CRHMException Except ("Function observation: '" + variable + "', cannot be generated from the declared observation, '" + obs.c_str () + "'. Requested by '" + Name.c_str () + "'", TExcept :: TERMINATE);
                        LogError (Except);
                        throw Except;
                    }

                }

                if (obsVar -> FileData -> Times != NULL)
                {
                    CRHMException Except ("Function observation: '" + variable + "', uses a 'sparse' Data file. Requested by '" + Name.c_str () + "'", TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

                if (obsVar -> dim != newVar -> dim)
                {
                    delete [] newVar -> ivalues;
                    newVar -> ivalues = new long [obsVar -> dim];
                    newVar -> dim = obsVar -> dim;
                }

                switch (typeFun) {
                case TFun :: AVG:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Avg;
                    newVar -> FunKind = TFun :: AVG;
                    break;
                case TFun :: MIN:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Min;
                    newVar -> FunKind = TFun :: MIN;
                    break;
                case TFun :: MAX:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Max;
                    newVar -> FunKind = TFun :: MAX;
                    break;
                case TFun :: DTOT:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Dtot;
                    newVar -> FunKind = TFun :: DTOT;
                    break;
                case TFun :: TOT:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Tot;
                    newVar -> FunKind = TFun :: TOT;
                    break;
                case TFun :: FIRST:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: First;
                    newVar -> FunKind = TFun :: FIRST;
                    break;
                case TFun :: LAST:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Last;
                    newVar -> FunKind = TFun :: LAST;
                    break;
                case TFun :: POS:
                    newVar -> FunctVar = obsVar;
                    newVar -> UserFunct = & ClassVar :: Pos;
                    newVar -> FunKind = TFun :: POS;
                    break;
                default:
                    break;
                }
                * value = newVar -> ivalues;
                newVar -> No_ReadVar = 0;
                newVar -> HRU_OBS_indexed = obsVar -> HRU_OBS_indexed;
                addtofunctlist (newVar);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
                return (obsVar -> cnt - 1);
            }

            else
            {
                CRHMException Except ("Variable not found obs: " + obs, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declobsfunc(string obs, string variable, long ** value, TFun typeFun, bool optional)@@@ClassModule.cpp>");
}
long ClassModule :: declputvar (string source, string name, string units, double ** value, double ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    long GetUnit;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: VARG, Name, name, name, source, ID);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            string s = string (source.c_str ()) + " " + name.c_str ();
            PairstrV Item2 = PairstrV (s, variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapputvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: DECL:
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: INIT:
        {
            if ((itVar = Global :: MapVars.find (source + " " + name)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                * value = newVar -> values;
                if (layvalue != NULL)
                {
                    (* layvalue) = newVar -> layvalues;
                }

                PairVar Item = PairVar (Name + " " + name, newVar);
                Global :: MapVarsPut.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
                return newVar -> GetUnit ();
            }

            else
            if (source [0] == '*')
            {
                GetUnit = FindWildVarFloat (name, newVar);
                if (GetUnit > - 1)
                {
                    * value = newVar -> values;
                    if (layvalue != NULL)
                    {
                        (* layvalue) = newVar -> layvalues;
                    }

                    PairVar Item = PairVar (Name + " " + name, newVar);
                    Global :: MapVarsPut.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
                    return GetUnit;
                }

                CRHMException Except ("Wild variable not found: " + source + ' ' + name, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

            else
            {
                CRHMException Except ("Variable not found: " + source + ' ' + name, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
}
long ClassModule :: declputvar (string source, string name, string units, long ** value, long ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
    MapVar :: iterator itVar;
    ClassVar * newVar;
    long GetUnit;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
        return (- 1);
    }

    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: VARG, Name, name, name, source, ID);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            string s = string (source.c_str ()) + " " + name.c_str ();
            PairstrV Item2 = PairstrV (s, variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapputvar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: DECL:
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
            return 0;
        }
    case TBuild :: INIT:
        {
            if (name == "Not_Used")
            {
                name = "Not_Used_int";
            }

            if ((itVar = Global :: MapVars.find (source + " " + name)) != Global :: MapVars.end ())
            {
                newVar = (* itVar).second;
                * value = newVar -> ivalues;
                if (layvalue != NULL)
                {
                    (* layvalue) = newVar -> ilayvalues;
                }

                PairVar Item = PairVar (Name + " " + name, newVar);
                Global :: MapVarsPut.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
                return newVar -> GetUnit ();
            }

            else
            if (source [0] == '*')
            {
                GetUnit = FindWildVarFloat (name, newVar);
                if (GetUnit > - 1)
                {
                    * value = newVar -> ivalues;
                    if (layvalue != NULL)
                    {
                        (* layvalue) = newVar -> ilayvalues;
                    }

                    PairVar Item = PairVar (Name + " " + name, newVar);
                    Global :: MapVarsPut.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
                    return GetUnit;
                }

                CRHMException Except ("Wild variable not found: " + source + ' ' + name, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

            else
            {
                CRHMException Except ("Variable not found: " + source + ' ' + name, TExcept :: TERMINATE);
                LogError (Except);
                throw Except;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
    return (- 1);
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputvar(string source, string name, string units, long ** value, long ** * layvalue)@@@ClassModule.cpp>");
}
void ClassModule :: AKAhook (TAKA type, string module, string OrgName, string & NewName)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::AKAhook(TAKA type, string module, string OrgName, string & NewName)@@@ClassModule.cpp>");
    Mapstr2 :: iterator it;
    string ID = AKAstrings [(int) type] + ' ' + module + ' ' + OrgName;
    bool Added = false;
    bool Explicit = false;
    if ((it = Global :: MapAKA.find (ID)) != Global :: MapAKA.end ())
    {
        NewName = (* it).second;
        NewName = NewName.substr (0, NewName.find (' '));
        Explicit = true;
    }

    else
    {
        if (type == TAKA :: OBSD)
        {
            string :: size_type idx = ID.size ();
            if (ID [idx - 1] == '#')
            {
                ID.erase (idx, 1);
            }

            else
            {
                ID.insert (idx, "#");
                Added = true;
            }

            if ((it = Global :: MapAKA.find (ID)) != Global :: MapAKA.end ())
            {
                NewName = (* it).second;
                NewName = NewName.substr (0, NewName.find (' '));
                if (Added)
                {
                    NewName = NewName.substr (0, NewName.size () - 1);
                }

                Explicit = true;
            }

            else
            {
                Added = false;
            }

        }

        else
        if (GroupCnt && type == TAKA :: VARD)
        {
            string A;
            Common :: GroupEnding (A, GroupCnt);
            NewName.append (A);
            ID = AKAstrings [(int) type] + ' ' + module + ' ' + NewName;
            if ((it = Global :: MapAKA.find (ID)) != Global :: MapAKA.end ())
            {
                NewName = (* it).second;
                NewName = NewName.substr (0, NewName.find (' '));
                Explicit = true;
            }

        }

        else
        if (StructCnt && type == TAKA :: VARD)
        {
            string A ("@`");
            A [1] += (char) StructCnt;
            NewName.append (A);
        }

    }

    if (Global :: ReportList && Explicit)
    {
        string ID = AKAstrings [(int) type] + ' ' + module + ' ' + OrgName;
        if (Added)
        {
            NewName.insert (NewName.size (), "#");
        }

        if (Explicit)
        {
            NewName.insert (NewName.size (), " **** explicit");
        }

        Global :: ReportList -> push_back ((ID + " ---> " + NewName).c_str ());
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::AKAhook(TAKA type, string module, string OrgName, string & NewName)@@@ClassModule.cpp>");
}
void ClassModule :: AKAhook (TAKA type, string module, string OrgName, string & NewName, string & source, string base)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::AKAhook(TAKA type, string module, string OrgName, string & NewName, string & source, string base)@@@ClassModule.cpp>");
    Mapstr2 :: iterator it;
    OUTCOME Outcome = OUTCOME :: None;
    TAKA typeL = type;
    if (type == TAKA :: OBSF)
    {
        type = TAKA :: OBSR;
    }

    string Try;
    string ID = AKAstrings [(int) type] + ' ' + module + ' ' + OrgName;
    if ((it = Global :: MapAKA.find (ID)) != Global :: MapAKA.end ())
    {
        Try = (* it).second;
        Try = Try.substr (0, Try.find (' '));
        bool foundInDeclRootList = false;
        for (std :: list < std :: string > :: iterator it = Global :: DeclRootList -> begin (); it != Global :: DeclRootList -> end (); it ++) {
            if (it -> c_str () == std :: string (base + " " + Try).c_str ())
            {
                foundInDeclRootList = true;
            }

        }
        if (foundInDeclRootList == true)
        {
            if (typeL == TAKA :: OBSF)
            {
                Outcome = OUTCOME :: IgnoreObsFunct;
            }

            else
            {
                Outcome = OUTCOME :: IgnoreObs;
            }

        }

        else
        if (GroupCnt && type == TAKA :: VARG)
        {
            string A;
            Common :: GroupEnding (A, GroupCnt);
            NewName.append (A);
            ID = AKAstrings [(int) type] + ' ' + module + ' ' + NewName;
            if ((it = Global :: MapAKA.find (ID)) != Global :: MapAKA.end ())
            {
                Try = (* it).second;
                Try = Try.substr (0, Try.find (' '));
                NewName = (* it).second;
                NewName = NewName.substr (0, NewName.find (' '));
                Outcome = OUTCOME :: Implicit;
            }

        }

        else
        if (typeL == TAKA :: OBSF && Try [Try.size () - 1] == '#')
        {
            Outcome = OUTCOME :: IgnoreObsFunct;
        }

        else
        if (type == TAKA :: OBSR)
        {
            if (Try [Try.size () - 1] == '#')
            {
                bool foundInDeclRootList = false;
                for (std :: list < std :: string > :: iterator it = Global :: DeclRootList -> begin (); it != Global :: DeclRootList -> end (); it ++) {
                    if (it -> c_str () == std :: string (base + " " + Try).c_str ())
                    {
                        foundInDeclRootList = true;
                    }

                }
                if (foundInDeclRootList == true)
                {
                    Outcome = OUTCOME :: IgnoreObs;
                }

            }

            NewName = (* it).second;
            NewName = NewName.substr (0, NewName.find (' '));
            source = (* it).second;
            source = source.substr (source.find (' ') + 1) + ' ';
            Outcome = OUTCOME :: Explicit;
        }

        else
        if (type == TAKA :: VARG)
        {
            NewName = (* it).second;
            NewName = NewName.substr (0, NewName.find (' '));
            source = (* it).second;
            source = source.substr (source.find (' ') + 1);
            Outcome = OUTCOME :: Explicit;
        }

    }

    else
    {
        if (GroupCnt && typeL == TAKA :: OBSR)
        {
            if ((Global :: MapVars.find ("obs " + OrgName)) != Global :: MapVars.end ())
            {
                NewName = OrgName;
                source = "obs ";
                Try = NewName;
                Outcome = OUTCOME :: Implicit;
            }

            else
            {
                string mod_var = ID.substr (ID.find (' ') + 1) + '#';
                if ((Global :: MapVars.find (mod_var)) != Global :: MapVars.end ())
                {
                    NewName = OrgName + '#';
                    source = module + ' ';
                    Try = NewName;
                    Outcome = OUTCOME :: Implicit;
                }

            }

        }

        else
        if ((GroupCnt || StructCnt) && type == TAKA :: VARG)
        {
            string A;
            if (GroupCnt)
            {
                Common :: GroupEnding (A, GroupCnt);
            }

            if (StructCnt)
            {
                Common :: GroupEnding (A, StructCnt);
            }

            NewName.append (A);
            ID = AKAstrings [(int) type] + ' ' + module + ' ' + NewName;
            if ((it = Global :: MapAKA.find (ID)) != Global :: MapAKA.end ())
            {
                Try = (* it).second;
                Try = Try.substr (0, Try.find ('@'));
                bool foundInDeclRootList = false;
                for (std :: list < std :: string > :: iterator it = Global :: DeclRootList -> begin (); it != Global :: DeclRootList -> end (); it ++) {
                    if (it -> c_str () == std :: string (base + " " + Try).c_str ())
                    {
                        foundInDeclRootList = true;
                    }

                }
                if (foundInDeclRootList == true)
                {
                    Outcome = OUTCOME :: IgnoreVar;
                }

                else
                {
                    NewName = (* it).second;
                    NewName = NewName.substr (0, NewName.find (' '));
                    Outcome = OUTCOME :: Implicit;
                }

            }

        }

    }

    ID = AKAstrings [(int) typeL] + " (" + base + ") " + module + " -> " + OrgName + ' ';
    switch (Outcome) {
    case OUTCOME :: IgnoreObs:
        LogMessage (string (ID + " *** AKA warning, not changed to *** " + source + "->" + Try).c_str ());
        break;
    case OUTCOME :: IgnoreObsFunct:
        LogMessage (string (ID + "*** AKA warning, not changed to *** " + source + "->" + Try).c_str ());
        break;
    case OUTCOME :: IgnoreVar:
        LogMessage (string (ID + "*** AKA warning, not changed to *** " + source + "->" + Try).c_str ());
        break;
    default:
        break;
    }
    if (Global :: ReportList && Outcome != OUTCOME :: None)
    {
        string reason = "";
        switch (Outcome) {
        case OUTCOME :: Explicit:
            source = (ID + " changed to " + source + "-> " + Try).c_str ();
            reason = " *** Explicit *** ";
            break;
        case OUTCOME :: Implicit:
            source = (ID + " changed to " + source + "-> " + Try).c_str ();
            reason = " *** Implicit *** ";
            break;
        case OUTCOME :: IgnoreObs:
            source = (ID + " AKA warning, not changed to " + source + "-> " + Try).c_str ();
            reason = " *** source module AKA observation redirection would loop output to input";
            break;
        case OUTCOME :: IgnoreObsFunct:
            source = (ID + " AKA warning, not changed to " + source + "-> " + Try).c_str ();
            reason = " *** daily function cannot be a declared observation";
            break;
        case OUTCOME :: IgnoreVar:
            source = (ID + " AKA warning, not changed to " + source + "-> " + Try).c_str ();
            reason = " *** source module AKA variable redirection would loop output to input";
            break;
        default:
            break;
        }
        Global :: ReportList -> push_back ((source + reason).c_str ());
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::AKAhook(TAKA type, string module, string OrgName, string & NewName, string & source, string base)@@@ClassModule.cpp>");
}
void ClassModule :: InitReadObs (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::InitReadObs(void)@@@ClassModule.cpp>");
    ReadListN -> clear ();
    FunctListN -> clear ();
InstrumentLogger::instance()->log_instrument_log("</ClassModule::InitReadObs(void)@@@ClassModule.cpp>");
}
void ClassModule :: ReadObs (bool Reset)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::ReadObs(bool Reset)@@@ClassModule.cpp>");
    static size_t p;
    static size_t pN;
    static long LastGroup;
    if (Reset)
    {
        p = 0;
        pN = 0;
        LastGroup = - 1;
    }

    if (Global :: DTindx % Global :: Freq == 0)
    {
        while (pN != FunctListN -> size () && FunctListN -> at (pN).first == Name.c_str ()) {
            ClassVar * P = FunctListN -> at (pN).second;
            P -> dim = nhru;
            if (P -> FunctVar -> FileData -> GoodDay)
            {
                (P ->* (P -> UserFunct)) ();
            }

            if (P -> CustomFunct)
            {
                (P ->* (P -> CustomFunct)) (this);
            }

            ++ pN;
        }
    }

    while (p < ReadListN -> size () && ((ReadListN -> at (p).first == Name.c_str ()) || GroupCnt == 0)) {
        ClassVar * P = ReadListN -> at (p).second;
        P -> dim = nhru;
        if (P -> module == "obs" || (GroupCnt && ! P -> No_ReadVar))
        {
            P -> ReadVar ();
        }

        if (P -> CustomFunct)
        {
            (P ->* (P -> CustomFunct)) (this);
        }

        ++ p;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::ReadObs(bool Reset)@@@ClassModule.cpp>");
}
bool ClassModule :: ReadAheadObs (long inc)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::ReadAheadObs(long inc)@@@ClassModule.cpp>");
    if (Global :: DTindx + inc >= Global :: DTmax || Global :: DTindx + inc < Global :: DTmin)
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::ReadAheadObs(long inc)@@@ClassModule.cpp>");
        return false;
    }

    long CurrentDTindx = Global :: DTindx;
    TDateTime CurrentDTnow = Global :: DTnow;
    Global :: DTindx += inc;
    Global :: DTnow = Global :: DTstart + Global :: Interval * ((long long) Global :: DTindx + 1ll);
    size_t p = 0;
    while (p < ReadListN -> size ()) {
        ClassVar * P = ReadListN -> at (p).second;
        if (P -> FileData -> GoodInterval)
        {
            P -> ReadVar ();
        }

        ++ p;
    }
    if (Global :: DTindx % Global :: Freq == 0)
    {
        p = 0;
        while (p < FunctListN -> size () && FunctListN -> at (p).first == Name.c_str ()) {
            ClassVar * P = FunctListN -> at (p).second;
            if (P -> FunctVar -> FileData -> GoodDay)
            {
                (P ->* (P -> UserFunct)) ();
            }

            ++ p;
        }
    }

    Global :: DTindx = CurrentDTindx;
    Global :: DTnow = CurrentDTnow;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::ReadAheadObs(long inc)@@@ClassModule.cpp>");
    return true;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::ReadAheadObs(long inc)@@@ClassModule.cpp>");
}
bool ClassModule :: ReadAheadObsMacro (long inc)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::ReadAheadObsMacro(long inc)@@@ClassModule.cpp>");
    if (Global :: DTindx + inc >= Global :: DTmax || Global :: DTindx + inc < Global :: DTmin)
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::ReadAheadObsMacro(long inc)@@@ClassModule.cpp>");
        return false;
    }

    long CurrentDTindx = Global :: DTindx;
    long ** Save_HRU_obs = Global :: HRU_OBS;
    Global :: DTindx += inc;
    Global :: DTnow = Global :: DTstart + Global :: Interval * ((long long) Global :: DTindx + 1ll);
    size_t p = 0;
    while (p < ReadListN -> size ()) {
        ClassVar * P = (ClassVar *) ReadListN -> at (p).second;
        if (P -> FileData -> GoodInterval)
        {
            P -> ReadVar ();
        }

        ++ p;
    }
    if (Global :: DTindx % Global :: Freq == 0)
    {
        p = 0;
        while (p < FunctListN -> size ()) {
            ClassVar * P = FunctListN -> at (p).second;
            if (P -> FunctVar -> FileData -> GoodDay)
            {
                (P ->* (P -> UserFunct)) ();
            }

            ++ p;
        }
    }

    Global :: DTindx = CurrentDTindx;
    Global :: HRU_OBS = Save_HRU_obs;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::ReadAheadObsMacro(long inc)@@@ClassModule.cpp>");
    return true;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::ReadAheadObsMacro(long inc)@@@ClassModule.cpp>");
}
bool ClassModule :: WriteAheadObsMacro (long inc)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::WriteAheadObsMacro(long inc)@@@ClassModule.cpp>");
    if (Global :: DTindx + inc >= Global :: DTmax || Global :: DTindx + inc < Global :: DTmin)
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::WriteAheadObsMacro(long inc)@@@ClassModule.cpp>");
        return false;
    }

    long CurrentDTindx = Global :: DTindx;
    Global :: DTindx += inc;
    size_t p = 0;
    while (p < ReadListN -> size ()) {
        ClassVar * P = ReadListN -> at (p).second;
        if (P -> FileData -> GoodInterval)
        {
            P -> WriteVar ();
        }

        ++ p;
    }
    Global :: DTindx = CurrentDTindx;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::WriteAheadObsMacro(long inc)@@@ClassModule.cpp>");
    return true;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::WriteAheadObsMacro(long inc)@@@ClassModule.cpp>");
}
void ClassModule :: DelReadObs (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::DelReadObs(void)@@@ClassModule.cpp>");
    ReadListN -> clear ();
    FunctListN -> clear ();
InstrumentLogger::instance()->log_instrument_log("</ClassModule::DelReadObs(void)@@@ClassModule.cpp>");
}
bool ClassModule :: AnyOne (double * Data, int Cnt, double Val)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::AnyOne(double * Data, int Cnt, double Val)@@@ClassModule.cpp>");
    if (Data == NULL)
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::AnyOne(double * Data, int Cnt, double Val)@@@ClassModule.cpp>");
        return false;
    }

    for (int ii = 0; ii < Cnt; ++ ii)
        if (Data [ii] != Val)
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::AnyOne(double * Data, int Cnt, double Val)@@@ClassModule.cpp>");
            return true;
        }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::AnyOne(double * Data, int Cnt, double Val)@@@ClassModule.cpp>");
    return false;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::AnyOne(double * Data, int Cnt, double Val)@@@ClassModule.cpp>");
}
bool ClassModule :: UsingObservations (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::UsingObservations(void)@@@ClassModule.cpp>");
    if (ReadListN -> size () || FunctListN -> size ())
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::UsingObservations(void)@@@ClassModule.cpp>");
        return true;
    }

    else
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::UsingObservations(void)@@@ClassModule.cpp>");
        return false;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::UsingObservations(void)@@@ClassModule.cpp>");
}
void ClassModule :: addtoreadlist (ClassVar * newVar)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::addtoreadlist(ClassVar * newVar)@@@ClassModule.cpp>");
    size_t p = 0;
    ClassVar * P;
    while (p < ReadListN -> size () && GroupCnt == 0) {
        P = (ClassVar *) ReadListN -> at (p).second;
        if (newVar == P)
        {
            if (this -> Name == this -> NameRoot)
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::addtoreadlist(ClassVar * newVar)@@@ClassModule.cpp>");
                return;
            }

        }

        p ++;
    }
    if (((Global :: DTmax - Global :: DTmin) / Global :: Freq + newVar -> FileData -> Dt1 > newVar -> FileData -> Dt2) && ! newVar -> FileData -> Times)
    {
        CRHMException TExcept ((string ("Observation file '") + string (newVar -> FileData -> DataFileName.c_str ()) + "' shorter than model run!").c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (GroupCnt)
    {
        ReadListN -> push_back (std :: pair < std :: string, ClassVar * > (Name.c_str (), newVar));
    }

    else
    {
        ReadListN -> push_back (std :: pair < std :: string, ClassVar * > (newVar -> module.c_str (), newVar));
    }

    newVar -> CustomFunct = NULL;
    if (! Global :: OBS_AS_IS)
    {
        string :: size_type idx = newVar -> name.find ('@');
        string root_var;
        if (idx != string :: npos)
        {
            root_var = newVar -> name.substr (0, idx);
        }

        if (newVar -> name == "t_max" || newVar -> name == "t_min")
        {
            if (AnyOne (Global :: Warming_t, nhru, 0.0))
            {
                newVar -> CustomFunct = & ClassVar :: do_t_Clim;
                newVar -> CustomFunctName = "do_t_Clim";
            }

            else
            {
                newVar -> CustomFunct = & ClassVar :: do_t;
                newVar -> CustomFunctName = "do_t";
            }

        }

        else
        if ((! GroupCnt && newVar -> name == "p") || (GroupCnt && root_var == "p"))
        {
            if (AnyOne (Global :: Warming_p, nhru, 1.0))
            {
                newVar -> CustomFunct = & ClassVar :: do_p_Clim;
                newVar -> CustomFunctName = "do_p_Clim";
            }

            else
            {
                newVar -> CustomFunct = & ClassVar :: do_p;
                newVar -> CustomFunctName = "do_p";
            }

        }

    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::addtoreadlist(ClassVar * newVar)@@@ClassModule.cpp>");
}
void ClassModule :: addtofunctlist (ClassVar * newVar)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::addtofunctlist(ClassVar * newVar)@@@ClassModule.cpp>");
    size_t p = 0;
    ClassVar * P;
    string :: size_type indx;
    while (p < FunctListN -> size ()) {
        P = (ClassVar *) FunctListN -> at (p).second;
        if (newVar == P)
        {
            if (this -> Name == this -> NameRoot && GroupCnt == 0)
            {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::addtofunctlist(ClassVar * newVar)@@@ClassModule.cpp>");
                return;
            }

        }

        p ++;
    }
    FunctListN -> push_back (std :: pair < std :: string, ClassVar * > (Name.c_str (), newVar));
    if (NameRoot == "obs" || Name == "obs")
    {
        string RootName = newVar -> name;
        if (indx = RootName.find ("@"), indx != std :: string :: npos)
        {
            RootName = newVar -> name.substr (0, indx);
        }

        if (! Global :: OBS_AS_IS)
        {
            if (RootName == "Tday")
            {
                if (AnyOne (Global :: Warming_t, nhru, 0.0))
                {
                    newVar -> CustomFunct = & ClassVar :: do_t_day_Clim;
                    newVar -> CustomFunctName = "do_t_day_Clim";
                }

                else
                {
                    newVar -> CustomFunct = & ClassVar :: do_t_day;
                    newVar -> CustomFunctName = "do_t_day";
                }

            }

            else
            if (RootName == "RHday")
            {
                if (AnyOne (Global :: Warming_t, nhru, 0.0))
                {
                    newVar -> CustomFunct = & ClassVar :: do_rh_day_Clim;
                    newVar -> CustomFunctName = "do_rh_day_Clim";
                }

                else
                {
                    newVar -> CustomFunct = & ClassVar :: do_rh_day;
                    newVar -> CustomFunctName = "do_rh_day";
                }

            }

            else
            if (RootName == "EAday")
            {
                if (! AnyOne (Global :: Warming_t, nhru, 0.0))
                {
                    newVar -> CustomFunct = & ClassVar :: do_ea_day;
                    newVar -> CustomFunctName = "do_ea_day";
                }

            }

            else
            if (RootName == "t_min" || newVar -> name == "t_max")
            {
                if (AnyOne (Global :: Warming_t, nhru, 0.0))
                {
                    newVar -> CustomFunct = & ClassVar :: do_t_Clim;
                    newVar -> CustomFunctName = "do_t_Clim";
                }

                else
                {
                    newVar -> CustomFunct = & ClassVar :: do_t;
                    newVar -> CustomFunctName = "do_t";
                }

            }

            else
            if (RootName.substr (0, 4) == "pptD")
            {
                if (AnyOne (Global :: Warming_p, nhru, 1.0))
                {
                    newVar -> CustomFunct = & ClassVar :: do_ppt_Clim;
                    newVar -> CustomFunctName = "do_ppt_Clim";
                }

                else
                {
                    newVar -> CustomFunct = & ClassVar :: do_ppt;
                    newVar -> CustomFunctName = "do_ppt";
                }

            }

        }

        else
        {
            newVar -> CustomFunct = NULL;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::addtofunctlist(ClassVar * newVar)@@@ClassModule.cpp>");
}
long ClassModule :: getdim (TDim dimen)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
    MapDim :: iterator itDim;
    string s;
    switch (dimen) {
    case TDim :: BASIN:

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
        return ((int) TDim :: ONE);
    case TDim :: ONE:
    case TDim :: TWO:
    case TDim :: THREE:
    case TDim :: FOUR:
    case TDim :: FIVE:
    case TDim :: SIX:
    case TDim :: SEVEN:
    case TDim :: EIGHT:
    case TDim :: NINE:
    case TDim :: TEN:
    case TDim :: ELEVEN:
    case TDim :: TWELVE:

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
        return ((int) dimen);
    case TDim :: NHRU:
        if (nhru)
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
            return (nhru);
        }

        else
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
            return (Global :: nhru);
        }

    case TDim :: NOBS:

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
        return (Global :: nhru);
    case TDim :: NLAY:

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
        return (Global :: nlay);
    default:

InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
        return (- 1);
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::getdim(TDim dimen)@@@ClassModule.cpp>");
}
TAKA AKAtype (string type)
{
InstrumentLogger::instance()->log_instrument_log("<AKAtype(string type)@@@ClassModule.cpp>");
    TAKA Type = TAKA :: AKAERROR;
    for (int ii = (int) TAKA :: VARG; ii < (int) TAKA :: AKAEND; ++ ii)
        if (type == AKAstrings [ii])
        {
            Type = (TAKA) ii;
            break;
        }

InstrumentLogger::instance()->log_instrument_log("</AKAtype(string type)@@@ClassModule.cpp>");
    return Type;
InstrumentLogger::instance()->log_instrument_log("</AKAtype(string type)@@@ClassModule.cpp>");
}
bool ClassModule :: Variation_Skip (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::Variation_Skip(void)@@@ClassModule.cpp>");
    if ((Global :: BuildFlag == TBuild :: BUILD && variation == 0) || ((variation_set & 2048) != 0 && variation == 0) || (variation_set & 4096) != 0 || (variation_set == 0) || (variation_set & variation) != 0)
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::Variation_Skip(void)@@@ClassModule.cpp>");
        return false;
    }

    else
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::Variation_Skip(void)@@@ClassModule.cpp>");
        return true;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::Variation_Skip(void)@@@ClassModule.cpp>");
}
long ClassModule :: declputparam (string source, string param, string units, double ** value, double ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar = NULL;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
        return - 1;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, param, units);
    AKAhook (TAKA :: PARD, Name, param, param);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (param.c_str (), variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclpar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
            return - 1;
        }
    case TBuild :: DECL:
        {
            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                newPar -> Inhibit_share = 1;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
                return 0;
            }

            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                if ((* itPar).second -> dim == this -> nhru)
                {
                    newPar = (* itPar).second;
                }

                newPar -> Inhibit_share = 1;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
                return 0;
            }

            newPar = new ClassPar (string (Name.c_str ()), string (param), TDim :: NHRU, "", 0, 0, "", units, TVar :: Float);
            newPar -> basemodule = this -> NameRoot;
            newPar -> variation_set = variation_set;
            newPar -> visibility = TVISIBLE :: USUAL;
            if (this -> GroupCnt)
            {
                newPar -> Inhibit_share = 2;
            }

            Myparser ob;
            ob.eval_exp (newPar);
            PairPar Item = PairPar (Name + " " + param, newPar);
            Global :: MapPars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
            return - 1;
        }
    case TBuild :: INIT:
        {
            if (source [0] != '*')
            {
                if ((itPar = Global :: MapPars.find (source + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share != 2)
                    {
                        * value = newPar -> values;
                        if (layvalue != NULL)
                        {
                            (* layvalue) = newPar -> layvalues;
                        }

                    }

                }

            }

            else
            {
                if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * value = newPar -> values;
                    if (layvalue != NULL)
                    {
                        (* layvalue) = newPar -> layvalues;
                    }

                }

                else
                if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share != 2)
                    {
                        * value = newPar -> values;
                        if (layvalue != NULL)
                        {
                            (* layvalue) = newPar -> layvalues;
                        }

                    }

                }

                else
                if ((newPar = ClassParFindPar (param)))
                {
                    if (newPar -> Inhibit_share != 2)
                    {
                        * value = newPar -> values;
                        if (layvalue != NULL)
                        {
                            (* layvalue) = newPar -> layvalues;
                        }

                    }

                }

                else
                {
                    CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
                return 0;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
    return 0;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, double ** value, double ** * layvalue)@@@ClassModule.cpp>");
}
long ClassModule :: declgetparam (string source, string param, string units, const double ** value, const double ** * layvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declgetparam(string source, string param, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar;
    if (Variation_Skip ())
    {
        * value = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
        return - 1;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, param, units);
    AKAhook (TAKA :: PARD, Name, param, param);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (param.c_str (), variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclpar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
            return - 1;
        }
    case TBuild :: DECL:
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
            return - 1;
        }
    case TBuild :: INIT:
        {
            if (source [0] != '*')
            {
                if ((itPar = Global :: MapPars.find (source + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * value = newPar -> values;
                    if (layvalue != NULL)
                    {
                        * layvalue = (const double **) newPar -> layvalues;
                    }

                }

            }

            else
            {
                if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * value = (const double *) newPar -> values;
                    if (layvalue != NULL)
                    {
                        * layvalue = (const double **) newPar -> layvalues;
                    }

                }

                else
                if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * value = (const double *) newPar -> values;
                    if (layvalue != NULL)
                    {
                        * layvalue = (const double **) newPar -> layvalues;
                    }

                }

                else
                if ((newPar = ClassParFindPar (param)))
                {
                    * value = newPar -> values;
                    if (layvalue != NULL)
                    {
                        * layvalue = (const double **) newPar -> layvalues;
                    }

                }

                else
                {
                    CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
                return 0;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
    return 0;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const double ** value, const double ** * layvalue)@@@ClassModule.cpp>");
}
ClassModule * ClassModule :: FindModule_from_parameter (string source, string param)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::FindModule_from_parameter(string source, string param)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar = NULL;
    variation_max = variation_max | variation_set;
    AKAhook (TAKA :: PARD, Name, param, param);
    if (source [0] != '*')
    {
        if ((itPar = Global :: MapPars.find (source + " " + param)) != Global :: MapPars.end ())
        {
            newPar = (* itPar).second;
        }

    }

    else
    {
        if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
        {
            newPar = (* itPar).second;
        }

        else
        if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
        {
            newPar = (* itPar).second;
        }

        else
        if ((newPar = ClassParFindPar (param)))
        {
        }

        else
        {
            CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
            LogError (Except);
            throw Except;
        }

    }

    if (GroupCnt)
    {
    }

    else
    {
        std :: list < std :: pair < std :: string, ClassModule * >> :: iterator pos = Global :: OurModulesList -> end ();
        for (std :: list < std :: pair < std :: string, ClassModule * >> :: iterator it = Global :: OurModulesList -> begin (); it != Global :: OurModulesList -> end (); it ++) {
            if (it -> first == newPar -> basemodule.c_str ())
            {
                pos = it;
            }

        }
        if (pos != Global :: OurModulesList -> end ())
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindModule_from_parameter(string source, string param)@@@ClassModule.cpp>");
            return pos -> second;
        }

    }

    CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
    LogError (Except);
    throw Except;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::FindModule_from_parameter(string source, string param)@@@ClassModule.cpp>");
}
long ClassModule :: declputparam (string source, string param, string units, long ** ivalue, long ** * ilayvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar = NULL;
    if (Variation_Skip ())
    {
        * ivalue = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
        return - 1;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, param, units);
    AKAhook (TAKA :: PARD, Name, param, param);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (param.c_str (), variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclpar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
            return - 1;
        }
    case TBuild :: DECL:
        {
            if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
            {
                newPar = (* itPar).second;
                newPar -> Inhibit_share = 1;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
                return 0;
            }

            if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
            {
                if ((* itPar).second -> dim == this -> nhru)
                {
                    newPar = (* itPar).second;
                }

                newPar -> Inhibit_share = 1;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
                return 0;
            }

            newPar = new ClassPar (string (Name.c_str ()), string (param), TDim :: NHRU, "", 0, 0, "", units, TVar :: Int);
            newPar -> basemodule = this -> NameRoot;
            newPar -> variation_set = variation_set;
            newPar -> visibility = TVISIBLE :: USUAL;
            newPar -> Inhibit_share = 2;
            Myparser ob;
            ob.eval_exp (newPar);
            PairPar Item = PairPar (Name + " " + param, newPar);
            Global :: MapPars.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
            return - 1;
        }
    case TBuild :: INIT:
        {
            if (source [0] != '*')
            {
                if ((itPar = Global :: MapPars.find (source + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share != 2)
                    {
                        * ivalue = newPar -> ivalues;
                        if (ilayvalue != NULL)
                        {
                            (* ilayvalue) = newPar -> ilayvalues;
                        }

                    }

                }

            }

            else
            {
                if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * ivalue = newPar -> ivalues;
                    if (ilayvalue != NULL)
                    {
                        (* ilayvalue) = newPar -> ilayvalues;
                    }

                }

                else
                if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    if (newPar -> Inhibit_share != 2)
                    {
                        * ivalue = newPar -> ivalues;
                        if (ilayvalue != NULL)
                        {
                            (* ilayvalue) = newPar -> ilayvalues;
                        }

                    }

                }

                else
                if ((newPar = ClassParFindPar (param)))
                {
                    if (newPar -> Inhibit_share != 2)
                    {
                        * ivalue = newPar -> ivalues;
                        if (ilayvalue != NULL)
                        {
                            (* ilayvalue) = newPar -> ilayvalues;
                        }

                    }

                }

                else
                {
                    CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
                return 0;
            }

        }
    default:
        break;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
    return 0;
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declputparam(string source, string param, string units, long ** ivalue, long ** * ilayvalue)@@@ClassModule.cpp>");
}
void ClassModule :: declgetparam (string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
    MapPar :: iterator itPar;
    ClassPar * newPar;
    if (Variation_Skip ())
    {
        * ivalue = NULL;

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
        return;
    }

    variation_max = variation_max | variation_set;
    Convert convert;
    convert.CheckUnitsString (Name, param, units);
    AKAhook (TAKA :: PARD, Name, param, param);
    switch (Global :: BuildFlag) {
    case TBuild :: BUILD:
        {
            PairstrV Item2 = PairstrV (param.c_str (), variation_set);
            PairstrI Item = PairstrI (Name.c_str (), Item2);
            Global :: Mapdeclpar.insert (Item);

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: DECL:
        {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
            return;
        }
    case TBuild :: INIT:
        {
            if (source [0] != '*')
            {
                if ((itPar = Global :: MapPars.find (source + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * ivalue = newPar -> ivalues;
                    if (ilayvalue != NULL)
                    {
                        * ilayvalue = (const long **) newPar -> ilayvalues;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
                    return;
                }

            }

            else
            {
                if ((itPar = Global :: MapPars.find (Name + " " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * ivalue = (const long *) newPar -> ivalues;
                    if (ilayvalue != NULL)
                    {
                        * ilayvalue = (const long **) newPar -> ilayvalues;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
                    return;
                }

                else
                if ((itPar = Global :: MapPars.find ("Shared " + param)) != Global :: MapPars.end ())
                {
                    newPar = (* itPar).second;
                    * ivalue = newPar -> ivalues;
                    if (ilayvalue != NULL)
                    {
                        * ilayvalue = (const long **) newPar -> ilayvalues;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
                    return;
                }

                else
                if ((newPar = ClassParFindPar (param)))
                {
                    * ivalue = newPar -> ivalues;
                    if (ilayvalue != NULL)
                    {
                        * ilayvalue = (const long **) newPar -> ilayvalues;
                    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
                    return;
                }

                else
                {
                    CRHMException Except ("Parameter not found: " + Name + " " + param, TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

            }

        }
    default:
        break;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassModule::declgetparam(string source, string param, string units, const long ** ivalue, const long ** * ilayvalue)@@@ClassModule.cpp>");
}
ClassModule * ClassModule :: link (string Module)
{
InstrumentLogger::instance()->log_instrument_log("<ClassModule::link(string Module)@@@ClassModule.cpp>");
    if (Name == Module.c_str ())
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::link(string Module)@@@ClassModule.cpp>");
        return NULL;
    }

    std :: list < std :: pair < std :: string, ClassModule * >> :: iterator pos;
    for (std :: list < std :: pair < std :: string, ClassModule * >> :: iterator it = Global :: OurModulesList -> begin (); it != Global :: OurModulesList -> end (); it ++) {
        if (it -> first == Module.c_str ())
        {
            pos = it;
        }

    }
    if (pos == Global :: OurModulesList -> end ())
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::link(string Module)@@@ClassModule.cpp>");
        return NULL;
    }

    else
    {

InstrumentLogger::instance()->log_instrument_log("</ClassModule::link(string Module)@@@ClassModule.cpp>");
        return pos -> second;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassModule::link(string Module)@@@ClassModule.cpp>");
}