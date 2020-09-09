//created by Manishankar Mondal

#include "ClassevapX.h"
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

ClassevapX* ClassevapX::klone(string name) const{
  return new ClassevapX(name);
}

void ClassevapX::decl(void) {

  Description = "'Calculates interval evaporation. Parameter \"evap_type\" selects Granger/Priestley-Taylor/Penman-Monteith.'";

  declvar("hru_actet", NHRU, "actual evapotranspiration over HRU, limited by the amount of soil moisture available", "(mm/int)", &hru_actet);

  declstatdiag("hru_cum_actet", NHRU, "cumulative actual evapotranspiration over HRU", "(mm)", &hru_cum_actet); // , NULL, false, CRHM::DIAGNOSTIC

  declvar("hru_evap", NHRU, "interval evaporation", "(mm/int)", &evap);

  declvar("hru_evapD", NHRU, "daily sum of interval evaporation", "(mm/d)", &evap_D);

  declstatdiag("hru_cum_evap", NHRU, "cumulative interval evaporation", "(mm)", &cum_evap);

  decllocal("Granger_D", NHRU, "daily Granger evaporation", "(mm/d)", &Granger_D);

  decldiag("rc", NHRU, "stomatal resistance (used by Penman-Monteith and Dalton)", "(s/m)", &rc);

  decllocal("Pa", NHRU, "Atmospheric pressure", "(kPa)", &Pa);


  declparam("Ht", NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "vegetation height", "(m)", &Ht);

  declparam("evap_type", NHRU, "0", "0", "2", "Evaporation method for this HRU, 0 = Granger, 1 = Priestley-Taylor, 2 = Penman-Monteith.",
            "()", &evap_type);

  declparam("hru_elev", NHRU, "[637]", "0.0", "100000.0", "altitude", "(m)", &hru_elev);

  declparam("basin_area", BASIN, "3", "1e-6", "1e+09", "total basin area", "(km^2)", &basin_area);

  declparam("hru_area", NHRU, "[1]", "1e-6", "1e+09", "hru area", "(km^2)", &hru_area);

  declparam("inhibit_evap", NHRU, "[0]", "0", "1", "inhibit evaporation, 1 -> inhibit", "()", &inhibit_evap);

  decldiagparam("F_Qg", NHRU, "[0.1]", "0.0", "1.0", "fraction to ground flux, Qg = F_Qg*Rn", "()", &F_Qg);

  declparam("Zwind", NHRU, "[10]", "0.01", "100.0", "wind measurement height (used by Penman-Monteith).", "(m)", &Zwind);


  declparam("rcs", NHRU, "[25]", "25.0", "5000.0", "stomatal resistance (used by Penman-Monteith).", "(s/m)", &rcs);

  declparam("Htmax", NHRU, "[0.1]", "0.1", "100.0", "maximum vegetation height (used by Penman-Monteith).", "(m)", &Htmax);

  declparam("LAImax", NHRU, "[3.0]", "0.0", "20.0", "maximum leaf area index (used by Penman-Monteith).", "(m^2/m^2)", &LAImax);

  declparam("LAImin", NHRU, "[0.0]", "0.0", "20.0", "minimum leaf area index (used by Penman-Monteith).", "(m^2/m^2)", &LAImin);

  declparam("s", NHRU, "[1]", "0", "1", "seasonal growth index (used by Penman-Monteith).", "()", &s);

  declparam("PM_method", NHRU, "[0]", "0", "2", "Penman-Monteith method (used by Penman-Monteith), 0 = RC min, 1 = LAI, 2 = bulk.", "()", &PM_method);

  declparam("soil_type", NHRU, "[2]", " 1", "12",
    "HRU soil type (used by Penman-Monteith) [1->11]: sand/loamsand/sandloam/loam/siltloam/sasclloam/clayloam/siclloam/sandclay/siltclay/clay.", "()", &soil_type);

  declparam("soil_Depth", NHRU, "[1.0]", "0.0", "10.0", "depth of soil column (used by Penman-Monteith).", "(m)", &soil_Depth);

  declgetvar("*",  "soil_moist", "(mm)", &soil_moist);

  declgetvar("*", "Rn", "(mm/m^2*int)", &Rn);
  declgetvar("*", "RnD", "(mm/m^2*d)", &RnD);
  declgetvar("*", "RnD_POS", "(mm/m^2*d)", &RnD_POS);

  declgetvar("*",  "hru_t", "(°C)", &hru_t);
  declgetvar("*",  "hru_u", "(m/s)", &hru_u);
  declgetvar("*",  "hru_ea", "(kPa)", &hru_ea);

  declgetvar("*",  "hru_tmean", "(°C)", &hru_tmean);
  declgetvar("*",  "hru_umean", "(m/s)", &hru_umean);
  declgetvar("*",  "hru_eamean", "kPa", &hru_eamean);

  declreadobs("Qsi", NHRU, "incident short-wave", "(W/m^2)", &Qsi, HRU_OBS_Q, false);

  declreadobs("RnObs", NHRU, "all-wave", "(W/m^2)", &RnObs, HRU_OBS_Q, true);
}

