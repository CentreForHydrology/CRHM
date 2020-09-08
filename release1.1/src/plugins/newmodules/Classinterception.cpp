//created by Manishankar Mondal

#include "Classinterception.h"
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


Classinterception* Classinterception::klone(string name) const{
  return new Classinterception(name);
}

void Classinterception::decl(void) {

  Description = "'Calculates canopy sublimation and unloading.'";

  declvar("SI_Lo", NHRU, "Canopy load", "(kg/m^2)", &SI_Lo);

  declvar("net_snow", NHRU, "Canopy unload ", "(kg/m^2)", &net_snow);

  decldiag("Cum_net_snow", NHRU, "Cumulative Canopy unload ", "(kg/m^2)", &Cum_net_snow);

  declvar("SI_Subl", NHRU, "Canopy sublimation", "(kg/m^2*int)", &SI_Subl);

  decldiag("Cum_Subl", NHRU, "Cumulative Canopy sublimation", "(kg/m^2)", &Cum_Subl);


  declvar("net_rain", NHRU, "hru_rain minus interception", "(mm/int)", &net_rain);

  decldiag("v", NHRU, "ventilation velocity", "(m/s)", &v);


  declparam("Sbar", NHRU, "[6.6]", "0.0", "100.0", "Maximum snow load", "(kg/m^2)", &Sbar);

  declparam("LAI", NHRU, "[2.2]", "0.0", "10.0", "Leaf are index", "(m^2/m^2)", &LAI);

  decldiagparam("k", NHRU, "[0.0113]", "0.0", "1.0", "Snow shape coefficient", "()", &k);

  declparam("Ht", NHRU, "[20]", "0.0", "100.0", "Canopy height", "(m)", &Ht);

  decldiagparam("WidthJ", NHRU, "[75]", "0.0", "100.0", "Canopy ", "(m)", &WidthJ);

  decldiagparam("velW", NHRU, "[0.75]", "0.0", "1.0", "Canopy wind coefficient", "(m)", &velw);

  declparam("basin_area", BASIN, "3", "1e-6", "1e+09", "total basin area", "(km^2)", &basin_area);

  declparam("hru_area", NHRU, "[1]", "1e-6", "1e+09", "hru area", "(km^2)", &hru_area);


  declreadobs("Qsi", NHRU, "incident short-wave", "(W/m^2)", &Qsi, HRU_OBS_Q);

  declgetvar("*", "hru_t", "(°C)", &hru_t);
  declgetvar("*", "hru_snow", "(mm/int)", &hru_snow);
  declgetvar("*", "hru_rain", "(mm/int)", &hru_rain);
  declgetvar("*", "hru_rh", "()", &hru_rh);
  declgetvar("*", "hru_u", "(m/s)", &hru_u);
}

void Classinterception::init(void) {

  nobs = getdim(NOBS);
  nhru = getdim(NHRU);

  for(hh = 0; hh < nhru; ++hh) {
    SI_Lo[hh] = 0.0;
    net_snow[hh] = 0.0;
    net_rain[hh] = 0.0;
    SI_Subl[hh] = 0.0;
    Cum_Subl[hh] = 0.0;
    Cum_net_snow[hh] = 0.0;
  }
}

