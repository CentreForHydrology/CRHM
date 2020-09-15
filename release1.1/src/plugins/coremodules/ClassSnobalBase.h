#ifndef ClassSnobalBaseH
#define ClassSnobalBaseH

//created by Manishankar Mondal

#include "ClassModule.h"
#include "INPUT_REC.h"
#include "TSTEP_REC.h"
#include "PRECIP_REC.h"

class ClassSnobalBase : public ClassModule {

public:

    ClassSnobalBase(string Name, string Version = "undefined", CRHM::LMODULE Lvl = CRHM::PROTO) : ClassModule(Name, Version, Lvl) {};

    // declared variables

    //   snowpack information

    long* layer_count;    // number of layers in snowcover: 0, 1, or 2
    float* z_s;            // total snowcover thickness (m)
    float* z_s_0;          // active layer depth (m)
    float* z_s_l;          // lower layer depth (m)
    float* rho;            // average snowcover density (kg/m^3)
    float* m_s;            // snowcover's specific mass (kg/m^2). Init by init_snow.
    float* m_s_0;          // active layer specific mass (kg/m^2). Init by init_snow.
    float* m_s_l;          // lower layer specific mass (kg/m^2). Init by init_snow.
    float* T_s;            // average snowcover temp (K). Init by init_snow
    float* T_s_0;          // active snow layer temp (K)
    float* T_s_l;          // lower layer temp (C)
    float* cc_s;           // snowcover's cold content (J/m^2). Init by init_snow.
    float* cc_s_0;         // active layer cold content (J/m^2). Init by init_snow.
    float* cc_s_l;         // lower layer cold content (J/m^2). Init by init_snow.
    float* h2o_sat;        // % of liquid H2O saturation (relative water content, i.e., ratio of water in snowcover
//                                 to water that snowcover could hold at saturation)
    float* h2o_vol;        // liquid h2o content as volume ratio: V_water/(V_snow - V_ice) (unitless).init_snow
    float* h2o;            // liquid h2o content as specific mass(kg/m^2)
    float* h2o_max;        // max liquid h2o content as specific mass(kg/m^2)
    float* h2o_total;      // total liquid h2o: includes h2o in snowcover, melt, and rainfall (kg/m^2)

//   energy balance info for current timestep

    float* R_n;            // net allwave radiation (W/m^2)
    float* H;              // sensible heat xfr (W/m^2)
    float* L_v_E;          // latent heat xfr (W/m^2)
    float* G;              // heat xfr by conduction & diffusion from soil to snowcover (W/m^2)
    float* G_0;            // heat xfr by conduction & diffusion from soil or lower layer to active layer (W/m^2)
    float* M;              // advected heat from precip (W/m^2)
    float* delta_Q;        // change in snowcover's energy (W/m^2)
    float* delta_Q_0;      // change in active layer's energy (W/m^2)

//   mass balance vars for current timestep

    float* melt_direct_int;       // specific melt (kg/m^2 or m)
    float* sum;       // specific melt (kg/m^2 or m)
    float* SWE_change;       // interval change in SWE
    float* E_int;	    // mass flux by evap into air from active layer (kg/m^2/s)
    float* E_s_int;	    // mass of evap into air & soil from snowcover (kg/m^2)
    float* snowmelt_int;   // predicted specific runoff (m/sec)
    float* snowmeltD;      // daily predicted specific runoff (m/sec)
    float* snowmeltD_acc;      // daily predicted specific runoff accumulator (m/sec)

//   mass balance vars for variable timestep

    float* melt;        // specific melt (kg/m^2 or m)
    float* E;		 // mass flux by evap into air from active layer (kg/m^2/s)
    float* E_s;	 // mass of evap into air & soil from snowcover (kg/m^2)
    float* ro_predict;  // predicted specific runoff (m/sec)

    float* E_s_0;        // mass of evaporation to air (kg/m^2)
    float* E_s_l;        // mass of evaporation to soil (kg/m^2)
    float* E_l;	  // mass flux by evap/cond to soil (kg/m^2/s)

    float* E_s_0_int;        // mass of evaporation to air (kg/m^2)
    float* E_s_l_int;        // mass of evaporation to soil (kg/m^2)
    float* E_l_int;	  // mass flux by evap/cond to soil (kg/m^2/s)

//   precipitation info adjusted for current run timestep

    float* m_precip;	// specific mass of total precip (kg/m^2)
    float* m_rain;	// specific mass of rain in precip (kg/m^2)
    float* m_snow;	// specific mass in snow in precip (kg/m^2)
    float* m_drift;	// specific mass in snow drift (kg/m^2)
    float* m_subl;	// specific mass in snow sublimation (kg/m^2)
    float* rho_snow;   // density of snowfall (kg/m^3)
    float* T_pp;       // precip temp (K)
    float* z_snow;	// depth of snow in precip (m)

//   precipitation info for the current DATA timestep

