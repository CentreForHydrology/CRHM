//created by Manishankar Mondal

#include "Class_z_s_rho.h"

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

Class_z_s_rho* Class_z_s_rho::klone(string name) const{
  return new Class_z_s_rho(name);
}

void Class_z_s_rho::decl(void) {

  Description = "'Required when pbsmSnobal used with glacier and glacier_org. Replaces z_s and rho normally supplied by module SnobalCRHM to pbsmSnobal.'";

  declstatvar("z_s", TDim::NHRU, "total snowcover thickness", "(m)", &z_s);

  declstatvar("rho", TDim::NHRU, "average snowcover density", "(kg/m^3)", &rho);

  declgetvar("*", "SWE", "(mm)",        &SWE);

}

void Class_z_s_rho::init(void) {

  nhru = getdim(TDim::NHRU);

  for (hh = 0; chkStruct(); ++hh){
    z_s[hh] = 0.0;
    rho[hh] = 218.0; // 10/18/16
  }
}

void Class_z_s_rho::run(void) {

  //double SWE_Max; varible is unreferenced commenting out for now - jhs507

  for (hh = 0; chkStruct(); ++hh){
    z_s[hh] = Common::DepthofSnow(SWE[hh]); // returns zero if SWE <= 2.05 mm
    if(z_s[hh] > 0.0)
      rho[hh] = SWE[hh]/z_s[hh];
    else
      rho[hh] = 218.0; // 10/18/16
  }
}