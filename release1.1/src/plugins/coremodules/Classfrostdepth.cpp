//created by Manishankar Mondal

#include "Classfrostdepth.h"
#include "GlobalDll.h"
#include <algorithm>
#include "ClassCRHM.h"

#include "SnobalDefines.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace CRHM;

Classfrostdepth* Classfrostdepth::klone(string name) const{
  return new Classfrostdepth(name);
}

void Classfrostdepth::decl(void) {

  Description = "'Frost penetration (Van Wijk W. R., (1963) Physics of Plant Environment. North-Holland Publishing Company - Amsterdam, pp.166).'";

  declstatvar("frostdepth", NHRU, "frost penetration", "(m)", &frostdepth);

  declstatvar("Findex", NHRU, "freezing index", "(d* °C)", &Findex);

  declstatvar("Tfreeze", NHRU, "duration of freezing period in days", "(d)", &Tfreeze);

  declstatvar("Lacc", NHRU, "cumulative effective latent heat", "(MJ/m^3)", &Lacc);

  declstatvar("Cacc", NHRU, "cumulative effective heat capacity", "(MJ/(m^3*K))", &Cacc);

  declstatvar("Kacc", NHRU, "cumulative effective thermal conductivity", "(W/(m*K))", &Kacc);


  declparam("Ta", NHRU, "[10]", "-10.0", "50.0", "annual air temperature", "(°C)", &Ta);

  declparam("soil_type", NLAY, "[2]", "1", "4",
            "HRU soil type: 1= sand, 2= loam, 3= clay, 4 = organic", "()", &soil_type, &soil_type_lay);

  declparam("por", NLAY, "[0.5]", "0.0", "1.0", "porosity", "(m^3/m^3)", &por, &por_lay);

  declparam("theta", NLAY, "[0.5]", "0.0", "1.0", "degree of saturation", "(m^3/m^3)", &theta, &theta_lay);

  declparam("d", NLAY, "[2.0]", "0.0", "100.0", "layer depth", "(m)", &d, &d_lay);

  declparam("hru_lat", NHRU, "[51.317]", "-90.0", "90.0", "latitude. Negative values for Southern Hemisphere.", "(°)", &hru_lat);


  declgetvar("*", "hru_tmean", "(°C)", &hru_tmean);

  declgetvar("*",   "SWE", "(mm)", &SWE);

  declgetvar("*",   "snowdepth", "(m)", &snowdepth);
}

const float ko = 0.21;  // W/(m K) organic material
const float km = 2.50;  // W/(m K) mineral
const float ka = 0.025; // W/(m K) air
const float ki = 2.24;  // W/(m K) ice
const float kw = 0.57;  // W/(m K) water
const float Cm = 2.000; // MJ/(m3.K) mineral
const float Cw = 4.185; // MJ/(m3.K) water
const float Ca = 0.001; // MJ/(m3.K) air
const float Co = 0.110; // MJ/(m3.K) organic
const float Ci = 1.950; // MJ/(m3.K) ice

void Classfrostdepth::init(void) {

  nlay = getdim(NLAY);
  nhru = getdim(NHRU);

//  Lacc = new float[nhru];
//  Cacc = new float[nhru];
//  Kacc = new float[nhru];

  k_lay = new float*[nlay];   // Array  [nlay][nhru]
  L_lay = new float*[nlay];   // Array  [nlay][nhru]
  c_lay = new float*[nlay];   // Array  [nlay][nhru]

  for (int nn = 0; nn < nlay; ++nn) {
    k_lay[nn] = new float[nhru];
    L_lay[nn] = new float[nhru];
    c_lay[nn] = new float[nhru];
  }

  for(hh = 0; hh < nhru; ++hh) {
    Tfreeze[hh] = 0;
    Findex[hh] = 0.0;
    frostdepth[hh] = 0.0;
    Lacc[hh] = 0.0;
    Cacc[hh] = 0.0;
    Kacc[hh] = 0.0;

    for(long nn = 0; nn < nlay; ++nn) {
      L_lay[nn][hh] = theta_lay[nn][hh]*por_lay[nn][hh]; // water m3/m3
      if(soil_type_lay[nn][hh]!=4) {
        k_lay[nn][hh] = (1.0 - por_lay[nn][hh])*km + L_lay[nn][hh]*kw + (por_lay[nn][hh] - L_lay[nn][hh])*ka;
        c_lay[nn][hh] = (1.0 - por_lay[nn][hh])*Cm + L_lay[nn][hh]*Cw + (por_lay[nn][hh] - L_lay[nn][hh])*Ca;
      }
      else {
        k_lay[nn][hh] = (1.0 - por_lay[nn][hh])*ko + L_lay[nn][hh]*kw + (por_lay[nn][hh] - L_lay[nn][hh])*ka;
        c_lay[nn][hh] = (1.0 - por_lay[nn][hh])*Co + L_lay[nn][hh]*Cw + (por_lay[nn][hh] - L_lay[nn][hh])*Ca;
      }
      L_lay[nn][hh] *= 333.0; // MJ/m3
    }
  }
}

