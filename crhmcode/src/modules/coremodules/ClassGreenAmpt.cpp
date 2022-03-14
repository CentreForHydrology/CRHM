#include "ClassGreenAmpt.h"
#include <algorithm>
#include "../../core/GlobalDll.h"
#include "../../core/ClassCRHM/ClassCRHM.h"
#include "../newmodules/SnobalDefines.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include "../../core/InstrumentLogger.h"

using namespace CRHM;
ClassGreenAmpt * ClassGreenAmpt :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::klone(string name) const@@@ClassGreenAmpt.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::klone(string name) const@@@ClassGreenAmpt.cpp>");
    return new ClassGreenAmpt (name);
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::klone(string name) const@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::decl(void)@@@ClassGreenAmpt.cpp>");
    Description = "'Summer Green Ampt model.'";
    declvar ("infil", TDim :: NHRU, "Potential amount of water infiltrating the soil on each HRU", "(mm/int)", & infil);
    declstatdiag ("cuminfil", TDim :: NHRU, "cumulative potential infiltration on each HRU", "(mm)", & cuminfil);
    declvar ("runoff", TDim :: NHRU, "rainfall runoff", "(mm/int)", & runoff);
    declstatdiag ("cumrunoff", TDim :: NHRU, "cumulative rainfall runoff", "(mm)", & cumrunoff);
    declvar ("snowinfil", TDim :: NHRU, "infiltration", "(mm/int)", & snowinfil);
    declstatdiag ("cumsnowinfil", TDim :: NHRU, "cumulative infiltration", "(mm)", & cumsnowinfil);
    declvar ("meltrunoff", TDim :: NHRU, "melt runoff", "(mm/int)", & meltrunoff);
    declstatvar ("cummeltrunoff", TDim :: NHRU, "cumulative melt runoff", "(mm/int)", & cummeltrunoff);
    decllocal ("k", TDim :: NHRU, "(mm/h)", "()", & k);
    declstatdiag ("F0", TDim :: NHRU, "last HRU cumulative infiltration", "(mm)", & F0);
    decllocal ("f0", TDim :: NHRU, "", "(mm/h)", & f0);
    declstatdiag ("F1", TDim :: NHRU, "HRU cumulative infiltration", "(mm)", & F1);
    decllocal ("f1", TDim :: NHRU, "", "(mm/h)", & f1);
    decllocal ("dthbot", TDim :: NHRU, "", "()", & dthbot);
    decllocal ("psidthbot", TDim :: NHRU, "", "(mm)", & psidthbot);
    declparam ("basin_area", TDim :: BASIN, "3", "1e-6", "1e+09", "total basin area", "(km^2)", & basin_area);
    declparam ("hru_area", TDim :: NHRU, "[1]", "1e-6", "1e+09", "hru area", "(km^2)", & hru_area);
    declparam ("soil_type", TDim :: NHRU, "[4]", "0", "12", "water/sand/loamsand/sandloam/loam/siltloam/sasclloam/clayloam/siclloam/sandclay/siltclay/clay/pavement"
        " 0 - 12", "()", & soil_type);
    declparam ("soil_moist_max", TDim :: NHRU, "[375.0]", "0.0", "5000.0", "Maximum available water holding capacity of soil profile."
        "Soil profile is surface to bottom of rooting zone", "(mm)", & soil_moist_max);
    declparam ("soil_moist_init", TDim :: NHRU, "[187.0]", "0.0", "2500.0", "Initial value of available water in soil profile", "(mm)", & soil_moist_init);
    declgetvar ("*", "net_rain", "(mm/int)", & net_rain);
    declgetvar ("*", "snowmeltD", "(mm/d)", & snowmelt);
    declputvar ("*", "soil_moist", "(mm)", & soil_moist);
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::decl(void)@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::init(void)@@@ClassGreenAmpt.cpp>");
    nhru = getdim (TDim :: NHRU);
    for (hh = 0; hh < nhru; ++ hh) {
        cuminfil [hh] = 0.0;
        cumrunoff [hh] = 0.0;
        cumsnowinfil [hh] = 0.0;
        cummeltrunoff [hh] = 0.0;
        F1 [hh] = soil_moist_max [hh];
        k [hh] = soilproperties [soil_type [hh]] [KSAT];
        dthbot [hh] = (1.0 - soil_moist_init [hh] / soil_moist_max [hh]);
        psidthbot [hh] = soilproperties [soil_type [hh]] [PSI] * dthbot [hh];
        f1 [hh] = calcf1 (F1 [hh], psidthbot [hh]) * Global :: Interval * 24.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::init(void)@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::run(void)@@@ClassGreenAmpt.cpp>");
    for (hh = 0; chkStruct (); ++ hh) {
        infil [hh] = 0.0;
        runoff [hh] = 0.0;
        snowinfil [hh] = 0.0;
        meltrunoff [hh] = 0.0;
        double melt = snowmelt [hh] / Global :: Freq;
        double All = net_rain [hh] + melt;
        if (All > 0.0)
        {
            garain = All;
            intensity = All * Global :: Freq / 24.0;
            if (garain > 0.0)
            {
                if (soil_type [hh] == 12)
                {
                    runoff [hh] = garain;
                }

                else
                if (soil_type [hh] == 0 || soil_moist [hh] <= 0.0)
                {
                    infil [hh] = garain;
                }

                else
                {
                    F1 [hh] = soil_moist [hh];
                    dthbot [hh] = (1.0 - soil_moist [hh] / soil_moist_max [hh]);
                    psidthbot [hh] = soilproperties [soil_type [hh]] [PSI] * dthbot [hh];
                    if (soil_type [hh] > 0)
                    {
                        f1 [hh] = calcf1 (F1 [hh], psidthbot [hh]) * Global :: Interval * 24.0;
                    }

                    infiltrate ();
                    infil [hh] = F1 [hh] - F0 [hh];
                    if (pond > 0.0)
                    {
                        runoff [hh] = pond;
                    }

                }

            }

            if (melt >= infil [hh])
            {
                snowinfil [hh] = melt;
                infil [hh] = 0.0;
            }

            else
            if (melt > 0.0)
            {
                snowinfil [hh] = melt;
                infil [hh] -= snowinfil [hh];
            }

            else
            {
                snowinfil [hh] = 0.0;
            }

            if (melt - snowinfil [hh] >= pond)
            {
                meltrunoff [hh] = melt - snowinfil [hh];
                runoff [hh] = 0.0;
            }

            else
            if (melt - snowinfil [hh] > 0.0)
            {
                meltrunoff [hh] = melt - snowinfil [hh];
                runoff [hh] = pond - meltrunoff [hh];
            }

            else
            {
                meltrunoff [hh] = 0.0;
                runoff [hh] = pond;
            }

            cuminfil [hh] += infil [hh];
            cumrunoff [hh] += runoff [hh];
            cumsnowinfil [hh] += snowinfil [hh];
            cummeltrunoff [hh] += meltrunoff [hh];
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::run(void)@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::finish(bool good)@@@ClassGreenAmpt.cpp>");
    for (hh = 0; chkStruct (); ++ hh) {
        LogMessageA (hh, string ("'" + Name + " (GreenAmpt)' cuminfil(rain)      (mm) (mm*hru) (mm*hru/basin): ").c_str (), cuminfil [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (GreenAmpt)' cumrunoff(rain)     (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumrunoff [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (GreenAmpt)' cumsnowinfil(snow)  (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumsnowinfil [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (GreenAmpt)' cummeltrunoff(snow) (mm) (mm*hru) (mm*hru/basin): ").c_str (), cummeltrunoff [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
    }
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::finish(bool good)@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: infiltrate (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::infiltrate(void)@@@ClassGreenAmpt.cpp>");
    F0 [hh] = F1 [hh];
    f0 [hh] = f1 [hh];
    if (soil_type [hh] == 0)
    {
        pond += garain;

InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::infiltrate(void)@@@ClassGreenAmpt.cpp>");
        return;
    }

    pond = 0.0;
    f0 [hh] = calcf1 (F0 [hh], psidthbot [hh]) * Global :: Interval * 24.0;
    if (intensity > f0 [hh])
    {
        ponding ();

InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::infiltrate(void)@@@ClassGreenAmpt.cpp>");
        return;
    }

    F1 [hh] = F0 [hh] + garain;
    f1 [hh] = calcf1 (F1 [hh], psidthbot [hh]) * Global :: Interval * 24.0;
    if (intensity > f1 [hh])
    {
        startponding ();
    }

InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::infiltrate(void)@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: ponding (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::ponding(void)@@@ClassGreenAmpt.cpp>");
    F1 [hh] = F0 [hh] + garain;
    howmuch (F0 [hh], Global :: Interval * 24.0);
    pond += garain - (F1 [hh] - F0 [hh]);
    howmuch (F0 [hh], Global :: Interval * 24.0);
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::ponding(void)@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: startponding (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::startponding(void)@@@ClassGreenAmpt.cpp>");
    double Fp = k [hh] * psidthbot [hh] / (intensity - k [hh]);
    double dt = (Fp - F0 [hh]) / intensity;
    howmuch (Fp, Global :: Interval * 24.0 - dt);
    pond += garain - (F1 [hh] - F0 [hh]);
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::startponding(void)@@@ClassGreenAmpt.cpp>");
}
void ClassGreenAmpt :: howmuch (double F0, double dt)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::howmuch(double F0, double dt)@@@ClassGreenAmpt.cpp>");
    double LastF1;
    do {
        LastF1 = F1 [hh];
        F1 [hh] = F0 + k [hh] * dt + psidthbot [hh] * log ((F1 [hh] + psidthbot [hh]) / (F0 + psidthbot [hh]));
    }
    while (fabs (LastF1 - F1 [hh]) > 0.001);
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::howmuch(double F0, double dt)@@@ClassGreenAmpt.cpp>");
}
double ClassGreenAmpt :: calcf1 (double F, double psidth)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGreenAmpt::calcf1(double F, double psidth)@@@ClassGreenAmpt.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::calcf1(double F, double psidth)@@@ClassGreenAmpt.cpp>");
    return k [hh] * (psidth / F + 1.0);
InstrumentLogger::instance()->log_instrument_log("</ClassGreenAmpt::calcf1(double F, double psidth)@@@ClassGreenAmpt.cpp>");
}