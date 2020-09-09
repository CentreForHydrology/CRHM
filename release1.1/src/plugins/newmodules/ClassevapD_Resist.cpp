//created by Manishankar Mondal

#include "ClassevapD_Resist.h"
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

ClassevapD_Resist* ClassevapD_Resist::klone(string name) const{
  return new ClassevapD_Resist(name);
}

void ClassevapD_Resist::decl(void) {

  Description = "'Evaluation daily evaporation model using Penman-Monteith and Dalton Bulk transfer.'";

  declvar("hru_actet", NHRU, "actual evapotranspiration over HRU, limited by the amount of soil moisture available", "(mm/int)", &hru_actet);

  declstatdiag("hru_cum_actet", NHRU, "cumulative actual evapotranspiration over HRU", "(mm)", &hru_cum_actet);

  declvar("hru_evapD", NHRU, "daily evaporation", "(mm/d)", &evapD);

  declstatdiag("hru_cum_evap", NHRU, "cumulative interval evaporation", "(mm)", &cum_evap);

  decldiag("rc", NHRU, "stomatal resistance (Penman-Monteith)", "(s/m)", &rc);

  decllocal("Pa", NHRU, "Atmospheric pressure", "(kPa)", &Pa);


  declparam("Ht", NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "vegetation height", "(m)", &Ht);

  decldiagparam("Zwind", NHRU, "[10.0]", "0.0", "100.0", "Wind instrument height", "(m)", &Zwind);

  declparam("evap_type", NHRU, "0", "0", "1", "Evaporation method for this HRU, 0 = Penman-Monteith, 1 = Dalton bulk transfer",
            "()", &evap_type);

  declparam("hru_elev", NHRU, "[637]", "0.0", "100000.0", "altitude", "(m)", &hru_elev);

  declparam("basin_area", BASIN, "1", "1e-6", "1e+09", "total basin area", "(km^2)", &basin_area);

  declparam("hru_area", NHRU, "[1]", "1e-6", "1e+09", "hru area", "(km^2)", &hru_area);

  declparam("inhibit_evap", NHRU, "[0]", "0", "1", "inhibit evaporation, 1 -> inhibit", "()", &inhibit_evap);

  decldiagparam("F_Qg", NHRU, "[0.1]", "0.0", "1.0", "fraction to ground flux, Qg = F_Qg*Rn", "()", &F_Qg);

  declparam("rcs", NHRU, "[25]", "25.0", "5000.0", "stomatal resistance (Penman-Monteith/Dalton bulk transfer)", "(s/m)", &rcs);

  declparam("Htmax", NHRU, "[0.1]", "0.1", "100.0", "maximum vegetation height (Penman-Monteith)", "(m)", &Htmax);

  declparam("LAImax", NHRU, "[3.0]", "0.0", "20.0", "maximum leaf area index (Penman-Monteith)", "(m^2/m^2)", &LAImax);

  declparam("LAImin", NHRU, "[0.0]", "0.0", "20.0", "minimum leaf area index (Penman-Monteith)", "(m^2/m^2)", &LAImin);

  declparam("s", NHRU, "[1]", "0", "1", "seasonal growth index (Penman-Monteith)", "()", &s);

  declparam("PM_method", NHRU, "[0]", "0", "2", "Penman-Monteith method, 0 = RC min, 1 = LAI, 2 = bulk", "()", &PM_method);

  declparam("soil_type", NHRU, "[2]", " 1", "12",
    "HRU soil type (1->11): sand/loamsand/sandloam/loam/siltloam/sasclloam/clayloam/siclloam/sandclay/siltclay/clay", "()", &soil_type);

  declparam("soil_Depth", NHRU, "[1.0]", "0.0", "10.0", "depth of soil column", "(m)", &soil_Depth);

  declgetvar("*",  "RnD", "(mm/m^2*d)", &RnD);
  declgetvar("*",  "hru_tmean", "(°C)", &hru_tmean);
  declgetvar("*",  "hru_umean", "(m/s)", &hru_umean);
  declgetvar("*",  "hru_eamean", "(kPa)", &hru_eamean);

  declgetvar("*",  "soil_moist", "(mm)", &soil_moist);

  declreadobs("Qsi", NHRU, "incident short-wave", "(W/m^2)", &Qsi, HRU_OBS_Q, false);

  declobsfunc("Qsi", "Qsi_mean", &Qsi_mean, AVG, NULL, false);

  declreadobs("Ts", NHRU, "surface temperature", "(°C)", &Ts, HRU_OBS_misc, true);

  declobsfunc("Ts", "Tsmean", &Tsmean, AVG, NULL, true);
}