void ClassevapX::init(void) {

  for(hh = 0; chkStruct(); ++hh) {

    Pa[hh] = 101.3f*pow((293.0f-0.0065f*hru_elev[hh])/293.0f, 5.26f);  // kPa
    cum_evap[hh] = 0.0;
    hru_cum_actet[hh] = 0.0;
    Granger_D[hh] = 0.0;
  } // for
}

void ClassevapX::run(void) {

   const float Cp = 1.005; // (kJ/kg/K)

   float Q, Z0, d, U, ra, f1, f2, f3, f4, Soil_Moist, p, rcstar, LAI, D, G;

   long nstep = getstep() % Global::Freq;


   for(hh = 0; chkStruct(); ++hh) {

     evap[hh] = 0.0;
     hru_actet[hh] = 0.0;

     if(inhibit_evap[hh]){
       evap_D[hh] = 0.0;
       Granger_D[hh] = 0.0;
       continue;
     }

     if(nstep == 1 || Global::Freq == 1) { // beginning of every day

       evap_D[hh] = 0.0;
       Granger_D[hh] = 0.0;

       if(evap_type[hh] == 0){ // Granger

           Q = RnD[hh]*(1.0 - F_Qg[hh]); // daily value (mm/d)

           if(Q > 0.0) {
             double Eal = fdaily(hru_umean[hh], Ht[hh])*(Common::estar(hru_tmean[hh]) - hru_eamean[hh]);
             if(Eal > 0.0){
               D = Eal / ( Eal + Q);
               if(D > 1.0)
                 D = 1.0;
             }
             else // happens when hru_eamean[hh] > hru_tmean[hh] because of lapse rate adjustment with increased height
               D = 0.0;

             G = 1.0 / ( 0.793 + 0.2*exp(4.902*D)) + 0.006 * D;
             Granger_D[hh] = (delta(hru_tmean[hh]) * G * Q + gamma(Pa[hh], hru_tmean[hh]) *
               G * Eal)/(delta(hru_tmean[hh]) * G + gamma(Pa[hh], hru_tmean[hh]));
           }


       } // Granger

     } // beginning of every day

// calculated every interval

     Q = Rn[hh]*(1.0 - F_Qg[hh]); // (mm/d)

     switch (evap_type[hh]){

       case 0: // Granger

         if(Q > 0.0 && Granger_D[hh] > 0.0 && RnD_POS[hh] > 0.0)
           evap[hh] = Q/RnD_POS[hh]/(1.0 - F_Qg[hh])*Granger_D[hh];

         break;

       case 1: // Priestley-Taylor

           if(Q > 0.0)
             evap[hh] = 1.26*delta(hru_t[hh])*Q/(delta(hru_t[hh]) + gamma(Pa[hh], hru_t[hh]));

         break;

       case 2: // Penman-Monteith

           if(soil_moist[hh] > 0.0){ // else ignore

             Z0 = Ht[hh]/7.6;
             d  = Ht[hh]*0.67;
             U = hru_u[hh]; // Wind speed (m/d)
             ra = sqr(log((Zwind[hh] - d)/Z0))/(sqr(CRHM_constants::kappa)*U);

             rcstar = rcs[hh]; // rc min

             if(PM_method[hh] == 1){ // LAI
               LAI = Ht[hh]/Htmax[hh]*(LAImin[hh] + s[hh]*(LAImax[hh] - LAImin[hh]));
               rcstar = rcs[hh]*LAImax[hh]/LAI;
             }

             f1 = 1.0;
             if(Qsi[hh] > 0.0)
               f1 = max <float> (1.0, 500.0/(Qsi[hh]) - 1.5);

             f2 = max <float> (1.0, 2.0*(Common::estar(hru_t[hh]) - hru_ea[hh]));

             Soil_Moist = (soil_moist[hh]/soil_Depth[hh] +
               SetSoilproperties[soil_type[hh]][1])/SetSoilproperties[soil_type[hh]][3];

             p = soilproperties[soil_type[hh]][AIRENT]*
               pow(soilproperties[soil_type[hh]][PORE]/Soil_Moist, soilproperties[soil_type[hh]][PORESZ]);

             f3 = max <float> (1.0, p/40.0);

             f4 = 1.0;
             if(hru_t[hh] < 5.0 || hru_t[hh] > 40.0)
               f4 = 5000/50;

             if(RnObs && RnObs[hh] <= 0)
               rc[hh] = 5000.0;
             else if(Qsi[hh] <= 0)
               rc[hh] = 5000.0;
             else{
               rc[hh] = rcstar*f1*f2*f3*f4;
               if(rc[hh] > 5000.0)
                 rc[hh] = 5000.0;
             }

             float ratio_rs_ra = rc[hh]/ra;

             evap[hh] = (delta(hru_t[hh])*Q*Global::Freq + (RHOa(hru_t[hh], hru_ea[hh], Pa[hh])
                     *Cp/(lambda(hru_t[hh])*1e3)*(Common::estar(hru_t[hh]) - hru_ea[hh])/(ra/86400)))
                     /(delta(hru_t[hh]) + gamma(Pa[hh], hru_t[hh])*(1.0 + ratio_rs_ra))/Global::Freq;
         }

         break;

     } // switch

     cum_evap[hh] += evap[hh];
     evap_D[hh] += evap[hh];
   } // for
}

