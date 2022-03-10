#include "ClassGrow_Crop.h"
#include "GlobalDll.h"
#include <algorithm>
#include "ClassCRHM.h"
#include "SnobalDefines.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include "../../core/InstrumentLogger.h"

using namespace CRHM;
ClassGrow_Crop * ClassGrow_Crop :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_Crop::klone(string name) const@@@ClassGrow_Crop.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassGrow_Crop::klone(string name) const@@@ClassGrow_Crop.cpp>");
    return new ClassGrow_Crop (name);
InstrumentLogger::instance()->log_instrument_log("</ClassGrow_Crop::klone(string name) const@@@ClassGrow_Crop.cpp>");
}
void ClassGrow_Crop :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_Crop::decl(void)@@@ClassGrow_Crop.cpp>");
    Description = "'Grows crop height from initial to maturiy.'";
    declparam ("Ht", TDim :: NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "vegetation height(m)", "(m)", & Ht);
    declparam ("Crop_Grow_Rate", TDim :: NHRU, "[0.0]", "0.0", "1.0", "crop growth rate", "(m/d)", & Crop_Grow_Rate);
    declparam ("JCrop_Start", TDim :: NHRU, "[121]", "1", "366", "start Julian day", "()", & JCrop_Start);
    declparam ("JCrop_Mature", TDim :: NHRU, "[182]", "1", "366", "maturity Julian day", "()", & JCrop_Mature);
    declparam ("JCrop_Harvest", TDim :: NHRU, "[228]", "1", "366", "harvest Julian day", "()", & JCrop_Harvest);
    declparam ("Init_Crop_Ht", TDim :: NHRU, "0.001]", "0.001", "100.0", "Crop height at beginning of growth season. Used to reset crop Ht(parameter Ht)", "(m)", & Init_Crop_Ht);
    declvar ("Crop_Ht", TDim :: NHRU, "crop height", "(m)", & Crop_Ht);
InstrumentLogger::instance()->log_instrument_log("</ClassGrow_Crop::decl(void)@@@ClassGrow_Crop.cpp>");
}
void ClassGrow_Crop :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_Crop::init(void)@@@ClassGrow_Crop.cpp>");
    nhru = getdim (TDim :: NHRU);
InstrumentLogger::instance()->log_instrument_log("</ClassGrow_Crop::init(void)@@@ClassGrow_Crop.cpp>");
}
void ClassGrow_Crop :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_Crop::run(void)@@@ClassGrow_Crop.cpp>");
    long nstep = getstep () % Global :: Freq;
    long JULIAN;
    if (nstep == 0)
    {
        JULIAN = julian ("now");
        for (hh = 0; chkStruct (); ++ hh) {
            if (JULIAN >= JCrop_Start [hh] && JULIAN < JCrop_Mature [hh] && Crop_Grow_Rate [hh] > 0.0)
            {
                const_cast < double * > (Ht) [hh] = Ht [hh] + Crop_Grow_Rate [hh];
            }

            else
            if (JULIAN == JCrop_Harvest [hh])
            {
                const_cast < double * > (Ht) [hh] = Init_Crop_Ht [hh];
            }

            Crop_Ht [hh] = Ht [hh];
        }
    }

InstrumentLogger::instance()->log_instrument_log("</ClassGrow_Crop::run(void)@@@ClassGrow_Crop.cpp>");
}