void Classinterception::run(void) {

const float Radius  = 0.0005;     /* Ice sphere radius, metres */
const float R = 8313.0;          /* Universal gas constant, J/(mole*K) */
const float M = 18.01;           /* Molecular weight of water */
const float RhoI = 900.0;        /* Density of ice, kg/m^3 */
//const float k1 = 0.0114;         /* Snow shape coefficient, Jackpine site */
const float Fract = 0.4;         /* Fractal dimension */
//const float SnowAlb = 0.8;       /* Albedo for snow */
//const float CanAlb = 0.2;        /* Albedo for canopy */
const float Hs = 2838000.0;      // Latent heat of sublimation, J/kg

const float Cc = 0.82;
const float Viscosity = 1.88E-5;
const float Albedo = 0.8; // particle albedo

  float RhoSat, RhoT, Nr, Nu, Sstar, A, B, J, D, Vs, Ce;
  float I1, Unld, Istar, Cp, RhoS;

  for(hh = 0; chkStruct(); ++hh) {

    net_rain[hh] = hru_rain[hh];

    RhoS = 67.92 + 51.25*exp(hru_t[hh]/2.59);
    Istar = Sbar[hh]*(0.27 + 46.0/RhoS)*LAI[hh];

    net_snow[hh] = 0.0;

    if (SI_Lo[hh] > Istar) { // after increase in temperature
      net_snow[hh] = SI_Lo[hh] - Istar;
      Cum_net_snow[hh] += net_snow[hh];
      SI_Lo[hh] = Istar;
    }

    if (hru_snow[hh] > 0.0) { // add new snow
      Cp = Cc/(1.0 - (Cc*hru_u[hh]*Ht[hh])/(velw[hh]*WidthJ[hh]));
      if(Cp <= 0.0 || Cp > 1.0) Cp = 1.0;

      I1 = (Istar-SI_Lo[hh])*(1 - exp(-Cp*hru_snow[hh]/Istar));

      SI_Lo[hh] = SI_Lo[hh] + I1; // add new snowfall

      net_snow[hh] += hru_snow[hh] - I1;
      Cum_net_snow[hh] += hru_snow[hh] - I1;
    }
    else if (SI_Lo[hh] <= 0.0){
      SI_Lo[hh] = 0.0;
      continue; // exit if no snow in canopy
    }

// calculate sublimation
    RhoSat = 1.324*exp(22.452*hru_t[hh]/(hru_t[hh] + 273.0))/(hru_t[hh] + 273.0);  // saturation water vapour density
    RhoT = 0.00063*(hru_t[hh]+273.0) + 0.0673; // thermal conductivity of atmosphere


    float n = 2.43*Cc + 2.97*(1.0-Cc);
    float mx = 3.46*Cc + 3.2*(1.0-Cc);
    float Is_CioncoA = n + mx*exp(-Ht[hh]);
    float For_vent = velw[hh]*Ht[hh];
    float Is_CioncoB = For_vent/Ht[hh] - 1.0;
    float Is_CioncoAB = Is_CioncoA*Is_CioncoB;
    v[hh] = hru_u[hh]*exp(Is_CioncoAB); // estimated windspeed z

    Nr = 2*Radius*v[hh]/Viscosity;
    Nu = 1.79 + 0.606*sqrt(Nr);
    Sstar = M_PI*sqr(Radius)*(1.0 - Albedo)*Qsi[hh]; //Solar incident radiation
    A = RhoT*(hru_t[hh]+273)*Nu;
    B = Hs*M/(R*(hru_t[hh]+273.0)) - 1.0;
    J = B/A;
    D = 2.06E-5f*pow((hru_t[hh]+273.0f)/273.0f, 1.75f);   // diffusivity of water vapour
    Vs = (2.0*M_PI*Radius*(1.0 - hru_rh[hh]) + Sstar*J)/
            (Hs*J + (1.0f/(D*RhoSat*Nu)));  // NB reversed (Rh - 1.0)
    Vs = Vs/(4.0f/3.0f*M_PI*pow(Radius, 3.0f)*RhoI);
    Ce = k[hh]*pow(SI_Lo[hh]/Istar, -Fract);

    SI_Subl[hh] = -SI_Lo[hh]*Vs*Ce*3600.0f*24.0f/Global::Freq/1000.0f; // sublimation is positive!  Units???
    Cum_Subl[hh] += SI_Subl[hh];

// take sublimation from canopy snow
    if (SI_Lo[hh] > SI_Subl[hh])
      SI_Lo[hh] = SI_Lo[hh] - SI_Subl[hh];
    else {
      SI_Subl[hh] = SI_Lo[hh];
      SI_Lo[hh] = 0.0;
       continue;
    }

    if (hru_snow[hh] > 0.0) {  // Unload canopy snow
      if (I1 > SI_Subl[hh]) {
        Unld = (I1-SI_Subl[hh])*0.322;
        SI_Lo[hh] = SI_Lo[hh] - Unld;
        net_snow[hh] += Unld;
        Cum_net_snow[hh] += Unld;
      }
    }
  }
}

void Classinterception::finish(bool good) {
  for(hh = 0; chkStruct(); ++hh) {
    LogMessageA(hh, string("'" + Name + " (interception)' Cumulative Canopy Unload      (mm) (mm*hru) (mm*hru/basin): ").c_str(), Cum_net_snow[hh], hru_area[hh], basin_area[0]);
    LogMessageA(hh, string("'" + Name + " (interception)' Cumulative Canopy sublimation (mm) (mm*hru) (mm*hru/basin): ").c_str(), Cum_Subl[hh], hru_area[hh], basin_area[0]);
    LogDebug(" ");
  }
}