//created by Manishankar Mondal

#include "ClassNeedle.h"
#include "GlobalDll.h"
#include <algorithm>
#include "ClassCRHM.h"

#include "SnobalDefines.h"
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <bitset>

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

using namespace CRHM;

ClassNeedle* ClassNeedle::klone(string name) const{
  return new ClassNeedle(name);
}

void ClassNeedle::decl(void) {

  Description = "'Calculates short, long and all-wave radiation components at the snow surface.' \
                 'Inputs are observations Qsi (W/m^2) and Qli (W/m^2), ' \
                 'Inputs are observation Qsi (W/m^2) and variable QliVt_Var (W/m^2), ' \
                 'Inputs are variable QsiS_Var (W/m^2)(slope) from Annandale and observation Qli (W/m^2), ' \
                 'Inputs are variable QsiS_Var (W/m^2)(slope) from Annandale and variable QliVt_Var (W/m^2), '\
                 'Inputs are variable QsiA_Var (W/m^2)(horizontal) from Annandale and variable QliVt_Var (W/m^2), '";

  variation_set = VARIATION_0 + VARIATION_1;

  declreadobs("Qsi", NHRU, "incident short-wave", "(W/m^2)", &Qsi, HRU_OBS_Q);


  variation_set = VARIATION_0 + VARIATION_2;

  declreadobs("Qli", NHRU, "incident long-wave", "(W/m^2)", &Qli, HRU_OBS_Q);


  variation_set = VARIATION_1 + VARIATION_3 + VARIATION_4;

  declgetvar("*",  "QliVt_Var", "(W/m^2)", &QliVt_Var);


  variation_set = VARIATION_2 + VARIATION_3;

  declgetvar("*",  "QsiS_Var", "(W/m^2)", &QsiS_Var);


  variation_set = VARIATION_4;

  declgetvar("*",  "QsiA_Var", "(W/m^2)", &QsiA_Var);


  variation_set = VARIATION_ORG;

  declobs("Ts", NHRU, "snow surface temperature", "(°C)", &Ts);

  declobs("Qnsn", NHRU, "net all-wave at snow surface", "(W/m^2)", &Qnsn);

  declobs("Qsisn", NHRU, "incident short-wave at surface", "(W/m^2)", &Qsisn);

  declobs("Qlisn", NHRU, "incident long-wave at surface", "(W/m^2)", &Qlisn);

  declobs("Qlosn", NHRU, "reflected long-wave at surface", "(W/m^2)", &Qlosn);


  decldiag("k", NHRU, "extinction coefficient", "()", &k);

  decldiag("Tauc", NHRU, "short-wave transmissivity", "(W/m^2)", &Tauc);

  decllocal("Pa", NHRU, "Average surface pressure", "(kPa)", &Pa);

  declvar("ra", NHRU, "", "(s/m)", &ra);

  declvar("Qnsn_Var", NHRU, "net all-wave at snow surface", "(W/m^2*int)", &Qnsn_Var);


  declparam("hru_elev", NHRU, "[637]", "0.0", "100000.0", "altitude", "(m)", &hru_elev);

  declparam("LAI", NHRU, "[2.2]", "0.0", "20.0", "Leaf area index", "(m^2/m^2)", &LAI);


  declparam("Ht", NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "Canopy height(m)", "(m)", &Ht);

  declparam("Z0snow", NHRU, "[0.01]", "0.0001", "0.01", "snow roughness length", "(m)", &Z0snow);

  declparam("Zref", NHRU, "[1.5]", "0.01", "100.0", "temperature measurement height", "(m)", &Zref);

  declparam("Zwind", NHRU, "[10]", "0.01", "100.0", "wind measurement height", "(m)", &Zwind);


  declgetvar("*",  "hru_t", "(°C)", &hru_t);

  declgetvar("*",  "hru_u", "(m/s)", &hru_u);

  declgetvar("*",  "hru_ea", "(kPa)", &hru_ea);

  declgetvar("*",  "hru_rh", "()", &hru_rh);

  declgetvar("*", "SolAng", "(r)", &beta);

  declgetvar("*", "SWE", "(mm)", &SWE);

  declgetvar("*", "Albedo", "()", &Albedo);
}