void ClassevapD_Resist::init(void) {

  nhru = getdim(NHRU);

  for(hh = 0; hh < nhru; ++hh) {

    if(evap_type[hh] == 1 && Ts == NULL){
      CRHMException TExcept("'evapD' Ts observation not available for Dalton bulk transport method!.", TERMINATE);
      LogError(TExcept);
    }

    Pa[hh] = 101.3f*pow((293.0f-0.0065f*hru_elev[hh])/293.0f, 5.26f);  // kPa
    evapD[hh] = 0.0;
    cum_evap[hh] = 0.0;
    hru_cum_actet[hh] = 0.0;

    if(Ht[hh] > Zwind[hh]/0.67 && evap_type[hh] != 0){
      CRHMException TExcept("Vegetation height greater than wind reference height, i.e. (Ht > Zwind/0.67)!", WARNING);
      LogError(TExcept);
    }
  }
}

void ClassevapD_Resist::run(void) {

   float Q, rcstar, LAI, Z0, d, U, f1, f2, f3, f4, p, ra, ratio_rs_ra;

   const float Cp = 1.005; // (kJ/kg/K)

   long nstep = getstep() % Global::Freq;

   for(hh = 0; chkStruct(); ++hh) {

     if(inhibit_evap[hh] && nstep == 1){
       evapD[hh] = 0.0;
       hru_actet[hh] = 0.0;
       continue;
     }

     if(nstep == 1 || Global::Freq == 1) { // beginning of day

       hru_actet[hh] = 0.0;

       Q = RnD[hh]*(1.0 - F_Qg[hh]); // daily value (mm/d)

       float Soil_Moist = (soil_moist[hh]/soil_Depth[hh] + SetSoilproperties[soil_type[hh]][1])/SetSoilproperties[soil_type[hh]][3];

       switch (evap_type[hh]){

         case 0: // Penman-Monteith

           Z0 = Ht[hh]/7.6;
           d  = Ht[hh]*0.67;
           U = max<float>(0.1, hru_umean[hh]);
           ra = sqr(log((Zwind[hh] - d)/Z0))/(sqr(CRHM_constants::kappa)*U);

           rcstar = rcs[hh];

           if(PM_method[hh] == 1){
             float LAI = Ht[hh]/Htmax[hh]*(LAImin[hh] + s[hh]*(LAImax[hh] - LAImin[hh]));
             rcstar = rcs[hh]*LAImax[hh]/LAI;
           }

           f1 = max <float> (1.0, 500.0/(Qsi_mean[hh]) - 1.5);

           f2 = max <float> (1.0, 2.0*(Common::estar(hru_tmean[hh]) - hru_eamean[hh]));
           if(f2 < 0.0) // happens when hru_eamean[hh] > hru_tmean[hh] because of lapse rate adjustment with increased height
             f2 = 0.0;

           p = soilproperties[soil_type[hh]][AIRENT]*
             pow(soilproperties[soil_type[hh]][PORE]/Soil_Moist, soilproperties[soil_type[hh]][PORESZ]); // /100.0

           f3 = max <float> (1.0, p/40.0);

           f4 = 1.0;
           if(hru_tmean[hh] < 0.0 || hru_tmean[hh] > 40.0)
             f4 = 5000/50;

           rc[hh] = rcstar*f1*f2*f3*f4;
           if(rc[hh] > 5000.0)
             rc[hh] = 5000.0;

           ratio_rs_ra = rc[hh]/ra;

           evapD[hh] = (delta(hru_tmean[hh])*Q + (RHOa(hru_tmean[hh], hru_eamean[hh], Pa[hh]) * Cp/(lambda(hru_tmean[hh])*1e3)
                     *(Common::estar(hru_tmean[hh]) - hru_eamean[hh])/(ra/86400)))
                     /(delta(hru_tmean[hh]) + gamma(Pa[hh], hru_tmean[hh])*(1.0 + ratio_rs_ra));
         break;

       case 1: // Dalton Bulk transfer

         Z0 = Ht[hh]/7.6;
         d  = Ht[hh]*0.67;
         U = max<float>(0.1, hru_umean[hh]);
         ra = sqr(log((Zwind[hh] - d)/Z0))/(sqr(CRHM_constants::kappa)*U);

         rcstar = rcs[hh]; // rc min

         f1 = 1.0;
         if(Qsi_mean[hh] > 0.0)
           f1 = max <float> (1.0, 500.0/(Qsi_mean[hh]) - 1.5);

         f2 = max <float> (1.0, 2.0*(Common::estar(hru_tmean[hh]) - hru_eamean[hh]));
         if(f2 < 0.0) // happens when hru_eamean[hh] > hru_tmean[hh] because of lapse rate adjustment with increased height
           f2 = 0.0;

         p = soilproperties[soil_type[hh]][AIRENT]*
           pow(soilproperties[soil_type[hh]][PORE]/Soil_Moist, soilproperties[soil_type[hh]][PORESZ]); // /100.0

         f3 = max <float> (1.0, p/40.0);

         f4 = 1.0;
         if(hru_tmean[hh] < 5.0 || hru_tmean[hh] > 40.0)
           f4 = 5000/50;

         rc[hh] = rcstar*f1*f2*f3*f4;
         if(rc[hh] > 5000.0)
             rc[hh] = 5000.0;

         float qs = 0.622*Common::estar(Tsmean[hh])/(Pa[hh] - Common::estar(Tsmean[hh])*0.378); // Specific humidity (kg/kg)
         float q  = 0.622*hru_eamean[hh]/(Pa[hh] - hru_eamean[hh]*0.378);

         evapD[hh] = RHOa(hru_tmean[hh], hru_eamean[hh], Pa[hh])*(qs - q)/((ra + rc[hh])/86400);
         break;

       } // switch

     cum_evap[hh] += evapD[hh];
     }
   } // for
}