void Classfrostdepth::run(void) {

  const float csnow = 0.25*Ci;       // MJ/(m3.°C)
  const float ksnow = 0.25*ki+0.75*ka; // J/(m.K.s)

  float FrozenD;     // depth frozen of next layer
  long  FrozenL;     // last fully frozen layer
  float dsum;        // soil and snow frozen depth
  float dsoil;       // soil frozen depth

  long nstep = getstep()%Global::Freq;

  if(nstep != 0) return; // not end of day

  long jday = julian("now");
  float hemisphere = (hru_lat[0] < 0.0); // use hru #1
  if((!hemisphere && (jday < 300) || hemisphere && (jday < 117)) && Tfreeze[0] == 0) return; // use hru #1

  for(hh = 0; chkStruct(); ++hh) {

    if(hru_tmean[hh] > 0.0){
      if(hru_tmean[hh] > 20.0) {
        frostdepth[hh] = 0.0;
        Tfreeze[0] = 0;
      }
      continue;
    }
    else {
      float Lastsnowdepth = snowdepth[hh];
//      snowdepth[hh] = SWE[hh]/250.0;
      if(snowdepth[hh] > Lastsnowdepth && Lastsnowdepth > 0.05) {
        Findex[hh] = (Lacc[hh]+Cacc[hh]*Ta[hh]*1.8)/
          (24.0*3600*1E-6*Kacc[hh]/sqr(frostdepth[hh]+snowdepth[hh])-Cacc[hh]/(2.0*Tfreeze[hh]));
      }
      Tfreeze[hh]++;
      Findex[hh] += -hru_tmean[hh]; // /Global::Freq;
    }

    dsum = snowdepth[hh] + 0.001; // avoid divide by zero errors

    Lacc[hh] = 0.0;
    Cacc[hh] = dsum*csnow;
    Kacc[hh] = dsum/ksnow;

    dsoil = 0.0;
    FrozenL = 0;

    while(nlay-1 > FrozenL) {
      if(frostdepth[hh] < dsoil + d_lay[FrozenL][hh]) break;
      dsoil += d_lay[FrozenL][hh];
      dsum+= d_lay[FrozenL][hh];
      Lacc[hh]+= L_lay[FrozenL][hh]*d_lay[FrozenL][hh];
      Cacc[hh]+= c_lay[FrozenL][hh]*d_lay[FrozenL][hh];
      Kacc[hh]+= d_lay[FrozenL][hh]/k_lay[FrozenL][hh];
      FrozenL++;
    }

    FrozenD = frostdepth[hh] - dsoil;

    dsum += FrozenD;
    Lacc[hh] += L_lay[FrozenL][hh]*FrozenD;
    Cacc[hh] += c_lay[FrozenL][hh]*FrozenD;
    Kacc[hh] += FrozenD/k_lay[FrozenL][hh];

    Lacc[hh] = Lacc[hh]/dsum;
    Cacc[hh] = Cacc[hh]/dsum;
    Kacc[hh] = dsum/Kacc[hh];

    frostdepth[hh] = sqrt((24.0*3600*1e-6*Kacc[hh]*Findex[hh])/
                       (Lacc[hh] + Cacc[hh]*(Ta[hh]*1.8+Findex[hh]/(2.0*Tfreeze[hh])))) - snowdepth[hh];

    if(frostdepth[hh] < 0.0) frostdepth[hh] = 0.0;
  }
}

void Classfrostdepth::finish(bool good) {

  if(k_lay != NULL)
    for (int nn = 0; nn < nlay; ++nn) {
        delete[] k_lay[nn];  // Array  [nlay][nhru]
        delete[] L_lay[nn];  // Array  [nlay][nhru]
        delete[] c_lay[nn];  // Array  [nlay][nhru]
    }
  else
    return;

  delete[] k_lay;
  k_lay = NULL;
  delete[] L_lay;
  L_lay = NULL;
  delete[] c_lay;
  c_lay = NULL;
}