void ClassNeedle::init(void) {

  nhru = getdim(NHRU); // transfers current # of HRU's to module

  for (hh = 0; hh < nhru; ++hh) {
    Pa[hh] = 101.3f*pow((293.0f-0.0065f*hru_elev[hh])/293.0f, 5.26f);  // kPa

    if(Ht[hh] > Zwind[hh]){
      CRHMException TExcept("Vegetation height greater than Intrument reference height or wind reference height, i.e. (Ht > Zref or Ht > Zwind)!", WARNING);
      LogError(TExcept);
    }
  }
}

void ClassNeedle::run(void) { // executed every interval

  for (hh = 0; chkStruct(); ++hh) {

    switch (variation){
      case VARIATION_ORG:
      Qsi_ =  Qsi[hh];
      Qli_ =  Qli[hh];
      break;
      case VARIATION_1:
      Qsi_ =  Qsi[hh];
      Qli_ =  QliVt_Var[hh];
      break;
      case VARIATION_2:
      Qsi_ =  QsiS_Var[hh];
      Qli_ =  Qli[hh];
      break;
      case VARIATION_3:
      Qsi_ =  QsiS_Var[hh];
      Qli_ =  QliVt_Var[hh];
      break;
      case VARIATION_4:
      Qsi_ =  QsiA_Var[hh];
      Qli_ =  QliVt_Var[hh];
      break;
    } // switch

    if(SWE[hh] <= 0.0) { // Do nothing!
      Ts[hh] = 0.0;
      Qnsn[hh] = 0.0;
      Qnsn_Var[hh] = 0.0;

      Qsisn[hh] = 0.0;
      Qlisn[hh] = 0.0;
      Qlosn[hh] = 0.0;

      continue;
    }

    float Exposure = Ht[hh] - Common::DepthofSnow(SWE[hh]); /* depths(m) SWE(mm) */
    if(Exposure < 0.0) Exposure = 0.0;

    float LAI_ = LAI[hh]*Exposure/Ht[hh];

    float Vf = 0.45 - 0.29*log(LAI[hh]);

    float Vf_ = Vf + (1.0 - Vf)*sin((Ht[hh] - Exposure)/Ht[hh]*M_PI_2);

    float T1 = hru_t[hh] + CRHM_constants::Tm;

    if(beta[hh] > 0.001) {
      k[hh] = 1.081*beta[hh]*cos(beta[hh])/sin(beta[hh]);

      Tauc[hh] = exp(-k[hh]*LAI_);
    }
    else {
      k[hh] = 0.0;
      Tauc[hh] = 0.0;
    }

    if(SWE[hh] <= 0.0) { // Do nothing!
      Ts[hh] = 0.0;
      continue;
    }

    Qlisn[hh] = Qli_*Vf_ + (1.0f - Vf_)*CRHM_constants::emiss_c*CRHM_constants::sbc*pow(T1, 4.0f);

    Qsisn[hh] = Qsi_*Tauc[hh];


    float rho = Pa[hh]*1000/(CRHM_constants::Rgas*T1);

    float U1 = hru_u[hh]; // Wind speed (m/s)

    ra[hh] = (log(Zref[hh]/Z0snow[hh])*log(Zwind[hh]/Z0snow[hh]))/sqr(CRHM_constants::kappa)/U1;

    float delta = 0.622*CRHM_constants::Ls*Common::Qs(Pa[hh], T1)/(CRHM_constants::Rgas*sqr(T1));

    float q = (hru_rh[hh]/100)*Common::Qs(Pa[hh], T1); // specific humidity (kg/kg)


    Ts[hh] = T1 + (CRHM_constants::emiss*(Qli_ - CRHM_constants::sbc*pow(T1, 4.0f)) + CRHM_constants::Ls*(q - Common::Qs(Pa[hh], T1))*rho/ra[hh])/
             (4*CRHM_constants::emiss*CRHM_constants::sbc*pow(T1, 3.0f) + (CRHM_constants::Cp + CRHM_constants::Ls*delta)*rho/ra[hh]);

    Ts[hh] -= CRHM_constants::Tm;

    if(Ts[hh] > 0.0) Ts[hh] = 0.0;

    Qlosn[hh] = CRHM_constants::emiss*CRHM_constants::sbc*pow(Ts[hh] + CRHM_constants::Tm, 4.0f);

// Canopy temperature is approximated by the air temperature.

    Qnsn[hh] = Qlisn[hh] - Qlosn[hh] + Qsisn[hh]*(1-Albedo[hh]);
    Qnsn_Var[hh] = Qnsn[hh];
  }
}