    long* precip_now;	// precipitation occur for current timestep?
    float* T_rain;	// rain's temp (K)
    float* T_snow;	// snowfall's temp (K)
    float* h2o_sat_snow; // snowfall's % of liquid H2O saturation

//   local climate-data values for the current run timestep
    float* S_n;      // net solar radiation (W/m^2)
    float* I_lw;     // incoming longwave (thermal) rad (W/m^2)
    float* T_a;      // air temp (K)
    float* e_a;      // vapor pressure (Pa)
    float* u;        // wind speed (m/sec)
    float* T_g;      // soil temp at depth z_g (C)
    float* F_g;      // soil flux at depth z_g (W/m^2)

    long* isothermal; // melting?
    long* snowcover;  // snow on gnd at start of current timestep?
    long* stop_no_snow;       //

//   local variables
    float* P_a;            // air pressure (Pa)
    float* m_precip_cum;   //
    float* m_rain_cum;     //
    float* m_snow_cum;     //
    float* m_drift_cum;    //
    float* m_subl_cum;    //
    float* E_s_cum;        //
    float* cumsnowmelt;    //
    float* melt_direct_cum;       //
    float* Fault;       //

// debug variables
/*    float *Length;
    float **Length_array;
    float *Ustar;
    float **Ustar_array;
    float *e;
    float **e_array;
    float *h;
    float **h_array;
    long *ier;
    long **ier_array;
    long  *ArrayCnt; */

    // declared parameters

    //   measurement heights/depths

    const float* hru_elev;    // HRU elevation
    const float* basin_area;  // [BASIN]
    const float* hru_area;
    const float* KT_sand; // thermal conductivity of wet sand

    const long* relative_hts; // true if measurements heights, z_T and z_u, are relative to snow surface
                              // false if they are absolute heights above the ground
    const float* T_g_or_G_flux;    // soil temp or ground flux option
    const float* z_g;         // depth of soil temp meas (m)
    const float* z_u;         // height of wind measurement (m)
    const float* z_T;         // height of air temp & vapor pressure measurement (m)
    const float* z_0;         // roughness length
    const float* max_z_s_0;   // maximum active layer thickness (m)
    const float* max_h2o_vol; // max liquid h2o content as volume ratio: V_water/(V_snow - V_ice) (unitless)

//    void decl(void);

    void init(void);

    //    void run(void);

    void finish(bool good); // delete local storage used

    void init_snow(void);

    void _calc_layers(void);

    void _layer_mass(void);

    float _cold_content(float	temp, float	mass); // temperature of layer specific mass of layer

    void do_data_tstep(void);

    int _divide_tstep(TSTEP_REC* tstep); // record of timestep to be divided

    int _below_thold(float threshold);	 // current timestep's threshold for a layer's mass

    int _do_tstep(TSTEP_REC* tstep); // timestep's record

    int _e_bal(void);

    void _net_rad(void);

    int _h_le(void);

    float g_soil(
        float	rho,	// snow layer's density (kg/m^3)
        float	tsno,	// snow layer's temperature (K)
        float	tg,	// soil temperature (K)
        float	ds,	// snow layer's thickness (m)
        float	dg,	// dpeth of soil temperature measurement (m)
        float	pa);	// air pressure (Pa)

    float g_snow(
        float	rho1,	// upper snow layer's density (kg/m^3)
        float	rho2,	// lower  "     "        "    (kg/m^3)
        float	ts1,	// upper snow layer's temperature (K)
        float	ts2,	// lower  "     "         "       (K)
        float	ds1,	// upper snow layer's thickness (m)
        float	ds2,	// lower  "     "         "     (m)
        float	pa);	// air pressure (Pa)

    void _advec(void);

    void _mass_bal(void);

    void _time_compact(void);

    void _precip(void);

    void _snowmelt(void);

    void _drift(void);

    void _new_density(void);

    void _adj_snow(float delta_z_s, float delta_m_s); // change in snowcover's depth change is snowcover's mass

    void _evap_cond(void);

    void _h2o_compact(void);

    void _adj_layers(void);

    void _runoff(void);

    float new_tsno(float spm, float t0, float ccon);

    float heat_stor(float cp, float spm, float tdif);

    float sati(float tk);

    float ssxfr(float  k1, float  k2, float  t1, float  t2, float  d1, float  d2);

    float efcon(float k, float t, float p);

    int hle1(float press, float ta, float ts, float za, float ea, float es, float zq, float u, float zu,
        float z0, float& h, float& le, float& e);

    // time step information

    TSTEP_REC** tstep_info; 	// array of info for each timestep [nhru] [4]:
                                  //           0 : data timestep
                                  //           1 : normal run timestep
                                  //           2 : medium  "     "
                                  //           3 : small   "     "

    long* time_step;     // length current timestep (sec)
    long* current_time;  // start time of current time step (sec)

  // climate-data input records

    INPUT_REC* input_rec1;	// input data for start of data timestep [nhru]
    INPUT_REC* input_rec2;	//   "     "   "  end   "   "      "     [nhru]

    INPUT_REC** input_deltas;	// deltas for climate-input parameters
                                  //  over each timestep [nhru] [4]

    PRECIP_REC** precip_info;	// array of precip info adjusted for
                                  //  each timestep [nhru] [4]

    int** computed;		// array of flags for each timestep;
                                  //  true if computed values for input
                                  //   deltas and precip arrays [nhru] [4]
};

#endif