void ClassevapX::finish(bool good) {

  float Allcum_evap = 0.0;
  float Allcum_actet = 0.0;

  string Evap_names[] = { "Granger", "Priestley-Taylor", "Penman-Monteith", "Dalton Bulk transfer" };

  for(hh = 0; chkStruct(); ++hh) {

    string s = "**** " + Evap_names[evap_type[hh]] + " ****";
    LogMessageA(hh, string("'" + Name + " (evapX)' hru_cum_evap (mm) (mm*hru) (mm*hru/basin): ").c_str(), evap[hh], hru_area[hh], basin_area[0], s.c_str());
    LogMessageA(hh, string("'" + Name + " (evapX)' hru_cum_actet (mm) (mm*hru) (mm*hru/basin): ").c_str(), hru_cum_actet[hh], hru_area[hh], basin_area[0]);
    LogDebug(" ");

    Allcum_evap += cum_evap[hh]*hru_area[hh];
    Allcum_actet += hru_cum_actet[hh]*hru_area[hh];
  }

  LogMessage(string("'" + Name + " (evapX)' Allcum_evap  (mm*basin): ").c_str(), Allcum_evap/basin_area[0]);
  LogMessage(string("'" + Name + " (evapX)' Allcum_actet (mm*basin): ").c_str(), Allcum_actet/basin_area[0]);
  LogDebug(" ");

}

double ClassevapX::gamma(float Pa, float t) // Psychrometric constant (kPa/°C)
{
   return( 0.00163 * Pa / lambda(t)); // lambda (mJ/(kg °C))
}

float ClassevapX::lambda(float t) // Latent heat of vaporization (mJ/(kg °C))
{
   return( 2.501 - 0.002361 * t );
}

double ClassevapX::delta(float t) // Slope of sat vap p vs t, kPa/°C
{
  if (t > 0.0)
    return(2504.0*exp(17.27 * t/(t+237.3)) / sqr(t+237.3));
  else
    return(3549.0*exp( 21.88 * t/(t+265.5)) / sqr(t+265.5));
}

double ClassevapX::fdaily(float u, float Ht){ // Drying power f(u) (mm/d/kPa)

   float Z0 = Ht*100.0/7.6;
   float a = 8.19 + 0.22*Z0;
   float b = 1.16 + 0.08*Z0;
   return a + b*u;
}

float ClassevapX::RHOa(float t, float ea, float Pa) // atmospheric density (kg/m^3)
{
  const float R0 = 2870;
   return (1E4*Pa /(R0*( 273.15 + t))*(1.0 - 0.379*(ea/Pa))); //
}