void ClassevapD_Resist::finish(bool good) {

    string Evap_names[] = { "Granger", "Priestley-Taylor", "Penman-Monteith", "Dalton Bulk transfer" };
  for(hh = 0; chkStruct(); ++hh) {

    string s = "**** " + Evap_names[evap_type[hh]+2] + " ****";
    LogMessageA(hh, string("'" + Name + " (evapD_Resist)' hru_cum_evap  (mm) (mm*hru) (mm*hru/basin): ").c_str(), cum_evap[hh],     hru_area[hh], basin_area[0], s.c_str());
    LogMessageA(hh, string("'" + Name + " (evapD_Resist)' hru_cum_actet (mm) (mm*hru) (mm*hru/basin): ").c_str(), hru_cum_actet[hh],     hru_area[hh], basin_area[0]);
    LogDebug(" ");
  }
}

double ClassevapD_Resist::gamma(float Pa, float t) // Psychrometric constant (kPa/°C)
{
   return( 0.00163 * Pa / lambda(t)); // lambda (mJ/(kg °C))
}


float ClassevapD_Resist::lambda(float t) // Latent heat of vaporization (mJ/(kg °C))
{
   return( 2.501 - 0.002361 * t );
}

double ClassevapD_Resist::delta(float t) // Slope of sat vap p vs t, kPa/°C
{
  if (t > 0.0)
    return(2504.0*exp(17.27 * t/(t+237.3)) / sqr(t+237.3));
  else
    return(3549.0*exp( 21.88 * t/(t+265.5)) / sqr(t+265.5));
}

float ClassevapD_Resist::RHOa(float t, float ea, float Pa) // atmospheric density (kg/m^3)
{
  const float R0 = 2870;
   return (1E4*Pa /(R0*( 273.15 + t))*(1.0 - 0.379*(ea/Pa)) ); //
}