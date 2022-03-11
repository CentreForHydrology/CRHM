//created by Manishankar Mondal

#include "ClassGrow_Crop.h"

#include <algorithm>
#include "../../core/GlobalDll.h"
#include "../../core/ClassCRHM/ClassCRHM.h"
#include "../newmodules/SnobalDefines.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace CRHM;

ClassGrow_Crop* ClassGrow_Crop::klone(string name) const{
  return new ClassGrow_Crop(name);
}

void ClassGrow_Crop::decl(void) {

  Description = "'Grows crop height from initial to maturiy.'";

  declparam("Ht", TDim::NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "vegetation height(m)", "(m)", &Ht);

  declparam("Crop_Grow_Rate", TDim::NHRU, "[0.0]", "0.0", "1.0", "crop growth rate", "(m/d)", &Crop_Grow_Rate);

  declparam("JCrop_Start", TDim::NHRU, "[121]", "1", "366", "start Julian day", "()", &JCrop_Start);

  declparam("JCrop_Mature", TDim::NHRU, "[182]", "1", "366", "maturity Julian day", "()", &JCrop_Mature);

  declparam("JCrop_Harvest", TDim::NHRU, "[228]", "1", "366", "harvest Julian day", "()", &JCrop_Harvest);

  declparam("Init_Crop_Ht", TDim::NHRU, "0.001]", "0.001", "100.0", "Crop height at beginning of growth season. Used to reset crop Ht(parameter Ht)", "(m)", &Init_Crop_Ht);


  declvar("Crop_Ht", TDim::NHRU, "crop height", "(m)", &Crop_Ht);

}

void ClassGrow_Crop::init(void) {

  nhru = getdim(TDim::NHRU);
}

void ClassGrow_Crop::run(void) {

  long nstep = getstep() % Global::Freq;
  long JULIAN;

  if(nstep == 0){ // end of day
    JULIAN = julian("now");
    for(hh = 0; chkStruct(); ++hh) {
      if(JULIAN >= JCrop_Start[hh] && JULIAN < JCrop_Mature[hh] && Crop_Grow_Rate[hh] > 0.0)
        const_cast<double*> (Ht)[hh] =  Ht[hh] + Crop_Grow_Rate[hh];
      else if(JULIAN == JCrop_Harvest[hh])
        const_cast<double*> (Ht)[hh] = Init_Crop_Ht[hh];

      Crop_Ht[hh] = Ht[hh];
    }
  }
}