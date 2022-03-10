#include "WQ_CRHM.h"
#include "NewModules.h"
#include "GlobalDll.h"
#include <sstream>
#include <cmath>
#include <stdlib.h>
#include "../../core/InstrumentLogger.h"

using namespace std; using namespace CRHM; const double minFlow_WQ = 0.001f;

ClassWQ_Soil * ClassWQ_Soil :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_Soil (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Handles soil moisture throughout the year.' \
                 'Standard version,' \
                 'Version with Culvert limited runoff.' \
                 'Version with Tile drainage calculated ssr.'";
    variation_set = VARIATION_1;
    declvar ("culvert_Q", TDim :: NHRU, "flow in culvert.", "(m^3/s)", & culvert_Q);
    declvar ("culvert_water_H", TDim :: NHRU, "depth of pond at culvert inlet.", "(m)", & culvert_water_H);
    declvar ("culvert_water_A", TDim :: NHRU, "surface area of culvert pond.", "(m^2)", & culvert_water_A);
    declvar ("culvert_water_V", TDim :: NHRU, "volume of water in culvert pond.", "(m^3)", & culvert_water_V);
    declvar ("culvert_over_Q", TDim :: NHRU, "flow over the road.", "(m^3/s)", & culvert_over_Q);
    declvar ("culvert_evap", TDim :: NHRU, "Depth of water evaporating from culvert pond.", "(mm/int)", & culvert_evap);
    declstatdiag ("cum_culvert", TDim :: NHRU, "Cumulative culvert HRU flow.", "(m^3)", & cum_culvert);
    declstatdiag ("cum_culvert_over", TDim :: NHRU, "Cumulative culvert HRU overflow.", "(m^3)", & cum_culvert_over);
    decldiag ("HD", TDim :: NHRU, "ratio of depth of water at culvert/culvert diameter.", "()", & HD);
    declparam ("channel_slope", TDim :: NHRU, "[0.002]", "0.0001", "0.01", "soil slope to culvert.", "()", & channel_slope);
    declparam ("side_slope", TDim :: NHRU, "[0.02]", "0.0001", "0.01", "side soil slope mormal to culvert slope.", "()", & side_slope);
    declparam ("culvert_diam", TDim :: NHRU, "[0.5]", "0.1", "5.0", "culvert diameter.", "(m)", & culvert_diam);
    declparam ("culvert_water_Dmax", TDim :: NHRU, "[2.0]", "0.1", "10.0", "maximum depth of pond at culvert inlet.", "(m)", & culvert_water_Dmax);
    declparam ("number_culverts", TDim :: NHRU, "[1.0]", "0.0", "10.0", "number of culverts and efficiency factor. Zero = no culvert.", "()", & number_culverts);
    declparam ("culvert_type", TDim :: NHRU, "[0]", "0", "4", "0- thin walled projection, 1- square edged (flush) inlet, 2- socket and concrete pipe, 3- 45 degree beveled inlet, 4- well-rounded (streamlined) inlet.", "()", & culvert_type);
    variation_set = VARIATION_2;
    declvar ("tile_flow", TDim :: NHRU, "flow in tile drainage.", "(m^3/s)", & tile_flow);
    declvar ("tile_flow_conc", TDim :: NDEFN, "Concentration: tile flow.", "(mg/l)", & tile_flow_conc, & tile_flow_conc_lay, numsubstances);
    declvar ("Dequiv_depth", TDim :: NHRU, "closed-form expression for equivalent depth.", "(m)", & Dequiv_depth);
    declvar ("x", TDim :: NHRU, ".", "(m)", & x);
    declvar ("Dw", TDim :: NHRU, "steady state depth of the watertable midway between the drain.", "(m)", & Dw);
    declparam ("Ka", TDim :: NHRU, "[1]", "0.1", "4", "hydraulic conductivity of the soil above drain level.", "(m/d)", & Ka);
    declparam ("Kb", TDim :: NHRU, "[1]", "0.1", "4", "hydraulic conductivity of the soil below drain level.", "(m/d)", & Kb);
    declparam ("Di", TDim :: NHRU, "[2]", "0", "10", "depth of the impermeable layer below drain level.", "(m)", & Di);
    declparam ("Dd", TDim :: NHRU, "[0.5]", "0", "4", "depth of drains.", "(m)", & Dd);
    declparam ("soil_poro_moist", TDim :: NHRU, "[0.5]", "0", "1", "soil porosity of moist layer.", "()", & soil_poro_moist);
    declparam ("L", TDim :: NHRU, "[2]", "0", "20", "spacing between drains.", "(m)", & L);
    declparam ("r", TDim :: NHRU, "[0.1]", "0.01", "1", "drain radius.", "(m)", & r);
    variation_set = VARIATION_ORG;
    if (Global :: nlay < 2)
    {
        Global :: nlay = 2;
        Global :: maxlay = 2;
    }

    declvar ("current_getstep", TDim :: BASIN, "current getstep", "()", & current_getstep);
    declvar ("redirected_residual", TDim :: NHRU, "redirected residual after topping up Sd and soil_rechar in Netroute/_D/_M/_M_D.", "(mm*km^2/int)", & redirected_residual);
    declvar ("redirected_residual_conc", TDim :: NDEFN, "Concentration:: redirected residual after topping up Sd and soil_rechar in Netroute/_D/_M/_M_D.", "(mg/l)", & redirected_residual_conc, & redirected_residual_conc_lay, numsubstances);
    declstatdiag ("cum_redirected_residual", TDim :: NHRU, "cumulative HRU redirected_residual to another HRU.", "(mm*km^2)", & cum_redirected_residual);
    declstatdiag ("cum_redirected_residual_mWQ", TDim :: NDEFN, "mass of solute redirected_residual to another HRU.", "(mg/l * mm*km^2/int)", & cum_redirected_residual_mWQ, & cum_redirected_residual_mWQ_lay, numsubstances);
    declstatvar ("Sd", TDim :: NHRU, "Depression storage.", "(mm)", & Sd);
    declstatvar ("Sd_conc", TDim :: NDEFN, "Concentration: Depression storage.", "(mg/l)", & Sd_conc, & Sd_conc_lay, numsubstances);
    declstatvar ("gw", TDim :: NHRU, "ground water storage.", "(mm)", & gw);
    declstatvar ("gw_conc", TDim :: NDEFN, "Concentration: ground water storage.", "(mg/l)", & gw_conc, & gw_conc_lay, numsubstances);
    declvar ("solute_deposit", TDim :: NHRU, "solute deposit left by evaporation.", "(mg)", & solute_deposit);
    declstatvar ("cum_solute_deposit", TDim :: NHRU, "cumulative solute deposit left by evaporation.", "(mg)", & cum_solute_deposit);
    declstatvar ("soil_rechr", TDim :: NHRU, "moisture content of soil recharge zone, ie, the"
        "portion of the soil profile from which evaporation can take place.", "(mm)", & soil_rechr);
    declstatvar ("soil_moist", TDim :: NHRU, "moisture content of soil profile to the depth"
        "of the rooting zone of the major vegetation type on the HRU.", "(mm)", & soil_moist);
    declstatvar ("soil_moist_conc", TDim :: NDEFN, "Concentration: moisture content of soil profile to the depth.", "(mg/l)", & soil_moist_conc, & soil_moist_conc_lay, numsubstances);
    declstatvar ("potential", TDim :: NHRU, ".", "(mm)", & potential);
    declstatvar ("direct_excs", TDim :: NHRU, ".", "(mm)", & direct_excs);
    declstatvar ("potential_mWQ", TDim :: NDEFN, ".", "(mg/l * mm*km^2/int)", & potential_mWQ, & potential_mWQ_lay, numsubstances);
    declstatvar ("direct_excs_mWQ", TDim :: NDEFN, ".", "(mg/l * mm*km^2/int)", & direct_excs_mWQ, & direct_excs_mWQ_lay, numsubstances);
    declstatvar ("soil_lower_max", TDim :: NHRU, "maximum moisture content of lower soil profile to the depth"
        "of the rooting zone of the major vegetation type on the HRU. (N.B. not Hype lower layer)", "(mm)", & soil_lower_max);
    declstatvar ("soil_lower", TDim :: NHRU, "moisture content of lower soil profile to the depth"
        "of the rooting zone of the major vegetation type on the HRU. (N.B. not Hype lower layer)", "(mm)", & soil_lower);
    decllocal ("cum_hru_condense", TDim :: NHRU, "cumulative condensation over HRU.", "(mm)", & cum_hru_condense);
    declvar ("cum_Sd_evap", TDim :: NHRU, "cumulative Sd evaporation(included in hru_actet).", "(mm)", & cum_Sd_evap);
    declstatvar ("cum_Sd_ssr", TDim :: NHRU, "Accumulation of Sd excess from a HRU to ssr.", "(mm)", & cum_Sd_ssr);
    declstatvar ("cum_Sd_gw", TDim :: NHRU, "Accumulation of Sd excess from a HRU to gw.", "(mm)", & cum_Sd_gw);
    declstatvar ("cum_lower_ssr", TDim :: NHRU, "Accumulation of Sd excess from a HRU to ssr.", "(mm)", & cum_lower_ssr);
    declvar ("soil_gw", TDim :: NHRU, "Portion of excess soil water from a HRU that enters groundwater reservoirs.", "(mm/int)", & soil_gw);
    declvar ("soil_gw_conc", TDim :: NDEFN, "Concentration: Portion of excess soil water from a HRU that enters groundwater reservoirs.", "(mg/l)", & soil_gw_conc, & soil_gw_conc_lay, numsubstances);
    declvar ("soil_gw_D", TDim :: NHRU, "Portion of excess soil water from a HRU that enters groundwater reservoirs.", "(mm/d)", & soil_gw_D);
    declvar ("gw_flow", TDim :: NHRU, "Drainage from HRU ground water reservoir.", "(mm/int)", & gw_flow);
    declvar ("gw_flow_conc", TDim :: NDEFN, "Concentration: Drainage from HRU ground water reservoir.", "(mg/l)", & gw_flow_conc, & gw_flow_conc_lay, numsubstances);
    declvar ("gw_flow_D", TDim :: NHRU, "Daily drainage from HRU ground water reservoir.", "(mm/d)", & gw_flow_D);
    declvar ("infil_act", TDim :: NHRU, "Actual amount of water infiltrating the soil on each HRU.", "(mm/int)", & infil_act);
    declvar ("infil_act_conc", TDim :: NDEFN, "Concentration: Actual amount of water infiltrating the soil on each HRU.", "(mm/int)", & infil_act_conc, & infil_act_conc_lay, numsubstances);
    declvar ("cum_infil_act", TDim :: NHRU, "Accumulation of the actual amount of water infiltrating the soil on each HRU.", "(mm)", & cum_infil_act);
    declvar ("cum_infil_act_mWQ", TDim :: NDEFN, "mass of solute infiltrating the soil on each HRU.", "(mg/l * mm*km^2/int)", & cum_infil_act_mWQ, & cum_infil_act_mWQ_lay, numsubstances);
    declvar ("infil_act_D", TDim :: NHRU, "Daily actual amount of water infiltrating the soil on each HRU.", "(mm/d)", & infil_act_D);
    declstatvar ("cum_gw_flow", TDim :: NHRU, "Accumulation of excess soil water from a HRU that enters groundwater reservoirs.", "(mm)", & cum_gw_flow);
    declstatvar ("cum_gw_flow_mWQ", TDim :: NDEFN, "mass of solute from a HRU that enters groundwater reservoirs.", "(mg/l * mm*km^2/int)", & cum_gw_flow_mWQ, & cum_gw_flow_mWQ_lay, numsubstances);
    declvar ("soil_ssr", TDim :: NHRU, "Portion of soil moisture and recharge excess from a HRU that enters subsurface reservoirs.", "(mm/int)", & soil_ssr);
    declvar ("soil_ssr_conc", TDim :: NDEFN, "Concentration: Portion of soil moisture and recharge excess from a HRU that enters subsurface reservoirs.", "(mg/l)", & soil_ssr_conc, & soil_ssr_conc_lay, numsubstances);
    declvar ("rechr_ssr", TDim :: NHRU, "Portion of excess soil water from a HRU that enters subsurface reservoirs.", "(mm/int)", & rechr_ssr);
    declvar ("rechr_ssr_conc", TDim :: NDEFN, "Concentration: Portion of excess soil water from a HRU that enters subsurface reservoirs.", "(mg/l)", & rechr_ssr_conc, & rechr_ssr_conc_lay, numsubstances);
    declvar ("scf", TDim :: NHRU, "Snow-cover fraction calculated from snow-cover depletion curves(Essery&Pomeroy,2004).", "()", & scf);
    declstatvar ("cum_soil_ssr", TDim :: NHRU, "Accumulation of soil moisture from a HRU to ssr.", "(mm)", & cum_soil_ssr);
    declstatvar ("cum_soil_ssr_mWQ", TDim :: NDEFN, "mass of solute from a HRU to ssr.", "(mg/l * mm*km^2/int)", & cum_soil_ssr_mWQ, & cum_soil_ssr_mWQ_lay, numsubstances);
    declstatvar ("cum_rechr_ssr", TDim :: NHRU, "Accumulation of Portion of excess from a HRU to ssr.", "(mm)", & cum_rechr_ssr);
    declstatvar ("cum_rechr_ssr_mWQ", TDim :: NDEFN, "mass of solute portion of excess from a HRU to ssr.", "(mg/l * mm*km^2/int)", & cum_rechr_ssr_mWQ, & cum_rechr_ssr_mWQ_lay, numsubstances);
    declvar ("soil_ssr_D", TDim :: NHRU, "Portion of excess soil water from a HRU that enters subsurface reservoirs.", "(mm/d)", & soil_ssr_D);
    declvar ("soil_runoff", TDim :: NHRU, "Portion of excess soil water from a HRU to runoff.", "(mm/int)", & soil_runoff);
    declvar ("soil_runoff_cWQ", TDim :: NDEFN, "Concentration: Portion of excess soil water from a HRU to runoff.", "()", & soil_runoff_cWQ, & soil_runoff_cWQ_lay, numsubstances);
    declstatvar ("cum_soil_runoff", TDim :: NHRU, "Accumulation of Portion of excess soil water from a HRU to runoff.", "(mm)", & cum_soil_runoff);
    declstatvar ("cum_soil_runoff_mWQ", TDim :: NDEFN, "mass of solute of portion of excess soil water from a HRU to runoff.", "(mg/l * mm*km^2/int)", & cum_soil_runoff_mWQ, & cum_soil_runoff_mWQ_lay, numsubstances);
    declvar ("soil_runoff_D", TDim :: NHRU, "Portion of excess soil water from a HRU that enters groundwater reservoirs.", "(mm/d)", & soil_runoff_D);
    declstatvar ("cum_runoff_to_Sd", TDim :: NHRU, "Cumulative portion of runoff to depression storage.", "(mm/int)", & cum_runoff_to_Sd);
    declstatvar ("cum_runoff_to_Sd_mWQ", TDim :: NDEFN, "mass of solute portion of runoff to depression storage.", "(mg/l * mm*km^2/int)", & cum_runoff_to_Sd_mWQ, & cum_runoff_to_Sd_mWQ_lay, numsubstances);
    declstatvar ("cum_runoff_to_ssr", TDim :: NHRU, "Cumulative portion of runoff to interflow(ssr).", "(mm/int)", & cum_runoff_to_ssr);
    declstatvar ("cum_soil_gw", TDim :: NHRU, "Accumulation of excess soil water from a HRU that enters groundwater reservoirs.", "(mm)", & cum_soil_gw);
    declstatvar ("cum_soil_gw_mWQ", TDim :: NDEFN, "mass of solute of excess soil water from a HRU that enters groundwater reservoirs.", "(mg/l * mm*km^2/int)", & cum_soil_gw_mWQ, & cum_soil_gw_mWQ_lay, numsubstances);
    decllocal ("snowinfil_buf", TDim :: NHRU, "buffer snow infiltration.", "(mm/d)", & snowinfil_buf);
    decllocal ("runoff_buf", TDim :: NHRU, "buffer runoff.", "(mm/d)", & runoff_buf);
    decllocal ("meltrunoff_buf", TDim :: NHRU, "buffer melt runoff.", "(mm/d)", & meltrunoff_buf);
    decllocal ("hru_evap_buf", TDim :: NHRU, "buffer evaporation.", "(mm/d)", & hru_evap_buf);
    decllocal ("soil_rechr_Init", TDim :: NHRU, "initial soil recharge.", "(mm)", & soil_rechr_Init);
    decllocal ("soil_moist_Init", TDim :: NHRU, "initial soil moisture.", "(mm)", & soil_moist_Init);
    decllocal ("soil_bottom_Init", TDim :: NHRU, "initial bottom soil moisture.", "(mm)", & soil_bottom_Init);
    decllocal ("Sd_Init", TDim :: NHRU, "initial Depression storage.", "(mm)", & Sd_Init);
    decllocal ("gw_Init", TDim :: NHRU, "initial ground water storage.", "(mm)", & gw_Init);
    declvar ("soil_moist_conc_Init", TDim :: NDEFN, "initial soil moisture conc.", "(mg/l)", & soil_moist_conc_Init, & soil_moist_conc_Init_lay, numsubstances);
    declvar ("soil_bottom_conc_Init", TDim :: NDEFN, "initial bottom soil moisture conc.", "(mg/l)", & soil_bottom_conc_Init, & soil_bottom_conc_Init_lay, numsubstances);
    declvar ("soil_top_conc_Init", TDim :: NDEFN, "initial top soil moisture conc.", "(mg/l)", & soil_top_conc_Init, & soil_top_conc_Init_lay, numsubstances);
    declvar ("Sd_conc_Init", TDim :: NDEFN, "initial concentration of nutrient species layer in the initial depression storage.", "(mg/l)", & Sd_conc_Init, & Sd_conc_Init_lay, numsubstances);
    declvar ("gw_conc_Init", TDim :: NDEFN, "initial concentration of nutrient species layer  in the groundwater reservoir.", "(mg/l)", & gw_conc_Init, & gw_conc_Init_lay, numsubstances);
    declparam ("calcN", TDim :: NHRU, "[0]", "0", "1", "flag for nitrogen simulation", "()", & calcN);
    declparam ("calcP", TDim :: NHRU, "[0]", "0", "1", "flag for phosphorus simulation", "()", & calcP);
    declparam ("calcC", TDim :: NHRU, "[0]", "0", "1", "flag for carbon simulation", "()", & calcC);
    declparam ("sr_mix_rate", TDim :: NHRU, "[0.01]", "0", "1", "Soil-runoff nutrient mass mixing rate.", "(1/int)", & sr_mix_rate);
    declparam ("parleach", TDim :: NHRU, "[1]", "0", "1", "Leaching from surfsoil to soil_rechr", "(1/T)", & parleach);
    declparam ("CV_SWE", TDim :: NHRU, "[1]", "0", "1", "Coefficient of variation: values can be taken from Gray, D. M., Toth, B., Zhao, L., Pomeroy, J. W., & Granger, R. J. (2001). Estimating areal snowmelt infiltration into frozen soils. Hydrological Processes, 15(16), 30953111. https://doi.org/10.1002/hyp.320", "()", & CV_SWE);
    declparam ("basin_area", TDim :: BASIN, "3", "1e-6", "1e+09", "total basin area.", "(km^2)", & basin_area);
    declparam ("hru_area", TDim :: NHRU, "[1]", "1e-6", "1e+09", "hru area.", "(km^2)", & hru_area);
    declparam ("Sdmax", TDim :: NHRU, "[0]", "0.0", "5000.0", "Maximum depression storage.", "(mm)", & Sdmax);
    declparam ("Sdinit", TDim :: NHRU, "[0]", "0.0", "5000.0", "Initial depression storage.", "(mm)", & Sdinit);
    declparam ("Sd_conc_init", TDim :: NDEFN, "[0]", "0.0", "10.0", "Initial depression storage.", "(mg/l)", & Sd_conc_init, & Sd_conc_init_lay, numsubstances);
    declparam ("soil_rechr_max", TDim :: NHRU, "[60.0]", "0.0", "350.0", "Maximum value for soil recharge zone (upper portion of soil_moist where losses occur as both evaporation "
        "and transpiration).  Must be less than or equal to soil_moist.", "( )", & soil_rechr_max);
    declparam ("soil_rechr_init", TDim :: NHRU, "[30.0]", "0.0", "250.0", "Initial value for soil recharge zone (upper part of "
        "soil_moist).  Must be less than or equal to soil_moist_init.", "(mm)", & soil_rechr_init);
    declparam ("soil_moist_max", TDim :: NHRU, "[375.0]", "0.0", "5000.0", "Maximum available water holding capacity of soil profile."
        "Soil profile is surface to bottom of rooting zone.", "(mm)", & soil_moist_max);
    declparam ("soil_moist_init", TDim :: NHRU, "[187.0]", "0.0", "5000.0", "Initial value of available water in soil profile.", "(mm)", & soil_moist_init);
    declparam ("soil_gw_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "The maximum amount of the soil water excess for an HRU "
        "that is routed directly to the associated groundwater reservoir each day.", "(mm/d)", & soil_gw_K);
    declparam ("gw_max", TDim :: NHRU, "[375.0]", "0.0", "5000.0", "Maximum available water holding capacity of ground water reservoir.", "(mm)", & gw_max);
    declparam ("gw_init", TDim :: NHRU, "[187.0]", "0.0", "5000.0", "Initial value of available water in ground water reservoir.", "(mm)", & gw_init);
    declparam ("gw_conc_init", TDim :: NDEFN, "[1]", "0.0", "1.0", "Initial value of available water in ground water reservoir.", "(mg/l)", & gw_conc_init, & gw_conc_init_lay, numsubstances);
    declparam ("gw_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily ground water drainage from gw reservoir.", "(mm/d)", & gw_K);
    declparam ("rechr_ssr_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily ssr drainage from recharge.", "(mm/d)", & rechr_ssr_K);
    declparam ("lower_ssr_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily ssr drainage from soil column.", "(mm/d)", & lower_ssr_K);
    declparam ("Sd_ssr_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily depression storage ssr drainage factor.", "(mm/d)", & Sd_ssr_K);
    declparam ("Sd_gw_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily depression storage gw drainage.", "(mm/d)", & Sd_gw_K);
    declparam ("cov_type", TDim :: NHRU, "[1]", "0", "2", "Vegetation evaporation type designation for HRU:  "
        "0 = bare soil (no evaporation), 1 = crops (recharge layer), 2 = grasses & shrubs (all soil moisture).", "()", & cov_type);
    declparam ("transp_limited", TDim :: NHRU, "[0]", "0", "1", "limit transpiration to recharge layer only  on-1/off-0.", "()", & transp_limited);
    declparam ("soil_ssr_runoff", TDim :: NHRU, "[1]", "0", "1", "soil column excess to interflow(ssr)/runoff (and possibly Sd)  interflow-0/runoff-1.", "()", & soil_ssr_runoff);
    declparam ("inhibit_evap", TDim :: NHRU, "[0]", "0", "1", "inhibit evapatation, 1 -> inhibit.", "()", & inhibit_evap);
    declparam ("rain_conc", TDim :: NDEFN, "0", "0", "1000", "rain solute concentration", "(mg/l)", & rain_conc, & rain_conc_lay, numsubstances);
    declparam ("atmos_mWQ", TDim :: NDEFN, "0", "0", "3", "atmospheric solute deposit", "(mg/int)", & atmos_mWQ, & atmos_mWQ_lay, numsubstances);
    declparam ("soil_withdrawal", TDim :: NDEFN, "[3]", "1", "4", "Select water withdrawal function for soil type: 1 = sand, 2 = loam, 3 = clay, 4 = organic. soil_withdrawal[1] - rechr layer, soil_withdrawal[2] - lower layer", "()", & soil_withdrawal, & soil_withdrawal_Tables, 2);
    declputvar ("*", "hru_actet", "(mm/int)", & hru_actet);
    declputvar ("*", "hru_cum_actet", "(mm)", & hru_cum_actet);
    evapDiv = declgetvar ("*", "hru_evap", "(mm/int)", & hru_evap);
    declgetvar ("*", "SWE", "(mm)", & SWE);
    declgetvar ("*", "SWE_max", "(mm)", & SWE_max);
    declgetvar ("*", "SWE_conc", "(mg/l)", & SWE_conc, & SWE_conc_lay);
    declputvar ("*", "conc_soil_rechr", "(mg/l)", & conc_soil_rechr, & conc_soil_rechr_lay);
    declputvar ("*", "surfsoil_solub_mWQ", "(mg/l)", & surfsoil_solub_mWQ, & surfsoil_solub_mWQ_lay);
    declputvar ("*", "conc_soil_lower", "(mg/l)", & conc_soil_lower, & conc_soil_lower_lay);
    declstatvar ("conc_soil_moist", TDim :: NDEFN, "concentration in soil_moist: (i_no3n=0) NO3-N, (i_nh4n=1) NH4-1, (i_don=2) DON, "
        "(i_srp=3) SRP, (i_dop=4) DOP, (i_pp=5) PP, (i_oc=6) OC", "(mg/l)", & conc_soil_moist, & conc_soil_moist_lay, numsubstances);
    declgetvar ("*", "infil", "(mm/int)", & infil);
    snowinfilDiv = declgetvar ("*", "snowinfil", "(mm/int)", & snowinfil);
    runoffDiv = declgetvar ("*", "runoff", "(mm/int)", & runoff);
    meltrunoffDiv = declgetvar ("*", "meltrunoff", "(mm/int)", & meltrunoff);
    decllocal ("redirected_residual_0", TDim :: NHRU, "", "", & redirected_residual_0);
    decllocal ("Sd_0", TDim :: NHRU, "Depression storage.", "(mm)", & Sd_0);
    decllocal ("gw_0", TDim :: NHRU, "ground water storage.", "(mm)", & gw_0);
    decllocal ("soil_rechr_0", TDim :: NHRU, "moisture content of soil recharge zone.", "(mm)", & soil_rechr_0);
    decllocal ("soil_moist_0", TDim :: NHRU, "moisture content of soil profile to the depth.", "(mm)", & soil_moist_0);
    decllocal ("soil_lower_0", TDim :: NHRU, "moisture content of soil profile to the depth.", "(mm)", & soil_lower_0);
    decllocal ("gw_flow_0", TDim :: NHRU, "Drainage from HRU ground water reservoir.", "(mm/int)", & gw_flow_0);
    decllocal ("hru_cum_actet_0", TDim :: NHRU, "cumulative saved evaporation.", "(mm*km^2)", & hru_cum_actet_0);
    decllocal ("cum_soil_runoff_0", TDim :: NHRU, "cumulative saved runoff.", "(mm*km^2)", & cum_soil_runoff_0);
    decllocal ("cum_redirected_residual_0", TDim :: NHRU, "cumulative saved redirected_residual to another HRU.", "(mm*km^2)", & cum_redirected_residual_0);
    decllocal ("cum_soil_ssr_0", TDim :: NHRU, "Accumulation of soil moisture from a HRU to ssr.", "(mm)", & cum_soil_ssr_0);
    decllocal ("cum_rechr_ssr_0", TDim :: NHRU, "Accumulation of Portion of excess from a HRU to ssr.", "(mm)", & cum_rechr_ssr_0);
    decllocal ("hru_actet_0", TDim :: NDEFN, "", "(mm/int)", & hru_actet_0);
    decllocal ("cum_hru_condense_0", TDim :: NHRU, "cumulative condensation over HRU.", "(mm)", & cum_hru_condense_0);
    decllocal ("cum_Sd_evap_0", TDim :: NHRU, "cumulative Sd evaporation(included in hru_actet).", "(mm)", & cum_Sd_evap_0);
    decllocal ("cum_Sd_ssr_0", TDim :: NHRU, "Accumulation of Sd excess from a HRU to ssr.", "(mm)", & cum_Sd_ssr_0);
    declstatvar ("cum_Sd_gw_0", TDim :: NHRU, "Accumulation of Sd excess from a HRU to gw.", "(mm)", & cum_Sd_gw_0);
    decllocal ("cum_lower_ssr_0", TDim :: NHRU, "Accumulation of Sd excess from a HRU to ssr.", "(mm)", & cum_lower_ssr_0);
    decllocal ("cum_infil_act_0", TDim :: NHRU, "Accumulation of the actual amount of water infiltrating the soil on each HRU.", "(mm)", & cum_infil_act_0);
    decllocal ("cum_gw_flow_0", TDim :: NHRU, "Accumulation of excess soil water from a HRU that enters groundwater reservoirs.", "(mm)", & cum_gw_flow_0);
    decllocal ("cum_soil_runoff_0", TDim :: NHRU, "Accumulation of Portion of excess soil water from a HRU to runoff.", "(mm)", & cum_soil_runoff_0);
    decllocal ("cum_runoff_to_Sd_0", TDim :: NHRU, "Cumulative portion of runoff to depression storage.", "(mm/int)", & cum_runoff_to_Sd_0);
    decllocal ("cum_runoff_to_ssr_0", TDim :: NHRU, "Cumulative portion of runoff to interflow(ssr).", "(mm/int)", & cum_runoff_to_ssr_0);
    decllocal ("cum_soil_gw_0", TDim :: NHRU, "Accumulation of excess soil water from a HRU that enters groundwater reservoirs.", "(mm)", & cum_soil_gw_0);
    decllocal ("cum_solute_deposit_0", TDim :: NHRU, "cumulative solute deposit left by evaporation.", "(mg)", & cum_solute_deposit_0);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::init(void)@@@WQ_CRHM.cpp>");
    if (Global :: nlay < numsubstances)
    {
        Global :: nlay = numsubstances;
        Global :: maxlay = numsubstances;
    }

    FaultsAllowed = 0;
    nhru = getdim (TDim :: NHRU);
    nlay = getdim (TDim :: NLAY);
    if (snowinfilDiv > 1)
    {
        string S = "Soil:  \"snowinfil\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (evapDiv > 1)
    {
        string S = "Soil:  \"hru_evap\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (meltrunoffDiv > 1)
    {
        string S = "Netroute:  \"meltrunoff\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (runoffDiv > 1)
    {
        string S = "Netroute:  \"runoff\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        for (hh = 0; hh < nhru; ++ hh) {
            if (Sub == 0)
            {
                soil_rechr [hh] = soil_rechr_init [hh];
                soil_moist [hh] = soil_moist_init [hh];
                soil_lower_max [hh] = soil_moist_max [hh] - soil_rechr_max [hh];
                soil_lower [hh] = soil_moist [hh] - soil_rechr [hh];
                soil_moist_conc_lay [Sub] [hh] = 0.0;
                solute_deposit [hh] = 0.0;
                cum_solute_deposit [hh] = 0.0;
                soil_runoff_D [hh] = 0.0;
                soil_ssr_D [hh] = 0.0;
                soil_gw_D [hh] = 0.0;
                gw_flow_D [hh] = 0.0;
                infil_act_D [hh] = 0.0;
                hru_cum_actet [hh] = 0.0;
                cum_hru_condense [hh] = 0.0;
                cum_Sd_evap [hh] = 0.0;
                cum_Sd_ssr [hh] = 0.0;
                cum_Sd_gw [hh] = 0.0;
                cum_lower_ssr [hh] = 0.0;
                cum_runoff_to_ssr [hh] = 0.0;
                if (soil_rechr [hh] > soil_moist [hh])
                {
                    soil_rechr [hh] = soil_moist [hh];
                    string S = string ("'") + Name + " (Soil)' Soil_rechr greater than soil_moist, soil_rechr set to soil_moist, hru = " + to_string (hh).c_str ();
                    CRHMException TExcept (S.c_str (), TExcept :: WARNING);
                    LogError (TExcept);
                    throw TExcept;
                }

                if (soil_rechr_max [hh] > soil_moist_max [hh])
                {
                    string S = string ("'") + Name + " (Soil)' Soil_rechr_max cannot be greater than soil_moist_max in hru = " + to_string (hh + 1).c_str ();
                    CRHMException TExcept (S.c_str (), TExcept :: TERMINATE);
                    LogError (TExcept);
                    throw TExcept;
                }

                if (Sdinit [hh] > Sdmax [hh])
                {
                    string S = string ("'") + Name + " (Soil)' Initial value of depression storage is greater than the maximum value in hru = " + to_string (hh + 1).c_str ();
                    CRHMException Except (S.c_str (), TExcept :: TERMINATE);
                    LogError (Except);
                    throw Except;
                }

                if (variation == VARIATION_1)
                {
                    if (culvert_water_Dmax [hh] / culvert_diam [hh] > 2.5)
                    {
                        string S = "soil: " + string (Name.c_str ()) + " ratio of H/D > 2.5 in HRU " + to_string (hh + 1);
                        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
                        LogError (TExcept);
                    }

                    culvert_water_V [hh] = 0.0;
                    culvert_water_H [hh] = 0.0;
                    culvert_water_A [hh] = 0.0;
                    culvert_over_Q [hh] = 0.0;
                    culvert_Q [hh] = 0.0;
                    culvert_evap [hh] = 0.0;
                    cum_culvert [hh] = 0.0;
                    cum_culvert_over [hh] = 0.0;
                }

                if (variation == VARIATION_2)
                {
                    tile_flow [hh] = 0.0;
                    tile_flow_conc_lay [Sub] [hh] = 0.0;
                    x [hh] = 2.0 * M_PI * (Di [hh] - Dd [hh]) / L [hh];
                    Dequiv_depth [hh] = M_PI * L [hh] / 8.0 * (M_PI * log (L [hh] / (r [hh])) + FunctX (x [hh]));
                    Dw [hh] = soil_moist_init [hh] / soil_poro_moist [hh];
                }

            }

            Reset_WQ (hh, infil_act, infil_act_conc_lay [Sub]);
            Reset_WQ (hh, redirected_residual, redirected_residual_conc_lay [Sub]);
            Reset_WQ (hh, cum_redirected_residual, cum_redirected_residual_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_infil_act, cum_infil_act_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_soil_runoff, cum_soil_runoff_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_soil_ssr, cum_soil_ssr_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_rechr_ssr, cum_rechr_ssr_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_soil_gw, cum_soil_gw_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_gw_flow, cum_gw_flow_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_runoff_to_Sd, cum_runoff_to_Sd_mWQ_lay [Sub]);
            Reset_WQ (hh, cum_runoff_to_Sd, cum_runoff_to_Sd_mWQ_lay [Sub]);
            Set_WQ (hh, Sd, Sd_conc_lay [Sub], Sdinit [hh], Sd_conc_init_lay [Sub] [hh]);
            Set_WQ (hh, gw, gw_conc_lay [Sub], gw_init [hh], gw_conc_init_lay [Sub] [hh]);
        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::run(void)@@@WQ_CRHM.cpp>");
    try {
        double excs, excs_mWQ, condense, et, direct_excs_Saved {0.0}, direct_excs_mWQ_lay_Saved {0.0}, potential_Saved, potential_mWQ_lay_Saved, leaching_mWQ;
        long step = getstep ();
        current_getstep [0] = step;
        long nstep = step % Global :: Freq;
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            if (Sub == 0)
            {
                Save ();
            }

            if (step == 1)
            {
                string S = string ("Initial Substance# ") + to_string (Sub + 1);
                LogDebug (S.c_str ());
                LogMessage (" ");
                for (hh = 0; chkStruct (); ++ hh) {
                    soil_top_conc_Init_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh];
                    soil_bottom_conc_Init_lay [Sub] [hh] = conc_soil_lower_lay [Sub] [hh];
                    if (soil_rechr [hh] + soil_lower [hh] > 0.0)
                    {
                        soil_moist_conc_Init_lay [Sub] [hh] = (soil_rechr [hh] * conc_soil_rechr_lay [Sub] [hh] + soil_lower [hh] * conc_soil_lower_lay [Sub] [hh]) / (soil_rechr [hh] + soil_lower [hh]);
                    }

                    else
                    {
                        soil_moist_conc_Init_lay [Sub] [hh] = 0.0;
                    }

                    soil_moist_Init [hh] = soil_moist [hh];
                    soil_rechr_Init [hh] = soil_rechr [hh];
                    Sd_Init [hh] = Sd [hh];
                    Sd_conc_Init_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh];
                    gw_Init [hh] = gw [hh];
                    gw_conc_Init_lay [Sub] [hh] = gw_conc_lay [Sub] [hh];
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_rechr_init     (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_rechr_init_conc     (mm) (mm*hru) (mm*hru/basin): ").c_str (), conc_soil_rechr_lay [Sub] [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_moist_init     (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_moist_init_conc     (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_moist_conc_Init_lay [Sub] [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_lower_init     (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_lower [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_lower_init_conc     (mm) (mm*hru) (mm*hru/basin): ").c_str (), conc_soil_lower_lay [Sub] [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' Sdinit              (mm) (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' Sd_init_conc             (mm) (mm*hru) (mm*hru/basin): ").c_str (), Sd_conc_lay [Sub] [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' gw_init             (mm) (mm*hru) (mm*hru/basin): ").c_str (), gw [hh], hru_area [hh], basin_area [0]);
                    LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' gw_init_conc             (mm) (mm*hru) (mm*hru/basin): ").c_str (), gw_conc_lay [Sub] [hh], hru_area [hh], basin_area [0]);
                    LogDebug (" ");
                }
            }

            for (hh = 0; chkStruct (); ++ hh) {
                if (Sub != 0)
                {
                    Restore (hh);
                }

                if (snowinfilDiv == 1)
                {
                    snowinfil_buf [hh] = snowinfil [hh];
                }

                if (runoffDiv == 1)
                {
                    runoff_buf [hh] = runoff [hh];
                }

                if (meltrunoffDiv == 1)
                {
                    meltrunoff_buf [hh] = meltrunoff [hh];
                }

                if (evapDiv == 1)
                {
                    hru_evap_buf [hh] = hru_evap [hh];
                }

                if (soil_rechr [hh] + soil_lower [hh] > 0.0)
                {
                    soil_moist_conc_lay [Sub] [hh] = (soil_rechr [hh] * conc_soil_rechr_lay [Sub] [hh] + soil_lower [hh] * conc_soil_lower_lay [Sub] [hh]) / (soil_rechr [hh] + soil_lower [hh]);
                }

                else
                {
                    soil_moist_conc_lay [Sub] [hh] = 0.0;
                }

                soil_runoff_D [hh] = 0.0;
                soil_ssr_D [hh] = 0.0;
                soil_gw_D [hh] = 0.0;
                gw_flow_D [hh] = 0.0;
                infil_act_D [hh] = 0.0;
                leaching_mWQ = 0.0f;
                solute_deposit [hh] = 0.0;
                hru_actet [hh] = 0.0;
                Reset_WQ (hh, soil_gw, soil_gw_conc_lay [Sub]);
                Reset_WQ (hh, soil_ssr, soil_ssr_conc_lay [Sub]);
                Reset_WQ (hh, rechr_ssr, rechr_ssr_conc_lay [Sub]);
                Reset_WQ (hh, infil_act, infil_act_conc_lay [Sub]);
                Reset_WQ (hh, soil_runoff, soil_runoff_cWQ_lay [Sub]);
                if (hru_evap_buf [hh] < 0.0)
                {
                    condense = - hru_evap_buf [hh];
                    cum_hru_condense [hh] += condense;
                    hru_evap_buf [hh] = 0.0;
                }

                else
                {
                    condense = 0.0;
                }

                if (soil_moist_max [hh] > 0.0)
                {
                    potential [hh] = infil [hh] + snowinfil_buf [hh] + condense;
                    if (potential [hh] > 0.0)
                    {
                        potential_mWQ_lay [Sub] [hh] = (infil [hh] + condense) * rain_conc_lay [Sub] [hh] + snowinfil_buf [hh] * SWE_conc_lay [Sub] [hh];
                        if (! inhibit_evap [hh])
                        {
                            potential_mWQ_lay [Sub] [hh] += atmos_mWQ_lay [Sub] [hh];
                        }

                    }

                    else
                    {
                        potential_mWQ_lay [Sub] [hh] = 0.0;
                    }

                    potential_Saved = potential [hh];
                    potential_mWQ_lay_Saved = potential_mWQ_lay [Sub] [hh];
                    if (soil_moist [hh] + potential [hh] > soil_moist_max [hh])
                    {
                        direct_excs [hh] = soil_moist [hh] + potential [hh] - soil_moist_max [hh];
                        direct_excs_Saved = direct_excs [hh];
                        direct_excs_mWQ_lay [Sub] [hh] = potential_mWQ_lay [Sub] [hh] * (potential [hh] - direct_excs [hh]) / potential [hh];
                        direct_excs_mWQ_lay_Saved = direct_excs_mWQ_lay [Sub] [hh];
                        potential_mWQ_lay [Sub] [hh] = std :: fmax (potential_mWQ_lay [Sub] [hh] - direct_excs_mWQ_lay [Sub] [hh], 0.0f);
                        potential [hh] -= direct_excs [hh];
                    }

                    else
                    {
                        direct_excs [hh] = 0.0;
                        direct_excs_Saved = 0.0;
                        direct_excs_mWQ_lay [Sub] [hh] = 0.0;
                        direct_excs_mWQ_lay_Saved = 0.0;
                    }

                    soil_lower [hh] = soil_moist [hh] - soil_rechr [hh];
                    if (soil_rechr [hh] + potential [hh] > soil_rechr_max [hh])
                    {
                        excs = soil_rechr [hh] + potential [hh] - soil_rechr_max [hh];
                        leaching_mWQ = parleach [hh] * (potential [hh] - excs) * surfsoil_solub_mWQ_lay [Sub] [hh];
                        leaching_mWQ = std :: fmax (0.0f, std :: fmin (leaching_mWQ, surfsoil_solub_mWQ_lay [Sub] [hh]));
                        surfsoil_solub_mWQ_lay [Sub] [hh] -= leaching_mWQ;
                        conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + leaching_mWQ + potential_mWQ_lay [Sub] [hh] * ((potential [hh] - excs) / potential [hh]);
                        conc_soil_rechr_lay [Sub] [hh] /= (soil_rechr [hh] + excs);
                        excs_mWQ = conc_soil_rechr_lay [Sub] [hh] * excs;
                        soil_rechr [hh] = soil_rechr_max [hh];
                    }

                    else
                    {
                        excs = 0.0;
                        excs_mWQ = 0.0;
                        leaching_mWQ = parleach [hh] * (potential [hh] - excs) * surfsoil_solub_mWQ_lay [Sub] [hh];
                        leaching_mWQ = std :: fmax (0.0f, std :: fmin (leaching_mWQ, surfsoil_solub_mWQ_lay [Sub] [hh]));
                        surfsoil_solub_mWQ_lay [Sub] [hh] -= leaching_mWQ;
                        if (soil_rechr [hh] > 0.0)
                        {
                            conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + leaching_mWQ + potential_mWQ_lay [Sub] [hh];
                            conc_soil_rechr_lay [Sub] [hh] /= (soil_rechr [hh] + potential [hh]);
                        }

                        else
                        {
                            conc_soil_rechr_lay [Sub] [hh] = 0.0;
                        }

                        soil_rechr [hh] += potential [hh];
                    }

                    if (excs > 0.0)
                    {
                        conc_soil_lower_lay [Sub] [hh] = (conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + excs_mWQ);
                        soil_lower [hh] += excs;
                        conc_soil_lower_lay [Sub] [hh] /= soil_lower [hh];
                        leaching_mWQ = parleach [hh] * (excs) * surfsoil_solub_mWQ_lay [Sub] [hh];
                        leaching_mWQ = std :: fmax (0.0f, std :: fmin (leaching_mWQ, surfsoil_solub_mWQ_lay [Sub] [hh]));
                        surfsoil_solub_mWQ_lay [Sub] [hh] -= leaching_mWQ;
                        soil_moist [hh] = soil_lower [hh] + soil_rechr [hh];
                        soil_moist_conc_lay [Sub] [hh] = (conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + leaching_mWQ + conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh]) / soil_moist [hh];
                        if (soil_moist [hh] > soil_moist_max [hh])
                        {
                            double excs0 = soil_moist [hh] - soil_moist_max [hh];
                            soil_moist_conc_lay [Sub] [hh] = (soil_moist_conc_lay [Sub] [hh] * soil_moist [hh] - excs_mWQ * excs0 / excs);
                            conc_soil_lower_lay [Sub] [hh] = (soil_moist_conc_lay [Sub] [hh] - conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh]);
                            soil_moist [hh] = soil_moist_max [hh];
                            soil_moist_conc_lay [Sub] [hh] /= soil_moist_max [hh];
                            soil_lower [hh] = soil_moist [hh] - soil_rechr [hh];
                            conc_soil_lower_lay [Sub] [hh] /= soil_lower [hh];
                            excs_mWQ = excs_mWQ * excs0 / excs;
                            excs = excs0;
                        }

                        else
                        {
                            soil_lower [hh] = soil_moist [hh] - soil_rechr [hh];
                            conc_soil_lower_lay [Sub] [hh] = (soil_moist_conc_lay [Sub] [hh] * soil_moist [hh] - conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh]) / soil_lower [hh];
                            excs = 0.0;
                            excs_mWQ = 0.0;
                        }

                    }

                    else
                    {
                        soil_moist [hh] = soil_lower [hh] + soil_rechr [hh];
                        soil_moist_conc_lay [Sub] [hh] = (conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh]) / soil_moist [hh];
                    }

                    infil_act [hh] = potential [hh];
                    cum_infil_act [hh] += infil_act [hh];
                    infil_act_D [hh] += infil_act [hh];
                    infil_act_conc_lay [Sub] [hh] = potential_mWQ_lay [Sub] [hh];
                    cum_infil_act_mWQ_lay [Sub] [hh] += infil_act_conc_lay [Sub] [hh];
                    potential [hh] = potential_Saved;
                    potential_mWQ_lay [Sub] [hh] = potential_mWQ_lay_Saved;
                    if (! inhibit_evap [hh])
                    {
                        if (variation == VARIATION_2)
                        {
                            Dw [hh] = soil_moist_init [hh] / soil_poro_moist [hh] / 1000;
                            tile_flow [hh] = 8.0 * Kb [hh] * Dequiv_depth [hh] * (Dd [hh] - Dw [hh]) + 4.0 * Ka [hh] * pow (Dd [hh] - Dw [hh], 2) / pow (L [hh], 2);
                            tile_flow [hh] /= Global :: Freq;
                            if (tile_flow [hh] > 0.0)
                            {
                                double Actual = tile_flow [hh];
                                double from_soil_rechr_to_tile_wq = 0.0f;
                                double from_soil_lower_to_tile_wq = 0.0f;
                                if (Actual > soil_rechr [hh])
                                {
                                    double from_soil_rechr_to_tile_wq = soil_rechr [hh];
                                    soil_rechr [hh] = 0.0f;
                                    if (Actual > soil_moist [hh])
                                    {
                                        tile_flow [hh] = soil_moist [hh];
                                        soil_moist [hh] = 0.0f;
                                        from_soil_lower_to_tile_wq = soil_moist [hh] - soil_rechr [hh];
                                    }

                                    else
                                    {
                                        soil_moist [hh] -= Actual;
                                        from_soil_lower_to_tile_wq = Actual - from_soil_rechr_to_tile_wq;
                                    }

                                }

                                else
                                {
                                    soil_rechr [hh] -= Actual;
                                    soil_moist [hh] -= Actual;
                                    double from_soil_rechr_to_tile = Actual;
                                }

                                ;
                                soil_lower [hh] = soil_moist [hh] - soil_rechr [hh];
                                tile_flow_conc_lay [Sub] [hh] = (from_soil_rechr_to_tile_wq * conc_soil_rechr_lay [Sub] [hh] + from_soil_lower_to_tile_wq * conc_soil_lower_lay [Sub] [hh]);
                                tile_flow_conc_lay [Sub] [hh] /= (from_soil_rechr_to_tile_wq + from_soil_lower_to_tile_wq);
                            }

                            rechr_ssr [hh] = soil_rechr [hh] / soil_rechr_max [hh] * rechr_ssr_K [hh] / Global :: Freq;
                            if (rechr_ssr [hh] > 0.0)
                            {
                                soil_ssr [hh] = rechr_ssr [hh];
                                soil_ssr_conc_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh];
                                soil_rechr [hh] -= rechr_ssr [hh];
                                rechr_ssr_conc_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh];
                                if (soil_rechr [hh] < 0.0)
                                {
                                    soil_moist [hh] -= soil_rechr [hh];
                                    soil_rechr [hh] = 0.0;
                                    conc_soil_rechr_lay [Sub] [hh] = 0.0;
                                }

                                else
                                {
                                    soil_moist [hh] -= rechr_ssr [hh];
                                }

                                soil_lower [hh] = soil_moist [hh] - soil_rechr [hh];
                                soil_moist_conc_lay [Sub] [hh] = (conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + soil_rechr [hh] * conc_soil_rechr_lay [Sub] [hh]) / soil_moist [hh];
                            }

                            cum_rechr_ssr [hh] += rechr_ssr [hh];
                            cum_rechr_ssr_mWQ_lay [Sub] [hh] += rechr_ssr_conc_lay [Sub] [hh] * rechr_ssr [hh];
                        }

                    }

                    double s2gw_k = soil_gw_K [hh] / Global :: Freq;
                    if (s2gw_k > 0)
                    if (direct_excs [hh] >= s2gw_k)
                    {
                        soil_gw [hh] = s2gw_k;
                        soil_gw_conc_lay [Sub] [hh] = direct_excs_mWQ_lay [Sub] [hh] / direct_excs [hh];
                        direct_excs_mWQ_lay [Sub] [hh] -= soil_gw_conc_lay [Sub] [hh] * s2gw_k;
                        direct_excs [hh] -= s2gw_k;
                    }

                    else
                    {
                        soil_gw [hh] = direct_excs [hh];
                        soil_gw_conc_lay [Sub] [hh] = direct_excs_mWQ_lay [Sub] [hh] / direct_excs [hh];
                        direct_excs [hh] = 0.0;
                        direct_excs_mWQ_lay [Sub] [hh] = 0.0;
                    }

                    if (! soil_ssr_runoff [hh] && direct_excs [hh] > 0.0)
                    {
                        soil_ssr_conc_lay [Sub] [hh] = soil_ssr_conc_lay [Sub] [hh] * soil_ssr [hh] + direct_excs_mWQ_lay [Sub] [hh];
                        soil_ssr [hh] += direct_excs [hh];
                        if (soil_ssr [hh] > 0.0)
                        {
                            soil_ssr_conc_lay [Sub] [hh] /= soil_ssr [hh];
                        }

                        else
                        {
                            soil_ssr_conc_lay [Sub] [hh] = 0.0;
                        }

                        direct_excs [hh] = 0.0;
                        direct_excs_mWQ_lay [Sub] [hh] = 0.0;
                    }

                }

                else
                {
                    excs = infil [hh] + snowinfil_buf [hh] + condense;
                    excs_mWQ = (infil [hh] + condense) * rain_conc_lay [Sub] [hh] + snowinfil_buf [hh] * SWE_conc_lay [Sub] [hh];
                    if (! inhibit_evap [hh])
                    {
                        excs_mWQ += atmos_mWQ_lay [Sub] [hh];
                    }

                }

                double runoff_to_Sd = 0.0;
                soil_runoff [hh] = direct_excs [hh] + meltrunoff_buf [hh] + runoff_buf [hh] + redirected_residual [hh] / hru_area [hh];
                if (soil_runoff [hh] > minFlow_WQ)
                {
                    if (SWE [hh] > 0.0 && SWE_max [hh] > 0.0)
                    {
                        scf [hh] = tanh (1.26 * SWE [hh] / (CV_SWE [hh] * SWE_max [hh]));
                    }

                    else
                    {
                        scf [hh] = 0.0;
                    }

                    double amount_surfs = sr_mix_rate [hh] * surfsoil_solub_mWQ_lay [Sub] [hh] * (1.0 - scf [hh]);
                    amount_surfs = std :: fmin (std :: max (amount_surfs, 0.0), surfsoil_solub_mWQ_lay [Sub] [hh]);
                    surfsoil_solub_mWQ_lay [Sub] [hh] -= amount_surfs;
                    soil_runoff_cWQ_lay [Sub] [hh] = (excs_mWQ + direct_excs_mWQ_lay [Sub] [hh] + meltrunoff_buf [hh] * SWE_conc_lay [Sub] [hh] + runoff_buf [hh] * rain_conc_lay [Sub] [hh] + redirected_residual_conc_lay [Sub] [hh] * redirected_residual [hh] / hru_area [hh] + amount_surfs);
                    soil_runoff_cWQ_lay [Sub] [hh] /= soil_runoff [hh];
                }

                else
                {
                    soil_runoff_cWQ_lay [Sub] [hh] = 0.0f;
                }

                direct_excs_mWQ_lay [Sub] [hh] = direct_excs_mWQ_lay_Saved;
                direct_excs [hh] = direct_excs_Saved;
                cum_redirected_residual_mWQ_lay [Sub] [hh] += redirected_residual_conc_lay [Sub] [hh] * redirected_residual [hh] / hru_area [hh];
                cum_redirected_residual [hh] += redirected_residual [hh];
                redirected_residual [hh] = 0;
                redirected_residual_conc_lay [Sub] [hh] = 0.0;
                if (soil_runoff [hh] > 0.0 && Sdmax [hh] > 0.0)
                {
                    double Fix = - 12.0;
                    if (soil_runoff [hh] / Sdmax [hh] < 12.0)
                    {
                        Fix = - soil_runoff [hh] / Sdmax [hh];
                    }

                    double Ds = (Sdmax [hh] - Sd [hh]) * (1 - exp (Fix));
                    if (soil_moist_max [hh] <= 0.0)
                    {
                        Ds = Sdmax [hh] - Sd [hh];
                    }

                    if (Ds > 0.0)
                    {
                        if (soil_runoff [hh] > Ds)
                        {
                            soil_runoff [hh] -= Ds;
                            if (soil_runoff [hh] < 0.0)
                            {
                                soil_runoff [hh] = 0.0;
                                soil_runoff_cWQ_lay [Sub] [hh] = 0.0;
                            }

                            Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + soil_runoff_cWQ_lay [Sub] [hh] * Ds;
                            Sd [hh] += Ds;
                            Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] / Sd [hh];
                            runoff_to_Sd += Ds;
                        }

                        else
                        {
                            Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + soil_runoff_cWQ_lay [Sub] [hh] * soil_runoff [hh];
                            Sd [hh] += soil_runoff [hh];
                            if (Sd [hh] + soil_runoff [hh] > 0.0f)
                            {
                                Sd_conc_lay [Sub] [hh] /= Sd [hh];
                            }

                            else
                            {
                                Sd_conc_lay [Sub] [hh] = 0.0f;
                            }

                            ;
                            runoff_to_Sd += soil_runoff [hh];
                            soil_runoff [hh] = 0.0;
                            soil_runoff_cWQ_lay [Sub] [hh] = 0.0;
                        }

                    }

                }

                if (variation == VARIATION_1)
                {
                    double culvert_C [5] = {0.5, 0.6, 0.7, 0.75, 0.97};
                    culvert_water_H [hh] = 0.0;
                    culvert_water_A [hh] = 0.0;
                    culvert_over_Q [hh] = 0.0;
                    culvert_Q [hh] = 0.0;
                    culvert_evap [hh] = 0.0;
                    if ((soil_runoff [hh] > 0.0 || culvert_water_V [hh] > 0.0) && number_culverts [hh] > 0.0)
                    {
                        culvert_water_V [hh] += soil_runoff [hh] * (hru_area [hh] * 1000.0);
                        soil_runoff [hh] = 0.0;
                        culvert_water_H [hh] = pow (3.0 * culvert_water_V [hh] * channel_slope [hh] * side_slope [hh], 1.0 / 3.0);
                        if (culvert_water_H [hh] > 0.0)
                        {
                            culvert_water_H [hh] = pow (3.0 * culvert_water_V [hh] * channel_slope [hh] * side_slope [hh], 1.0 / 3.0);
                            if (culvert_water_H [hh] > culvert_water_Dmax [hh])
                            {
                                culvert_water_H [hh] = culvert_water_Dmax [hh];
                                double maxVol = pow (culvert_water_Dmax [hh], 3.0) / (3.0 * channel_slope [hh] * side_slope [hh]);
                                culvert_over_Q [hh] = (culvert_water_V [hh] - maxVol) / 86400.0 * Global :: Freq;
                                culvert_water_V [hh] = maxVol;
                                cum_culvert_over [hh] += culvert_over_Q [hh] * 86400.0 / Global :: Freq;
                                soil_runoff [hh] += culvert_over_Q [hh] * 86400.0 / Global :: Freq / (hru_area [hh] * 1000.0);
                            }

                            HD [hh] = culvert_water_H [hh] / culvert_diam [hh];
                            if (HD [hh] <= 0.0)
                            {
                                culvert_Q [hh] = 0.0;
                            }

                            else
                            if (HD [hh] < 1.5)
                            {
                                culvert_Q [hh] = max < double > ((- 0.544443 * pow (HD [hh], 4.0) + 0.221892 * pow (HD [hh], 3.0) + 2.29756 * pow (HD [hh], 2.0) + 0.159413 * HD [hh] + 0.00772254) * culvert_C [culvert_type [hh]] * number_culverts [hh] * pow (culvert_diam [hh], 2.5), 0.0);
                            }

                            else
                            {
                                culvert_Q [hh] = culvert_C [culvert_type [hh]] * number_culverts [hh] * 0.785 * pow (culvert_diam [hh], 2.5) * sqrt (2.0 * 9.81 * (HD [hh] - 0.5));
                            }

                            if (culvert_water_V [hh] > culvert_Q [hh] * 86400.0 / Global :: Freq)
                            {
                                culvert_water_V [hh] -= culvert_Q [hh] * 86400.0 / Global :: Freq;
                            }

                            else
                            {
                                culvert_Q [hh] = culvert_water_V [hh] * Global :: Freq / 86400.0;
                                culvert_water_V [hh] = 0.0;
                            }

                            cum_culvert [hh] += culvert_Q [hh] * 86400.0 / Global :: Freq;
                            soil_runoff [hh] += culvert_Q [hh] * 86400.0 / Global :: Freq / (hru_area [hh] * 1000.0);
                        }

                        culvert_water_A [hh] = sqr (culvert_water_H [hh]) / (channel_slope [hh] * side_slope [hh]);
                    }

                }

                soil_runoff_D [hh] += soil_runoff [hh];
                cum_soil_runoff [hh] += soil_runoff [hh];
                cum_soil_runoff_mWQ_lay [Sub] [hh] += soil_runoff [hh] * soil_runoff_cWQ_lay [Sub] [hh];
                cum_runoff_to_Sd [hh] += runoff_to_Sd;
                cum_runoff_to_Sd_mWQ_lay [Sub] [hh] += runoff_to_Sd * soil_runoff_cWQ_lay [Sub] [hh];
                if (Sd [hh] > 0.0 && Sd_gw_K [hh] > 0.0)
                {
                    double Sd2gw_k = Sd_gw_K [hh] / Global :: Freq;
                    if (Sd2gw_k > Sd [hh])
                    {
                        Sd2gw_k = Sd [hh];
                    }

                    soil_gw_conc_lay [Sub] [hh] = soil_gw_conc_lay [Sub] [hh] * soil_gw [hh] + Sd_conc_lay [Sub] [hh] * Sd2gw_k;
                    soil_gw [hh] += Sd2gw_k;
                    if (soil_gw [hh] > 0.0)
                    {
                        soil_gw_conc_lay [Sub] [hh] /= soil_gw [hh];
                    }

                    else
                    {
                        soil_gw_conc_lay [Sub] [hh] = 0.0;
                    }

                    Sd [hh] -= Sd2gw_k;
                    if (Sd [hh] < 0.0)
                    {
                        Sd [hh] = 0.0;
                        Sd_conc_lay [Sub] [hh] = 0.0;
                    }

                    cum_Sd_gw [hh] += Sd2gw_k;
                }

                soil_gw_D [hh] += soil_gw [hh];
                cum_soil_gw [hh] += soil_gw [hh];
                cum_soil_gw_mWQ_lay [Sub] [hh] += soil_gw [hh] * Sd_conc_lay [Sub] [hh];
                gw_conc_lay [Sub] [hh] = gw [hh] * gw_conc_lay [Sub] [hh] + soil_gw [hh] * soil_gw_conc_lay [Sub] [hh];
                gw [hh] += soil_gw [hh];
                if (gw [hh] > 0.0)
                {
                    gw_conc_lay [Sub] [hh] /= gw [hh];
                }

                else
                {
                    gw_conc_lay [Sub] [hh] = 0.0;
                }

                gw_flow [hh] = 0.0;
                gw_flow_conc_lay [Sub] [hh] = 0.0;
                if (gw [hh] > gw_max [hh])
                {
                    gw_flow_conc_lay [Sub] [hh] = gw_conc_lay [Sub] [hh];
                    gw_flow [hh] += gw [hh] - gw_max [hh];
                    gw [hh] = gw_max [hh];
                }

                if (gw_max [hh] > 0.0 && gw_K [hh])
                {
                    gw_flow_conc_lay [Sub] [hh] = gw_conc_lay [Sub] [hh];
                    double spill = gw [hh] / gw_max [hh] * gw_K [hh] / Global :: Freq;
                    gw [hh] -= spill;
                    gw_flow [hh] += spill;
                }

                gw_flow_D [hh] += gw_flow [hh];
                cum_gw_flow [hh] += gw_flow [hh];
                cum_gw_flow_mWQ_lay [Sub] [hh] += gw_flow [hh] * gw_conc_lay [Sub] [hh];
                if (Sd [hh] > 0.0 && Sd_ssr_K [hh] > 0.0)
                {
                    double Sd2ssr_k = Sd_ssr_K [hh] / Global :: Freq;
                    if (Sd2ssr_k >= Sd [hh])
                    {
                        Sd2ssr_k = Sd [hh];
                    }

                    soil_ssr_conc_lay [Sub] [hh] = soil_ssr_conc_lay [Sub] [hh] * soil_ssr [hh] + Sd_conc_lay [Sub] [hh] * Sd2ssr_k;
                    soil_ssr [hh] += Sd2ssr_k;
                    if (soil_ssr [hh] > 0.0)
                    {
                        soil_ssr_conc_lay [Sub] [hh] /= soil_ssr [hh];
                    }

                    else
                    {
                        soil_ssr_conc_lay [Sub] [hh] = 0.0;
                    }

                    if (Sd [hh] - Sd2ssr_k < 0.0)
                    {
                        Sd [hh] = 0.0;
                        Sd_conc_lay [Sub] [hh] = 0.0;
                    }

                    else
                    {
                        Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] - Sd_conc_lay [Sub] [hh] * Sd2ssr_k;
                        Sd [hh] -= Sd2ssr_k;
                        if (Sd [hh] > 0.0)
                        {
                            Sd_conc_lay [Sub] [hh] /= Sd [hh];
                        }

                        else
                        {
                            Sd_conc_lay [Sub] [hh] = 0.0;
                        }

                    }

                }

                double s2ssr_k = lower_ssr_K [hh] / Global :: Freq;
                if (s2ssr_k > 0.00001)
                {
                    double avail = soil_lower [hh];
                    if (s2ssr_k >= avail)
                    {
                        s2ssr_k = avail;
                    }

                    soil_lower [hh] -= s2ssr_k;
                    soil_ssr_conc_lay [Sub] [hh] = soil_ssr_conc_lay [Sub] [hh] * soil_ssr [hh] + conc_soil_lower_lay [Sub] [hh] * s2ssr_k;
                    soil_ssr [hh] += s2ssr_k;
                    if (soil_ssr [hh] > 0.0)
                    {
                        soil_ssr_conc_lay [Sub] [hh] /= soil_ssr [hh];
                    }

                    else
                    {
                        soil_ssr_conc_lay [Sub] [hh] = 0.0;
                    }

                    soil_moist [hh] = soil_lower [hh] + soil_rechr [hh];
                    soil_moist_conc_lay [Sub] [hh] = (conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + soil_rechr [hh] * conc_soil_rechr_lay [Sub] [hh]) / soil_moist [hh];
                    cum_lower_ssr [hh] += s2ssr_k;
                }

                cum_soil_ssr [hh] += soil_ssr [hh];
                cum_soil_ssr_mWQ_lay [Sub] [hh] += soil_ssr [hh] * soil_moist_conc_lay [Sub] [hh];
                soil_ssr_D [hh] += soil_ssr [hh];
                double culvert_pond = 0.0;
                double culvert_evapL = 0;
                if (variation == VARIATION_1 && culvert_water_V [hh] > 0.0 && hru_evap_buf [hh] > 0.0)
                {
                    culvert_pond = culvert_water_V [hh] / (hru_area [hh] * 1000.0);
                    culvert_evapL = hru_evap_buf [hh] * culvert_water_A [hh] / (hru_area [hh] * 1e6);
                    if (culvert_evapL > culvert_pond)
                    {
                        culvert_evapL = culvert_pond;
                    }

                    culvert_evap [hh] = culvert_evapL;
                    hru_actet [hh] += culvert_evapL;
                    culvert_water_V [hh] = (culvert_pond - culvert_evapL) * (hru_area [hh] * 1000.0);
                }

                double avail_evap = hru_evap_buf [hh] - culvert_evapL;
                if (Sd [hh] + soil_moist [hh] + culvert_pond > 0.0)
                {
                    avail_evap *= (Sd [hh] / (Sd [hh] + soil_moist [hh]));
                }

                else
                {
                    avail_evap = 0.0;
                }

                if (Sd [hh] > 0.0 && avail_evap > 0.0)
                {
                    if (Sd [hh] >= avail_evap)
                    {
                        if (Sd [hh] - avail_evap < 0.0)
                        {
                            double Sdmass_2_soil = Sd_conc_lay [Sub] [hh] * Sd [hh];
                            Sd [hh] -= avail_evap;
                            Sd [hh] = 0.0;
                            Sd_conc_lay [Sub] [hh] = 0.0;
                            Sd_to_soil_mass (Sdmass_2_soil);
                        }

                        else
                        {
                            Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh];
                            Sd [hh] -= avail_evap;
                            if (Sd [hh] > 0.05)
                            {
                                Sd_conc_lay [Sub] [hh] /= Sd [hh];
                            }

                            else
                            {
                                double Sdmass_2_soil = Sd_conc_lay [Sub] [hh] * Sd [hh];
                                Sd_conc_lay [Sub] [hh] = 0.0;
                                Sd_to_soil_mass (Sdmass_2_soil);
                            }

                        }

                    }

                    else
                    {
                        avail_evap = Sd [hh];
                        double Sdmass_2_soil = Sd_conc_lay [Sub] [hh] * avail_evap;
                        Sd_conc_lay [Sub] [hh] = 0.0;
                        Sd [hh] = 0.0;
                        Sd_to_soil_mass (Sdmass_2_soil);
                    }

                    cum_Sd_evap [hh] += avail_evap;
                    hru_actet [hh] += avail_evap;
                }

                else
                {
                    avail_evap = 0.0;
                }

                avail_evap = hru_evap_buf [hh] - avail_evap - culvert_evapL;
                if (avail_evap > 0.0 && soil_moist [hh] > 0.0 && cov_type [hh] > 0)
                {
                    double pctl, pctr, etl, etr;
                    if ((soil_moist_max [hh] - soil_rechr_max [hh]) > 0.0)
                    {
                        pctl = (soil_moist [hh] - soil_rechr [hh]) / (soil_moist_max [hh] - soil_rechr_max [hh]);
                    }

                    else
                    {
                        pctl = 0.0;
                    }

                    pctr = soil_rechr [hh] / soil_rechr_max [hh];
                    etr = avail_evap;
                    switch (soil_withdrawal_Tables [0] [hh]) {
                    case 1:
                        if (pctr < 0.25)
                        {
                            etr = 0.5 * pctr * avail_evap;
                        }

                        break;
                    case 2:
                        if (pctr < 0.5)
                        {
                            etr = pctr * avail_evap;
                        }

                        break;
                    case 3:
                        if (pctr <= 0.33)
                        {
                            etr = 0.5 * pctr * avail_evap;
                        }

                        else
                        if (pctr < 0.67)
                        {
                            etr = pctr * avail_evap;
                        }

                        break;
                    case 4:
                        break;
                    }
                    if (etr > avail_evap)
                    {
                        etl = 0.0;
                        etr = avail_evap;
                    }

                    else
                    {
                        etl = avail_evap - etr;
                    }

                    switch (soil_withdrawal_Tables [1] [hh]) {
                    case 1:
                        if (pctl < 0.25)
                        {
                            etl = 0.5 * pctl * etl;
                        }

                        break;
                    case 2:
                        if (pctl < 0.5)
                        {
                            etl = pctl * etl;
                        }

                        break;
                    case 3:
                        if (pctl <= 0.33)
                        {
                            etl = 0.5 * pctl * etl;
                        }

                        else
                        if (pctr < 0.67)
                        {
                            etl = pctl * etl;
                        }

                        break;
                    case 4:
                        break;
                    }
                    long et_type = cov_type [hh];
                    if (transp_limited [hh] == 1 && et_type == 2)
                    {
                        et_type = 1;
                    }

                    et = 0.0;
                    switch (et_type) {
                    case 0:
                    case - 1:
                        break;
                    case 1:
                        if (etr > soil_rechr [hh])
                        {
                            soil_rechr [hh] = 0.0;
                            et = soil_rechr [hh];
                        }

                        else
                        {
                            soil_rechr [hh] = soil_rechr [hh] - etr;
                            et = etr;
                        }

                        soil_moist [hh] = soil_moist [hh] - et;
                        break;
                    case 2:
                        if (etr + etl >= soil_moist [hh])
                        {
                            et = soil_moist [hh];
                            soil_moist [hh] = 0.0;
                            soil_rechr [hh] = 0.0;
                            soil_lower [hh] = 0.0;
                        }

                        else
                        {
                            et = etr + etl;
                            soil_moist [hh] = soil_moist [hh] - et;
                            if (etr > soil_rechr [hh])
                            {
                                soil_lower [hh] = soil_lower [hh] - (et - soil_rechr [hh]);
                                soil_rechr [hh] = 0.0;
                            }

                            else
                            {
                                soil_rechr [hh] = soil_rechr [hh] - etr;
                                soil_lower [hh] = soil_lower [hh] - etl;
                            }

                        }

                        break;
                    }
                    hru_actet [hh] += et;
                }

                if (soil_moist_max [hh] <= 0.0 && Sdmax [hh] <= 0.0)
                {
                    hru_actet [hh] = hru_evap_buf [hh];
                }

                hru_cum_actet [hh] += hru_actet [hh];
                cum_solute_deposit [hh] += solute_deposit [hh];
                if (soil_moist [hh] > 0.0f)
                {
                    conc_soil_moist_lay [Sub] [hh] = (conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + conc_soil_lower_lay [Sub] [hh] * soil_lower [hh]) / soil_moist [hh];
                }

                else
                {
                    conc_soil_moist_lay [Sub] [hh] = 0.0f;
                }

            }
        }
        for (hh = 0; chkStruct (); ++ hh) {
            if (nstep == 0)
            {
                if (snowinfilDiv > 1)
                {
                    snowinfil_buf [hh] = snowinfil [hh] / snowinfilDiv;
                }

                if (runoffDiv > 1)
                {
                    runoff_buf [hh] = runoff [hh] / runoffDiv;
                }

                if (meltrunoffDiv > 1)
                {
                    meltrunoff_buf [hh] = meltrunoff [hh] / meltrunoffDiv;
                }

                if (evapDiv > 1)
                {
                    hru_evap_buf [hh] = hru_evap [hh] / evapDiv;
                }

            }

        }
    }
    catch (...) {
        string S = "Error" + string (" at ") + StandardConverterUtility :: GetDateTimeInString (Global :: DTnow) + " (" + to_string (getstep ()) + ") in '" + Global :: OurModulesList -> Strings [Global :: CurrentModuleRun] + "'" + " (" + FloatToStrF (Global :: DTnow, TFloatFormat :: ffGeneral, 10, 6) + ") hh = " + FloatToStrF (hh, TFloatFormat :: ffGeneral, 6, 0) + " Sub = " + FloatToStrF (Sub, TFloatFormat :: ffGeneral, 6, 0);
        LogError (S, TExcept :: WARNING);
        std :: cout << "Diogo: Exception occurred in \"ClassWQ_Soil::run\" " << std :: endl;
        if (++ FaultsAllowed == 1)
        {
            throw;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::finish(bool good)@@@WQ_CRHM.cpp>");
    double Allcum_soil_runoff = 0.0;
    double Allcum_soil_ssr = 0.0;
    double Allcum_rechr_ssr = 0.0;
    double Allcum_soil_gw = 0.0;
    double Allcum_gw_flow = 0.0;
    double Allcum_infil_act = 0.0;
    double Allcum_soil_moist_change = 0.0;
    double Allcum_Sd_change = 0.0;
    double Allcum_gw_change = 0.0;
    double Allcum_evap = 0.0;
    double Allcum_solute_deposit = 0.0;
    double Allcum_soil_runoff_mWQ = 0.0;
    double Allcum_soil_ssr_mWQ = 0.0;
    double Allcum_rechr_ssr_mWQ = 0.0;
    double Allcum_soil_gw_mWQ = 0.0;
    double Allcum_gw_flow_mWQ = 0.0;
    double Allcum_infil_act_mWQ = 0.0;
    double Allcum_soil_moist_change_mWQ = 0.0;
    double Allcum_Sd_change_mWQ = 0.0;
    double Allcum_gw_change_mWQ = 0.0;
    double AllTotal = 0.0;
    string S = string ("H2O");
    LogDebug (S.c_str ());
    LogMessage (" ");
    for (hh = 0; chkStruct (); ++ hh) {
        LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_rechr                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'soil_moist'.");
        LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_rechr_change           (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh] - soil_rechr_Init [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'soil_moist'.");
        LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' cum_rechr_ssr               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_rechr_ssr [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_moist                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (Soil_WQ)' soil_moist_change           (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh] - soil_moist_Init [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_lower                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_lower [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_lower_change           (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_lower [hh] - soil_bottom_Init [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' hru_cum_actet               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), hru_cum_actet [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_Sd_evap                 (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_Sd_evap [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'hru_cum_actet'.");
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_hru_condense            (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_hru_condense [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_infil_act(all)          (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_infil_act [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_gw                 (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_gw [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_runoff             (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_runoff [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_rechr_ssr               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_rechr_ssr [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_ssr                (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_ssr [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' Sd                          (mm)   (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' Sd_change                   (mm)   (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh] - Sd_Init [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_runoff_to_Sd            (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_runoff_to_Sd [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_Sd_ssr                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_Sd_ssr [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_lower_ssr               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_lower_ssr [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' gw                          (mm)   (mm*hru) (mm*hru/basin): ").c_str (), gw [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' gw_change                   (mm)   (mm*hru) (mm*hru/basin): ").c_str (), gw [hh] - gw_Init [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_gw_flow                 (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_gw_flow [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_redirected_residual     (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_redirected_residual [hh] / hru_area [hh], hru_area [hh], basin_area [0], " *** Added to this HRU surface runoff");
        LogDebug (" ");
        double Total = cum_soil_runoff [hh] + cum_soil_ssr [hh] + cum_soil_gw [hh] + cum_runoff_to_Sd [hh] + cum_gw_flow [hh] + (soil_moist [hh] - soil_moist_Init [hh]) + (Sd [hh] - Sd_Init [hh]) + (gw [hh] - gw_Init [hh]) + hru_cum_actet [hh] - cum_redirected_residual [hh] / hru_area [hh];
        AllTotal += Total;
        LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' Total                       (mm) (mm*hru) (mm*hru/basin): ").c_str (), Total / hru_area [hh], hru_area [hh], basin_area [0], " *** HRU mass balance");
        LogDebug (" ");
        if (variation == VARIATION_1)
        {
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_culvert      (m3) (m3*hru) (m3*hru/basin): ").c_str (), cum_culvert [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_culvert_over (m3) (m3*hru) (m3*hru/basin): ").c_str (), cum_culvert_over [hh], hru_area [hh], basin_area [0]);
            LogDebug (" ");
        }

        LogDebug (" ");
        Allcum_soil_runoff += cum_soil_runoff [hh] * hru_area [hh];
        Allcum_soil_ssr += cum_soil_ssr [hh] * hru_area [hh];
        Allcum_rechr_ssr += cum_rechr_ssr [hh] * hru_area [hh];
        Allcum_soil_gw += cum_soil_gw [hh] * hru_area [hh];
        Allcum_gw_flow += cum_gw_flow [hh] * hru_area [hh];
        Allcum_infil_act += cum_infil_act [hh] * hru_area [hh];
        Allcum_soil_moist_change += (soil_moist [hh] - soil_moist_Init [hh]) * hru_area [hh];
        Allcum_Sd_change += (Sd [hh] - Sd_Init [hh]) * hru_area [hh];
        Allcum_gw_change += (gw [hh] - gw_Init [hh]) * hru_area [hh];
        Allcum_evap += hru_cum_actet [hh] * hru_area [hh];
    }
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_runoff (mm*basin):           ").c_str (), Allcum_soil_runoff);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_ssr (mm*basin):              ").c_str (), Allcum_soil_ssr);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_rechr_ssr (mm*basin):             ").c_str (), Allcum_rechr_ssr);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_gw (mm*basin):               ").c_str (), Allcum_soil_gw);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_gw_flow (mm*basin):               ").c_str (), Allcum_gw_flow);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_infil_act (mm*basin):             ").c_str (), Allcum_infil_act);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_moist_change (mm*basin):     ").c_str (), Allcum_soil_moist_change);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_Sd_change (mm*basin):             ").c_str (), Allcum_Sd_change);
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_gw_change (mm*basin):             ").c_str (), Allcum_gw_change);
    LogDebug (" ");
    LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_actet (mm*basin):                 ").c_str (), Allcum_evap);
    LogDebug (" ");
    LogMessage (string ("'" + Name + " (WQ_Soil)' AllTotal              (mm*basin):        ").c_str (), AllTotal);
    LogDebug (" ");
    for (long Sub = 0; Sub < 1; ++ Sub) {
        string S = string ("Substance# ") + to_string (Sub + 1);
        LogDebug (S.c_str ());
        LogMessage (" ");
        Allcum_soil_runoff = 0.0;
        Allcum_soil_ssr = 0.0;
        Allcum_rechr_ssr = 0.0;
        Allcum_soil_gw = 0.0;
        Allcum_gw_flow = 0.0;
        Allcum_infil_act = 0.0;
        Allcum_soil_moist_change = 0.0;
        Allcum_Sd_change = 0.0;
        Allcum_gw_change = 0.0;
        Allcum_evap = 0.0;
        Allcum_solute_deposit = 0.0;
        Allcum_soil_runoff_mWQ = 0.0;
        Allcum_soil_ssr_mWQ = 0.0;
        Allcum_rechr_ssr_mWQ = 0.0;
        Allcum_soil_gw_mWQ = 0.0;
        Allcum_gw_flow_mWQ = 0.0;
        Allcum_infil_act_mWQ = 0.0;
        Allcum_soil_moist_change_mWQ = 0.0;
        Allcum_Sd_change_mWQ = 0.0;
        Allcum_gw_change_mWQ = 0.0;
        double AllTotal = 0.0;
        for (hh = 0; chkStruct (); ++ hh) {
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_rechr                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'soil_moist'.");
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_rechr_change           (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh] - soil_rechr_Init [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'soil_moist'.");
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_rechr_conc             (mg/l) (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh] * conc_soil_rechr_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_rechr_ssr               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_rechr_ssr [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_rechr_ssr_mWQ           (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_rechr_ssr_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_moist                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_moist_change           (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh] - soil_moist_Init [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_moist_conc             (mg/l) (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh] * soil_moist_conc_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_lower                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_lower [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' soil_lower_change           (mm)   (mm*hru) (mm*hru/basin): ").c_str (), soil_lower [hh] - soil_bottom_Init [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' hru_cum_actet               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), hru_cum_actet [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_Sd_evap                 (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_Sd_evap [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'hru_cum_actet'.");
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_hru_condense            (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_hru_condense [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_infil_act(all)          (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_infil_act [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_infil_act_mWQ(all)      (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_infil_act_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_gw                 (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_gw [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_gw_mWQ             (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_soil_gw_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_runoff             (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_runoff [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_runoff_mWQ         (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_soil_runoff_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_rechr_ssr               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_rechr_ssr [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_rechr_ssr_mWQ           (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_rechr_ssr_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_ssr                (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_ssr [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_soil_ssr_mWQ            (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_soil_ssr_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' Sd                          (mm)   (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' Sd_change                   (mm)   (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh] - Sd_Init [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' Sd_conc                     (mm)   (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh] * Sd_conc_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_runoff_to_Sd            (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_runoff_to_Sd [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_runoff_to_Sd_mWQ        (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_runoff_to_Sd_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_Sd_ssr                  (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_Sd_ssr [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_lower_ssr               (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_lower_ssr [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' gw                          (mm)   (mm*hru) (mm*hru/basin): ").c_str (), gw [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' gw_change                   (mm)   (mm*hru) (mm*hru/basin): ").c_str (), gw [hh] - gw_Init [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' gw_conc                     (mg/l) (mm*hru) (mm*hru/basin): ").c_str (), gw [hh] * gw_conc_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_gw_flow                 (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_gw_flow [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_gw_flow_mWQ             (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_gw_flow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_redirected_residual     (mm)   (mm*hru) (mm*hru/basin): ").c_str (), cum_redirected_residual [hh] / hru_area [hh], hru_area [hh], basin_area [0], " *** Added to this HRU surface runoff");
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_redirected_residual_mWQ (mg)   (mg*hru) (mg*hru/basin): ").c_str (), cum_redirected_residual_mWQ_lay [Sub] [hh] / hru_area [hh], hru_area [hh], basin_area [0], " *** Added to this HRU surface runoff");
            LogDebug (" ");
            double Total = cum_soil_runoff [hh] + cum_soil_ssr [hh] + cum_soil_gw [hh] + cum_runoff_to_Sd [hh] + cum_gw_flow [hh] + (soil_moist [hh] - soil_moist_Init [hh]) + (Sd [hh] - Sd_Init [hh]) + (gw [hh] - gw_Init [hh]) + hru_cum_actet [hh] - cum_redirected_residual [hh] / hru_area [hh];
            AllTotal += Total;
            LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' Total                       (mm) (mm*hru) (mm*hru/basin): ").c_str (), Total / hru_area [hh], hru_area [hh], basin_area [0], " *** HRU mass balance");
            LogDebug (" ");
            if (variation == VARIATION_1)
            {
                LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_culvert      (m3) (m3*hru) (m3*hru/basin): ").c_str (), cum_culvert [hh], hru_area [hh], basin_area [0]);
                LogMessageA (hh, string ("'" + Name + " (WQ_Soil)' cum_culvert_over (m3) (m3*hru) (m3*hru/basin): ").c_str (), cum_culvert_over [hh], hru_area [hh], basin_area [0]);
                LogDebug (" ");
            }

            LogDebug (" ");
            Allcum_soil_runoff += cum_soil_runoff [hh] * hru_area [hh];
            Allcum_soil_ssr += cum_soil_ssr [hh] * hru_area [hh];
            Allcum_rechr_ssr += cum_rechr_ssr [hh] * hru_area [hh];
            Allcum_soil_gw += cum_soil_gw [hh] * hru_area [hh];
            Allcum_gw_flow += cum_gw_flow [hh] * hru_area [hh];
            Allcum_infil_act += cum_infil_act [hh] * hru_area [hh];
            Allcum_soil_moist_change += (soil_moist [hh] - soil_moist_Init [hh]) * hru_area [hh];
            Allcum_Sd_change += (Sd [hh] - Sd_Init [hh]) * hru_area [hh];
            Allcum_gw_change += (gw [hh] - gw_Init [hh]) * hru_area [hh];
            Allcum_soil_runoff_mWQ += cum_soil_runoff_mWQ_lay [Sub] [hh] * hru_area [hh];
            Allcum_soil_ssr_mWQ += cum_soil_ssr_mWQ_lay [Sub] [hh] * hru_area [hh];
            Allcum_rechr_ssr_mWQ += cum_rechr_ssr_mWQ_lay [Sub] [hh] * hru_area [hh];
            Allcum_soil_gw_mWQ += cum_soil_gw_mWQ_lay [Sub] [hh] * hru_area [hh];
            Allcum_gw_flow_mWQ += cum_gw_flow_mWQ_lay [Sub] [hh] * hru_area [hh];
            Allcum_infil_act_mWQ += cum_infil_act_mWQ_lay [Sub] [hh] * hru_area [hh];
            Allcum_soil_moist_change_mWQ += (soil_moist [hh] * soil_moist_conc_lay [Sub] [hh] - soil_moist_Init [hh] * soil_moist_conc_Init [hh]) * hru_area [hh];
            Allcum_Sd_change_mWQ += (Sd [hh] * Sd_conc_lay [Sub] [hh] - Sd_Init [hh] * Sd_conc_Init_lay [Sub] [hh]) * hru_area [hh];
            Allcum_gw_change_mWQ += (gw [hh] * gw_conc_lay [Sub] [hh] - gw_Init [hh] * gw_conc_Init_lay [Sub] [hh]) * hru_area [hh];
            Allcum_evap += hru_cum_actet [hh] * hru_area [hh];
            Allcum_solute_deposit += cum_solute_deposit [hh] * hru_area [hh];
        }
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_runoff (mm*basin):           ").c_str (), Allcum_soil_runoff);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_runoff_mWQ (mg*basin):       ").c_str (), Allcum_soil_runoff_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_ssr (mm*basin):              ").c_str (), Allcum_soil_ssr);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_ssr_mWQ (mg*basin):          ").c_str (), Allcum_soil_ssr_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_rechr_ssr (mm*basin):             ").c_str (), Allcum_rechr_ssr);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_rechr_ssr_mWQ (mg*basin):         ").c_str (), Allcum_rechr_ssr_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_gw (mm*basin):               ").c_str (), Allcum_soil_gw);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_gw_mWQ (mg*basin):           ").c_str (), Allcum_soil_gw_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_gw_flow (mm*basin):               ").c_str (), Allcum_gw_flow);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_gw_flow_mWQ (mg*basin):           ").c_str (), Allcum_gw_flow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_infil_act (mm*basin):             ").c_str (), Allcum_infil_act);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_infil_act_mWQ (mg*basin):         ").c_str (), Allcum_infil_act_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_moist_change (mm*basin):     ").c_str (), Allcum_soil_moist_change);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_soil_moist_change_mWQ (mg*basin): ").c_str (), Allcum_soil_moist_change_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_Sd_change (mm*basin):             ").c_str (), Allcum_Sd_change);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_Sd_change_mWQ (mg*basin):         ").c_str (), Allcum_Sd_change_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_gw_change (mm*basin):             ").c_str (), Allcum_gw_change);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_gw_change_mWQ (mg*basin):         ").c_str (), Allcum_gw_change_mWQ);
        LogDebug (" ");
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_actet (mm*basin):                 ").c_str (), Allcum_evap);
        LogMessage (string ("'" + Name + " (WQ_Soil)' Allcum_solute_deposit (mg*basin):        ").c_str (), Allcum_solute_deposit);
        LogDebug (" ");
        LogMessage (string ("'" + Name + " (WQ_Soil)' AllTotal              (mm*basin):        ").c_str (), AllTotal);
        LogDebug (" ");
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::finish(bool good)@@@WQ_CRHM.cpp>");
}
double ClassWQ_Soil :: FunctX (const double x)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::FunctX(const double x)@@@WQ_CRHM.cpp>");
    double X = 0.0, F;
    for (long n = 1; n < 100; ++ n) {
        double y = - 2.0 * n * x;
        F = 4.0 * exp (y) / (n * (1.0 - exp (y)));
        X += F;
        if (fabs (F) < 0.001)
        {

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::FunctX(const double x)@@@WQ_CRHM.cpp>");
            return X;
        }

    }

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::FunctX(const double x)@@@WQ_CRHM.cpp>");
    return X;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::FunctX(const double x)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: Sd_to_soil_mass (double add_mass)
{
    if (soil_rechr [hh] > 0.0f)
    {
        conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + add_mass;
        conc_soil_rechr_lay [Sub] [hh] /= soil_rechr [hh];
    }

    else
    if (soil_moist [hh] > 0.0f)
    {
        conc_soil_rechr_lay [Sub] [hh] = 0.0f;
        conc_soil_lower_lay [Sub] [hh] = conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + add_mass;
        conc_soil_lower_lay [Sub] [hh] /= conc_soil_lower_lay [Sub] [hh];
    }

}
;

void ClassWQ_Soil :: Set_WQ (const long hru, double * var, double * var_cWQ, double amount, double amount_cWQ)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::Set_WQ(const long hru, double * var, double * var_cWQ, double amount, double amount_cWQ)@@@WQ_CRHM.cpp>");
    var [hru] = amount;
    if (amount > 0.0)
    {
        var_cWQ [hru] = amount_cWQ;
    }

    else
    {
        var_cWQ [hru] = 0.0;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::Set_WQ(const long hru, double * var, double * var_cWQ, double amount, double amount_cWQ)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: copy_array (double * from, double * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh)
        to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: restore_hru (double * from, double * to, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
    to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: Restore (long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::Restore(long hh)@@@WQ_CRHM.cpp>");
    restore_hru (redirected_residual_0, redirected_residual, hh);
    restore_hru (Sd_0, Sd, hh);
    restore_hru (gw_0, gw, hh);
    restore_hru (soil_rechr_0, soil_rechr, hh);
    restore_hru (soil_moist_0, soil_moist, hh);
    restore_hru (soil_lower_0, soil_lower, hh);
    restore_hru (gw_flow_0, gw_flow, hh);
    restore_hru (hru_actet_0, hru_actet, hh);
    restore_hru (hru_cum_actet_0, hru_cum_actet, hh);
    restore_hru (cum_soil_runoff_0, cum_soil_runoff, hh);
    restore_hru (cum_redirected_residual_0, cum_redirected_residual, hh);
    restore_hru (cum_hru_condense_0, cum_hru_condense, hh);
    restore_hru (cum_Sd_evap_0, cum_Sd_evap, hh);
    restore_hru (cum_Sd_ssr_0, cum_Sd_ssr, hh);
    restore_hru (cum_Sd_gw_0, cum_Sd_gw, hh);
    restore_hru (cum_lower_ssr_0, cum_lower_ssr, hh);
    restore_hru (cum_infil_act_0, cum_infil_act, hh);
    restore_hru (cum_gw_flow_0, cum_gw_flow, hh);
    restore_hru (cum_soil_ssr_0, cum_soil_ssr, hh);
    restore_hru (cum_rechr_ssr_0, cum_rechr_ssr, hh);
    restore_hru (cum_runoff_to_Sd_0, cum_runoff_to_Sd, hh);
    restore_hru (cum_runoff_to_ssr_0, cum_runoff_to_ssr, hh);
    restore_hru (cum_soil_gw_0, cum_soil_gw, hh);
    restore_hru (cum_infil_act_0, cum_infil_act, hh);
    restore_hru (cum_solute_deposit_0, cum_solute_deposit, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::Restore(long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: Save ()
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::Save()@@@WQ_CRHM.cpp>");
    copy_array (redirected_residual, redirected_residual_0);
    copy_array (Sd, Sd_0);
    copy_array (gw, gw_0);
    copy_array (soil_rechr, soil_rechr_0);
    copy_array (soil_moist, soil_moist_0);
    copy_array (soil_lower, soil_lower_0);
    copy_array (gw_flow, gw_flow_0);
    copy_array (hru_actet, hru_actet_0);
    copy_array (hru_cum_actet, hru_cum_actet_0);
    copy_array (cum_soil_runoff, cum_soil_runoff_0);
    copy_array (cum_redirected_residual, cum_redirected_residual_0);
    copy_array (cum_hru_condense, cum_hru_condense_0);
    copy_array (cum_Sd_evap, cum_Sd_evap_0);
    copy_array (cum_Sd_ssr, cum_Sd_ssr_0);
    copy_array (cum_Sd_gw, cum_Sd_gw_0);
    copy_array (cum_lower_ssr, cum_lower_ssr_0);
    copy_array (cum_gw_flow, cum_gw_flow_0);
    copy_array (cum_soil_ssr, cum_soil_ssr_0);
    copy_array (cum_rechr_ssr, cum_rechr_ssr_0);
    copy_array (cum_runoff_to_Sd, cum_runoff_to_Sd_0);
    copy_array (cum_runoff_to_ssr, cum_runoff_to_ssr_0);
    copy_array (cum_soil_gw, cum_soil_gw_0);
    copy_array (cum_infil_act, cum_infil_act_0);
    copy_array (cum_solute_deposit, cum_solute_deposit_0);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::Save()@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: Reset_WQ (long hru, double * var, double * var_cWQ)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::Reset_WQ(long hru, double * var, double * var_cWQ)@@@WQ_CRHM.cpp>");
    var [hru] = 0.0;
    var_cWQ [hru] = 0.0;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::Reset_WQ(long hru, double * var, double * var_cWQ)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Soil :: Reset_WQ (long hru, double * var, double ** var_cWQ_lay)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Soil::Reset_WQ(long hru, double * var, double ** var_cWQ_lay)@@@WQ_CRHM.cpp>");
    var [hru] = 0.0;
    for (long Sub = 0; Sub < numsubstances; Sub ++) {
        var_cWQ_lay [Sub] [hru] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Soil::Reset_WQ(long hru, double * var, double ** var_cWQ_lay)@@@WQ_CRHM.cpp>");
}
ClassWQ_Netroute * ClassWQ_Netroute :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_Netroute (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Handles the routing of surface runoff, subsurface runoff and HRU routing using the lag and route method.'\
                    'uses Clark,' \
                    'uses Muskingum.'";
    declvar ("inflow", TDim :: NHRU, "inflow from other HRUs", "(mm*km^2/int)", & inflow);
    declvar ("inflow_mWQ", TDim :: NDEFN, "Mass: inflow from other HRUs", "(mg/l * mm*km^2/int)", & inflow_mWQ, & inflow_mWQ_lay, numsubstances);
    declstatvar ("cuminflow", TDim :: NHRU, "cumulative inflow from other HRUs", "(mm*km^2)", & cuminflow);
    declstatvar ("cuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute inflow from other HRUs", "(mg/l * mm*km^2/int)", & cuminflow_mWQ, & cuminflow_mWQ_lay, numsubstances);
    declvar ("outflow", TDim :: NHRU, "HRU outflow", "(mm*km^2/int)", & outflow);
    declvar ("outflow_mWQ", TDim :: NDEFN, "Substance mass: HRU outflow", "(mg/l * mm*km^2/int)", & outflow_mWQ, & outflow_mWQ_lay, numsubstances);
    declvar ("outflow_cWQ", TDim :: NDEFN, "Substance concentration: HRU outflow", "(mg/l)", & outflow_cWQ, & outflow_cWQ_lay, numsubstances);
    declstatvar ("cumoutflow", TDim :: NHRU, "cumulative HRU outflow", "(mm*km^2)", & cumoutflow);
    declstatvar ("cumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow", "(mg/l * mm*km^2/int)", & cumoutflow_mWQ, & cumoutflow_mWQ_lay, numsubstances);
    declvar ("outflow_diverted", TDim :: NHRU, "HRU outflow diverted to another HRU", "(mm*km^2/int)", & outflow_diverted);
    declvar ("outflow_diverted_conc", TDim :: NDEFN, "Concentration: HRU outflow diverted to another HRU", "(mg/l)", & outflow_diverted_conc, & outflow_diverted_conc_lay, numsubstances);
    declstatvar ("cumoutflow_diverted", TDim :: NHRU, "cumulative HRU outflow diverted to another HRU", "(mm*km^2/int)", & cumoutflow_diverted);
    declstatvar ("cumoutflow_diverted_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow diverted to another HRU", "(mg/l * mm*km^2/int)", & cumoutflow_diverted_mWQ, & cumoutflow_diverted_mWQ_lay, numsubstances);
    declstatvar ("cum_to_Sd", TDim :: NHRU, "cumulative other HRU to depressional storage (Sd) of this HRU", "(mm)", & cum_to_Sd);
    declstatvar ("cum_to_Sd_mWQ", TDim :: NDEFN, "cumulative mass of solute from other HRU to depressional storage (Sd) of this HRU", "(mg/l * mm*km^2/int)", & cum_to_Sd_mWQ, & cum_to_Sd_mWQ_lay, numsubstances);
    declstatvar ("cum_to_soil_rechr", TDim :: NHRU, "cumulative other HRU to soil_rechr of this HRU", "(mm)", & cum_to_soil_rechr);
    declstatvar ("cum_to_soil_rechr_mWQ", TDim :: NDEFN, "cumulative mass of solute from other HRU to soil_rechr of this HRU", "(mg/l * mm*km^2/int)", & cum_to_soil_rechr_mWQ, & cum_to_soil_rechr_mWQ_lay, numsubstances);
    declvar ("gwinflow", TDim :: NHRU, "ground water inflow", "(mm*km^2/int)", & gwinflow);
    declvar ("gwinflow_mWQ", TDim :: NDEFN, "Concentration: ground water inflow", "(mg/l * mm*km^2/int)", & gwinflow_mWQ, & gwinflow_mWQ_lay, numsubstances);
    declstatvar ("gwcuminflow", TDim :: NHRU, "cumulative gw inflow", "(mm*km^2)", & gwcuminflow);
    declstatvar ("gwcuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute gw inflow", "(mg/l * mm*km^2/int)", & gwcuminflow_mWQ, & gwcuminflow_mWQ_lay, numsubstances);
    declvar ("gwoutflow", TDim :: NHRU, "HRU gw outflow", "(mm*km^2/int)", & gwoutflow);
    declvar ("gwoutflow_mWQ", TDim :: NDEFN, "Concentration: HRU gw outflow", "(mg/l * mm*km^2/int)", & gwoutflow_mWQ, & gwoutflow_mWQ_lay, numsubstances);
    declstatvar ("gwcumoutflow", TDim :: NHRU, "cumulative HRU gw outflow", "(mm*km^2)", & gwcumoutflow);
    declstatvar ("gwcumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU gw outflow", "(mg/l * mm*km^2/int)", & gwcumoutflow_mWQ, & gwcumoutflow_mWQ_lay, numsubstances);
    decldiag ("gwoutflow_diverted", TDim :: NHRU, "HRU gw outflow diverted to another HRU", "(mm*km^2/int)", & gwoutflow_diverted);
    decldiag ("gwoutflow_diverted_conc", TDim :: NDEFN, "HRU gw outflow diverted to another HRU", "(mm*km^2/int)", & gwoutflow_diverted_conc, & gwoutflow_diverted_conc_lay, numsubstances);
    declstatvar ("gwcumoutflow_diverted", TDim :: NHRU, "cumulative HRU gw outflow diverted to another HRU", "(mm*km^2/int)", & gwcumoutflow_diverted);
    declstatvar ("gwcumoutflow_diverted_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU gw outflow diverted to another HRU", "(mg/l * mm*km^2/int)", & gwcumoutflow_diverted_mWQ, & gwcumoutflow_diverted_mWQ_lay, numsubstances);
    declvar ("ssrinflow", TDim :: NHRU, "inflow from other HRUs", "(mm*km^2/int)", & ssrinflow);
    declvar ("ssrinflow_mWQ", TDim :: NDEFN, "Concentration: inflow from other HRUs", "(mg/l * mm*km^2/int)", & ssrinflow_mWQ, & ssrinflow_mWQ_lay, numsubstances);
    declstatvar ("ssrcuminflow", TDim :: NHRU, "cumulative inflow from other HRUs", "(mm*km^2)", & ssrcuminflow);
    declstatvar ("ssrcuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute of inflow from other HRUs", "(mg/l * mm*km^2/int)", & ssrcuminflow_mWQ, & ssrcuminflow_mWQ_lay, numsubstances);
    declvar ("ssroutflow", TDim :: NHRU, "HRU outflow", "(mm*km^2/int)", & ssroutflow);
    declvar ("ssroutflow_mWQ", TDim :: NDEFN, "Concentration: HRU outflow", "(mg/l * mm*km^2/int)", & ssroutflow_mWQ, & ssroutflow_mWQ_lay, numsubstances);
    declstatvar ("ssrcumoutflow", TDim :: NHRU, "cumulative HRU outflow", "(mm*km^2)", & ssrcumoutflow);
    declstatvar ("ssrcumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow", "(mg/l * mm*km^2/int)", & ssrcumoutflow_mWQ, & ssrcumoutflow_mWQ_lay, numsubstances);
    declstatvar ("HRU_cumbasinflow", TDim :: NHRU, "cumulative HRU to basinflow", "(mm*km^2)", & HRU_cumbasinflow);
    declstatvar ("HRU_cumbasinflow_mWQ", TDim :: NDEFN, "cumulative HRU to basinflow", "(mg/l * mm*km^2/int)", & HRU_cumbasinflow_mWQ, & HRU_cumbasinflow_mWQ_lay, numsubstances);
    declvar ("runinflow", TDim :: NHRU, "inflow from other HRUs", "(mm*km^2/int)", & runinflow);
    declvar ("runinflow_mWQ", TDim :: NDEFN, "Concentration: inflow from other HRUs", "(mg/l * mm*km^2/int)", & runinflow_mWQ, & runinflow_mWQ_lay, numsubstances);
    declstatvar ("runcuminflow", TDim :: NHRU, "cumulative inflow from other HRUs", "(mm*km^2)", & runcuminflow);
    declstatvar ("runcuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute inflow from other HRUs", "(mg/l * mm*km^2/int)", & runcuminflow_mWQ, & runcuminflow_mWQ_lay, numsubstances);
    declvar ("runoutflow", TDim :: NHRU, "HRU outflow", "(mm*km^2/int)", & runoutflow);
    declvar ("runoutflow_mWQ", TDim :: NDEFN, "Concentration: HRU outflow", "(mg/l * mm*km^2/int)", & runoutflow_mWQ, & runoutflow_mWQ_lay, numsubstances);
    declstatvar ("runcumoutflow", TDim :: NHRU, "cumulative HRU outflow", "(mm*km^2)", & runcumoutflow);
    declstatvar ("runcumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow", "(mg/l * mm*km^2/int)", & runcumoutflow_mWQ, & runcumoutflow_mWQ_lay, numsubstances);
    declstatvar ("cum_preferential_flow_to_gw", TDim :: NHRU, "cumulative other HRU's runoff to gw of this HRU via preferential flow path", "(mm)", & cum_preferential_flow_to_gw);
    declvar ("basinflow", TDim :: BASIN, "basin surface and sub-surface outflow", "(m^3/int)", & basinflow);
    declvar ("basinflow_conc", TDim :: NDEF, "basin surface and sub-surface outflow", "(mg/l)", & basinflow_conc, & basinflow_conc_lay, numsubstances);
    decldiag ("basinflow_s", TDim :: BASIN, "basin surface and sub-surface outflow", "(m^3/s)", & basinflow_s);
    declvar ("cumbasinflow", TDim :: BASIN, "cumulative basin surface and sub-surface outflow", "(m^3)", & cumbasinflow);
    declvar ("cumbasinflow_mWQ", TDim :: NDEF, "cumulative mass of solute basin surface and sub-surface outflow", "(mg/l * mm*km^2/int)", & cumbasinflow_mWQ, & cumbasinflow_mWQ_lay, numsubstances);
    declvar ("basingw", TDim :: BASIN, "cumulative basin groundwater outflow", "(m^3/int)", & basingw);
    declvar ("basingw_conc", TDim :: NDEF, "cumulative basin groundwater outflow", "(m^3/int)", & basingw_conc, & basingw_conc_lay, numsubstances);
    decldiag ("basingw_s", TDim :: BASIN, "cumulative basin groundwater outflow", "(m^3/s)", & basingw_s);
    declstatvar ("cumbasingw", TDim :: BASIN, "cumulative basin groundwater outflow", "(m^3)", & cumbasingw);
    declvar ("cumbasingw_mWQ", TDim :: NDEF, "cumulative mass of solute basin groundwater outflow", "(mg/l * mm*km^2/int)", & cumbasingw_mWQ, & cumbasingw_mWQ_lay, numsubstances);
    decllocal ("soil_ssr_Buf", TDim :: NHRU, "buffer subsurface runoff", "(mm/d)", & soil_ssr_Buf);
    declvar ("soil_ssr_Buf_conc", TDim :: NDEFN, "buffer subsurface runoff", "(mm/d)", & soil_ssr_Buf_conc, & soil_ssr_Buf_conc_lay, numsubstances);
    decllocal ("soil_runoff_Buf", TDim :: NHRU, "buffer rain runoff", "(mm/d)", & soil_runoff_Buf);
    declvar ("soil_runoff_Buf_conc", TDim :: NDEFN, "buffer rain runoff", "(mm/d)", & soil_runoff_Buf_conc, & soil_runoff_Buf_conc_lay, numsubstances);
    decllocal ("soil_gw_Buf", TDim :: NHRU, "buffer soil_gw(gw_flow) runoff", "(mm/d)", & soil_gw_Buf);
    declvar ("soil_gw_Buf_conc", TDim :: NDEFN, "buffer soil_gw(gw_flow) runoff", "(mm/d)", & soil_gw_Buf_conc, & soil_gw_Buf_conc_lay, numsubstances);
    declparam ("basin_area", TDim :: BASIN, "3", "1e-6", "1e09", "Total basin area", "(km^2)", & basin_area);
    declparam ("hru_area", TDim :: NHRU, "[1]", "1e-6", "1e09", "HRU area", "(km^2)", & hru_area);
    declparam ("Lag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4.0", "aggregated lag delay", "(h)", & Lag);
    declparam ("ssrKstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "subsurface runoff storage constant", "(d)", & ssrKstorage);
    declparam ("ssrLag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4.0", "subsurface runoff lag delay", "(h)", & ssrLag);
    declparam ("runKstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "runoff storage constant", "(d)", & runKstorage);
    declparam ("runLag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4", "runoff lag delay", "(h)", & runLag);
    declparam ("gwKstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "gw storage constant", "(d)", & gwKstorage);
    declparam ("gwLag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4", "gw lag delay", "(h)", & gwLag);
    declparam ("order", TDim :: NHRU, "[1,2,3,4,5!]", "1", "1000", "HRU routing process order", "()", & order);
    declparam ("whereto", TDim :: NHRU, "[0]", "0", "1000", "send to; 0 - basin outflow, or HRU input", "()", & whereto);
    declparam ("gwwhereto", TDim :: NHRU, "[0]", "-1000", "1000", "send to: 0 - basingw, >0 - other HRU surface input <0 - other abs(-HRU) gw input, or (< -HRUmax or > +HRUmax) - surface basinflow", "()", & gwwhereto);
    declparam ("Sdmax", TDim :: NHRU, "[0]", "0.0", "1000.0", "Maximum depression storage", "(mm)", & Sdmax);
    declparam ("soil_rechr_max", TDim :: NHRU, "[60.0]", "0.0", "350.0", "soil recharge maximum (<= soil_moist_max).", "(mm)", & soil_rechr_max);
    declparam ("Sd_ByPass", TDim :: NHRU, "[0]", "0", "1", "0 - normal, 1 - Bypass Pond/Depressional storage (i.e. Sd).", "()", & Sd_ByPass);
    declparam ("soil_rechr_ByPass", TDim :: NHRU, "[1]", "0", "1", "0 - normal, 1 - Bypass recharge layer (i.e. soil_rechr).", "()", & soil_rechr_ByPass);
    declparam ("preferential_flow", TDim :: NHRU, "[0]", "0", "1", "0 - no preferential and remain as runoff routing to other HRU, 1 - preferential flow and route runoff to other HRU's gw.", "()", & preferential_flow);
    soil_gwDiv = declgetvar ("*", "gw_flow", "(mm/int)", & soil_gw);
    soil_ssrDiv = declgetvar ("*", "soil_ssr", "(mm/int)", & soil_ssr);
    soil_runoffDiv = declgetvar ("*", "soil_runoff", "(mm/int)", & soil_runoff);
    declgetvar ("*", "soil_gw_conc", "(mg)", & soil_gw_conc, & soil_gw_conc_lay);
    declgetvar ("*", "soil_ssr_conc", "(mg)", & soil_ssr_conc, & soil_ssr_conc_lay);
    declgetvar ("*", "soil_runoff_cWQ", "(mg)", & soil_runoff_cWQ, & soil_runoff_cWQ_lay);
    declputvar ("*", "Sd", "(mm)", & Sd);
    declputvar ("*", "Sd_conc", "(mg)", & Sd_conc, & Sd_conc_lay);
    declputvar ("*", "soil_moist", "(mm)", & soil_moist);
    declputvar ("*", "soil_moist_conc", "(mg/l)", & soil_moist_conc, & soil_moist_conc_lay);
    declputvar ("*", "soil_lower", "(mm)", & soil_lower);
    declputvar ("*", "soil_rechr", "(mm)", & soil_rechr);
    declputvar ("*", "redirected_residual", "(mg)", & redirected_residual);
    declputvar ("*", "redirected_residual_conc", "(mm*km^2/int)", & redirected_residual_conc, & redirected_residual_conc_lay);
    declputvar ("*", "cum_redirected_residual", "(mg)", & cum_redirected_residual);
    declputvar ("*", "cum_redirected_residual_mWQ", "(mg/l * mm*km^2/int)", & cum_redirected_residual_mWQ, & cum_redirected_residual_mWQ_lay);
    declputvar ("*", "gw", "(mm)", & gw);
    declputvar ("*", "gw_conc", "(mg/l)", & gw_conc, & gw_conc_lay);
    declputvar ("*", "conc_soil_rechr", "(mg/l)", & conc_soil_rechr, & conc_soil_rechr_lay);
    declputvar ("*", "conc_soil_lower", "(mg/l)", & conc_soil_lower, & conc_soil_lower_lay);
    decllocal ("gwoutflow_0", TDim :: NHRU, "", "", & gwoutflow_0);
    decllocal ("outflow_0", TDim :: NHRU, "", "", & outflow_0);
    variation_set = VARIATION_1;
    decllocal ("Ktravel", TDim :: NHRU, "travel time", "(d)", & Ktravel);
    declparam ("route_n", TDim :: NHRU, "[0.025]", "0.016", "0.2", "Manning roughness coefficient", "()", & route_n);
    declparam ("route_R", TDim :: NHRU, "[0.5]", "0.01", "1.0E4", "hydraulic radius", "(m)", & route_R);
    declparam ("route_S0", TDim :: NHRU, "[1e-3]", "1e-6", "1.0", "longitudinal channel slope", "()", & route_S0);
    declparam ("route_L", TDim :: NHRU, "[200.0]", "0.01", "1.0E10", "routing length", "(m)", & route_L);
    declparam ("route_X_M", TDim :: NHRU, "[0.25]", "0.0", "0.5", "dimensionless weighting factor", "()", & route_X_M);
    declparam ("Channel_shp", TDim :: NHRU, "[0]", "0", "2", "rectangular - 0/parabolic - 1/triangular - 2", "()", & route_Cshp);
    variation_set = VARIATION_0;
    declparam ("Kstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "aggregated storage constant", "(d)", & Kstorage);
    variation_set = VARIATION_ORG;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    try {
        ssrDelay = new ClassClark (ssrinflow, ssroutflow, ssrKstorage, ssrLag, nhru);
        runDelay = new ClassClark (runinflow, runoutflow, runKstorage, runLag, nhru);
        gwDelay = new ClassClark (gwinflow, gwoutflow, gwKstorage, gwLag, nhru);
        ssrDelay_mWQ = new ClassClark * [numsubstances];
        runDelay_mWQ = new ClassClark * [numsubstances];
        gwDelay_mWQ = new ClassClark * [numsubstances];
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            ssrDelay_mWQ [Sub] = new ClassClark (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], ssrKstorage, ssrLag, nhru);
            runDelay_mWQ [Sub] = new ClassClark (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], runKstorage, runLag, nhru);
            gwDelay_mWQ [Sub] = new ClassClark (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], gwKstorage, gwLag, nhru);
        }
        if (variation == VARIATION_ORG)
        {
            Clark_hruDelay = new ClassClark (inflow, outflow, Kstorage, Lag, nhru);
            Clark_hruDelay_mWQ = new ClassClark * [numsubstances];
            for (long Sub = 0; Sub < numsubstances; ++ Sub)
                Clark_hruDelay_mWQ [Sub] = new ClassClark (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Kstorage, Lag, nhru);
        }

        else
        if (variation == VARIATION_1)
        {
            const double Vw [3] = {1.67, 1.44, 1.33};
            for (hh = 0; hh < nhru; ++ hh) {
                double Vavg = (1.0 / route_n [hh]) * pow (route_R [hh], 2.0 / 3.0) * pow (route_S0 [hh], 0.5f);
                Ktravel [hh] = route_L [hh] / (Vw [route_Cshp [hh]] * Vavg) / 86400.0;
            }
            hruDelay = new ClassMuskingum (inflow, outflow, Ktravel, route_X_M, Lag, nhru);
            for (hh = 0; hh < nhru; ++ hh) {
                if (Ktravel [hh] >= (Global :: Interval / (2.0 * route_X_M [hh])))
                {
                    string S = string ("'" + Name + " (Netroute_M_D) Muskingum coefficient negative in HRU ").c_str () + to_string (hh + 1);
                    CRHMException TExcept (S.c_str (), TExcept :: WARNING);
                    LogError (TExcept);
                }

            }
            hruDelay_mWQ = new ClassMuskingum * [numsubstances];
            for (long Sub = 0; Sub < numsubstances; ++ Sub)
                hruDelay_mWQ [Sub] = new ClassMuskingum (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Ktravel, route_X_M, Lag, nhru);
            for (hh = 0; hh < nhru; ++ hh) {
                if (Ktravel [hh] < (Global :: Interval / (2.0 * (1.0 - route_X_M [hh]))))
                {
                    hruDelay -> c0 [hh] = 0.0;
                    hruDelay -> c1 [hh] = 1.0;
                    hruDelay -> c2 [hh] = 0.0;
                    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
                        hruDelay_mWQ [Sub] -> c0 [hh] = 0.0;
                        hruDelay_mWQ [Sub] -> c1 [hh] = 1.0;
                        hruDelay_mWQ [Sub] -> c2 [hh] = 0.0;
                    }
                }

            }
        }

    }
    catch (std :: bad_alloc) {
        CRHMException Except ("Could not allocate in module CRACK.", TExcept :: TERMINATE);
        LogError (Except);
        throw Except;
    }
    if (soil_ssrDiv > 1)
    {
        string S = "Netroute:  \"soil_ssr\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (soil_runoffDiv > 1)
    {
        string S = "Netroute:  \"soil_runoff\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (soil_gwDiv > 1)
    {
        string S = "Netroute:  \"gw_flow\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    Reset_WQ (0, basinflow, basinflow_conc_lay);
    Reset_WQ (0, cumbasinflow, cumbasinflow_mWQ_lay);
    Reset_WQ (0, basingw, basingw_conc_lay);
    Reset_WQ (0, cumbasingw, cumbasingw_mWQ_lay);
    basinflow_s [0] = 0.0;
    basingw_s [0] = 0.0;
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        if (variation == VARIATION_ORG)
        {
            Clark_hruDelay_mWQ [Sub] = new ClassClark (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Kstorage, Lag, nhru);
        }

        else
        {
            hruDelay_mWQ [Sub] = new ClassMuskingum (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Ktravel, route_X_M, Lag, nhru);
        }

        ssrDelay_mWQ [Sub] = new ClassClark (ssrinflow_mWQ_lay [Sub], ssroutflow_mWQ_lay [Sub], ssrKstorage, ssrLag, nhru, - 1);
        runDelay_mWQ [Sub] = new ClassClark (runinflow_mWQ_lay [Sub], runoutflow_mWQ_lay [Sub], runKstorage, runLag, nhru, - 1);
        gwDelay_mWQ [Sub] = new ClassClark (gwinflow_mWQ_lay [Sub], gwoutflow_mWQ_lay [Sub], gwKstorage, gwLag, nhru, - 1);
    }
    for (hh = 0; hh < nhru; ++ hh) {
        Reset_WQ (hh, inflow, inflow_mWQ_lay);
        Reset_WQ (hh, cuminflow, cuminflow_mWQ_lay);
        Reset_WQ (hh, outflow, outflow_mWQ_lay);
        Reset_WQ (hh, cumoutflow, cumoutflow_mWQ_lay);
        Reset_WQ (hh, gwinflow, gwinflow_mWQ_lay);
        ;
        Reset_WQ (hh, gwcuminflow, gwcuminflow_mWQ_lay);
        ;
        Reset_WQ (hh, gwoutflow, gwoutflow_mWQ_lay);
        Reset_WQ (hh, gwcumoutflow, gwcumoutflow_mWQ_lay);
        Reset_WQ (hh, ssrinflow, ssrinflow_mWQ_lay);
        Reset_WQ (hh, ssrcuminflow, ssrcuminflow_mWQ_lay);
        Reset_WQ (hh, ssroutflow, ssroutflow_mWQ_lay);
        Reset_WQ (hh, ssrcumoutflow, ssrcumoutflow_mWQ_lay);
        Reset_WQ (hh, runinflow, runinflow_mWQ_lay);
        Reset_WQ (hh, runcuminflow, runcuminflow_mWQ_lay);
        Reset_WQ (hh, runoutflow, runoutflow_mWQ_lay);
        Reset_WQ (hh, runcumoutflow, runcumoutflow_mWQ_lay);
        Reset_WQ (hh, outflow_diverted, outflow_diverted_conc_lay);
        Reset_WQ (hh, cumoutflow_diverted, cumoutflow_diverted_mWQ_lay);
        Reset_WQ (hh, gwoutflow_diverted, gwoutflow_diverted_conc_lay);
        Reset_WQ (hh, gwcumoutflow_diverted, gwcumoutflow_diverted_mWQ_lay);
        Reset_WQ (hh, cum_to_Sd, cum_to_Sd_mWQ_lay);
        Reset_WQ (hh, cum_to_soil_rechr, cum_to_soil_rechr_mWQ_lay);
        Reset_WQ (hh, HRU_cumbasinflow, HRU_cumbasinflow_mWQ_lay);
        cum_preferential_flow_to_gw [hh] = 0.0;
        soil_ssr_Buf [hh] = 0.0;
        soil_runoff_Buf [hh] = 0.0;
        soil_gw_Buf [hh] = 0.0;
        bool OK = false;
        for (long jj = 0; chkStruct (jj); ++ jj)
            if (order [jj] - 1 == hh)
            {
                OK = true;
                break;
            }

        if (! OK)
        {
            string SS = string ("'" + Name + " (Netroute)' the 'order' parameter does not have a unique value for each HRU");
            CRHMException Except (SS.c_str (), TExcept :: ERR);
            LogError (Except);
            throw Except;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::run(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    double gw_Amount = 0.0;
    double gw_Amount_mWQ = 0.0;
    double Amount = 0.0;
    double Amount_mWQ = 0.0;
    Reset_WQ (0, basinflow, basinflow_conc_lay);
    Reset_WQ (0, basingw, basingw_conc_lay);
    for (long hh = 0; chkStruct (hh); ++ hh) {
        Reset_WQ (hh, outflow_diverted, outflow_diverted_conc_lay);
        Reset_WQ (hh, gwoutflow_diverted, gwoutflow_diverted_conc_lay);
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            if (soil_gwDiv == 1)
            {
                soil_gw_Buf [hh] = soil_gw [hh];
                soil_gw_Buf_conc_lay [Sub] [hh] = soil_gw_conc_lay [Sub] [hh];
            }

            if (soil_ssrDiv == 1)
            {
                soil_ssr_Buf [hh] = soil_ssr [hh];
                soil_ssr_Buf_conc_lay [Sub] [hh] = soil_ssr_conc_lay [Sub] [hh];
            }

            if (soil_runoffDiv == 1)
            {
                soil_runoff_Buf [hh] = soil_runoff [hh];
                soil_runoff_Buf_conc_lay [Sub] [hh] = soil_runoff_cWQ_lay [Sub] [hh];
            }

        }
    }
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        if (Sub == 0)
        {
            Save ();
        }

        for (long jj = 0; chkStruct (jj); ++ jj) {
            for (hh = 0; chkStruct (hh); ++ hh)
                if (order [hh] - 1 == jj)
                {
                    break;
                }

            if (Sub != 0)
            {
                Restore (hh);
            }

            gw_Amount = 0.0;
            gw_Amount_mWQ = 0.0;
            Amount = 0.0;
            Amount_mWQ = 0.0;
            gwinflow [hh] = soil_gw_Buf [hh] * hru_area [hh];
            gwinflow_mWQ_lay [Sub] [hh] = soil_gw_conc_lay [Sub] [hh] * gwinflow [hh];
            gwoutflow_diverted [hh] = 0.0;
            gwoutflow_diverted_conc_lay [Sub] [hh] = 0.0;
            for (long hhh = 0; chkStruct (hhh); ++ hhh) {
                if (gwoutflow [hhh] > 0.0)
                {
                    gwoutflow_diverted [hhh] = gwoutflow [hhh];
                    gw_Amount = gwoutflow [hhh] / hru_area [hh];
                    gw_Amount_mWQ = gwoutflow_mWQ_lay [Sub] [hhh] / hru_area [hh];
                    gwoutflow [hh] = 0.0;
                    gwcumoutflow_diverted [hhh] += gw_Amount;
                    gwcumoutflow_diverted_mWQ_lay [Sub] [hhh] += gw_Amount_mWQ;
                    if (abs (gwwhereto [hhh]) <= nhru)
                    {
                        if (gwwhereto [hhh] > 0)
                        {
                            double free = soil_rechr_max [hh] - soil_rechr [hh];
                            if (free > 0.0 && ! soil_rechr_ByPass [hh])
                            {
                                if (free > gw_Amount)
                                {
                                    conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + gw_Amount_mWQ;
                                    conc_soil_rechr_lay [Sub] [hh] /= (soil_rechr [hh] + gw_Amount);
                                    soil_moist_conc_lay [Sub] [hh] = soil_moist_conc_lay [Sub] [hh] * soil_moist [hh] + gw_Amount_mWQ;
                                    soil_moist_conc_lay [Sub] [hh] /= (soil_moist [hh] + gw_Amount);
                                    cum_to_soil_rechr_mWQ_lay [Sub] [hh] += gw_Amount_mWQ;
                                    if (Sub == numsubstances - 1)
                                    {
                                        soil_rechr [hh] += gw_Amount;
                                        soil_moist [hh] += gw_Amount;
                                        cum_to_soil_rechr [hh] += gw_Amount;
                                    }

                                    gw_Amount = 0.0;
                                    gw_Amount_mWQ = 0.0;
                                }

                                else
                                {
                                    double free_WQ_2rechr = gw_Amount_mWQ * free / gw_Amount;
                                    gw_Amount_mWQ = gw_Amount_mWQ * (gw_Amount - free) / gw_Amount;
                                    gw_Amount = (gw_Amount - free);
                                    conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + free_WQ_2rechr;
                                    conc_soil_rechr_lay [Sub] [hh] /= soil_rechr_max [hh];
                                    soil_moist_conc_lay [Sub] [hh] = soil_moist_conc_lay [Sub] [hh] * soil_moist [hh] + free_WQ_2rechr;
                                    soil_moist_conc_lay [Sub] [hh] /= (soil_moist [hh] + free);
                                    cum_to_soil_rechr_mWQ_lay [Sub] [hh] += free_WQ_2rechr;
                                    if (Sub == numsubstances - 1)
                                    {
                                        soil_rechr [hh] = soil_rechr_max [hh];
                                        soil_moist [hh] = soil_moist [hh] + free;
                                        cum_to_soil_rechr [hh] += free;
                                    }

                                }

                            }

                            free = Sdmax [hh] - Sd [hh];
                            if (free > 0.0 && ! Sd_ByPass [hh] && gw_Amount > 0.0)
                            {
                                if (free > gw_Amount)
                                {
                                    Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + gw_Amount_mWQ;
                                    Sd_conc_lay [Sub] [hh] /= (Sd [hh] + gw_Amount);
                                    cum_to_Sd_mWQ_lay [Sub] [hh] += gw_Amount_mWQ;
                                    if (Sub == numsubstances - 1)
                                    {
                                        Sd [hh] += gw_Amount;
                                        cum_to_Sd [hh] += gw_Amount;
                                    }

                                    gw_Amount = 0.0;
                                    gw_Amount_mWQ = 0.0;
                                }

                                else
                                {
                                    double free_mWQ_2Sd = gw_Amount_mWQ * free / gw_Amount;
                                    gw_Amount_mWQ = gw_Amount_mWQ * (gw_Amount - free) / gw_Amount;
                                    Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + free_mWQ_2Sd;
                                    Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] / Sdmax [hh];
                                    cum_to_Sd_mWQ_lay [Sub] [hh] += free_mWQ_2Sd;
                                    if (Sub == numsubstances - 1)
                                    {
                                        gw_Amount = (gw_Amount - free);
                                        cum_to_Sd [hh] += free;
                                    }

                                }

                            }

                        }

                        else
                        if (gwwhereto [hh] == 0)
                        {
                            cumbasingw_mWQ_lay [Sub] [0] += gw_Amount_mWQ;
                            if ((basingw [0] + gw_Amount) > 0.0)
                            {
                                basingw_conc_lay [Sub] [0] = basingw_conc_lay [Sub] [0] * basingw [0] + gw_Amount_mWQ * 1000;
                                basingw_conc_lay [Sub] [0] /= (basingw [0] + gw_Amount * 1000);
                            }

                            else
                            {
                                basingw_conc_lay [Sub] [0] = 0.0;
                            }

                            gwcumoutflow_mWQ_lay [Sub] [hh] += gw_Amount_mWQ;
                            if (Sub == numsubstances - 1)
                            {
                                basingw [0] += gw_Amount * 1000;
                                cumbasingw [0] += basingw [0];
                                gwcumoutflow [hh] += gw_Amount;
                            }

                            gw_Amount = 0.0;
                            gw_Amount_mWQ = 0.0;
                        }

                        else
                        {
                            gw_conc_lay [Sub] [hh] = gw_conc_lay [Sub] [hh] * gw [hh] + gw_Amount_mWQ * gw_Amount;
                            gw_conc_lay [Sub] [hh] /= (gw [hh] + gw_Amount);
                            if (Sub == numsubstances - 1)
                            {
                                gw [hh] += gw_Amount;
                            }

                            gw_Amount = 0.0;
                            gw_Amount_mWQ = 0.0;
                        }

                    }

                    else
                    {
                        cumbasinflow_mWQ_lay [Sub] [0] += gw_Amount_mWQ;
                        basinflow_conc_lay [Sub] [0] = basinflow_conc_lay [Sub] [0] * basinflow [0] + gw_Amount_mWQ * 1000;
                        if ((basinflow [0] + gw_Amount) > 0.0)
                        {
                            basinflow_conc_lay [Sub] [hh] /= (basinflow [0] + gw_Amount * 1000);
                        }

                        else
                        {
                            basinflow_conc_lay [Sub] [hh] = 0.0;
                        }

                        gwcumoutflow_mWQ_lay [Sub] [hh] += gw_Amount_mWQ;
                        if (Sub == numsubstances - 1)
                        {
                            HRU_cumbasinflow [hh] += gw_Amount;
                            basinflow [0] += gw_Amount * hru_area [hh] * 1000;
                            cumbasinflow [0] += basinflow [0];
                            cumoutflow [hh] += gw_Amount * hru_area [hh];
                        }

                        gw_Amount = 0.0;
                        gw_Amount_mWQ = 0.0;
                    }

                }

            }
            runinflow [hh] = soil_runoff_Buf [hh] * hru_area [hh];
            ssrinflow [hh] = soil_ssr_Buf [hh] * hru_area [hh];
            runinflow_mWQ_lay [Sub] [hh] = soil_runoff_Buf_conc_lay [Sub] [hh] * runinflow [hh];
            runcuminflow_mWQ_lay [Sub] [hh] += runinflow_mWQ_lay [Sub] [hh];
            runcumoutflow_mWQ_lay [Sub] [hh] += runoutflow_mWQ_lay [Sub] [hh];
            ssrinflow_mWQ_lay [Sub] [hh] = soil_ssr_Buf_conc_lay [Sub] [hh] * soil_ssr_Buf [hh] * hru_area [hh];
            ssrcuminflow_mWQ_lay [Sub] [hh] += ssrinflow_mWQ_lay [Sub] [hh];
            ssrcumoutflow_mWQ_lay [Sub] [hh] += ssroutflow_mWQ_lay [Sub] [hh];
            inflow [hh] = runoutflow [hh] + ssroutflow [hh];
            inflow_mWQ_lay [Sub] [hh] = runoutflow_mWQ_lay [Sub] [hh] + ssroutflow_mWQ_lay [Sub] [hh];
            if (Sub == numsubstances - 1)
            {
                gwcuminflow [hh] += gwinflow [hh];
                runcuminflow [hh] += runinflow [hh];
                runcumoutflow [hh] += runoutflow [hh];
                ssrcuminflow [hh] += ssrinflow [hh];
                ssrcumoutflow [hh] += ssroutflow [hh];
                cuminflow [hh] += inflow [hh];
                cumoutflow [hh] += inflow [hh];
            }

            for (long hhh = 0; chkStruct (hhh); ++ hhh) {
                Amount = 0.0;
                Amount_mWQ = 0.0;
                if ((whereto [hhh] - 1 == hh || whereto [hhh] == 0) && outflow [hhh] > 0.0)
                {
                    Amount = outflow [hhh] / hru_area [hh];
                    Amount_mWQ = outflow_mWQ_lay [Sub] [hhh] / hru_area [hh];
                    outflow [hhh] = 0.0;
                    cumoutflow_mWQ_lay [Sub] [hhh] += outflow_mWQ_lay [Sub] [hhh];
                    outflow_diverted_conc_lay [Sub] [hhh] += Amount_mWQ / Amount;
                    if (Sub == numsubstances - 1)
                    {
                        outflow_diverted [hhh] += Amount;
                    }

                    if (whereto [hhh] - 1 == hh)
                    {
                        if (preferential_flow [hhh])
                        {
                            gw_conc_lay [Sub] [hh] = gw_conc_lay [Sub] [hh] * gw [hh] + Amount_mWQ;
                            gw_conc_lay [Sub] [hh] /= (gw [hh] + Amount);
                            if (Sub == numsubstances - 1)
                            {
                                gw [hh] += Amount;
                                cum_preferential_flow_to_gw [hh] += Amount;
                            }

                            Amount = 0.0;
                            Amount_mWQ = 0.0;
                            break;
                        }

                        else
                        {
                            if (! soil_rechr_ByPass [hh])
                            {
                                if (soil_rechr [hh] + Amount >= soil_rechr_max [hh])
                                {
                                    double Excess = soil_rechr [hh] + Amount - soil_rechr_max [hh];
                                    double Free = Amount - Excess;
                                    conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + Amount_mWQ * Free / (Amount);
                                    conc_soil_rechr_lay [Sub] [hh] /= (soil_rechr [hh] + Free);
                                    soil_moist_conc_lay [Sub] [hh] = (conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + Amount_mWQ * Free / Amount) / (soil_lower [hh] + soil_rechr [hh] + Free);
                                    if (Sub == numsubstances - 1)
                                    {
                                        soil_rechr [hh] += Free;
                                        soil_moist [hh] = soil_lower [hh] + soil_rechr [hh];
                                    }

                                    Amount_mWQ = Amount_mWQ * Excess / Amount;
                                    Amount = Excess;
                                }

                                else
                                {
                                    if (soil_rechr [hh] + Amount > 0.0)
                                    {
                                        conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + Amount_mWQ;
                                        conc_soil_rechr_lay [Sub] [hh] /= (soil_rechr [hh] + Amount);
                                    }

                                    else
                                    {
                                        conc_soil_rechr_lay [Sub] [hh] = 0.0;
                                    }

                                    soil_moist_conc_lay [Sub] [hh] = (conc_soil_lower_lay [Sub] [hh] * soil_lower [hh] + conc_soil_rechr_lay [Sub] [hh] * (soil_rechr [hh] + Amount_mWQ)) / (soil_lower [hh] + soil_rechr [hh] + Amount);
                                    if (Sub == numsubstances - 1)
                                    {
                                        soil_rechr [hh] = soil_rechr [hh] + Amount;
                                        soil_moist [hh] = soil_lower [hh] + soil_rechr [hh];
                                    }

                                    Amount = 0.0;
                                    Amount_mWQ = 0.0;
                                }

                            }

                            else
                            if (! Sd_ByPass [hh] && Amount > 0.0)
                            {
                                if (Sd [hh] + Amount >= Sdmax [hh])
                                {
                                    double Excess = Sd [hh] + Amount - Sdmax [hh];
                                    double Free = Amount - Excess;
                                    Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + Amount_mWQ * Free / Amount;
                                    Sd_conc_lay [Sub] [hh] /= (Sd [hh] + Free);
                                    cum_to_Sd_mWQ_lay [Sub] [hh] += Amount_mWQ * Free / Amount;
                                    if (Sub == numsubstances - 1)
                                    {
                                        Sd [hh] += Free;
                                        cum_to_Sd [hh] += Amount;
                                    }

                                    Amount_mWQ = Sd_conc_lay [Sub] [hh] * Excess;
                                    Amount = Excess;
                                }

                                else
                                {
                                    if (Sd [hh] + Amount > 0.0)
                                    {
                                        Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + Amount_mWQ;
                                        Sd_conc_lay [Sub] [hh] /= (Sd [hh] + Amount);
                                    }

                                    else
                                    {
                                        Sd_conc_lay [Sub] [hh] = 0.0;
                                    }

                                    if (Sub == numsubstances - 1)
                                    {
                                        Sd [hh] = Sd [hh] + Amount;
                                        cum_to_Sd [hh] += Amount;
                                    }

                                    Amount = 0.0;
                                    Amount_mWQ = 0.0;
                                }

                            }

                        }

                        if ((redirected_residual [hh] + Amount * hru_area [hh]) > minFlow_WQ)
                        {
                            redirected_residual_conc_lay [Sub] [hh] = redirected_residual_conc_lay [Sub] [hh] * redirected_residual [hh] + Amount_mWQ * hru_area [hh];
                            redirected_residual_conc_lay [Sub] [hh] /= (redirected_residual [hh] + Amount * hru_area [hh]);
                            if (Sub == numsubstances - 1)
                            {
                                redirected_residual [hh] += Amount * hru_area [hh];
                            }

                            Amount = 0.0;
                            Amount_mWQ = 0.0;
                            outflow [hhh] = 0.0;
                        }

                    }

                    else
                    if (whereto [hhh] == 0 && Amount > 0.0)
                    {
                        cumbasinflow_mWQ_lay [Sub] [0] += Amount_mWQ;
                        basinflow_conc_lay [Sub] [0] = basinflow_conc_lay [Sub] [0] * basinflow [0] + Amount_mWQ * 1000;
                        basinflow_conc_lay [Sub] [0] /= (basinflow [0] + Amount * 1000);
                        if (Sub == numsubstances - 1)
                        {
                            basinflow [0] = Amount * 1000;
                            cumbasinflow [0] += basinflow [0];
                            cumoutflow [hh] += Amount * hru_area [hh];
                            outflow_diverted [hhh] += Amount;
                        }

                        Amount = 0.0;
                        Amount_mWQ = 0.0;
                    }

                }

            }
        }
    }
    if (nstep == 0)
    {
        if (soil_ssrDiv > 1)
        {
            soil_ssr_Buf [hh] = soil_ssr [hh] / soil_ssrDiv;
        }

        if (soil_runoffDiv > 1)
        {
            soil_runoff_Buf [hh] = soil_runoff [hh] / soil_runoffDiv;
        }

        if (soil_gwDiv > 1)
        {
            soil_gw_Buf [hh] = soil_gw [hh] / soil_gwDiv;
        }

    }

    if (variation == VARIATION_ORG)
    {
        Clark_hruDelay -> DoClark ();
        for (long Sub = 0; Sub < numsubstances; ++ Sub)
            Clark_hruDelay_mWQ [Sub] -> DoClark ();
    }

    else
    if (variation == VARIATION_1)
    {
        hruDelay -> DoMuskingum ();
        for (long Sub = 0; Sub < numsubstances; ++ Sub)
            hruDelay_mWQ [Sub] -> DoMuskingum ();
    }

    runDelay -> DoClark ();
    ssrDelay -> DoClark ();
    gwDelay -> DoClark ();
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        runDelay_mWQ [Sub] -> DoClark ();
        ssrDelay_mWQ [Sub] -> DoClark ();
        gwDelay_mWQ [Sub] -> DoClark ();
    }
    basinflow_s [0] = basinflow [0] * Global :: Freq / 86400.0;
    basingw_s [0] = basingw [0] * Global :: Freq / 86400.0;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::finish(bool good)@@@WQ_CRHM.cpp>");
    double Allcuminflow = 0.0;
    double Allcumoutflow = 0.0;
    double Allcumoutflowdiverted = 0.0;
    double Allcuminflow_mWQ = 0.0;
    double Allcumoutflow_mWQ = 0.0;
    double Allcumoutflowdiverted_mWQ = 0.0;
    double Allgwcuminflow = 0.0;
    double Allgwcumoutflow = 0.0;
    double Allgwcumoutflowdiverted = 0.0;
    double Allgwcuminflow_mWQ = 0.0;
    double Allgwcumoutflow_mWQ = 0.0;
    double Allgwcumoutflowdiverted_mWQ = 0.0;
    double Allssrcuminflow = 0.0;
    double Allssrcumoutflow = 0.0;
    double Allruncuminflow = 0.0;
    double Allruncumoutflow = 0.0;
    double Allssrcuminflow_mWQ = 0.0;
    double Allssrcumoutflow_mWQ = 0.0;
    double Allruncuminflow_mWQ = 0.0;
    double Allruncumoutflow_mWQ = 0.0;
    double AllSdcuminflow = 0.0;
    double Allrechrcuminflow = 0.0;
    double AllSdcuminflow_mWQ = 0.0;
    double Allrechrcuminflow_mWQ = 0.0;
    double AllTotal = 0.0;
    double Total = 0.0;
    string S = string ("H2O");
    LogDebug (S.c_str ());
    LogMessage (" ");
    for (hh = 0; chkStruct (); ++ hh) {
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cuminflow                   (mm) (mm*km^2) (mm*basin): ").c_str (), cuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cumoutflow                  (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cumoutflow_diverted         (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow_diverted [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        if (variation == VARIATION_ORG)
        {
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' hruDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), Clark_hruDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        }

        else
        {
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' hruDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), hruDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        }

        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrcuminflow                (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrcumoutflow               (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), ssrDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runoffcuminflow             (mm) (mm*km^2) (mm*basin): ").c_str (), runcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runoffcumoutflow            (mm) (mm*km^2) (mm*basin): ").c_str (), runcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), runDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcuminflow                 (mm) (mm*km^2) (mm*basin): ").c_str (), gwcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcumoutflow                (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcumoutflow_diverted       (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow_diverted [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwDelay_in_storage          (mm) (mm*km^2) (mm*basin): ").c_str (), gwDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_to_Sd                   (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_Sd [hh], hru_area [hh], basin_area [0], " *** Added to this HRU Sd");
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_to_soil_rechr           (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_soil_rechr [hh], hru_area [hh], basin_area [0], " *** Added to this HRU recharge");
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_redirected_residual     (mm) (mm*km^2) (mm*basin): ").c_str (), cum_redirected_residual [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' HRU_cumbasinflow            (mm) (mm*km^2) (mm*basin): ").c_str (), HRU_cumbasinflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
        Total = cumoutflow [hh] + gwcumoutflow [hh] - cumbasinflow [hh] - cum_to_Sd [hh] - cum_to_soil_rechr [hh] - gwcumoutflow [hh] + cumoutflow_diverted [hh] + gwcumoutflow_diverted [hh] - cum_redirected_residual [hh];
        Allcuminflow += cuminflow [hh];
        Allcumoutflow += cumoutflow [hh];
        Allgwcuminflow += gwcuminflow [hh];
        Allgwcumoutflow += gwcumoutflow [hh];
        Allssrcumoutflow += ssrcumoutflow [hh];
        Allssrcuminflow += ssrcuminflow [hh];
        Allruncuminflow += runcuminflow [hh];
        Allruncumoutflow += runcumoutflow [hh];
        AllSdcuminflow += cum_to_Sd [hh];
        Allrechrcuminflow += cum_to_soil_rechr [hh];
        Allcumoutflowdiverted += cumoutflow_diverted [hh];
        Allgwcumoutflowdiverted += gwcumoutflow_diverted [hh];
        AllTotal += Total;
    }
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcuminflow (mm*basin):                ").c_str (), Allcuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcumoutflow_mWQ (mm*basin):           ").c_str (), Allcumoutflow_mWQ);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcumoutflowdiverted (mm*basin):       ").c_str (), Allcumoutflowdiverted);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcuminflow (mm*basin):              ").c_str (), Allgwcuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcumoutflow (mm*basin):             ").c_str (), Allgwcumoutflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcumoutflowdiverted (mm*basin):     ").c_str (), Allgwcumoutflowdiverted);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allssrcuminflow (mm*basin):             ").c_str (), Allssrcuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allssrcumoutflow (mm*basin):            ").c_str (), Allssrcumoutflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allruncuminflow (mm*basin):             ").c_str (), Allruncuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Allruncumoutflow (mm*basin):            ").c_str (), Allruncumoutflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute)' Total (mm) (mm*hru) (mm*hru/basin): ").c_str (), Total);
    LogDebug (" ");
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        string S = string ("Substance# ") + to_string (Sub + 1);
        LogDebug (S.c_str ());
        LogMessage (" ");
        Allcuminflow_mWQ = 0.0;
        Allcumoutflow_mWQ = 0.0;
        Allcumoutflowdiverted_mWQ = 0.0;
        Allgwcuminflow_mWQ = 0.0;
        Allgwcumoutflow_mWQ = 0.0;
        Allgwcumoutflowdiverted_mWQ = 0.0;
        Allssrcuminflow_mWQ = 0.0;
        Allssrcumoutflow_mWQ = 0.0;
        Allruncuminflow_mWQ = 0.0;
        Allruncumoutflow_mWQ = 0.0;
        AllSdcuminflow_mWQ = 0.0;
        Allrechrcuminflow_mWQ = 0.0;
        for (hh = 0; chkStruct (); ++ hh) {
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cuminflow                   (mm) (mm*km^2) (mm*basin): ").c_str (), cuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cuminflow_mWQ               (mm) (mm*km^2) (mm*basin): ").c_str (), cuminflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cumoutflow                  (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cumoutflow_mWQ              (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cumoutflow_diverted         (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow_diverted [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cumoutflow_diverted_mWQ     (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow_diverted_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            if (variation == VARIATION_ORG)
            {
                LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' hruDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), Clark_hruDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
            }

            else
            {
                LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' hruDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), hruDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
            }

            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrcuminflow                (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrcuminflow_mWQ            (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcuminflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrcumoutflow               (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrcumoutflow_mWQ           (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcumoutflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' ssrDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), ssrDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runoffcuminflow             (mm) (mm*km^2) (mm*basin): ").c_str (), runcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runoffcuminflow_mWQ         (mm) (mm*km^2) (mm*basin): ").c_str (), runcuminflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runoffcumoutflow            (mm) (mm*km^2) (mm*basin): ").c_str (), runcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runoffcumoutflow_mWQ        (mm) (mm*km^2) (mm*basin): ").c_str (), runcumoutflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' runDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), runDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcuminflow                 (mm) (mm*km^2) (mm*basin): ").c_str (), gwcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcuminflow_mWQ             (mm) (mm*km^2) (mm*basin): ").c_str (), gwcuminflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcumoutflow                (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcumoutflow_mWQ            (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcumoutflow_diverted       (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow_diverted [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwcumoutflow_diverted_mWQ   (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow_diverted_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' gwDelay_in_storage          (mm) (mm*km^2) (mm*basin): ").c_str (), gwDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_to_Sd                   (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_Sd [hh], hru_area [hh], basin_area [0], " *** Added to this HRU Sd");
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_to_Sd_mWQ               (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_Sd_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0], " *** Added to this HRU Sd");
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_to_soil_rechr           (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_soil_rechr [hh], hru_area [hh], basin_area [0], " *** Added to this HRU recharge");
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_to_soil_rechr_mWQ       (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_soil_rechr_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0], " *** Added to this HRU recharge");
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_redirected_residual     (mm) (mm*km^2) (mm*basin): ").c_str (), cum_redirected_residual [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' cum_redirected_residual_mWQ (mm) (mm*km^2) (mm*basin): ").c_str (), cum_redirected_residual_mWQ [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' HRU_cumbasinflow            (mm) (mm*km^2) (mm*basin): ").c_str (), HRU_cumbasinflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute)' HRU_cumbasinflow_mwq        (mm) (mm*km^2) (mm*basin): ").c_str (), HRU_cumbasinflow_mWQ_lay [Sub] [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
            LogDebug (" ");
            Allcuminflow_mWQ += cuminflow_mWQ_lay [Sub] [hh];
            Allcumoutflow_mWQ += cumoutflow_mWQ_lay [Sub] [hh];
            Allcumoutflowdiverted_mWQ += cumoutflow_diverted_mWQ_lay [Sub] [hh];
            Allgwcuminflow_mWQ += gwcuminflow_mWQ_lay [Sub] [hh];
            Allgwcumoutflow_mWQ += gwcumoutflow_mWQ_lay [Sub] [hh];
            Allgwcumoutflowdiverted_mWQ += gwcumoutflow_diverted_mWQ_lay [Sub] [hh];
            Allssrcumoutflow_mWQ += ssrcumoutflow_mWQ_lay [Sub] [hh];
            Allssrcuminflow_mWQ += ssrcuminflow_mWQ_lay [Sub] [hh];
            Allruncuminflow_mWQ += runcuminflow_mWQ_lay [Sub] [hh];
            Allruncumoutflow_mWQ += runcumoutflow_mWQ_lay [Sub] [hh];
            AllSdcuminflow_mWQ += cum_to_Sd_mWQ_lay [Sub] [hh];
            Allrechrcuminflow_mWQ += cum_to_soil_rechr_mWQ_lay [Sub] [hh];
            LogMessage (" ");
        }
        LogMessage (string ("'" + Name + " (WQ_Netroute)' cumbasinflow(m^3):           ").c_str (), cumbasinflow [0]);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' cumbasinflow_mWQ_lay(mg):    ").c_str (), cumbasinflow_mWQ_lay [Sub] [0]);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' cumbasingw(m^3):             ").c_str (), cumbasingw [0]);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' cumbasingw_mWQ_lay(mg):      ").c_str (), cumbasingw_mWQ_lay [Sub] [0]);
        LogMessage (" ");
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcuminflow (mm*basin):              ").c_str (), Allgwcuminflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcuminflow_mWQ (mm*basin):          ").c_str (), Allgwcuminflow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcumoutflow (mm*basin):             ").c_str (), Allgwcumoutflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcumoutflow_mWQ (mm*basin):         ").c_str (), Allgwcumoutflow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcumoutflowdiverted (mm*basin):     ").c_str (), Allgwcumoutflowdiverted);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allgwcumoutflowdiverted_mWQ (mm*basin): ").c_str (), Allgwcumoutflowdiverted_mWQ);
        LogDebug (" ");
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcuminflow (mm*basin):                ").c_str (), Allcuminflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcuminflow_mWQ (mm*basin):            ").c_str (), Allcuminflow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcumoutflow (mm*basin):               ").c_str (), Allcumoutflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcumoutflow_mWQ (mm*basin):           ").c_str (), Allcumoutflow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcumoutflowdiverted (mm*basin):       ").c_str (), Allcumoutflowdiverted);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allcumoutflowdiverted_mWQ (mm*basin):   ").c_str (), Allcumoutflowdiverted_mWQ);
        LogDebug (" ");
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allssrcuminflow (mm*basin):             ").c_str (), Allssrcuminflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allssrcuminflow_mWQ (mm*basin):         ").c_str (), Allssrcuminflow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allssrcumoutflow (mm*basin):            ").c_str (), Allssrcumoutflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allssrcumoutflow_mWQ (mm*basin):        ").c_str (), Allssrcumoutflow_mWQ);
        LogDebug (" ");
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allruncuminflow (mm*basin):             ").c_str (), Allruncuminflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allruncuminflow_mWQ (mm*basin):         ").c_str (), Allruncuminflow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allruncumoutflow (mm*basin):            ").c_str (), Allruncumoutflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allruncumoutflow_mWQ (mm*basin):        ").c_str (), Allruncumoutflow_mWQ);
        LogDebug (" ");
        LogMessage (string ("'" + Name + " (WQ_Netroute)' AllSdcuminflow (mm*basin):              ").c_str (), AllSdcuminflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' AllSdcuminflow_mWQ (mm*basin):          ").c_str (), AllSdcuminflow_mWQ);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allrechrcuminflow (mm*basin):           ").c_str (), Allrechrcuminflow);
        LogMessage (string ("'" + Name + " (WQ_Netroute)' Allrechrcuminflow_mWQ (mm*basin):       ").c_str (), Allrechrcuminflow_mWQ);
        LogDebug (" ");
        LogMessage (string ("'" + Name + " (WQ_Netroute)' AllTotal              (mm*basin):       ").c_str (), AllTotal);
        LogDebug (" ");
    }
    LogDebug (" ");
    if (variation == VARIATION_ORG)
    {
        delete [] Clark_hruDelay_mWQ;
    }

    if (variation == VARIATION_1)
    {
        delete [] hruDelay_mWQ;
    }

    delete [] ssrDelay_mWQ;
    delete [] runDelay_mWQ;
    delete [] gwDelay_mWQ;
    delete ssrDelay;
    delete runDelay;
    delete gwDelay;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::finish(bool good)@@@WQ_CRHM.cpp>");
}
double ClassWQ_Netroute :: Function1 (double * I, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::Function1(double * I, long hh)@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Function1(double * I, long hh)@@@WQ_CRHM.cpp>");
    return runDelay -> ChangeLag (I, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Function1(double * I, long hh)@@@WQ_CRHM.cpp>");
}
double ClassWQ_Netroute :: Function2 (double * X, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::Function2(double * X, long hh)@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Function2(double * X, long hh)@@@WQ_CRHM.cpp>");
    return runDelay -> ChangeStorage (X, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Function2(double * X, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: Reset_WQ (long hru, double * var, double ** var_cWQ_lay)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::Reset_WQ(long hru, double * var, double ** var_cWQ_lay)@@@WQ_CRHM.cpp>");
    var [hru] = 0.0;
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        var_cWQ_lay [Sub] [hru] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Reset_WQ(long hru, double * var, double ** var_cWQ_lay)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: Set_WQ (const long hru, double * var, double * var_conc, double Amount, double amount_conc)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::Set_WQ(const long hru, double * var, double * var_conc, double Amount, double amount_conc)@@@WQ_CRHM.cpp>");
    var [hru] = Amount;
    if (Amount > 0.0)
    {
        var_conc [hru] = amount_conc;
    }

    else
    {
        var_conc [hru] = 0.0;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Set_WQ(const long hru, double * var, double * var_conc, double Amount, double amount_conc)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: copy_array (double * from, double * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh)
        to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: restore_hru (double * from, double * to, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
    to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: Restore (long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::Restore(long hh)@@@WQ_CRHM.cpp>");
    restore_hru (gwoutflow_0, gwoutflow, hh);
    restore_hru (outflow_0, outflow, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Restore(long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute :: Save ()
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute::Save()@@@WQ_CRHM.cpp>");
    copy_array (gwoutflow, gwoutflow_0);
    copy_array (outflow, outflow_0);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute::Save()@@@WQ_CRHM.cpp>");
}
ClassWQ_pbsm * ClassWQ_pbsm :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_pbsm (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'calculates snow transport and sublimation (Pomeroy and Li, 1999).' \
                 'original version using hru_u,' \
                 'uses hru_Uadjust from walmsley_wind instead of hru_u,' \
                 'using hru_u and a regression to use daily windspeed,' \
                 'uses hru_Uadjust from walmsley_wind instead of hru_u and a regression to use daily windspeed.'";
    variation_set = VARIATION_0 + VARIATION_2;
    declgetvar ("*", "hru_u", "(m/s)", & hru_u);
    variation_set = VARIATION_1 + VARIATION_3;
    declgetvar ("*", "hru_Uadjust", "(m/s)", & hru_Uadjust);
    variation_set = VARIATION_2 + VARIATION_3;
    declparam ("u_D", TDim :: NHRU, "[1.0]", "0.5", "2.0", "Daily windspeed correction", "()", & u_D);
    declparam ("Drift_offset", TDim :: NHRU, "[0.0]", "0.0", "100.0", "Daily windspeed drift offset correction", "()", & Drift_offset);
    declparam ("Drift_slope", TDim :: NHRU, "[1.0]", "0.5", "2.0", "Daily windspeed drift slope correction", "()", & Drift_slope);
    declparam ("Subl_offset", TDim :: NHRU, "[0.0]", "0.0", "100.0", "Daily windspeed sublimation offset correction", "()", & Subl_offset);
    declparam ("Subl_slope", TDim :: NHRU, "[1.0]", "0.5", "2.0", "Daily windspeed sublimation slope correction", "()", & Subl_slope);
    variation_set = VARIATION_ORG;
    declstatvar ("SWE", TDim :: NHRU, "snow water equivalent", "(mm)", & SWE);
    declstatvar ("SWE_max", TDim :: NHRU, "snow water equivalent seasonal maximum", "(mm)", & SWE_max);
    declstatvar ("SWE_conc", TDim :: NDEFN, "snow water equivalent", "(mg/l)", & SWE_conc, & SWE_conc_lay, numsubstances);
    declvar ("Subl", TDim :: NHRU, "interval sublimation", "(mm/int)", & Subl);
    declvar ("Subl_conc", TDim :: NDEFN, "interval sublimation", "(mm/int)", & Subl_conc, & Subl_conc_lay, numsubstances);
    declvar ("Drift_in", TDim :: NHRU, "interval transport into HRU", "(mm/int)", & Drift_in);
    declvar ("Drift_in_conc", TDim :: NDEFN, "interval transport into HRU", "(mg/l)", & Drift_in_conc, & Drift_in_conc_lay, numsubstances);
    declvar ("Drift_out", TDim :: NHRU, "interval transport out of HRU", "(mm/int)", & Drift_out);
    declvar ("Drift_out_conc", TDim :: NDEFN, "interval transport out of HRU", "(mg/l)", & Drift_out_conc, & Drift_out_conc_lay, numsubstances);
    decldiag ("DriftH", TDim :: NHRU, "interval transport", "(mm/int)", & DriftH);
    decldiag ("SublH", TDim :: NHRU, "interval sublimation", "(mm/int)", & SublH);
    decldiag ("BasinSnowLoss", TDim :: BASIN, "transport out of basin", "(mm/int)", & BasinSnowLoss);
    decldiag ("BasinSnowLoss_mWQ", TDim :: NDEF, "transport out of basin", "(mg/l * mm*km^2/int)", & BasinSnowLoss_mWQ, & BasinSnowLoss_mWQ_lay, numsubstances);
    decldiag ("BasinSnowGain", TDim :: BASIN, "cumulative transport to basin estimated from HRU 1", "(mm/int)", & BasinSnowGain);
    decldiag ("BasinSnowGain_mWQ", TDim :: NDEF, "cumulative transport to basin estimated from HRU 1", "(mg/l * mm*km^2/int)", & BasinSnowGain_mWQ, & BasinSnowGain_mWQ_lay, numsubstances);
    declstatvar ("cumSubl", TDim :: NHRU, "cumulative sublimation", "(mm)", & cumSubl);
    declstatvar ("cumSubl_mWQ", TDim :: NDEFN, "cumulative sublimation solute", "(mg/l * mm*km^2/int)", & cumSubl_mWQ, & cumSubl_mWQ_lay, numsubstances);
    declstatvar ("cumDriftOut", TDim :: NHRU, "cumulative transport from HRU", "(mm)", & cumDriftOut);
    declstatvar ("cumDriftOut_mWQ", TDim :: NDEFN, "mass solute from HRU", "(mg/l * mm*km^2/int)", & cumDriftOut_mWQ, & cumDriftOut_mWQ_lay, numsubstances);
    declstatvar ("cumBasinSnowLoss", TDim :: BASIN, "cumulative transport out of basin", "(mm)", & cumBasinSnowLoss);
    declstatvar ("cumBasinSnowLoss_mWQ", TDim :: NDEF, "cumulative mass of solute transport out of basin", "(mg/l * mm*km^2/int)", & cumBasinSnowLoss_mWQ, & cumBasinSnowLoss_mWQ_lay, numsubstances);
    declstatvar ("cumBasinSnowGain", TDim :: BASIN, "cumulative transport to basin estimated from HRU 1", "(mm)", & cumBasinSnowGain);
    declstatvar ("cumBasinSnowGain_mWQ", TDim :: NDEF, "cumulative mass of solute transport to basin estimated from HRU 1", "(mg/l * mm*km^2/int)", & cumBasinSnowGain_mWQ, & cumBasinSnowGain_mWQ_lay, numsubstances);
    declstatvar ("cumDriftIn", TDim :: NHRU, "cumulative transport to HRU", "(mm)", & cumDriftIn);
    declstatvar ("cumDriftIn_mWQ", TDim :: NDEFN, "cumulative mass of solute transport to HRU", "(mg/l * mm*km^2/int)", & cumDriftIn_mWQ, & cumDriftIn_mWQ_lay, numsubstances);
    decllocal ("hru_basin", TDim :: NHRU, "conversion factor", "()", & hru_basin);
    decldiag ("DrySnow", TDim :: NHRU, "DrySnow", "()", & DrySnow);
    declstatvar ("SnowAge", TDim :: NHRU, "SnowAge", "()", & SnowAge);
    declstatvar ("cumSno", TDim :: NHRU, "cumulative snow", "(mm)", & cumSno);
    declstatvar ("cumSno_mWQ", TDim :: NDEFN, "cumulative mass of solute snow", "(mg/l * mm*km^2/int)", & cumSno_mWQ, & cumSno_mWQ_lay, numsubstances);
    declvar ("Prob", TDim :: NHRU, "Probability", "()", & Prob);
    declvar ("snowdepth", TDim :: NHRU, "depth of snow using Gray/Pomeroy", "(m)", & snowdepth);
    decllocal ("SWE_Init", TDim :: NHRU, "initial SWE", "(mm)", & SWE_Init);
    declparam ("fetch", TDim :: NHRU, "[1000.0]", "300.0", "10000.0", "fetch distance", "(m)", & fetch);
    declparam ("Ht", TDim :: NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "vegetation height(m)", "(m)", & Ht);
    declparam ("distrib", TDim :: NHRU, "[0.0, 1.0]", "-10.0", "10.0", "distribution fractions - can sum to 1", "()", & distrib);
    declparam ("N_S", TDim :: NHRU, "[320]", "1", "500", "vegetation number density", "(1/m^2)", & N_S);
    declparam ("A_S", TDim :: NHRU, "[0.003]", "0.0", "2.0", "stalk diameter", "(m)", & A_S);
    declparam ("basin_area", TDim :: BASIN, "3", "1e-6", "1e+09", "total basin area", "(km^2)", & basin_area);
    declparam ("hru_area", TDim :: NHRU, "[1]", "1e-6", "1e+09", "hru area", "(km^2)", & hru_area);
    declparam ("inhibit_evap", TDim :: NHRU, "[0]", "0", "1", "inhibit evaporatation, 1 -> inhibit", "()", & inhibit_evap);
    declparam ("inhibit_bs", TDim :: NHRU, "[0]", "0", "1", "inhibit blowing snow, 1 -> inhibit", "()", & inhibit_bs);
    declparam ("inhibit_subl", TDim :: NHRU, "[0]", "0", "1", "inhibit sublimation, 1 -> inhibit", "()", & inhibit_subl);
    declparam ("rain_conc", TDim :: NDEFN, "0", "0", "1000", "rain solute concentration", "(mg/l)", & rain_conc, & rain_conc_lay, numsubstances);
    declparam ("snow_conc", TDim :: NDEFN, "0", "0", "1000", "snow solute concentration", "(mg/l)", & snow_conc, & snow_conc_lay, numsubstances);
    declparam ("atmos_mWQ", TDim :: NDEFN, "0", "0", "10", "total basin area", "(mg/int)", & atmos_mWQ, & atmos_mWQ_lay, numsubstances);
    decllocal ("DrySnow_0", TDim :: NHRU, "", "", & DrySnow_0);
    decllocal ("SnowAge_0", TDim :: NHRU, "", "", & SnowAge_0);
    decllocal ("BasinSnowGain_0", TDim :: NHRU, "", "", & BasinSnowGain_0);
    decllocal ("cumBasinSnowGain_0", TDim :: NHRU, "", "", & cumBasinSnowGain_0);
    decllocal ("BasinSnowLoss_0", TDim :: NHRU, "", "", & BasinSnowLoss_0);
    decllocal ("cumBasinSnowLoss_0", TDim :: NHRU, "", "", & cumBasinSnowLoss_0);
    decllocal ("Subl_0", TDim :: NHRU, "", "", & Subl_0);
    decllocal ("SublH_0", TDim :: NHRU, "", "", & SublH_0);
    decllocal ("cumSubl_0", TDim :: NHRU, "", "", & cumSubl_0);
    decllocal ("Drift_in_0", TDim :: NHRU, "", "", & Drift_in_0);
    decllocal ("cumDriftIn_0", TDim :: NHRU, "", "", & cumDriftIn_0);
    decllocal ("Drift_out_0", TDim :: NHRU, "", "", & Drift_out_0);
    decllocal ("cumDriftOut_0", TDim :: NHRU, "", "", & cumDriftOut_0);
    decllocal ("SWE_0", TDim :: NHRU, "", "", & SWE_0);
    decllocal ("SWE_Init_0", TDim :: NHRU, "", "", & SWE_Init_0);
    decllocal ("cumSno_0", TDim :: NHRU, "", "", & cumSno_0);
    decllocal ("DriftH_0", TDim :: NHRU, "", "", & DriftH_0);
    decllocal ("SublH_0", TDim :: NHRU, "", "", & SublH_0);
    decllocal ("Prob_0", TDim :: NHRU, "", "", & Prob_0);
    decllocal ("rho_0", TDim :: NHRU, "", "", & rho_0);
    decllocal ("z_s_0", TDim :: NHRU, "", "", & z_s_0);
    declgetvar ("*", "hru_t", "(°C)", & hru_t);
    declgetvar ("*", "hru_rh", "(%)", & hru_rh);
    declgetvar ("*", "hru_newsnow", "()", & hru_newsnow);
    declgetvar ("*", "net_snow", "(mm/int)", & net_snow);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    cumBasinSnowLoss [0] = 0.0;
    cumBasinSnowGain [0] = 0.0;
    BasinSnowLoss_mWQ [0] = 0.0;
    BasinSnowGain_mWQ [0] = 0.0;
    cumBasinSnowLoss_mWQ [0] = 0.0;
    cumBasinSnowGain_mWQ [0] = 0.0;
    for (hh = 0; hh < nhru; ++ hh) {
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            Reset_WQ (hh, SWE, SWE_conc_lay);
            Reset_WQ (hh, Drift_in, Drift_in_conc_lay);
            Reset_WQ (hh, Drift_out, Drift_out_conc_lay);
            Reset_WQ (hh, cumDriftOut, cumDriftOut_mWQ_lay);
            Reset_WQ (hh, cumDriftIn, cumDriftIn_mWQ_lay);
            Reset_WQ (hh, cumSno, cumSno_mWQ_lay);
            Reset_WQ (hh, cumSubl, cumSubl_mWQ_lay);
        }
        BasinSnowLoss [hh] = 0.0;
        BasinSnowGain [hh] = 0.0;
        cumBasinSnowLoss [hh] = 0.0;
        cumBasinSnowGain [hh] = 0.0;
        SnowAge [hh] = 0.0;
        DrySnow [hh] = 0;
        snowdepth [hh] = 0.0;
        Prob [hh] = 0.0;
        if ((hh > 0) && (Ht [hh] < Ht [hh - 1]) && distrib [hh - 1] > 0)
        {
            CRHMException TExcept (string ("'" + Name + " (pbsm)' vegetation heights not in ascending order.").c_str (), TExcept :: WARNING);
            LogError (TExcept);
        }

    }
    for (hh = 0; hh < nhru; ++ hh)
        hru_basin [hh] = hru_area [hh] / basin_area [0];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::run(void)@@@WQ_CRHM.cpp>");
    double Znod, Ustar, Ustn, E_StubHt, Lambda, Ut, Uten_Prob;
    double SumDrift, SumDrift_conc, total, transport;
    long Sub = 0;
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        if (getstep () == 1)
        {
            for (hh = 0; chkStruct (); ++ hh)
                SWE_Init [hh] = SWE [hh];
        }

        if (Sub == 0)
        {
            Save ();
        }

        for (hh = 0; chkStruct (); ++ hh) {
            if (Sub != 0)
            {
                Restore (hh);
            }

            if (net_snow [hh] > 0.0)
            {
                SWE_conc_lay [Sub] [hh] = (SWE_conc_lay [Sub] [hh] * SWE [hh] + atmos_mWQ_lay [Sub] [hh] * net_snow [hh]) / (SWE [hh] + net_snow [hh]);
                SWE [hh] = SWE [hh] + net_snow [hh];
                cumSno [hh] = cumSno [hh] + net_snow [hh];
                cumSno_mWQ_lay [Sub] [hh] += net_snow [hh] * atmos_mWQ_lay [Sub] [hh];
            }

            if (variation == VARIATION_ORG || variation == VARIATION_2)
            {
                hru_u_ = hru_u [hh];
            }

            else
            {
                hru_u_ = hru_Uadjust [hh];
            }

            if (variation == VARIATION_2 || variation == VARIATION_3)
            {
                hru_u_ = u_D [hh] * hru_u_;
            }

            Reset_WQ (hh, Drift_in, Drift_in_conc_lay);
            Reset_WQ (hh, Drift_out, Drift_out_conc_lay);
            Reset_WQ (hh, Subl, Subl_conc_lay);
            DriftH [hh] = 0.0;
            SublH [hh] = 0.0;
            Prob [hh] = 0.0;
            if (SWE [hh] > 0.0)
            {
                SWE_conc_lay [Sub] [hh] = (SWE_conc_lay [Sub] [hh] * SWE [hh] + atmos_mWQ_lay [Sub] [hh]) / SWE [hh];
            }

            else
            {
                SWE_conc_lay [Sub] [hh] = 0.0;
            }

            ;
            if (SWE [hh] > 0.0 && ! inhibit_bs [hh])
            {
                E_StubHt = Ht [hh] - Common :: DepthofSnow (SWE [hh]);
                if (E_StubHt < 0.0001)
                {
                    E_StubHt = 0.0001;
                }

                Ustar = 0.02264 * pow (hru_u_, 1.295f);
                if (E_StubHt > 0.01)
                {
                    Znod = (sqr (Ustar) / 163.3f) + 0.5 * N_S [hh] * E_StubHt * A_S [hh];
                    Lambda = N_S [hh] * A_S [hh] * E_StubHt;
                    Ustn = Ustar * sqrt ((PBSM_constants :: Beta * Lambda) / (1.0 + PBSM_constants :: Beta * Lambda));
                    Uten_Prob = (log (10.0 / Znod)) / PBSM_constants :: KARMAN * min < double > (0.0, Ustar - Ustn);
                }

                else
                {
                    Uten_Prob = hru_u_;
                }

                ProbabilityThresholdNew (SWE [hh], hru_t [hh], Uten_Prob, Prob [hh], Ut, hru_newsnow [hh], SnowAge [hh], DrySnow [hh]);
                if (Prob [hh] > 0.001)
                {
                    Ut = Ut * 0.8;
                    Pbsm (E_StubHt, Ut, DriftH [hh], SublH [hh], hru_t [hh], hru_u_, hru_rh [hh], fetch [hh], N_S [hh], A_S [hh]);
                    if (variation == VARIATION_2 || variation == VARIATION_3)
                    {
                        DriftH [hh] = Drift_offset [hh] + DriftH [hh] * Drift_slope [hh];
                        SublH [hh] = Subl_offset [hh] + SublH [hh] * Subl_slope [hh];
                    }

                    Drift_out [hh] = DriftH [hh] * Prob [hh] / fetch [hh];
                    if (! inhibit_subl [hh])
                    {
                        Subl [hh] = SublH [hh] * Prob [hh];
                    }

                    if (Drift_out [hh] + Subl [hh] > SWE [hh])
                    {
                        Subl [hh] = SWE [hh] * Subl [hh] / (Subl [hh] + Drift_out [hh]);
                        Drift_out [hh] = SWE [hh] - Subl [hh];
                    }

                    else
                    {
                        SWE [hh] = SWE [hh] - Subl [hh] - Drift_out [hh];
                    }

                    Drift_out_conc_lay [Sub] [hh] = SWE_conc_lay [Sub] [hh];
                    Subl_conc_lay [Sub] [hh] = SWE_conc_lay [Sub] [hh];
                    cumDriftOut [hh] = cumDriftOut [hh] + Drift_out [hh];
                    cumDriftOut_mWQ_lay [Sub] [hh] += Drift_out_conc_lay [Sub] [hh] * Drift_out [hh];
                    cumSubl [hh] = cumSubl [hh] + Subl [hh];
                    cumSubl_mWQ_lay [Sub] [hh] += Subl_conc_lay [Sub] [hh] * Subl [hh];
                }

            }

        }
        if (distrib [0] > 0.0 && Drift_out [0] > 0.0)
        {
            double Drft = Drift_out [0] * distrib [0];
            SWE_conc_lay [Sub] [0] = (SWE_conc_lay [Sub] [0] * SWE [0] + SWE_conc_lay [Sub] [0] * Drft) / (SWE [0] + Drft);
            SWE [0] += Drft;
            cumDriftIn [0] += Drft;
            BasinSnowGain [0] = Drft * hru_basin [0];
            BasinSnowGain_mWQ_lay [Sub] [0] = SWE_conc_lay [Sub] [hh];
            cumBasinSnowGain [0] += BasinSnowGain [0];
            cumDriftIn_mWQ_lay [Sub] [0] += Drft * SWE_conc_lay [Sub] [0];
            cumBasinSnowGain_mWQ_lay [Sub] [0] += BasinSnowGain [0] * SWE_conc_lay [Sub] [0];
        }

        BasinSnowLoss [0] = 0.0;
        long LastN = 0;
        if (! inhibit_bs [0] && nhru == 1)
        {
            BasinSnowLoss [0] = Drift_out [0];
            BasinSnowLoss_mWQ_lay [Sub] [0] = BasinSnowLoss [0] * SWE_conc_lay [Sub] [0];
            cumBasinSnowLoss [0] += BasinSnowLoss [0];
            cumBasinSnowLoss_mWQ_lay [Sub] [0] += BasinSnowLoss_mWQ_lay [Sub] [0];
        }

        for (long nn = LastN; chkStruct (nn); ++ nn) {
            if (distrib [nn] >= 0.0 && nn + 1 < nhru)
            {
                continue;
            }

            SumDrift = 0.0;
            SumDrift_conc = 0.0;
            for (long hhh = LastN; chkStruct (hhh, nn); ++ hhh) {
                SumDrift_conc = SumDrift * SumDrift_conc + Drift_out [hhh] * hru_basin [hhh] * Drift_out_conc_lay [Sub] [hhh];
                SumDrift += Drift_out [hhh] * hru_basin [hhh];
                if (SumDrift > 0.0)
                {
                    SumDrift_conc /= SumDrift;
                }

                else
                {
                    SumDrift_conc = 0.0;
                }

            }
            if (SumDrift > 0.0)
            {
                for (long hh = LastN + 1; chkStruct (hh, nn + 1); ++ hh) {
                    SWE_max [hh] = SWEfromDepth (Ht [hh]);
                    if (hh == nn)
                    {
                        if (distrib [nn] > 0)
                        {
                            double In = SumDrift / hru_basin [hh];
                            Drift_in_conc_lay [Sub] [hh] = SumDrift_conc;
                            if (SWE_max [hh] > SWE [hh] + In)
                            {
                                Drift_in [hh] = In;
                                SWE_conc_lay [Sub] [hh] = (SWE_conc_lay [Sub] [hh] * SWE [hh] + SumDrift_conc * In) / (SWE [hh] + In);
                                SWE [hh] += In;
                                cumDriftIn [hh] += In;
                                cumDriftIn_mWQ_lay [Sub] [hh] += SumDrift_conc * In;
                                transport = 0.0;
                            }

                            else
                            if (SWE_max [hh] > SWE [hh])
                            {
                                double used = SWE_max [hh] - SWE [hh];
                                Drift_in [hh] = used;
                                Drift_in_conc_lay [Sub] [hh] = SumDrift_conc;
                                cumDriftIn [hh] += Drift_in [hh];
                                transport -= (In - used) * hru_basin [hh];
                                SWE [hh] += used;
                                SWE_conc_lay [Sub] [hh] = (SWE_conc_lay [Sub] [hh] * SWE [hh] + SumDrift_conc * used) / (SWE [hh] + used);
                                cumDriftIn_mWQ_lay [Sub] [hh] += SumDrift_conc * used;
                            }

                            else
                            {
                                transport = SumDrift;
                            }

                        }

                        else
                        if (distrib [nn] < 0)
                        {
                            double used = SumDrift / hru_basin [hh];
                            SWE_conc_lay [Sub] [hh] = (SWE_conc_lay [Sub] [hh] * SWE [hh] + SumDrift_conc * used) / (SWE [hh] + used);
                            Drift_in [hh] = used;
                            Drift_in_conc_lay [Sub] [hh] = SumDrift_conc;
                            SWE [hh] += SumDrift / hru_basin [hh];
                            cumDriftIn [hh] += Drift_in [hh];
                            cumDriftIn_mWQ_lay [Sub] [hh] += SumDrift / hru_basin [hh] * SumDrift_conc;
                            transport = 0.0;
                        }

                        else
                        {
                            transport = SumDrift;
                        }

                        BasinSnowLoss [0] += (transport + Drift_out [hh] * hru_basin [hh]);
                        BasinSnowLoss_mWQ_lay [Sub] [0] = SumDrift_conc;
                        cumBasinSnowLoss [0] += (transport + Drift_out [hh] * hru_basin [hh]);
                        cumBasinSnowLoss_mWQ_lay [Sub] [0] += (transport * SumDrift_conc + Drift_out [hh] * Drift_out_conc_lay [Sub] [hh] * hru_basin [hh]);
                    }

                    else
                    if (SWE_max [hh] > SWE [hh] && distrib [hh] > 0.0)
                    {
                        total = 0.0;
                        for (long jj = hh; chkStruct (jj, nn + 1); ++ jj)
                            total += fabs (distrib [jj]);
                        transport = SumDrift * fabs (distrib [hh]) / total / hru_basin [hh];
                        if (SWE_max [hh] > SWE [hh] + transport)
                        {
                            SWE_conc_lay [Sub] [hh] = (SWE_conc_lay [Sub] [hh] * SWE [hh] + SumDrift_conc * transport) / (SWE [hh] + transport);
                            Drift_in [hh] = transport;
                            Drift_in_conc_lay [Sub] [hh] = SumDrift_conc;
                            cumDriftIn [hh] += Drift_in [hh];
                            SWE [hh] += transport;
                        }

                        else
                        {
                            transport = SWE_max [hh] - SWE [hh];
                            SWE_conc_lay [Sub] [hh] = (SWE_conc_lay [Sub] [hh] * SWE [hh] + SumDrift_conc * transport) / (SWE [hh] + transport);
                            Drift_in [hh] = transport;
                            Drift_in_conc_lay [Sub] [hh] = SumDrift_conc;
                            cumDriftIn [hh] += Drift_in [hh];
                            SWE [hh] = SWE_max [hh];
                        }

                        SumDrift_conc = SumDrift * SumDrift_conc - transport * hru_basin [hh] * SumDrift_conc;
                        SumDrift -= transport * hru_basin [hh];
                        cumDriftIn [hh] += transport;
                        cumDriftIn_mWQ_lay [Sub] [hh] += transport * SumDrift_conc;
                        SumDrift_conc = SumDrift_conc / SumDrift;
                    }

                }
            }

            LastN = nn + 1;
        }
        for (hh = 0; chkStruct (); ++ hh) {
            if (SWE [hh] > 0.0)
            {
                const_cast < long * > (inhibit_evap) [hh] = 1;
                snowdepth [hh] = Common :: DepthofSnow (SWE [hh]);
            }

            else
            {
                const_cast < long * > (inhibit_evap) [hh] = 0;
                snowdepth [hh] = 0.0;
                SWE_conc_lay [Sub] [hh] = 0.0;
            }

        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::finish(bool good)@@@WQ_CRHM.cpp>");
    if (! good)
    {

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::finish(bool good)@@@WQ_CRHM.cpp>");
        return;
    }

    double AllcumSubl = 0.0;
    double AllcumCover = cumBasinSnowGain [0] - cumBasinSnowLoss [0];
    long Sub = 0;
    for (hh = 0; chkStruct (); ++ hh) {
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumSno     (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSno [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumDriftOut(mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftOut [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumDriftIn (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftIn [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumSubl    (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSubl [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumCover   (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSno [hh] + cumDriftIn [hh] - cumDriftOut [hh] - cumSubl [hh], hru_area [hh], basin_area [0], "*** SWE just before melt");
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' SWE        (mm) (mm*hru) (mm*hru/basin): ").c_str (), SWE [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' SWE_change (mm) (mm*hru) (mm*hru/basin): ").c_str (), SWE [hh] - SWE_Init [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
        AllcumSubl += cumSubl [hh] * hru_area [hh];
        AllcumCover += (cumSno [hh] + cumDriftIn [hh] - cumDriftOut [hh] - cumSubl [hh]) * hru_area [hh];
        LogDebug (" ");
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumSno_mWQ     (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSno_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumDriftOut_mWQ(mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftOut_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsm)' cumDriftIn_mWQ (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftIn_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
    }
    LogMessage (string ("'" + Name + " (WQ_pbsm)' AllcumSubl  (mm*basin): ").c_str (), AllcumSubl, "*** cumulative sum of all HRUs cumSubl");
    LogMessage (string ("'" + Name + " (WQ_pbsm)' AllcumCover (mm*basin): ").c_str (), AllcumCover, "*** SWE just before melt cumulative sum of all HRUs cumCover");
    LogDebug (" ");
    LogMessage ("'WQ_pbsm' cumBasinSnowLoss (mm): ", cumBasinSnowLoss [0]);
    LogMessage ("'WQ_pbsm' cumBasinSnowGain (mm): ", cumBasinSnowGain [0]);
    LogDebug (" ");
    LogMessage ("'WQ_pbsm' cumBasinSnowLoss_mWQ (substance) (mm): ", cumBasinSnowLoss_mWQ_lay [Sub] [0]);
    LogMessage ("'WQ_pbsm' cumBasinSnowGain_mWQ (substance) (mm): ", cumBasinSnowGain_mWQ_lay [Sub] [0]);
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::finish(bool good)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: Reset_WQ (long hru, double * var, double ** var_WQ_lay)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::Reset_WQ(long hru, double * var, double ** var_WQ_lay)@@@WQ_CRHM.cpp>");
    var [hru] = 0.0;
    for (long Sub = 0; Sub < numsubstances; ++ Sub)
        var_WQ_lay [Sub] [hru] = 0.0;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::Reset_WQ(long hru, double * var, double ** var_WQ_lay)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: copy_array (double * from, double * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh)
        to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: copy_array (long * from, long * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::copy_array(long * from, long * to)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh)
        to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::copy_array(long * from, long * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: copy_basin (double * from, double * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::copy_basin(double * from, double * to)@@@WQ_CRHM.cpp>");
    to [0] = from [0];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::copy_basin(double * from, double * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: restore_hru (double * from, double * to, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
    to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: restore_hru (long * from, long * to, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::restore_hru(long * from, long * to, long hh)@@@WQ_CRHM.cpp>");
    to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::restore_hru(long * from, long * to, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: Restore (long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::Restore(long hh)@@@WQ_CRHM.cpp>");
    restore_hru (DrySnow_0, DrySnow, hh);
    restore_hru (SnowAge_0, SnowAge, hh);
    restore_hru (BasinSnowGain, BasinSnowGain_0, hh);
    restore_hru (cumBasinSnowGain, cumBasinSnowGain_0, hh);
    restore_hru (BasinSnowLoss, BasinSnowLoss_0, hh);
    restore_hru (cumBasinSnowLoss, cumBasinSnowLoss_0, hh);
    restore_hru (SWE_0, SWE, hh);
    restore_hru (SWE_0, SWE, hh);
    restore_hru (cumSno_0, cumSno, hh);
    restore_hru (DriftH_0, DriftH, hh);
    restore_hru (SublH_0, SublH, hh);
    restore_hru (Prob_0, Prob, hh);
    restore_hru (Drift_in_0, Drift_in, hh);
    restore_hru (cumDriftIn_0, cumDriftIn, hh);
    restore_hru (Drift_out_0, Drift_out, hh);
    restore_hru (cumDriftOut_0, cumDriftOut, hh);
    restore_hru (Subl_0, Subl, hh);
    restore_hru (cumSubl_0, cumSubl, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::Restore(long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsm :: Save ()
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsm::Save()@@@WQ_CRHM.cpp>");
    copy_array (DrySnow, DrySnow_0);
    copy_array (SnowAge, SnowAge_0);
    copy_basin (BasinSnowGain, BasinSnowGain_0);
    copy_basin (cumBasinSnowGain, cumBasinSnowGain_0);
    copy_basin (BasinSnowLoss, BasinSnowLoss_0);
    copy_basin (cumBasinSnowLoss, cumBasinSnowLoss_0);
    copy_array (SWE, SWE_0);
    copy_array (SWE_Init, SWE_0);
    copy_array (cumSno, cumSno_0);
    copy_array (DriftH, DriftH_0);
    copy_array (SublH, SublH_0);
    copy_array (Prob, Prob_0);
    copy_array (Drift_in, Drift_in_0);
    copy_array (cumDriftIn, cumDriftIn_0);
    copy_array (Drift_out, Drift_out_0);
    copy_array (cumDriftOut, cumDriftOut_0);
    copy_array (Subl, Subl_0);
    copy_array (cumSubl, cumSubl_0);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsm::Save()@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Handles the routing of surface runoff, subsurface runoff and HRU routing using the lag and route method.'\
                    'uses Muskingum,' \
                    'uses Clark.'";
    declvar ("inflow", TDim :: NHRU, "inflow from other HRUs", "(mm*km^2/int)", & inflow);
    declvar ("inflow_mWQ", TDim :: NDEFN, "Concentration: inflow from other HRUs", "(mg/l * mm*km^2/int)", & inflow_mWQ, & inflow_mWQ_lay, numsubstances);
    declstatvar ("cuminflow", TDim :: NHRU, "cumulative inflow from other HRUs", "(mm*km^2)", & cuminflow);
    declstatvar ("cuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute inflow from other HRUs", "(g*km^2)", & cuminflow_mWQ, & cuminflow_mWQ_lay, numsubstances);
    declvar ("outflow", TDim :: NHRU, "HRU outflow", "(mm*km^2/int)", & outflow);
    declvar ("outflow_mWQ", TDim :: NDEFN, "Substance mass: HRU outflow", "(mg/l * mm*km^2/int)", & outflow_mWQ, & outflow_mWQ_lay, numsubstances);
    declvar ("outflow_cWQ", TDim :: NDEFN, "Substance concentration: HRU outflow", "(mg/l * mm*km^2/int)", & outflow_cWQ, & outflow_cWQ_lay, numsubstances);
    declstatvar ("cumoutflow", TDim :: NHRU, "cumulative HRU outflow", "(mm*km^2)", & cumoutflow);
    declstatvar ("cumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow", "(mg/l * mm*km^2/int)", & cumoutflow_mWQ, & cumoutflow_mWQ_lay, numsubstances);
    declvar ("outflow_diverted", TDim :: NHRU, "HRU outflow diverted to another HRU", "(mm*km^2/int)", & outflow_diverted);
    declvar ("outflow_diverted_conc", TDim :: NDEFN, "Concentration: HRU outflow diverted to another HRU", "(g/l)", & outflow_diverted_conc, & outflow_diverted_conc_lay, numsubstances);
    declstatvar ("cumoutflow_diverted", TDim :: NHRU, "cumulative HRU outflow diverted to another HRU", "(mm*km^2/int)", & cumoutflow_diverted);
    declstatvar ("cumoutflow_diverted_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow diverted to another HRU", "(mg/l * mm*km^2/int)", & cumoutflow_diverted_mWQ, & cumoutflow_diverted_mWQ_lay, numsubstances);
    declstatvar ("cum_to_Sd", TDim :: NHRU, "cumulative other HRU to depressional storage (Sd) of this HRU", "(mm)", & cum_to_Sd);
    declstatvar ("cum_to_Sd_mWQ", TDim :: NDEFN, "cumulative mass of solute from other HRU to depressional storage (Sd) of this HRU", "(mg/l * mm*km^2/int)", & cum_to_Sd_mWQ, & cum_to_Sd_mWQ_lay, numsubstances);
    declstatvar ("cum_to_soil_rechr", TDim :: NHRU, "cumulative other HRU to soil_rechr of this HRU", "(mm)", & cum_to_soil_rechr);
    declstatvar ("cum_to_soil_rechr_mWQ", TDim :: NDEFN, "cumulative mass of solute from other HRU to soil_rechr of this HRU", "(mg/l * mm*km^2/int)", & cum_to_soil_rechr_mWQ, & cum_to_soil_rechr_mWQ_lay, numsubstances);
    declvar ("gwinflow", TDim :: NHRU, "ground water inflow", "(mm*km^2/int)", & gwinflow);
    declvar ("gwinflow_mWQ", TDim :: NDEFN, "Concentration: ground water inflow", "(mg/l * mm*km^2/int)", & gwinflow_mWQ, & gwinflow_mWQ_lay, numsubstances);
    declstatvar ("gwcuminflow", TDim :: NHRU, "cumulative gw inflow", "(mm*km^2)", & gwcuminflow);
    declstatvar ("gwcuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute gw inflow", "(mg/l * mm*km^2/int)", & gwcuminflow_mWQ, & gwcuminflow_mWQ_lay, numsubstances);
    declvar ("gwoutflow", TDim :: NHRU, "HRU gw outflow", "(mm*km^2/int)", & gwoutflow);
    declvar ("gwoutflow_mWQ", TDim :: NDEFN, "Concentration: HRU gw outflow", "(mg/l * mm*km^2/int)", & gwoutflow_mWQ, & gwoutflow_mWQ_lay, numsubstances);
    declstatvar ("gwcumoutflow", TDim :: NHRU, "cumulative HRU gw outflow", "(mm*km^2)", & gwcumoutflow);
    declstatvar ("gwcumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU gw outflow", "(mg/l * mm*km^2/int)", & gwcumoutflow_mWQ, & gwcumoutflow_mWQ_lay, numsubstances);
    decldiag ("gwoutflow_diverted", TDim :: NHRU, "HRU gw outflow diverted to another HRU", "(mm*km^2/int)", & gwoutflow_diverted);
    decldiag ("gwoutflow_diverted_conc", TDim :: NDEFN, "HRU gw outflow diverted to another HRU", "(mm*km^2/int)", & gwoutflow_diverted_conc, & gwoutflow_diverted_conc_lay, numsubstances);
    declstatvar ("gwcumoutflow_diverted", TDim :: NHRU, "cumulative HRU gw outflow diverted to another HRU", "(mm*km^2/int)", & gwcumoutflow_diverted);
    declstatvar ("gwcumoutflow_diverted_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU gw outflow diverted to another HRU", "(mg/l * mm*km^2/int)", & gwcumoutflow_diverted_mWQ, & gwcumoutflow_diverted_mWQ_lay, numsubstances);
    declvar ("ssrinflow", TDim :: NHRU, "inflow from other HRUs", "(mm*km^2/int)", & ssrinflow);
    declvar ("ssrinflow_mWQ", TDim :: NDEFN, "Concentration: inflow from other HRUs", "(mg/l * mm*km^2/int)", & ssrinflow_mWQ, & ssrinflow_mWQ_lay, numsubstances);
    declstatvar ("ssrcuminflow", TDim :: NHRU, "cumulative inflow from other HRUs", "(mm*km^2)", & ssrcuminflow);
    declstatvar ("ssrcuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute of inflow from other HRUs", "(mg/l * mm*km^2/int)", & ssrcuminflow_mWQ, & ssrcuminflow_mWQ_lay, numsubstances);
    declvar ("ssroutflow", TDim :: NHRU, "HRU outflow", "(mm*km^2/int)", & ssroutflow);
    declvar ("ssroutflow_mWQ", TDim :: NDEFN, "Concentration: HRU outflow", "(mg/l * mm*km^2/int)", & ssroutflow_mWQ, & ssroutflow_mWQ_lay, numsubstances);
    declstatvar ("ssrcumoutflow", TDim :: NHRU, "cumulative HRU outflow", "(mm*km^2)", & ssrcumoutflow);
    declstatvar ("ssrcumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow", "(mg/l * mm*km^2/int)", & ssrcumoutflow_mWQ, & ssrcumoutflow_mWQ_lay, numsubstances);
    declstatvar ("HRU_cumbasinflow", TDim :: NHRU, "cumulative HRU to basinflow", "(mm*km^2)", & HRU_cumbasinflow);
    declstatvar ("HRU_cumbasinflow_mWQ", TDim :: NDEFN, "cumulative HRU to basinflow", "(mg/l * mm*km^2/int)", & HRU_cumbasinflow_mWQ, & HRU_cumbasinflow_mWQ_lay, numsubstances);
    declvar ("runinflow", TDim :: NHRU, "inflow from other HRUs", "(mm*km^2/int)", & runinflow);
    declvar ("runinflow_mWQ", TDim :: NDEFN, "Concentration: inflow from other HRUs", "(mg/l * mm*km^2/int)", & runinflow_mWQ, & runinflow_mWQ_lay, numsubstances);
    declstatvar ("runcuminflow", TDim :: NHRU, "cumulative inflow from other HRUs", "(mm*km^2)", & runcuminflow);
    declstatvar ("runcuminflow_mWQ", TDim :: NDEFN, "cumulative mass of solute inflow from other HRUs", "(mg/l * mm*km^2/int)", & runcuminflow_mWQ, & runcuminflow_mWQ_lay, numsubstances);
    declvar ("runoutflow", TDim :: NHRU, "HRU outflow", "(mm*km^2/int)", & runoutflow);
    declvar ("runoutflow_mWQ", TDim :: NDEFN, "Concentration: HRU outflow", "(mg/l * mm*km^2/int)", & runoutflow_mWQ, & runoutflow_mWQ_lay, numsubstances);
    declstatvar ("runcumoutflow", TDim :: NHRU, "cumulative HRU outflow", "(mm*km^2)", & runcumoutflow);
    declstatvar ("runcumoutflow_mWQ", TDim :: NDEFN, "cumulative mass of solute HRU outflow", "(mg/l * mm*km^2/int)", & runcumoutflow_mWQ, & runcumoutflow_mWQ_lay, numsubstances);
    declstatvar ("cum_preferential_flow_to_gw", TDim :: NHRU, "cumulative other HRU's runoff to gw of this HRU via preferential flow path", "(mm)", & cum_preferential_flow_to_gw);
    declvar ("basinflow", TDim :: BASIN, "basin surface and sub-surface outflow", "(m^3/int)", & basinflow);
    declvar ("basinflow_conc", TDim :: NDEF, "basin surface and sub-surface outflow", "(g/l)", & basinflow_conc, & basinflow_conc_lay, numsubstances);
    declvar ("Used", TDim :: NHRU, "directed to basinbasin surface and sub-surface outflow", "()", & Used);
    declvar ("Used_mWQ", TDim :: NDEFN, "directed to basinbasin surface and sub-surface outflow", "(mg/l * mm*km^2/int)", & Used_mWQ, & Used_mWQ_lay, numsubstances);
    decldiag ("basinflow_s", TDim :: BASIN, "basin surface and sub-surface outflow", "(m^3/s)", & basinflow_s);
    declstatvar ("cumbasinflow", TDim :: BASIN, "cumulative basin surface and sub-surface outflow", "(m^3)", & cumbasinflow);
    declvar ("cumbasinflow_mWQ", TDim :: NDEF, "cumulative mass of solute basin surface and sub-surface outflow", "(mg/l * mm*km^2/int)", & cumbasinflow_mWQ, & cumbasinflow_mWQ_lay, numsubstances);
    declvar ("basingw", TDim :: BASIN, "cumulative basin groundwater outflow", "(m^3/int)", & basingw);
    declvar ("basingw_conc", TDim :: NDEF, "cumulative basin groundwater outflow", "(m^3/int)", & basingw_conc, & basingw_conc_lay, numsubstances);
    decldiag ("basingw_s", TDim :: BASIN, "cumulative basin groundwater outflow", "(m^3/s)", & basingw_s);
    declstatvar ("cumbasingw", TDim :: BASIN, "cumulative basin groundwater outflow", "(m^3)", & cumbasingw);
    declstatvar ("cumbasingw_mWQ", TDim :: NDEF, "cumulative mass of solute basin groundwater outflow", "(mg/l * mm*km^2/int)", & cumbasingw_mWQ, & cumbasingw_mWQ_lay, numsubstances);
    decllocal ("soil_ssr_Buf", TDim :: NHRU, "buffer subsurface runoff", "(mm/d)", & soil_ssr_Buf);
    declvar ("soil_ssr_Buf_conc", TDim :: NDEFN, "buffer subsurface runoff", "(mm/d)", & soil_ssr_Buf_conc, & soil_ssr_Buf_conc_lay, numsubstances);
    decllocal ("soil_runoff_Buf", TDim :: NHRU, "buffer rain runoff", "(mm/d)", & soil_runoff_Buf);
    declvar ("soil_runoff_Buf_conc", TDim :: NDEFN, "buffer rain runoff", "(mm/d)", & soil_runoff_Buf_conc, & soil_runoff_Buf_conc_lay, numsubstances);
    decllocal ("soil_gw_Buf", TDim :: NHRU, "buffer rain runoff", "(mm/d)", & soil_gw_Buf);
    declvar ("soil_gw_Buf_conc", TDim :: NDEFN, "buffer soil_gw(gw_flow) runoff", "(mm/d)", & soil_gw_Buf_conc, & soil_gw_Buf_conc_lay, numsubstances);
    decllocal ("distrib_sum", TDim :: NHRU, "HRU distribution sum", "()", & distrib_sum);
    declparam ("basin_area", TDim :: BASIN, "3", "1e-6", "1e09", "Total basin area", "(km^2)", & basin_area);
    declparam ("hru_area", TDim :: NHRU, "[1]", "1e-6", "1e09", "HRU area", "(km^2)", & hru_area);
    declparam ("Kstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "HRU storage constant", "(d)", & Kstorage);
    declparam ("Lag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4.0", "HRU lag delay", "(h)", & Lag);
    declparam ("ssrKstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "subsurface runoff storage constant", "(d)", & ssrKstorage);
    declparam ("ssrLag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4.0", "subsurface runoff lag delay", "(h)", & ssrLag);
    declparam ("runKstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "runoff storage constant", "(d)", & runKstorage);
    declparam ("runLag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4", "runoff lag delay", "(h)", & runLag);
    declparam ("gwKstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "gw storage constant", "(d)", & gwKstorage);
    declparam ("gwLag", TDim :: NHRU, "[0.0]", "0.0", "1.0E4", "gw lag delay", "(h)", & gwLag);
    declparam ("gwwhereto", TDim :: NHRU, "[0]", "-1000", "1000", "send to: 0 - basingw, >0 - other HRU surface input <0 - other abs(-HRU) gw input, or (< -HRUmax or > +HRUmax) - surface basinflow", "()", & gwwhereto);
    declparam ("order", TDim :: NHRU, "[1,2,3,4,5!]", "1", "1000", "HRU routing process order", "()", & order);
    declparam ("distrib_Route", TDim :: NDEFN, "[0.0]", "-1.0E6.0", "1.0E6.0", "route this HRU to these HRUs (distribute by area otherwise by fraction)", "()", & distrib, & distrib_hru, nhru);
    declparam ("distrib_Basin", TDim :: NHRU, "[1.0]", "0.0", "100.0", "route this HRU to basin (and other HRU(s) determined by 'distrib_hru')", "()", & distrib_Basin);
    declparam ("Sdmax", TDim :: NHRU, "[0]", "0.0", "1000.0", "Maximum depression storage", "(mm)", & Sdmax);
    declparam ("soil_rechr_max", TDim :: NHRU, "[60.0]", "0.0", "350.0", "soil recharge maximum (<= soil_moist_max).", "(mm)", & soil_rechr_max);
    declparam ("Sd_ByPass", TDim :: NHRU, "[0]", "0", "1", "0 - normal, 1 - Bypass Pond/Depressional storage (i.e. Sd).", "()", & Sd_ByPass);
    declparam ("soil_rechr_ByPass", TDim :: NHRU, "[1]", "0", "1", "0 - normal, 1 - Bypass recharge layer (i.e. soil_rechr).", "()", & soil_rechr_ByPass);
    declparam ("order", TDim :: NHRU, "[1,2,3,4,5!]", "1", "1000", "HRU routing process order", "()", & order);
    declparam ("preferential_flow", TDim :: NHRU, "[0]", "0", "1", "0 - no preferential and remain as runoff routing to other HRU, 1 - preferential flow and route runoff to other HRU's gw.", "()", & preferential_flow);
    soil_gwDiv = declgetvar ("*", "gw_flow", "(mm/int)", & soil_gw);
    soil_ssrDiv = declgetvar ("*", "soil_ssr", "(mm/int)", & soil_ssr);
    soil_runoffDiv = declgetvar ("*", "soil_runoff", "(mm/int)", & soil_runoff);
    declgetvar ("*", "soil_ssr_conc", "(g)", & soil_ssr_conc, & soil_ssr_conc_lay);
    declgetvar ("*", "soil_gw_conc", "(mg)", & soil_gw_conc, & soil_gw_conc_lay);
    declgetvar ("*", "soil_runoff_cWQ", "(mg)", & soil_runoff_cWQ, & soil_runoff_cWQ_lay);
    declputvar ("*", "Sd", "(mm)", & Sd);
    declputvar ("*", "Sd_conc", "(g)", & Sd_conc, & Sd_conc_lay);
    declputvar ("*", "soil_moist", "(mm)", & soil_moist);
    declputvar ("*", "soil_moist_conc", "(g)", & soil_moist_conc, & soil_moist_conc_lay);
    declputvar ("*", "soil_lower", "(mm)", & soil_lower);
    declputvar ("*", "conc_soil_lower", "(g)", & conc_soil_lower, & conc_soil_lower_lay);
    declputvar ("*", "soil_rechr", "(mm)", & soil_rechr);
    declputvar ("*", "redirected_residual", "(mm*km^2/int)", & redirected_residual);
    declputvar ("*", "redirected_residual_conc", "(g)", & redirected_residual_conc, & redirected_residual_conc_lay);
    declputvar ("*", "cum_redirected_residual", "(mm*km^2/int)", & cum_redirected_residual);
    declputvar ("*", "cum_redirected_residual_mWQ", "(mg/l * mm*km^2/int)", & cum_redirected_residual_mWQ, & cum_redirected_residual_mWQ_lay);
    declputvar ("*", "gw", "(mm)", & gw);
    declputvar ("*", "gw_conc", "(mg/l)", & gw_conc, & gw_conc_lay);
    declputvar ("*", "conc_soil_rechr", "(mg/l)", & conc_soil_rechr, & conc_soil_rechr_lay);
    declputvar ("*", "conc_soil_lower", "(mg/l)", & conc_soil_lower, & conc_soil_lower_lay);
    decllocal ("gwoutflow_0", TDim :: NHRU, "", "", & gwoutflow_0);
    decllocal ("outflow_0", TDim :: NHRU, "", "", & outflow_0);
    variation_set = VARIATION_0;
    decllocal ("Ktravel", TDim :: NHRU, "travel time", "(d)", & Ktravel);
    declparam ("route_n", TDim :: NHRU, "[0.025]", "0.016", "0.2", "Manning roughness coefficient", "()", & route_n);
    declparam ("route_R", TDim :: NHRU, "[0.5]", "0.01", "1.0E4", "hydraulic radius", "(m)", & route_R);
    declparam ("route_S0", TDim :: NHRU, "[1e-3]", "1e-6", "1.0", "longitudinal channel slope", "()", & route_S0);
    declparam ("route_L", TDim :: NHRU, "[200.0]", "0.01", "1.0E10", "routing length", "(m)", & route_L);
    declparam ("route_X_M", TDim :: NHRU, "[0.25]", "0.0", "0.5", "dimensionless weighting factor", "()", & route_X_M);
    declparam ("Channel_shp", TDim :: NHRU, "[0]", "0", "2", "rectangular - 0/parabolic - 1/triangular - 2", "()", & route_Cshp);
    variation_set = VARIATION_1;
    declparam ("Kstorage", TDim :: NHRU, "[0.0]", "0.0", "200.0", "aggregated storage constant", "(d)", & Kstorage);
    variation_set = VARIATION_ORG;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    try {
        ssrDelay = new ClassClark (ssrinflow, ssroutflow, ssrKstorage, ssrLag, nhru);
        runDelay = new ClassClark (runinflow, runoutflow, runKstorage, runLag, nhru);
        gwDelay = new ClassClark (gwinflow, gwoutflow, gwKstorage, gwLag, nhru);
        ssrDelay_mWQ = new ClassClark * [numsubstances];
        runDelay_mWQ = new ClassClark * [numsubstances];
        gwDelay_mWQ = new ClassClark * [numsubstances];
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            ssrDelay_mWQ [Sub] = new ClassClark (ssrinflow_mWQ_lay [Sub], ssroutflow_mWQ_lay [Sub], ssrKstorage, ssrLag, nhru);
            runDelay_mWQ [Sub] = new ClassClark (runinflow_mWQ_lay [Sub], runoutflow_mWQ_lay [Sub], runKstorage, runLag, nhru);
            gwDelay_mWQ [Sub] = new ClassClark (gwinflow_mWQ_lay [Sub], gwoutflow_mWQ_lay [Sub], gwKstorage, gwLag, nhru);
        }
        if (variation == VARIATION_1)
        {
            Clark_hruDelay = new ClassClark (inflow, outflow, Kstorage, Lag, nhru);
            Clark_hruDelay_mWQ = new ClassClark * [numsubstances];
            for (long Sub = 0; Sub < numsubstances; ++ Sub)
                Clark_hruDelay_mWQ [Sub] = new ClassClark (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Kstorage, Lag, nhru);
        }

        else
        if (variation == VARIATION_ORG)
        {
            const double Vw [3] = {1.67, 1.44, 1.33};
            for (hh = 0; hh < nhru; ++ hh) {
                double Vavg = (1.0 / route_n [hh]) * pow (route_R [hh], 2.0 / 3.0) * pow (route_S0 [hh], 0.5f);
                Ktravel [hh] = route_L [hh] / (Vw [route_Cshp [hh]] * Vavg) / 86400.0;
            }
            hruDelay = new ClassMuskingum (inflow, outflow, Ktravel, route_X_M, Lag, nhru);
            for (hh = 0; hh < nhru; ++ hh) {
                if (Ktravel [hh] >= (Global :: Interval / (2.0 * route_X_M [hh])))
                {
                    string S = string ("'" + Name + " (Netroute_M_D) Muskingum coefficient negative in HRU ").c_str () + to_string (hh + 1);
                    CRHMException TExcept (S.c_str (), TExcept :: WARNING);
                    LogError (TExcept);
                }

            }
            hruDelay_mWQ = new ClassMuskingum * [numsubstances];
            for (long Sub = 0; Sub < numsubstances; ++ Sub)
                hruDelay_mWQ [Sub] = new ClassMuskingum (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Ktravel, route_X_M, Lag, nhru);
            for (hh = 0; hh < nhru; ++ hh) {
                if (Ktravel [hh] < (Global :: Interval / (2.0 * (1.0 - route_X_M [hh]))))
                {
                    hruDelay -> c0 [hh] = 0.0;
                    hruDelay -> c1 [hh] = 1.0;
                    hruDelay -> c2 [hh] = 0.0;
                    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
                        hruDelay_mWQ [Sub] -> c0 [hh] = 0.0;
                        hruDelay_mWQ [Sub] -> c1 [hh] = 1.0;
                        hruDelay_mWQ [Sub] -> c2 [hh] = 0.0;
                    }
                }

            }
        }

    }
    catch (std :: bad_alloc) {
        CRHMException Except ("Could not allocate in module Netroute_M_D.", TExcept :: TERMINATE);
        LogError (Except);
        throw Except;
    }
    if (soil_ssrDiv > 1)
    {
        string S = "WQ_Netroute_M_D:  \"soil_ssr\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (soil_runoffDiv > 1)
    {
        string S = "WQ_Netroute_M_D:  \"soil_runoff\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (soil_gwDiv > 1)
    {
        string S = "WQ_Netroute_M_D:  \"gw_flow\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    Reset_WQ (0, basinflow, basinflow_conc_lay);
    Reset_WQ (0, cumbasinflow, cumbasinflow_mWQ_lay);
    Reset_WQ (0, basingw, basingw_conc_lay);
    Reset_WQ (0, cumbasingw, cumbasingw_mWQ_lay);
    basinflow_s [0] = 0.0;
    basingw_s [0] = 0.0;
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        if (variation == VARIATION_0)
        {
            Clark_hruDelay_mWQ [Sub] = new ClassClark (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Kstorage, Lag, nhru);
        }

        else
        {
            hruDelay_mWQ [Sub] = new ClassMuskingum (inflow_mWQ_lay [Sub], outflow_mWQ_lay [Sub], Ktravel, route_X_M, Lag, nhru);
        }

        ssrDelay_mWQ [Sub] = new ClassClark (ssrinflow_mWQ_lay [Sub], ssroutflow_mWQ_lay [Sub], ssrKstorage, ssrLag, nhru, - 1);
        runDelay_mWQ [Sub] = new ClassClark (runinflow_mWQ_lay [Sub], runoutflow_mWQ_lay [Sub], runKstorage, runLag, nhru, - 1);
        gwDelay_mWQ [Sub] = new ClassClark (gwinflow_mWQ_lay [Sub], gwoutflow_mWQ_lay [Sub], gwKstorage, gwLag, nhru, - 1);
    }
    for (hh = 0; hh < nhru; ++ hh) {
        Reset_WQ (hh, inflow, inflow_mWQ_lay);
        Reset_WQ (hh, cuminflow, cuminflow_mWQ_lay);
        Reset_WQ (hh, outflow, outflow_mWQ_lay);
        Reset_WQ (hh, cumoutflow, cumoutflow_mWQ_lay);
        Reset_WQ (hh, gwinflow, gwinflow_mWQ_lay);
        Reset_WQ (hh, gwcuminflow, gwcuminflow_mWQ_lay);
        Reset_WQ (hh, gwoutflow, gwoutflow_mWQ_lay);
        Reset_WQ (hh, gwcumoutflow, gwcumoutflow_mWQ_lay);
        Reset_WQ (hh, ssrinflow, ssrinflow_mWQ_lay);
        Reset_WQ (hh, ssrcuminflow, ssrcuminflow_mWQ_lay);
        Reset_WQ (hh, ssroutflow, ssroutflow_mWQ_lay);
        Reset_WQ (hh, ssrcumoutflow, ssrcumoutflow_mWQ_lay);
        Reset_WQ (hh, runinflow, runinflow_mWQ_lay);
        Reset_WQ (hh, runcuminflow, runcuminflow_mWQ_lay);
        Reset_WQ (hh, runoutflow, runoutflow_mWQ_lay);
        Reset_WQ (hh, runcumoutflow, runcumoutflow_mWQ_lay);
        Reset_WQ (hh, outflow_diverted, outflow_diverted_conc_lay);
        Reset_WQ (hh, cumoutflow_diverted, cumoutflow_diverted_mWQ_lay);
        Reset_WQ (hh, gwoutflow_diverted, gwoutflow_diverted_conc_lay);
        Reset_WQ (hh, gwcumoutflow_diverted, gwcumoutflow_diverted_mWQ_lay);
        Reset_WQ (hh, cum_to_Sd, cum_to_Sd_mWQ_lay);
        Reset_WQ (hh, cum_to_soil_rechr, cum_to_soil_rechr_mWQ_lay);
        Reset_WQ (hh, HRU_cumbasinflow, HRU_cumbasinflow_mWQ_lay);
        cum_preferential_flow_to_gw [hh] = 0.0;
        soil_ssr_Buf [hh] = 0.0;
        soil_runoff_Buf [hh] = 0.0;
        soil_gw_Buf [hh] = 0.0;
        bool OK = false;
        for (long jj = 0; chkStruct (jj); ++ jj)
            if (order [jj] - 1 == hh)
            {
                OK = true;
                break;
            }

        if (! OK)
        {
            string SS = string ("'" + Name + " (Netroute)' the 'order' parameter does not have a unique value for each HRU");
            CRHMException Except (SS.c_str (), TExcept :: ERR);
            LogError (Except);
            throw Except;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::run(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    double Amount = 0.0;
    double Amount_mWQ = 0.0;
    double gw_Amount = 0.0;
    double gw_Amount_mWQ = 0.0;
    for (hh = 0; chkStruct (hh); ++ hh) {
        if (nstep == 1)
        {
            distrib_sum [hh] = 0.0;
            for (long hhh = 0; chkStruct (hhh); ++ hhh) {
                if (distrib_hru [hh] [hhh] < 0.0)
                {
                    const_cast < double ** > (distrib_hru) [hh] [hhh] = - distrib_hru [hh] [hhh] * hru_area [hh];
                }

                distrib_sum [hh] += distrib_hru [hh] [hhh];
            }
            if (distrib_sum [hh] <= 0 && distrib_Basin [hh] <= 0.0)
            {
                const_cast < double * > (distrib_Basin) [hh] = 1;
            }

            distrib_sum [hh] += distrib_Basin [hh];
        }

        Reset_WQ (hh, outflow_diverted, outflow_diverted_conc_lay);
        Reset_WQ (hh, gwoutflow_diverted, gwoutflow_diverted_conc_lay);
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            if (soil_gwDiv == 1)
            {
                soil_gw_Buf [hh] = soil_gw [hh];
                soil_gw_Buf_conc_lay [Sub] [hh] = soil_gw_conc_lay [Sub] [hh];
            }

            if (soil_ssrDiv == 1)
            {
                soil_ssr_Buf [hh] = soil_ssr [hh];
                soil_ssr_Buf_conc_lay [Sub] [hh] = soil_ssr_conc_lay [Sub] [hh];
            }

            if (soil_runoffDiv == 1)
            {
                soil_runoff_Buf [hh] = soil_runoff [hh];
                soil_runoff_Buf_conc_lay [Sub] [hh] = soil_runoff_cWQ_lay [Sub] [hh];
            }

        }
    }
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        if (Sub == 0)
        {
            Save ();
        }

        for (long jj = 0; chkStruct (jj); ++ jj) {
            for (hh = 0; chkStruct (hh); ++ hh) {
                if (order [hh] - 1 == jj)
                {
                    break;
                }

            }
            if (Sub != 0)
            {
                Restore (hh);
            }

            gw_Amount = 0.0;
            gw_Amount_mWQ = 0.0;
            gwinflow [hh] = soil_gw_Buf [hh] * hru_area [hh];
            gwoutflow_diverted [hh] = 0.0;
            if (Sub == numsubstances - 1)
            {
                gwcuminflow [hh] += gwinflow [hh];
                gwcumoutflow [hh] += gwoutflow [hh];
            }

            gwinflow_mWQ_lay [Sub] [hh] = soil_gw_conc_lay [Sub] [hh] * gwinflow [hh];
            gwoutflow_diverted_conc_lay [Sub] [hh] = 0.0;
            for (long hhh = 0; chkStruct (hhh); ++ hhh) {
                if (gwoutflow [hhh] > 0.0 && gwwhereto [hhh] && (abs (gwwhereto [hhh]) - 1 == hh || abs (gwwhereto [hhh]) > nhru))
                {
                    gw_Amount = gwoutflow [hhh];
                    gwoutflow [hhh] = 0.0;
                    gw_Amount_mWQ = gwoutflow_mWQ_lay [Sub] [hhh];
                    if (Sub == numsubstances - 1)
                    {
                        gwoutflow_diverted [hhh] = gw_Amount;
                        gwcumoutflow_diverted [hhh] += gw_Amount;
                    }

                    gwcumoutflow_diverted_mWQ_lay [Sub] [hhh] += gw_Amount_mWQ;
                    if (abs (gwwhereto [hhh]) <= nhru)
                    {
                        gw_Amount /= hru_area [hh];
                        gw_Amount_mWQ /= hru_area [hh];
                        if (gwwhereto [hhh] > 0)
                        {
                            double Excess = soil_rechr_max [hh] + gw_Amount - soil_rechr [hh];
                            double Free = gw_Amount - Excess;
                            double Free_mWQ = gw_Amount_mWQ * Free / gw_Amount;
                            if (Free > 0.0 && ! soil_rechr_ByPass [hh])
                            {
                                if (Free > gw_Amount)
                                {
                                    if (soil_rechr [hh] + gw_Amount > minFlow_WQ)
                                    {
                                        conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + gw_Amount_mWQ;
                                        conc_soil_rechr_lay [Sub] [hh] /= (soil_rechr [hh] + gw_Amount);
                                    }

                                    else
                                    {
                                        conc_soil_rechr_lay [Sub] [hh] = 0.0f;
                                    }

                                    soil_moist_conc_lay [Sub] [hh] = soil_moist_conc_lay [Sub] [hh] * soil_moist [hh] + Amount_mWQ;
                                    soil_moist_conc_lay [Sub] [hh] /= (soil_moist [hh] + gw_Amount);
                                    cum_to_soil_rechr_mWQ_lay [Sub] [hh] += gw_Amount_mWQ;
                                    if (Sub == numsubstances - 1)
                                    {
                                        soil_rechr [hh] += gw_Amount;
                                        soil_moist [hh] += gw_Amount;
                                        cum_to_soil_rechr [hh] += gw_Amount;
                                    }

                                    gw_Amount = 0.0;
                                    gw_Amount_mWQ = 0.0;
                                }

                                else
                                {
                                    gw_Amount_mWQ = gw_Amount_mWQ * Excess / gw_Amount;
                                    gw_Amount = (gw_Amount - Free) * hru_area [hh];
                                    if (soil_rechr_max [hh] > minFlow_WQ)
                                    {
                                        conc_soil_rechr_lay [Sub] [hh] = conc_soil_rechr_lay [Sub] [hh] * soil_rechr [hh] + Free_mWQ;
                                        conc_soil_rechr_lay [Sub] [hh] /= soil_rechr_max [hh];
                                    }

                                    else
                                    {
                                        conc_soil_rechr_lay [Sub] [hh] = 0.0f;
                                    }

                                    soil_moist_conc_lay [Sub] [hh] = soil_moist_conc_lay [Sub] [hh] * soil_moist [hh] + Free_mWQ;
                                    soil_moist_conc_lay [Sub] [hh] /= (soil_moist [hh] + gw_Amount);
                                    cum_to_soil_rechr_mWQ_lay [Sub] [hh] += Free_mWQ;
                                    if (Sub == numsubstances - 1)
                                    {
                                        soil_rechr [hh] = soil_rechr_max [hh];
                                        soil_moist [hh] += Free;
                                        cum_to_soil_rechr [hh] += Free;
                                    }

                                }

                            }

                            Excess = Sdmax [hh] + gw_Amount - Sd [hh];
                            Free = Sdmax [hh] - Sd [hh];
                            Free_mWQ = Amount_mWQ * Free / gw_Amount;
                            if (Free > 0.0 && ! Sd_ByPass [hh] && gw_Amount > 0.0)
                            {
                                if (Free > gw_Amount)
                                {
                                    if (Sd [hh] + gw_Amount > minFlow_WQ)
                                    {
                                        Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + Amount_mWQ / hru_area [hh];
                                        Sd_conc_lay [Sub] [hh] /= (Sd [hh] + gw_Amount);
                                        cum_to_Sd_mWQ_lay [Sub] [hh] += Amount_mWQ / hru_area [hh];
                                    }

                                    else
                                    {
                                        Sd_conc_lay [Sub] [hh] = 0.0f;
                                    }

                                    if (Sub == numsubstances - 1)
                                    {
                                        Sd [hh] += gw_Amount;
                                        cum_to_Sd [hh] += gw_Amount;
                                    }

                                    gw_Amount = 0.0;
                                    gw_Amount_mWQ = 0.0;
                                }

                                else
                                {
                                    gw_Amount_mWQ = gw_Amount_mWQ * (Excess) / gw_Amount;
                                    if (Free_mWQ > minFlow_WQ)
                                    {
                                        Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] * Sd [hh] + Free_mWQ;
                                        cum_to_Sd_mWQ_lay [Sub] [hh] += Free_mWQ;
                                        Sd_conc_lay [Sub] [hh] = Sd_conc_lay [Sub] [hh] / Sdmax [hh];
                                    }

                                    else
                                    {
                                        Sd_conc_lay [Sub] [hh] = 0.0f;
                                    }

                                    if (Sub == numsubstances - 1)
                                    {
                                        gw_Amount = gw_Amount - Free;
                                        Sd [hh] = Sdmax [hh];
                                        cum_to_Sd [hh] += Free;
                                    }

                                }

                            }

                        }

                        else
                        {
                            if (gw [hh] + gw_Amount > minFlow_WQ)
                            {
                                gw_conc_lay [Sub] [hh] = gw_conc_lay [Sub] [hh] * gw [hh] + gw_Amount_mWQ * gw_Amount;
                                gw_conc_lay [Sub] [hh] /= (gw [hh] + gw_Amount);
                            }

                            else
                            {
                                gw_conc_lay [Sub] [hh] = 0.0f;
                            }

                            if (Sub == numsubstances - 1)
                            {
                                gw [hh] += gw_Amount;
                            }

                            gw_Amount = 0.0;
                            gw_Amount_mWQ = 0.0;
                        }

                    }

                    else
                    {
                        if (gw_Amount > 0.0)
                        {
                            if (basinflow [0] + gw_Amount * 1000 > minFlow_WQ)
                            {
                                basinflow_conc_lay [Sub] [0] = basinflow_conc_lay [Sub] [0] * basinflow [0] + gw_Amount_mWQ * 1000;
                                basinflow_conc_lay [Sub] [0] /= (basinflow [0] + gw_Amount * 1000);
                                cumbasinflow_mWQ_lay [Sub] [0] += gw_Amount_mWQ;
                                gwcumoutflow_mWQ_lay [Sub] [hh] += gw_Amount_mWQ;
                            }

                            else
                            {
                                basinflow_conc_lay [Sub] [0] = 0.0f;
                            }

                        }

                        if (Sub == numsubstances - 1)
                        {
                            basinflow [0] += gw_Amount * 1000;
                            HRU_cumbasinflow [hh] += gw_Amount;
                            cumoutflow [hh] += gw_Amount;
                            cumbasinflow [0] += gw_Amount;
                        }

                        gw_Amount = 0.0;
                        gw_Amount_mWQ = 0.0;
                    }

                }

            }
            inflow [hh] = gw_Amount;
            runinflow [hh] = soil_runoff_Buf [hh] * hru_area [hh];
            ssrinflow [hh] = soil_ssr_Buf [hh] * hru_area [hh];
            runinflow_mWQ_lay [Sub] [hh] = soil_runoff_Buf_conc_lay [Sub] [hh] * soil_runoff_Buf [hh] * hru_area [hh];
            runcuminflow_mWQ_lay [Sub] [hh] += runinflow_mWQ_lay [Sub] [hh];
            runcumoutflow_mWQ_lay [Sub] [hh] += runoutflow_mWQ_lay [Sub] [hh];
            ssrinflow_mWQ_lay [Sub] [hh] = soil_ssr_Buf_conc_lay [Sub] [hh] * soil_ssr_Buf [hh] * hru_area [hh];
            ssrcuminflow_mWQ_lay [Sub] [hh] += ssrinflow_mWQ_lay [Sub] [hh];
            ssrcumoutflow_mWQ_lay [Sub] [hh] += ssroutflow_mWQ_lay [Sub] [hh];
            inflow [hh] += runoutflow [hh] + ssroutflow [hh];
            inflow_mWQ_lay [Sub] [hh] = runoutflow_mWQ_lay [Sub] [hh] + ssroutflow_mWQ_lay [Sub] [hh];
            if (Sub == numsubstances - 1)
            {
                runcuminflow [hh] += runinflow [hh];
                runcumoutflow [hh] += runoutflow [hh];
                ssrcuminflow [hh] += ssrinflow [hh];
                ssrcumoutflow [hh] += ssroutflow [hh];
                cuminflow [hh] += inflow [hh];
            }

            if (outflow [hh] > 0.0)
            {
                Amount = outflow [hh];
                Amount_mWQ = outflow_mWQ_lay [Sub] [hh];
                if (Amount > minFlow_WQ)
                {
                    outflow_diverted_conc_lay [Sub] [hh] = Amount_mWQ / Amount;
                    cumoutflow_mWQ_lay [Sub] [hh] += outflow_mWQ_lay [Sub] [hh];
                }

                else
                {
                    outflow_diverted_conc_lay [Sub] [hh] = 0.0f;
                }

                if (Sub == numsubstances - 1)
                {
                    outflow_diverted [hh] += Amount;
                }

                Reset_WQ (hh, Used, Used_mWQ_lay);
                if (distrib_Basin [hh] > 0.0)
                {
                    Used [hh] = Amount * distrib_Basin [hh] / distrib_sum [hh];
                    if (distrib_sum [hh] > minFlow_WQ)
                    {
                        Used_mWQ_lay [Sub] [hh] = Amount_mWQ * distrib_Basin [hh] / distrib_sum [hh];
                    }

                    else
                    {
                        Used_mWQ_lay [Sub] [hh] = 0.0f;
                    }

                    HRU_cumbasinflow_mWQ_lay [Sub] [hh] += Used_mWQ_lay [Sub] [hh];
                    if ((basinflow [0] + Used [hh]) > minFlow_WQ)
                    {
                        basinflow_conc_lay [Sub] [0] = basinflow_conc_lay [Sub] [0] * basinflow [0] + Used_mWQ_lay [Sub] [hh] * 1000;
                        basinflow_conc_lay [Sub] [0] /= (basinflow [0] + Used [hh] * 1000);
                    }

                    else
                    {
                        basinflow_conc_lay [Sub] [0] = 0.0;
                    }

                    if (Sub == numsubstances - 1)
                    {
                        basinflow [0] += Used [hh] * 1000;
                        cumbasinflow [0] += basinflow [0];
                        cumoutflow [hh] += Used [hh];
                        HRU_cumbasinflow [hh] += Used [hh];
                    }

                    Amount = 0.0;
                    Amount_mWQ = 0.0;
                }

                for (long To = 0; chkStruct (To); ++ To) {
                    if (hh != To && distrib_hru [hh] [To] > 0.0)
                    {
                        Amount = outflow [hh] - Used [hh];
                        Amount = Amount / hru_area [To] * distrib_hru [hh] [To] / (distrib_sum [hh] - distrib_Basin [hh]);
                        if (distrib_sum [hh] - distrib_Basin [hh] > minFlow_WQ)
                        {
                            Amount_mWQ = (outflow_mWQ_lay [Sub] [hh] - Used_mWQ_lay [Sub] [hh]) / hru_area [To] * distrib_hru [hh] [To];
                            Amount_mWQ /= (distrib_sum [hh] - distrib_Basin [hh]);
                            Amount_mWQ = 0.0f;
                        }

                        if (preferential_flow [hh])
                        {
                            if (gw [To] + Amount > minFlow_WQ)
                            {
                                gw_conc_lay [Sub] [To] = gw_conc_lay [Sub] [To] * gw [To] + Amount_mWQ;
                                gw_conc_lay [Sub] [To] /= (gw [To] + Amount);
                            }

                            else
                            {
                                gw_conc_lay [Sub] [To] = 0.0f;
                            }

                            if (Sub == numsubstances - 1)
                            {
                                gw [To] += Amount;
                                cum_preferential_flow_to_gw [To] += Amount;
                            }

                            Amount = 0.0;
                            Amount_mWQ = 0.0;
                        }

                        else
                        if (! soil_rechr_ByPass [To] && Amount > 0.0)
                        {
                            if (soil_rechr [To] + Amount >= soil_rechr_max [To])
                            {
                                double Excess = soil_rechr [To] + Amount - soil_rechr_max [To];
                                double Free = Amount - Excess;
                                if (soil_rechr [To] + Amount > minFlow_WQ)
                                {
                                    conc_soil_rechr_lay [Sub] [To] = conc_soil_rechr_lay [Sub] [To] * soil_rechr [To] + Amount_mWQ;
                                    conc_soil_rechr_lay [Sub] [To] /= (soil_rechr [To] + Amount);
                                }

                                else
                                {
                                    conc_soil_rechr_lay [Sub] [To] = 0.0f;
                                }

                                if (soil_lower [To] + soil_rechr [To] + Free > minFlow_WQ)
                                {
                                    soil_moist_conc_lay [Sub] [To] = (conc_soil_lower_lay [Sub] [To] * soil_lower [To] + conc_soil_rechr_lay [Sub] [To] * soil_rechr [To] + Amount_mWQ * Free);
                                    soil_moist_conc_lay [Sub] [To] /= (soil_lower [To] + soil_rechr [To] + Free);
                                }

                                else
                                {
                                    soil_moist_conc_lay [Sub] [To] = 0.0f;
                                }

                                if (Sub == numsubstances - 1)
                                {
                                    soil_rechr [To] += Free;
                                    soil_moist [To] = soil_lower [To] + soil_rechr [To];
                                }

                                Amount = Excess;
                                Amount_mWQ = conc_soil_rechr_lay [Sub] [To] * Excess;
                            }

                            else
                            {
                                if (soil_rechr [To] + Amount > minFlow_WQ)
                                {
                                    conc_soil_rechr_lay [Sub] [To] = conc_soil_rechr_lay [Sub] [To] * soil_rechr [To] + Amount_mWQ;
                                    conc_soil_rechr_lay [Sub] [To] /= (soil_rechr [To] + Amount);
                                }

                                else
                                {
                                    conc_soil_rechr_lay [Sub] [To] = 0.0;
                                }

                                if (soil_lower [To] + soil_rechr [To] + Amount > minFlow_WQ)
                                {
                                    soil_moist_conc_lay [Sub] [To] = (conc_soil_lower_lay [Sub] [To] * soil_lower [To] + conc_soil_rechr_lay [Sub] [To] * (soil_rechr [To] + Amount_mWQ));
                                    soil_moist_conc_lay [Sub] [To] /= (soil_lower [To] + soil_rechr [To] + Amount);
                                }

                                else
                                {
                                    soil_moist_conc_lay [Sub] [To] = 0.0f;
                                }

                                if (Sub == numsubstances - 1)
                                {
                                    soil_rechr [To] = soil_rechr [To] + Amount;
                                    soil_moist [To] = soil_lower [To] + soil_rechr [To];
                                }

                                Amount = 0.0;
                                Amount_mWQ = 0.0;
                            }

                        }

                        else
                        if (! Sd_ByPass [To] && Amount > 0.0)
                        {
                            if (Sd [To] + Amount >= Sdmax [To])
                            {
                                double Excess = Sd [To] + Amount - Sdmax [To];
                                double Free = Amount - Excess;
                                if (Sd [To] + Amount > minFlow_WQ)
                                {
                                    Sd_conc_lay [Sub] [To] = Sd_conc_lay [Sub] [To] * Sd [To] + Amount_mWQ * Free / Amount;
                                    Sd_conc_lay [Sub] [To] /= (Sd [To] + Free);
                                }

                                else
                                {
                                    Sd_conc_lay [Sub] [To] = 0.0f;
                                }

                                cum_to_Sd_mWQ_lay [Sub] [To] += Amount_mWQ * Free / Amount;
                                if (Sub == numsubstances - 1)
                                {
                                    Sd [To] += Free;
                                    cum_to_Sd [To] += Amount;
                                }

                                Amount = Excess;
                                Amount_mWQ = Sd_conc_lay [Sub] [To] * Excess;
                            }

                            else
                            {
                                if (Sd [To] + Amount > minFlow_WQ)
                                {
                                    Sd_conc_lay [Sub] [To] = Sd_conc_lay [Sub] [To] * Sd [To] + Amount_mWQ;
                                    Sd_conc_lay [Sub] [To] /= (Sd [To] + Amount);
                                }

                                else
                                {
                                    Sd_conc_lay [Sub] [To] = 0.0;
                                }

                                if (Sub == numsubstances - 1)
                                {
                                    Sd [To] = Sd [To] + Amount;
                                    cum_to_Sd [To] += Amount;
                                }

                                Amount = 0.0;
                                Amount_mWQ = 0.0;
                            }

                        }

                        if ((redirected_residual [To] + Amount * hru_area [To]) > minFlow_WQ)
                        {
                            redirected_residual_conc_lay [Sub] [To] = redirected_residual_conc_lay [Sub] [To] * redirected_residual [To] + Amount_mWQ * hru_area [To];
                            redirected_residual_conc_lay [Sub] [To] /= (redirected_residual [To] + Amount * hru_area [To]);
                        }

                        else
                        {
                            redirected_residual_conc_lay [Sub] [To] = 0.0f;
                        }

                        if (Sub == numsubstances - 1)
                        {
                            redirected_residual [To] += Amount * hru_area [To];
                        }

                        Amount = 0.0;
                        Amount_mWQ = 0.0;
                    }

                }
            }

            if (nstep == 0 && Sub == numsubstances - 1)
            {
                if (soil_ssrDiv > 1)
                {
                    soil_ssr_Buf [hh] = soil_ssr [hh] / soil_ssrDiv;
                }

                if (soil_runoffDiv > 1)
                {
                    soil_runoff_Buf [hh] = soil_runoff [hh] / soil_runoffDiv;
                }

                if (soil_gwDiv > 1)
                {
                    soil_gw_Buf [hh] = soil_gw [hh] / soil_gwDiv;
                }

            }

            if (outflow [hh] > minFlow_WQ)
            {
                outflow_mWQ_lay [Sub] [hh] = std :: fmax (outflow_mWQ_lay [Sub] [hh], 0.0f);
                outflow_cWQ_lay [Sub] [hh] = outflow_mWQ_lay [Sub] [hh] / outflow [hh];
            }

            else
            {
                outflow_cWQ_lay [Sub] [hh] = 0.0f;
            }

        }
    }
    if (variation == VARIATION_ORG)
    {
        hruDelay -> DoMuskingum ();
        for (long Sub = 0; Sub < numsubstances; ++ Sub)
            hruDelay_mWQ [Sub] -> DoMuskingum ();
    }

    else
    if (variation == VARIATION_1)
    {
        Clark_hruDelay -> DoClark ();
        for (long Sub = 0; Sub < numsubstances; ++ Sub)
            Clark_hruDelay_mWQ [Sub] -> DoClark ();
    }

    runDelay -> DoClark ();
    ssrDelay -> DoClark ();
    gwDelay -> DoClark ();
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        runDelay_mWQ [Sub] -> DoClark ();
        ssrDelay_mWQ [Sub] -> DoClark ();
        gwDelay_mWQ [Sub] -> DoClark ();
    }
    basinflow_s [0] = basinflow [0] * Global :: Freq / 86400.0;
    basingw_s [0] = basingw [0] * Global :: Freq / 86400.0;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::finish(bool good)@@@WQ_CRHM.cpp>");
    double Allcuminflow = 0.0;
    double Allcumoutflow = 0.0;
    double Allcumoutflowdiverted = 0.0;
    double Allcuminflow_mWQ = 0.0;
    double Allcumoutflow_mWQ = 0.0;
    double Allcumoutflowdiverted_mWQ = 0.0;
    double Allgwcuminflow = 0.0;
    double Allgwcumoutflow = 0.0;
    double Allgwcumoutflowdiverted = 0.0;
    double Allgwcuminflow_mWQ = 0.0;
    double Allgwcumoutflow_mWQ = 0.0;
    double Allgwcumoutflowdiverted_mWQ = 0.0;
    double Allssrcuminflow = 0.0;
    double Allssrcumoutflow = 0.0;
    double Allruncuminflow = 0.0;
    double Allruncumoutflow = 0.0;
    double Allssrcuminflow_mWQ = 0.0;
    double Allssrcumoutflow_mWQ = 0.0;
    double Allruncuminflow_mWQ = 0.0;
    double Allruncumoutflow_mWQ = 0.0;
    double AllSdcuminflow = 0.0;
    double Allrechrcuminflow = 0.0;
    double AllSdcuminflow_mWQ = 0.0;
    double Allrechrcuminflow_mWQ = 0.0;
    double AllTotal = 0.0;
    double Total = 0.0;
    string S = string ("*H2O*");
    LogDebug (S.c_str ());
    LogMessage (" ");
    for (hh = 0; chkStruct (); ++ hh) {
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' cuminflow                   (mm) (mm*km^2) (mm*basin): ").c_str (), cuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' cumoutflow                  (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' cumoutflow_diverted         (mm) (mm*km^2) (mm*basin): ").c_str (), cumoutflow_diverted [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        if (variation == VARIATION_1)
        {
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' hruDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), Clark_hruDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        }

        else
        if (variation == VARIATION_ORG)
        {
            LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' hruDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), hruDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        }

        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' ssrcuminflow                (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' ssrcumoutflow               (mm) (mm*km^2) (mm*basin): ").c_str (), ssrcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' ssrDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), ssrDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' runoffcuminflow             (mm) (mm*km^2) (mm*basin): ").c_str (), runcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' runoffcumoutflow            (mm) (mm*km^2) (mm*basin): ").c_str (), runcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' runDelay_in_storage         (mm) (mm*km^2) (mm*basin): ").c_str (), runDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' gwcuminflow                 (mm) (mm*km^2) (mm*basin): ").c_str (), gwcuminflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' gwcumoutflow                (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' gwcumoutflow_diverted       (mm) (mm*km^2) (mm*basin): ").c_str (), gwcumoutflow_diverted [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' gwDelay_in_storage          (mm) (mm*km^2) (mm*basin): ").c_str (), gwDelay -> Left (hh) / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' cum_to_Sd                   (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_Sd [hh], hru_area [hh], basin_area [0], " *** Added to this HRU Sd");
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' cum_to_soil_rechr           (mm) (mm*km^2) (mm*basin): ").c_str (), cum_to_soil_rechr [hh], hru_area [hh], basin_area [0], " *** Added to this HRU recharge");
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' cum_redirected_residual     (mm) (mm*km^2) (mm*basin): ").c_str (), cum_redirected_residual [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' cumbasinflow                (mm) (mm*km^2) (mm*basin): ").c_str (), cumbasinflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_Netroute_M_D)' HRU_cumbasinflow            (mm) (mm*km^2) (mm*basin): ").c_str (), HRU_cumbasinflow [hh] / hru_area [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
        Total = cumoutflow [hh] + gwcumoutflow [hh] - cumbasinflow [hh] - cum_to_soil_rechr [hh] - cum_to_Sd [hh] - gwcumoutflow [hh] + cumoutflow_diverted [hh] + gwcumoutflow_diverted [hh] - cum_redirected_residual [hh];
        Allcuminflow += cuminflow [hh];
        Allcumoutflow += cumoutflow [hh];
        Allgwcuminflow += gwcuminflow [hh];
        Allgwcumoutflow += gwcumoutflow [hh];
        Allssrcumoutflow += ssrcumoutflow [hh];
        Allssrcuminflow += ssrcuminflow [hh];
        Allruncuminflow += runcuminflow [hh];
        Allruncumoutflow += runcumoutflow [hh];
        AllSdcuminflow += cum_to_Sd [hh];
        Allrechrcuminflow += cum_to_soil_rechr [hh];
        Allcumoutflowdiverted += cumoutflow_diverted [hh];
        Allgwcumoutflowdiverted += gwcumoutflow_diverted [hh];
        AllTotal += Total;
    }
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allcuminflow (mm*basin):                ").c_str (), Allcuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allcumoutflow_mWQ (mm*basin):           ").c_str (), Allcumoutflow_mWQ);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allcumoutflowdiverted (mm*basin):       ").c_str (), Allcumoutflowdiverted);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allgwcuminflow (mm*basin):              ").c_str (), Allgwcuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allgwcumoutflow (mm*basin):             ").c_str (), Allgwcumoutflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allgwcumoutflowdiverted (mm*basin):     ").c_str (), Allgwcumoutflowdiverted);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allssrcuminflow (mm*basin):             ").c_str (), Allssrcuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allssrcumoutflow (mm*basin):            ").c_str (), Allssrcumoutflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allruncuminflow (mm*basin):             ").c_str (), Allruncuminflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Allruncumoutflow (mm*basin):            ").c_str (), Allruncumoutflow);
    LogMessage (string ("'" + Name + " (WQ_Netroute_M_D)' Total (mm) (mm*hru) (mm*hru/basin): ").c_str (), Total);
    LogDebug (" ");
    LogDebug (" ");
    if (variation == VARIATION_1)
    {
        delete [] Clark_hruDelay_mWQ;
    }

    if (variation == VARIATION_ORG)
    {
        delete [] hruDelay_mWQ;
    }

    delete [] ssrDelay_mWQ;
    delete [] runDelay_mWQ;
    delete [] gwDelay_mWQ;
    delete ssrDelay;
    delete runDelay;
    delete gwDelay;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::finish(bool good)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: Reset_WQ (long hru, double * var, double ** var_WQ_lay)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::Reset_WQ(long hru, double * var, double ** var_WQ_lay)@@@WQ_CRHM.cpp>");
    var [hru] = 0.0;
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        var_WQ_lay [Sub] [hru] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Reset_WQ(long hru, double * var, double ** var_WQ_lay)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: Set_WQ (const long hru, double * var, double * var_conc, double Amount, double amount_conc)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::Set_WQ(const long hru, double * var, double * var_conc, double Amount, double amount_conc)@@@WQ_CRHM.cpp>");
    var [hru] = Amount;
    if (Amount > 0.0)
    {
        var_conc [hru] = amount_conc;
    }

    else
    {
        var_conc [hru] = 0.0;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Set_WQ(const long hru, double * var, double * var_conc, double Amount, double amount_conc)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: copy_array (double * from, double * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh)
        to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: restore_hru (double * from, double * to, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
    to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: Restore (long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::Restore(long hh)@@@WQ_CRHM.cpp>");
    restore_hru (gwoutflow_0, gwoutflow, hh);
    restore_hru (outflow_0, outflow, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Restore(long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Netroute_M_D :: Save ()
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::Save()@@@WQ_CRHM.cpp>");
    copy_array (gwoutflow, gwoutflow_0);
    copy_array (outflow, outflow_0);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Save()@@@WQ_CRHM.cpp>");
}
ClassWQ_Netroute_M_D * ClassWQ_Netroute_M_D :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_Netroute_M_D (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::klone(string name) const@@@WQ_CRHM.cpp>");
}
double ClassWQ_Netroute_M_D :: Function1 (double * I, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::Function1(double * I, long hh)@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Function1(double * I, long hh)@@@WQ_CRHM.cpp>");
    return runDelay -> ChangeLag (I, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Function1(double * I, long hh)@@@WQ_CRHM.cpp>");
}
double ClassWQ_Netroute_M_D :: Function2 (double * X, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Netroute_M_D::Function2(double * X, long hh)@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Function2(double * X, long hh)@@@WQ_CRHM.cpp>");
    return runDelay -> ChangeStorage (X, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Netroute_M_D::Function2(double * X, long hh)@@@WQ_CRHM.cpp>");
}
ClassWQ_Test_Hype * ClassWQ_Test_Hype :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Test_Hype::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Test_Hype::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_Test_Hype (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Test_Hype::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_Test_Hype :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Test_Hype::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Provides necessary inputs to test the operation of CRHM and Hype separately.'\
                 'Provides necessary inputs to test WQ_soil, WQ_Netroute and Hype.'\
                 'Provides necessary inputs to test Hype alone.'";
    variation_set = VARIATION_1;
    declvar ("soil_moist", TDim :: NHRU, "moisture content of soil of the HRU.", "(mm)", & soil_moist);
    declvar ("soil_rechr", TDim :: NHRU, "moisture content of soil recharge zone of the HRU.", "(mm)", & soil_rechr);
    declparam ("soil_moist_0", TDim :: NHRU, "[60]", "0", "1000", "initial soil moisture.", "()", & soil_moist_0);
    declparam ("soil_rechr_0", TDim :: NHRU, "[30]", "0", "1000", "initial soil recharge.", "()", & soil_rechr_0);
    declparam ("soil_moist_max", TDim :: NHRU, "[375.0]", "0.0", "5000.0", "Maximum available water holding capacity of soil profile.", "(mm)", & soil_moist_max);
    declparam ("soil_rechr_max", TDim :: NHRU, "[375.0]", "0.0", "5000.0", "soil recharge maximum (<= soil_moist_max).", "(mm)", & soil_rechr_max);
    variation_set = VARIATION_ORG;
    declvar ("infil", TDim :: NHRU, "infil", "(mm/int)", & infil);
    declvar ("snowinfil", TDim :: NHRU, "snowinfil", "(mm/int)", & snowinfil);
    declvar ("runoff", TDim :: NHRU, "runoff", "(mm/int)", & runoff);
    declvar ("meltrunoff", TDim :: NHRU, "meltrunoff", "(mm/int)", & meltrunoff);
    declvar ("hru_evap", TDim :: NHRU, "hru_evap", "(mm/int)", & hru_evap);
    declvar ("hru_cum_evap", TDim :: NHRU, "hru_cum_evap", "(mm)", & hru_cum_evap);
    declvar ("hru_cum_actet", TDim :: NHRU, "hru_cum_actet", "(mm)", & hru_cum_actet);
    declvar ("hru_actet", TDim :: NHRU, "hru_actet", "(mm/int)", & hru_actet);
    declvar ("net_rain", TDim :: NHRU, "net_rain", "(mm/int)", & net_rain);
    declvar ("SWE", TDim :: NHRU, "SWE", "(mm)", & SWE);
    declvar ("SWE_max", TDim :: NHRU, "maximum seasonal SWE", "(mm)", & SWE_max);
    declvar ("hru_t", TDim :: NHRU, "hru_t", "(mm/int)", & hru_t);
    declvar ("SWE_conc", TDim :: NDEFN, "SWE_conc", "(mg/l)", & SWE_conc, & SWE_conc_lay, numsubstances);
    declparam ("runoff_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "runoff_0", "(mm/int)", & runoff_0);
    declparam ("infil_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "infil_0", "(mm/int)", & infil_0);
    declparam ("snowinfil_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "snowinfil_0", "(mm/int)", & snowinfil_0);
    declparam ("runoff_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "runoff_0", "(mm/int)", & runoff_0);
    declparam ("meltrunoff_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "meltrunoff_0", "(mm/int)", & meltrunoff_0);
    declparam ("hru_evap_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "hru_evap_0", "(mm/int)", & hru_evap_0);
    declparam ("hru_actet_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "hru_actet_0", "(mm/int)", & hru_actet_0);
    declparam ("hru_cum_evap_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "hru_cum_evap_0", "(mm/int)", & hru_cum_evap_0);
    declparam ("hru_cum_actet_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "hru_cum_actet_0", "(mm/int)", & hru_cum_evap_0);
    declparam ("net_rain_0", TDim :: NHRU, "[0.0]", "0.0", "100.0", "net rain", "(mm/int)", & net_rain_0);
    declparam ("SWE_0", TDim :: NHRU, "[0.0]", "0.0", "500.0", "SWE", "(mm)", & SWE_0);
    declparam ("hru_t_0", TDim :: NHRU, "[20.0]", "-50.0", "100.0", "hru_t_0", "(mm/int)", & hru_t_0);
    declparam ("SWE_conc_0", TDim :: NDEFN, "[0.0]", "0.0", "100.0", "SWE_conc_0", "(mg/l)", & SWE_conc_0, & SWE_conc_lay_0, numsubstances);
    declparam ("Julian_start", TDim :: NHRU, "[30]", "0", "366", "enable input.", "()", & Julian_start);
    declparam ("Julian_end", TDim :: NHRU, "[30]", "0", "366", "disable input.", "()", & Julian_end);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Test_Hype::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Test_Hype :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Test_Hype::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    for (hh = 0; hh < nhru; ++ hh) {
        infil [hh] = 0.0;
        snowinfil [hh] = 0.0;
        runoff [hh] = 0.0;
        meltrunoff [hh] = 0.0;
        hru_evap [hh] = 0.0;
        hru_cum_evap [hh] = 0.0;
        hru_cum_actet [hh] = 0.0;
        hru_actet [hh] = 0.0;
        net_rain [hh] = 0.0;
        SWE [hh] = 0.0;
        SWE_max [hh] = 0.0;
        hru_t [hh] = 0.0;
        SWE_conc [hh] = 0.0;
        if (variation == VARIATION_1)
        {
            soil_moist [hh] = 0.0;
            soil_rechr [hh] = 0.0;
        }

        for (long Sub = 0; Sub < numsubstances; ++ Sub)
            SWE_conc_lay [Sub] [hh] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Test_Hype::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Test_Hype :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Test_Hype::run(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    for (hh = 0; chkStruct (); ++ hh) {
        if (nstep == 1)
        {
            if (Julian_start [hh] <= julian ("now") && julian ("now") < Julian_end [hh])
            {
                infil [hh] = infil_0 [hh];
                snowinfil [hh] = snowinfil_0 [hh];
                runoff [hh] = runoff_0 [hh];
                meltrunoff [hh] = meltrunoff_0 [hh];
                hru_t [hh] = hru_t_0 [hh];
                SWE [hh] = SWE_0 [hh];
                SWE_max [hh] = SWE [hh];
                for (long Sub = 0; Sub < numsubstances; ++ Sub)
                    SWE_conc_lay [Sub] [hh] = SWE_conc_lay_0 [Sub] [hh];
                hru_evap [hh] = hru_evap_0 [hh];
                hru_actet [hh] = hru_actet_0 [hh];
                net_rain [hh] = net_rain_0 [hh];
                if (variation == VARIATION_1)
                {
                    soil_moist [hh] = soil_moist_0 [hh];
                    soil_rechr [hh] = soil_moist_0 [hh];
                }

            }

            else
            {
                infil [hh] = 0.0;
                snowinfil [hh] = 0.0;
                runoff [hh] = 0.0;
                meltrunoff [hh] = 0.0;
                hru_evap [hh] = 0.0;
                hru_cum_evap [hh] = 0.0;
                hru_cum_actet [hh] = 0.0;
                hru_actet [hh] = 0.0;
                net_rain [hh] = 0.0;
            }

        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Test_Hype::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Test_Hype :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Test_Hype::finish(bool good)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh) {
        LogDebug (" ");
    }
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Test_Hype::finish(bool good)@@@WQ_CRHM.cpp>");
}
ClassWQ_pbsmSnobal * ClassWQ_pbsmSnobal :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_pbsmSnobal (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Special \"pbsm\" module compatible with \"snobal\".' \
                 'original version using hru_u,' \
                 'uses hru_Uadjust from walmsley_wind instead of hru_u,' \
                 'using hru_u and a regression to use daily windspeed,' \
                 'uses hru_Uadjust from walmsley_wind instead of hru_u and a regression to use daily windspeed.'";
    variation_set = VARIATION_0 + VARIATION_2;
    declgetvar ("*", "hru_u", "(m/s)", & hru_u);
    variation_set = VARIATION_1 + VARIATION_3;
    declgetvar ("*", "hru_Uadjust", "(m/s)", & hru_Uadjust);
    variation_set = VARIATION_2 + VARIATION_3;
    declparam ("u_D", TDim :: NHRU, "[1.0]", "0.5", "2.0", "Daily windspeed correction", "()", & u_D);
    declparam ("Drift_offset", TDim :: NHRU, "[0.0]", "-100.0", "100.0", "Daily windspeed drift offset correction", "()", & Drift_offset);
    declparam ("Drift_slope", TDim :: NHRU, "[1.0]", "0.5", "2.0", "Daily windspeed drift slope correction", "()", & Drift_slope);
    declparam ("Subl_offset", TDim :: NHRU, "[0.0]", "-100.0", "100.0", "Daily windspeed sublimation offset correction", "()", & Subl_offset);
    declparam ("Subl_slope", TDim :: NHRU, "[1.0]", "0.5", "2.0", "Daily windspeed sublimation slope correction", "()", & Subl_slope);
    variation_set = VARIATION_ORG;
    declstatvar ("SWE_max", TDim :: NHRU, "snow water equivalent seasonal maximum", "(mm)", & SWE_max);
    declvar ("SWE_conc", TDim :: NDEFN, "snow water equivalent", "(mg/l)", & SWE_conc, & SWE_conc_lay, numsubstances);
    declvar ("Subl", TDim :: NHRU, "interval sublimation", "(mm/int)", & Subl);
    declvar ("Subl_conc", TDim :: NDEFN, "interval sublimation", "(mm/int)", & Subl_conc, & Subl_conc_lay, numsubstances);
    declvar ("cumDriftOut", TDim :: NHRU, "cumulative transport from HRU", "(mm)", & cumDriftOut);
    declvar ("cumDriftOut_mWQ", TDim :: NDEFN, "mass solute from HRU", "(mg/l * mm*km^2/int)", & cumDriftOut_mWQ, & cumDriftOut_mWQ_lay, numsubstances);
    declvar ("Drift_out", TDim :: NHRU, "interval transport out of HRU", "(mm/int)", & Drift_out);
    declvar ("Drift_out_conc", TDim :: NDEFN, "interval transport out of HRU", "(mg/l)", & Drift_out_conc, & Drift_out_conc_lay, numsubstances);
    declvar ("hru_subl", TDim :: NHRU, "interval sublimation", "(mm/int)", & Subl);
    declvar ("hru_drift", TDim :: NHRU, "interval composite transport", "(mm/int)", & Drift);
    declvar ("Drift_in", TDim :: NHRU, "interval transport into HRU", "(mm/int)", & Drift_in);
    declvar ("Drift_in_conc", TDim :: NDEFN, "interval transport into HRU", "(mg/l)", & Drift_in_conc, & Drift_in_conc_lay, numsubstances);
    decldiag ("DriftH", TDim :: NHRU, "interval transport", "(mm/int)", & DriftH);
    decldiag ("SublH", TDim :: NHRU, "interval sublimation", "(mm/int)", & SublH);
    decldiag ("BasinSnowLoss", TDim :: BASIN, "transport out of basin", "(mm/int)", & BasinSnowLoss);
    decldiag ("BasinSnowLoss_mWQ", TDim :: NDEF, "transport out of basin", "(mg/l * mm*km^2/int)", & BasinSnowLoss_mWQ, & BasinSnowLoss_mWQ_lay, numsubstances);
    decldiag ("BasinSnowGain", TDim :: BASIN, "cumulative transport to basin estimated from HRU 1", "(mm/int)", & BasinSnowGain);
    decldiag ("BasinSnowGain_mWQ", TDim :: NDEF, "cumulative transport to basin estimated from HRU 1", "(mg/l * mm*km^2/int)", & BasinSnowGain_mWQ, & BasinSnowGain_mWQ_lay, numsubstances);
    declvar ("BasinSnowLoss", TDim :: BASIN, "transport out of basin", "(mm/int)", & BasinSnowLoss);
    declvar ("cumSubl", TDim :: NHRU, "cumulative sublimation", "(mm)", & cumSubl);
    declvar ("cumSubl_mWQ", TDim :: NDEFN, "cumulative sublimation solute", "(mg/l * mm*km^2/int)", & cumSubl_mWQ, & cumSubl_mWQ_lay, numsubstances);
    declvar ("cumDrift", TDim :: NHRU, "cumulative transport from HRU", "(mm)", & cumDrift);
    declvar ("cumBasinSnowLoss", TDim :: BASIN, "cumulative transport out of basin", "(mm)", & cumBasinSnowLoss);
    declvar ("cumBasinSnowLoss_mWQ", TDim :: NDEF, "cumulative mass of solute transport out of basin", "(mg/l * mm*km^2/int)", & cumBasinSnowLoss_mWQ, & cumBasinSnowLoss_mWQ_lay, numsubstances);
    declvar ("cumBasinSnowGain", TDim :: BASIN, "cumulative transport to basin estimated from HRU 1", "(mm)", & cumBasinSnowGain);
    declvar ("cumBasinSnowGain_mWQ", TDim :: NDEF, "cumulative mass of solute transport to basin estimated from HRU 1", "(mg/l * mm*km^2/int)", & cumBasinSnowGain_mWQ, & cumBasinSnowGain_mWQ_lay, numsubstances);
    declvar ("cumDriftIn", TDim :: NHRU, "cumulative transport to HRU", "(mm)", & cumDriftIn);
    declvar ("cumDriftIn_mWQ", TDim :: NDEFN, "cumulative mass of solute transport to HRU", "(mg/l * mm*km^2/int)", & cumDriftIn_mWQ, & cumDriftIn_mWQ_lay, numsubstances);
    decllocal ("hru_basin", TDim :: NHRU, "conversion factor", "()", & hru_basin);
    decldiag ("DrySnow", TDim :: NHRU, "DrySnow", "()", & DrySnow);
    declvar ("SnowAge", TDim :: NHRU, "SnowAge", "()", & SnowAge);
    declvar ("cumSno", TDim :: NHRU, "cumulative snow", "(mm)", & cumSno);
    declvar ("cumSno_mWQ", TDim :: NDEFN, "cumulative mass of solute snow", "(mg/l * mm*km^2/int)", & cumSno_mWQ, & cumSno_mWQ_lay, numsubstances);
    decldiag ("Prob", TDim :: NHRU, "Probability", "()", & Prob);
    decldiag ("snowdepth", TDim :: NHRU, "depth of snow using Gray/Pomeroy", "(m)", & snowdepth);
    decllocal ("SWE_Init", TDim :: NHRU, "initial SWE", "(mm)", & SWE_Init);
    declparam ("fetch", TDim :: NHRU, "[1000.0]", "300.0", "10000.0", "fetch distance", "(m)", & fetch);
    declparam ("Ht", TDim :: NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "vegetation height(m)", "(m)", & Ht);
    declparam ("distrib", TDim :: NHRU, "[0.0, 1.0]", "-10.0", "10.0", "distribution fractions - can sum to 1", "()", & distrib);
    declparam ("N_S", TDim :: NHRU, "[320]", "1", "500", "vegetation number density", "(1/m^2)", & N_S);
    declparam ("A_S", TDim :: NHRU, "[0.003]", "0.0", "2.0", "stalk diameter", "(m)", & A_S);
    declparam ("basin_area", TDim :: BASIN, "3", "1e-6", "1e+09", "total basin area", "(km^2)", & basin_area);
    declparam ("hru_area", TDim :: NHRU, "[1]", "1e-6", "1e+09", "hru area", "(km^2)", & hru_area);
    declparam ("inhibit_evap", TDim :: NHRU, "[0]", "0", "1", "inhibit evaporatation, 1 -> inhibit", "()", & inhibit_evap);
    declparam ("inhibit_bs", TDim :: NHRU, "[0]", "0", "1", "inhibit blowing snow, 1 -> inhibit", "()", & inhibit_bs);
    declparam ("inhibit_subl", TDim :: NHRU, "[0]", "0", "1", "inhibit sublimation, 1 -> inhibit", "()", & inhibit_subl);
    declparam ("rain_conc", TDim :: NDEFN, "0", "0", "1000", "rain solute concentration", "(mg/l)", & rain_conc, & rain_conc_lay, numsubstances);
    declparam ("snow_conc", TDim :: NDEFN, "0", "0", "1000", "snow solute concentration", "(mg/l)", & snow_conc, & snow_conc_lay, numsubstances);
    declparam ("atmos_mWQ", TDim :: NDEFN, "0", "0", "10", "total basin area", "(mg/int??)", & atmos_mWQ, & atmos_mWQ_lay, numsubstances);
    decllocal ("DrySnow_0", TDim :: NHRU, "", "", & DrySnow_0);
    decllocal ("SnowAge_0", TDim :: NHRU, "", "", & SnowAge_0);
    decllocal ("BasinSnowGain_0", TDim :: NHRU, "", "", & BasinSnowGain_0);
    decllocal ("cumBasinSnowGain_0", TDim :: NHRU, "", "", & cumBasinSnowGain_0);
    decllocal ("BasinSnowLoss_0", TDim :: NHRU, "", "", & BasinSnowLoss_0);
    decllocal ("cumBasinSnowLoss_0", TDim :: NHRU, "", "", & cumBasinSnowLoss_0);
    decllocal ("Subl_0", TDim :: NHRU, "", "", & Subl_0);
    decllocal ("Subl_0", TDim :: NHRU, "", "", & Subl_0);
    decllocal ("cumSubl_0", TDim :: NHRU, "", "", & cumSubl_0);
    decllocal ("Drift_in_0", TDim :: NHRU, "", "", & Drift_in_0);
    decllocal ("cumDriftIn_0", TDim :: NHRU, "", "", & cumDriftIn_0);
    decllocal ("Drift_out_0", TDim :: NHRU, "", "", & Drift_out_0);
    decllocal ("cumDriftOut_0", TDim :: NHRU, "", "", & cumDriftOut_0);
    decllocal ("SWE_0", TDim :: NHRU, "", "", & SWE_0);
    decllocal ("SWE_Init_0", TDim :: NHRU, "", "", & SWE_Init_0);
    decllocal ("cumSno_0", TDim :: NHRU, "", "", & cumSno_0);
    declputvar ("*", "SWE", "(kg/m^2)", & SWE);
    declgetvar ("*", "z_s", "(m)", & z_s);
    declgetvar ("*", "rho", "(kg/m^3)", & rho);
    declgetvar ("*", "hru_t", "(°C)", & hru_t);
    declgetvar ("*", "hru_ea", "(kPa)", & hru_ea);
    declgetvar ("*", "hru_newsnow", "()", & hru_newsnow);
    declgetvar ("*", "net_snow", "(mm/int)", & net_snow);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    Reset_WQ (0, cumBasinSnowLoss, cumBasinSnowLoss_mWQ_lay);
    Reset_WQ (0, cumBasinSnowGain, cumBasinSnowGain_mWQ_lay);
    Reset_WQ (0, BasinSnowLoss, BasinSnowLoss_mWQ_lay);
    Reset_WQ (0, BasinSnowGain, BasinSnowGain_mWQ_lay);
    for (hh = 0; hh < nhru; ++ hh) {
        Reset_WQ (hh, Drift_in, Drift_in_conc_lay);
        Reset_WQ (hh, Drift_out, Drift_out_conc_lay);
        Reset_WQ (hh, cumDriftOut, cumDriftOut_mWQ_lay);
        Reset_WQ (hh, cumDriftIn, cumDriftIn_mWQ_lay);
        Reset_WQ (hh, cumSno, cumSno_mWQ_lay);
        Reset_WQ (hh, cumSubl, cumSubl_mWQ_lay);
        for (long Sub = 0; Sub < numsubstances; ++ Sub)
            SWE_conc_lay [Sub] [hh];
        SnowAge [hh] = 0.0;
        DrySnow [hh] = 0;
        snowdepth [hh] = 0.0;
        DriftH [hh] = 0.0;
        SublH [hh] = 0.0;
        Prob [hh] = 0.0;
        hru_basin [hh] = hru_area [hh] / basin_area [0];
        if ((hh > 0) && (Ht [hh] < Ht [hh - 1]) && distrib [hh - 1] > 0)
        {
            CRHMException TExcept (string ("'" + Name + " (pbsmSnobal)' vegetation heights not in ascending order.").c_str (), TExcept :: WARNING);
            LogError (TExcept);
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::run(void)@@@WQ_CRHM.cpp>");
    double Znod, Ustar, Ustn, E_StubHt, Lambda, Ut, Uten_Prob;
    double SumDrift, SumDrift_mWQ, total, transport, transport_mWQ;
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        if (getstep () == 1)
        {
            for (hh = 0; chkStruct (); ++ hh)
                SWE_Init [hh] = SWE [hh];
        }

        if (Sub == 0)
        {
            Save ();
        }

        for (hh = 0; chkStruct (); ++ hh) {
            if (Sub != 0)
            {
                Restore (hh);
            }

            if (variation == VARIATION_ORG || variation == VARIATION_2)
            {
                hru_u_ = hru_u [hh];
            }

            else
            {
                hru_u_ = hru_Uadjust [hh];
            }

            if (variation == VARIATION_2 || variation == VARIATION_3)
            {
                hru_u_ = u_D [hh] * hru_u_;
            }

            Reset_WQ (hh, Drift_in, Drift_in_conc_lay);
            Reset_WQ (hh, Drift_out, Drift_out_conc_lay);
            Reset_WQ (hh, Subl, Subl_conc_lay);
            Drift [hh] = 0.0;
            DriftH [hh] = 0.0;
            SublH [hh] = 0.0;
            Prob [hh] = 0.0;
            if (SWE [hh] > 0.0)
            {
                SWE_conc_lay [Sub] [hh] = ((SWE [hh] - net_snow [hh]) * SWE_conc_lay [Sub] [hh] + snow_conc_lay [Sub] [hh] * net_snow [hh] + atmos_mWQ_lay [Sub] [hh]) / SWE [hh];
            }

            else
            {
                SWE_conc_lay [Sub] [hh] = 0.0;
            }

            if (SWE [hh] > 0.0 && ! inhibit_bs [hh])
            {
                E_StubHt = Ht [hh] - z_s [hh];
                if (E_StubHt < 0.0001)
                {
                    E_StubHt = 0.0001;
                }

                Ustar = 0.02264 * pow (hru_u_, 1.295f);
                if (E_StubHt > 0.01)
                {
                    Znod = (sqr (Ustar) / 163.3f) + 0.5 * N_S [hh] * E_StubHt * A_S [hh];
                    Lambda = N_S [hh] * A_S [hh] * E_StubHt;
                    Ustn = Ustar * sqrt ((PBSM_constants :: Beta * Lambda) / (1.0 + PBSM_constants :: Beta * Lambda));
                    Uten_Prob = (log (10.0 / Znod)) / PBSM_constants :: KARMAN * sqrt (Ustar - Ustn);
                }

                else
                {
                    Uten_Prob = hru_u_;
                }

                bool newsnow = net_snow [hh];
                ProbabilityThresholdNew (SWE [hh], hru_t [hh], Uten_Prob, Prob [hh], Ut, newsnow, SnowAge [hh], DrySnow [hh]);
                if (Prob [hh] > 0.001)
                {
                    Ut = Ut * 0.8;
                    double RH = hru_ea [hh] / Common :: estar (hru_t [hh]);
                    Pbsm (E_StubHt, Ut, DriftH [hh], SublH [hh], hru_t [hh], hru_u_, RH, fetch [hh], N_S [hh], A_S [hh]);
                    if (variation == VARIATION_2 || variation == VARIATION_3)
                    {
                        DriftH [hh] = Drift_offset [hh] + DriftH [hh] * Drift_slope [hh];
                        SublH [hh] = Subl_offset [hh] + SublH [hh] * Subl_slope [hh];
                    }

                    Drift_out [hh] = DriftH [hh] * Prob [hh] / fetch [hh];
                    Drift_out_conc_lay [Sub] [hh] = SWE_conc_lay [Sub] [hh];
                    if (! inhibit_subl [hh])
                    {
                        Subl [hh] = SublH [hh] * Prob [hh];
                    }

                    if (Drift_out [hh] + Subl [hh] > SWE [hh])
                    {
                        if (Drift_out [hh] >= SWE [hh])
                        {
                            Drift_out [hh] = SWE [hh];
                            Subl [hh] = 0.0;
                        }

                        else
                        {
                            Subl [hh] = SWE [hh] - Drift_out [hh];
                        }

                    }

                    cumDriftOut_mWQ_lay [Sub] [hh] += Drift_out_conc_lay [Sub] [hh] * Drift_out [hh];
                    cumDriftOut [hh] += Drift_out [hh];
                    cumSubl [hh] += Subl [hh];
                    cumSubl_mWQ_lay [Sub] [hh] += Subl [hh];
                }

            }

        }
        if (distrib [0] > 0.0)
        {
            double Drft = Drift_out [0] * distrib [0];
            SWE_conc_lay [Sub] [0] = SWE_conc_lay [Sub] [0] * SWE [0] + Drift_out_conc_lay [Sub] [0] * Drft;
            SWE [0] += Drft;
            SWE_conc_lay [Sub] [0] /= SWE [0];
            cumDriftIn [0] += Drft;
            cumDriftIn_mWQ_lay [Sub] [0] += Drift_out_conc_lay [Sub] [0] * Drft;
            cumBasinSnowGain [0] += Drft * hru_basin [0];
        }

        BasinSnowLoss [0] = 0.0;
        long LastN = 0;
        if (! inhibit_bs [0] && nhru == 1)
        {
            BasinSnowLoss [0] = Drift_out [0];
            BasinSnowLoss_mWQ_lay [Sub] [0] += Drift_out_conc_lay [Sub] [0] * Drift_out [0];
            cumBasinSnowLoss [0] += BasinSnowLoss [0];
            cumBasinSnowLoss_mWQ_lay [Sub] [0] += BasinSnowLoss_mWQ_lay [Sub] [0];
        }

        for (long nn = LastN; chkStruct (nn); ++ nn) {
            if (distrib [nn] >= 0.0 && nn + 1 < nhru)
            {
                continue;
            }

            SumDrift = 0.0;
            SumDrift_mWQ = 0.0;
            for (long hhh = LastN; chkStruct (hhh, nn); ++ hhh) {
                SumDrift += Drift_out [hhh] * hru_basin [hhh];
                SumDrift_mWQ += Drift_out [hhh] * hru_basin [hhh] * Drift_out_conc_lay [Sub] [hhh];
            }
            if (SumDrift > 0.0)
            {
                for (long hh = LastN + 1; chkStruct (hh, nn + 1); ++ hh) {
                    if (Ht [hh] > z_s [hh])
                    {
                        SWE_max [hh] = SWE [hh] + rho [hh] * (Ht [hh] - z_s [hh]);
                    }

                    else
                    {
                        SWE_max [hh] = SWE [hh];
                    }

                    if (SWE_max [hh] <= 0.0)
                    {
                        SWE_max [hh] = Ht [hh];
                    }

                    if (hh == nn)
                    {
                        if (distrib [nn] > 0)
                        {
                            double In = SumDrift / hru_basin [hh];
                            if (SWE_max [hh] > SWE [hh] + In)
                            {
                                Drift_in [hh] = In;
                                Drift_in_conc_lay [Sub] [hh] = SumDrift_mWQ / hru_basin [hh] / In;
                                cumDriftIn [hh] += Drift_in [hh];
                                cumDriftIn_mWQ_lay [Sub] [hh] += SumDrift_mWQ / hru_basin [hh];
                                transport = 0.0;
                                transport_mWQ = 0.0;
                            }

                            else
                            if (SWE_max [hh] > SWE [hh])
                            {
                                Drift_in [hh] = SWE_max [hh] - SWE [hh];
                                double used = Drift_in [hh] / In;
                                Drift_in_conc_lay [Sub] [hh] = SumDrift_mWQ * used / Drift_in [hh];
                                cumDriftIn [hh] += Drift_in [hh];
                                cumDriftIn_mWQ_lay [Sub] [hh] += SumDrift_mWQ * used;
                                transport -= (In - (SWE_max [hh] - SWE [hh])) * hru_basin [hh];
                                transport_mWQ -= SumDrift_mWQ * (1.0 - used) * hru_basin [hh];
                            }

                            else
                            {
                                transport = SumDrift;
                                transport_mWQ = SumDrift_mWQ;
                            }

                        }

                        else
                        if (distrib [nn] < 0)
                        {
                            double In = SumDrift / hru_basin [hh];
                            Drift_in [hh] = SumDrift / hru_basin [hh];
                            Drift_in_conc_lay [Sub] [hh] = SumDrift_mWQ / hru_basin [hh] / In;
                            cumDriftIn [hh] += Drift_in [hh];
                            cumDriftIn_mWQ_lay [Sub] [hh] += SumDrift_mWQ / hru_basin [hh];
                            transport = 0.0;
                            transport_mWQ = 0.0;
                        }

                        else
                        {
                            transport = SumDrift;
                            transport_mWQ = SumDrift_mWQ;
                        }

                        BasinSnowLoss [0] += (transport + Drift_out [hh] * hru_basin [hh]);
                        BasinSnowLoss_mWQ_lay [Sub] [0] += (transport_mWQ + Drift_out_conc_lay [Sub] [hh] * Drift_out [hh] * hru_basin [hh]);
                        cumBasinSnowLoss [0] += BasinSnowLoss [0];
                        cumBasinSnowLoss_mWQ_lay [Sub] [0] += BasinSnowLoss_mWQ_lay [Sub] [0];
                    }

                    else
                    if (SWE_max [hh] > SWE [hh] && distrib [hh] > 0.0)
                    {
                        total = 0.0;
                        for (long jj = hh; chkStruct (jj, nn + 1); ++ jj)
                            total += fabs (distrib [jj]);
                        transport = SumDrift * fabs (distrib [hh]) / total / hru_basin [hh];
                        transport_mWQ = SumDrift_mWQ * fabs (distrib [hh]) / total / hru_basin [hh];
                        if (SWE_max [hh] > SWE [hh] + transport)
                        {
                            Drift_in [hh] += transport;
                        }

                        else
                        if (SWE_max [hh] > SWE [hh])
                        {
                            transport = SWE_max [hh] - SWE [hh];
                            Drift_in [hh] += transport;
                        }

                        else
                        {
                            transport = 0.0;
                            transport_mWQ = 0.0;
                        }

                        SumDrift -= transport * hru_basin [hh];
                        SumDrift_mWQ -= transport_mWQ * hru_basin [hh];
                        cumDriftIn [hh] += transport;
                        cumDriftIn_mWQ_lay [Sub] [hh] += transport_mWQ;
                    }

                }
                LastN = nn + 1;
            }

        }
        for (hh = 0; chkStruct (); ++ hh) {
            Drift [hh] = Drift_in [hh] - Drift_out [hh];
            if (SWE [hh] > 0.0)
            {
                const_cast < long * > (inhibit_evap) [hh] = 1;
                snowdepth [hh] = Common :: DepthofSnow (SWE [hh]);
            }

            else
            {
                const_cast < long * > (inhibit_evap) [hh] = 0;
                snowdepth [hh] = 0.0;
            }

        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::finish(bool good)@@@WQ_CRHM.cpp>");
    if (! good)
    {

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::finish(bool good)@@@WQ_CRHM.cpp>");
        return;
    }

    double AllcumSubl = 0.0;
    double AllcumCover = cumBasinSnowGain [0] - cumBasinSnowLoss [0];
    long Sub = 0;
    for (hh = 0; chkStruct (); ++ hh) {
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumSno     (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSno [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumDriftOut(mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftOut [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumDriftIn (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftIn [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumSubl    (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSubl [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumCover   (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSno [hh] + cumDriftIn [hh] - cumDriftOut [hh] - cumSubl [hh], hru_area [hh], basin_area [0], "*** SWE just before melt");
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' SWE        (mm) (mm*hru) (mm*hru/basin): ").c_str (), SWE [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' SWE_change (mm) (mm*hru) (mm*hru/basin): ").c_str (), SWE [hh] - SWE_Init [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
        AllcumSubl += cumSubl [hh] * hru_area [hh];
        AllcumCover += (cumSno [hh] + cumDriftIn [hh] - cumDriftOut [hh] - cumSubl [hh]) * hru_area [hh];
        LogDebug (" ");
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumSno_mWQ     (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumSno_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumDriftOut_mWQ(mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftOut_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (WQ_pbsmSnobal)' cumDriftIn_mWQ (mm) (mm*hru) (mm*hru/basin): ").c_str (), cumDriftIn_mWQ_lay [Sub] [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
    }
    LogMessage (string ("'" + Name + " (WQ_pbsmSnobal)' AllcumSubl  (mm*basin): ").c_str (), AllcumSubl, "*** cumulative sum of all HRUs cumSubl");
    LogMessage (string ("'" + Name + " (WQ_pbsmSnobal)' AllcumCover (mm*basin): ").c_str (), AllcumCover, "*** SWE just before melt cumulative sum of all HRUs cumCover");
    LogDebug (" ");
    LogMessage ("'WQ_pbsm' cumBasinSnowLoss (mm): ", cumBasinSnowLoss [0]);
    LogMessage ("'WQ_pbsm' cumBasinSnowGain (mm): ", cumBasinSnowGain [0]);
    LogDebug (" ");
    LogMessage ("'WQ_pbsm' cumBasinSnowLoss_mWQ (substance) (mm): ", cumBasinSnowLoss_mWQ_lay [Sub] [0]);
    LogMessage ("'WQ_pbsm' cumBasinSnowGain_mWQ (substance) (mm): ", cumBasinSnowGain_mWQ_lay [Sub] [0]);
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::finish(bool good)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: Reset_WQ (long hru, double * var, double ** var_WQ_lay)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::Reset_WQ(long hru, double * var, double ** var_WQ_lay)@@@WQ_CRHM.cpp>");
    var [hru] = 0.0;
    for (long Sub = 0; Sub < numsubstances; ++ Sub)
        var_WQ_lay [Sub] [hru] = 0.0;
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::Reset_WQ(long hru, double * var, double ** var_WQ_lay)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: copy_array (double * from, double * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh)
        to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::copy_array(double * from, double * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: copy_array (long * from, long * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::copy_array(long * from, long * to)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh)
        to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::copy_array(long * from, long * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: copy_basin (double * from, double * to)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::copy_basin(double * from, double * to)@@@WQ_CRHM.cpp>");
    to [0] = from [0];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::copy_basin(double * from, double * to)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: restore_hru (double * from, double * to, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
    to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::restore_hru(double * from, double * to, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: restore_hru (long * from, long * to, long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::restore_hru(long * from, long * to, long hh)@@@WQ_CRHM.cpp>");
    to [hh] = from [hh];
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::restore_hru(long * from, long * to, long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: Restore (long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::Restore(long hh)@@@WQ_CRHM.cpp>");
    restore_hru (DrySnow_0, DrySnow, hh);
    restore_hru (SnowAge_0, SnowAge, hh);
    restore_hru (BasinSnowGain_0, BasinSnowGain, hh);
    restore_hru (cumBasinSnowGain_0, cumBasinSnowGain, hh);
    restore_hru (BasinSnowLoss_0, BasinSnowLoss, hh);
    restore_hru (cumBasinSnowLoss_0, cumBasinSnowLoss, hh);
    restore_hru (SWE_0, SWE, hh);
    restore_hru (SWE_Init_0, SWE_Init, hh);
    restore_hru (cumSno_0, cumSno, hh);
    restore_hru (Drift_in_0, Drift_in, hh);
    restore_hru (cumDriftIn_0, cumDriftIn, hh);
    restore_hru (Drift_out_0, Drift_out, hh);
    restore_hru (cumDriftOut_0, cumDriftOut, hh);
    restore_hru (Subl_0, Subl, hh);
    restore_hru (cumSubl_0, cumSubl, hh);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::Restore(long hh)@@@WQ_CRHM.cpp>");
}
void ClassWQ_pbsmSnobal :: Save ()
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_pbsmSnobal::Save()@@@WQ_CRHM.cpp>");
    copy_array (DrySnow, DrySnow_0);
    copy_array (SnowAge, SnowAge_0);
    copy_basin (BasinSnowGain, BasinSnowGain_0);
    copy_basin (cumBasinSnowGain, cumBasinSnowGain_0);
    copy_basin (BasinSnowLoss, BasinSnowLoss_0);
    copy_basin (cumBasinSnowLoss, cumBasinSnowLoss_0);
    copy_array (SWE, SWE_0);
    copy_array (SWE_Init, SWE_Init_0);
    copy_array (cumSno, cumSno_0);
    copy_array (Drift_in, Drift_in_0);
    copy_array (cumDriftIn, cumDriftIn_0);
    copy_array (Drift_out, Drift_out_0);
    copy_array (cumDriftOut, cumDriftOut_0);
    copy_array (Subl, Subl_0);
    copy_array (cumSubl, cumSubl_0);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_pbsmSnobal::Save()@@@WQ_CRHM.cpp>");
}
ClassWQ_mass_conc * ClassWQ_mass_conc :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_mass_conc::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_mass_conc::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_mass_conc (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_mass_conc::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_mass_conc :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_mass_conc::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'converts _mWQ to concentrations'";
    declvar ("inflow_conc", TDim :: NDEFN, "Mass: inflow from other HRUs", "(mg)", & inflow_conc, & inflow_conc_lay, numsubstances);
    declvar ("outflow_conc", TDim :: NDEFN, "Mass: inflow from other HRUs", "(mg)", & outflow_conc, & outflow_conc_lay, numsubstances);
    declvar ("runflow_conc", TDim :: NDEFN, "Mass: inflow from other HRUs", "(mg)", & runoutflow_conc, & runoutflow_conc_lay, numsubstances);
    declvar ("ssrflow_conc", TDim :: NDEFN, "Mass: inflow from other HRUs", "(mg)", & ssroutflow_conc, & ssroutflow_conc_lay, numsubstances);
    declvar ("gwoutflow_conc", TDim :: NDEFN, "Mass: inflow from other HRUs", "(mg)", & gwoutflow_conc, & gwoutflow_conc_lay, numsubstances);
    declgetvar ("*", "inflow", "(mm/int)", & inflow);
    declgetvar ("*", "inflow_mWQ", "(mg)", & inflow_mWQ, & inflow_mWQ_lay);
    declgetvar ("*", "outflow", "(mm/int)", & outflow);
    declgetvar ("*", "outflow_mWQ", "(mg)", & outflow_mWQ, & outflow_mWQ_lay);
    declgetvar ("*", "outflow_cWQ", "(mg/l)", & outflow_cWQ, & outflow_cWQ_lay);
    declgetvar ("*", "runoutflow", "(mm/int)", & runoutflow);
    declgetvar ("*", "runoutflow_mWQ", "(mg)", & runoutflow_mWQ, & runoutflow_mWQ_lay);
    declgetvar ("*", "ssroutflow", "(mm/int)", & ssroutflow);
    declgetvar ("*", "ssroutflow_mWQ", "(mg)", & ssroutflow_mWQ, & ssroutflow_mWQ_lay);
    declgetvar ("*", "gwoutflow", "(mm/int)", & gwoutflow);
    declgetvar ("*", "gwoutflow_mWQ", "(mg)", & gwoutflow_mWQ, & gwoutflow_mWQ_lay);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_mass_conc::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_mass_conc :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_mass_conc::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    Reset_WQ (inflow_conc_lay);
    Reset_WQ (outflow_conc_lay);
    Reset_WQ (runoutflow_conc_lay);
    Reset_WQ (ssroutflow_conc_lay);
    Reset_WQ (gwoutflow_conc_lay);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_mass_conc::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_mass_conc :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_mass_conc::run(void)@@@WQ_CRHM.cpp>");
    mass_to_conc (inflow, inflow_mWQ_lay, inflow_conc_lay);
    mass_to_conc (outflow, outflow_mWQ_lay, outflow_conc_lay);
    mass_to_conc (runoutflow, runoutflow_mWQ_lay, runoutflow_conc_lay);
    mass_to_conc (ssroutflow, ssroutflow_mWQ_lay, ssroutflow_conc_lay);
    mass_to_conc (gwoutflow, gwoutflow_mWQ_lay, gwoutflow_conc_lay);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_mass_conc::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_mass_conc :: mass_to_conc (const double * var, const double ** var_mWQ, double ** var_conc)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_mass_conc::mass_to_conc(const double * var, const double ** var_mWQ, double ** var_conc)@@@WQ_CRHM.cpp>");
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        for (long hh = 0; hh < nhru; ++ hh) {
            if (var [hh] <= 0.0)
            {
                var_conc [Sub] [hh] = 0.0;
            }

            else
            {
                var_conc [Sub] [hh] = var_mWQ [Sub] [hh] / var [hh];
            }

        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_mass_conc::mass_to_conc(const double * var, const double ** var_mWQ, double ** var_conc)@@@WQ_CRHM.cpp>");
}
void ClassWQ_mass_conc :: Reset_WQ (double ** var_lay)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_mass_conc::Reset_WQ(double ** var_lay)@@@WQ_CRHM.cpp>");
    for (long hh = 0; hh < nhru; ++ hh) {
        for (long Sub = 0; Sub < numsubstances; ++ Sub)
            var_lay [Sub] [hh] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_mass_conc::Reset_WQ(double ** var_lay)@@@WQ_CRHM.cpp>");
}
ClassWQ_Substitute_Hype * ClassWQ_Substitute_Hype :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Substitute_Hype::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Substitute_Hype::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_Substitute_Hype (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Substitute_Hype::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_Substitute_Hype :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Substitute_Hype::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Provides necessary Hype variables conc_soil_rechr, conc_soil_lower and conc_soil_moist to run other WQ modules to run without proper Hype module.'";
    declstatvar ("conc_soil_rechr", TDim :: NDEFN, "concentration of inorganic nitrogen in soil moisture per land-soil", "(mg/l)", & conc_soil_rechr, & conc_soil_rechr_lay, numsubstances);
    declstatvar ("conc_soil_lower", TDim :: NDEFN, "concentration of organic nitrogen in soil moisture per land-soil", "(mg/l)", & conc_soil_lower, & conc_soil_lower_lay, numsubstances);
    declstatvar ("conc_soil_moist", TDim :: NDEFN, "concentration of soluble (reactive) phosphorus, i.e. phosphate in soil moisture per land-soil", "(mg/l)", & conc_soil_moist, & conc_soil_moist_lay, numsubstances);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Substitute_Hype::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Substitute_Hype :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Substitute_Hype::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    for (hh = 0; hh < nhru; ++ hh) {
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            conc_soil_rechr_lay [Sub] [hh] = 0.0;
            conc_soil_lower_lay [Sub] [hh] = 0.0;
            conc_soil_moist_lay [Sub] [hh] = 0.0;
        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Substitute_Hype::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Substitute_Hype :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Substitute_Hype::run(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    for (hh = 0; chkStruct (); ++ hh) {
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Substitute_Hype::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Substitute_Hype :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Substitute_Hype::finish(bool good)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh) {
        LogDebug (" ");
    }
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Substitute_Hype::finish(bool good)@@@WQ_CRHM.cpp>");
}
ClassWQ_Gen_Mass_Var_Soil * ClassWQ_Gen_Mass_Var_Soil :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Soil::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Soil::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_Gen_Mass_Var_Soil (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Soil::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Soil :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Soil::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Generates variable mWQ balances.'";
    declstatvar ("soil_top_change_mWQ", TDim :: NDEFN, "mass of solute soil_rechr change.", "(mg/l * mm*km^2/int)", & soil_top_change_mWQ, & soil_top_change_mWQ_lay, numsubstances);
    declstatvar ("soil_moist_change_mWQ", TDim :: NDEFN, "mass of solute soil_moist change.", "(mg/l * mm*km^2/int)", & soil_moist_change_mWQ, & soil_moist_change_mWQ_lay, numsubstances);
    declstatvar ("soil_bottom_change_mWQ", TDim :: NDEFN, "mass of solute soil_bottom change.", "(mg/l * mm*km^2/int)", & soil_bottom_change_mWQ, & soil_bottom_change_mWQ_lay, numsubstances);
    declstatvar ("soil_ssr_change_mWQ", TDim :: NDEFN, "mass of solute soil_bottom change.", "(mg/l * mm*km^2/int)", & soil_ssr_change_mWQ, & soil_ssr_change_mWQ_lay, numsubstances);
    declstatvar ("Sd_change_mWQ", TDim :: NDEFN, "mass of solute Sd change.", "(mg/l * mm*km^2/int)", & Sd_change_mWQ, & Sd_change_mWQ_lay, numsubstances);
    declstatvar ("gw_change_mWQ", TDim :: NDEFN, "mass of solute gw change.", "(mg/l * mm*km^2/int)", & gw_change_mWQ, & gw_change_mWQ_lay, numsubstances);
    declstatvar ("soil_gw_change_mWQ", TDim :: NDEFN, "mass of solute soil_gw change.", "(mg/l * mm*km^2/int)", & soil_gw_change_mWQ, & soil_gw_change_mWQ_lay, numsubstances);
    declstatvar ("gw_flow_change_mWQ", TDim :: NDEFN, "mass of solute gw_flow change.", "(mg/l * mm*km^2/int)", & gw_flow_change_mWQ, & gw_flow_change_mWQ_lay, numsubstances);
    declstatvar ("infil_act_change_mWQ", TDim :: NDEFN, "mass of solute infil_act change.", "(mg/l * mm*km^2/int)", & infil_act_change_mWQ, & infil_act_change_mWQ_lay, numsubstances);
    declstatvar ("redirected_residual_change_mWQ", TDim :: NDEFN, "mass of solute redirected_residual change.", "(mg/l * mm*km^2/int)", & redirected_residual_change_mWQ, & redirected_residual_change_mWQ_lay, numsubstances);
    declstatvar ("soil_runoff_change_mWQ", TDim :: NDEFN, "mass of solute soil_runoff change.", "(mg/l * mm*km^2/int)", & soil_runoff_change_mWQ, & soil_runoff_change_mWQ_lay, numsubstances);
    declstatvar ("soil_top_mWQ_last", TDim :: NDEFN, "mass of solute soil_rechr.", "(mg/l * mm*km^2/int)", & soil_top_mWQ_last, & soil_top_mWQ_lay_last, numsubstances);
    declstatvar ("soil_bottom_mWQ_last", TDim :: NDEFN, "mass of solute soil_bottom.", "(mmg/l * mm*km^2/intg)", & soil_bottom_mWQ_last, & soil_bottom_mWQ_lay_last, numsubstances);
    declstatvar ("soil_ssr_mWQ_last", TDim :: NDEFN, "mass of solute soil_ssr.", "(mg/l * mm*km^2/int)", & soil_ssr_mWQ_last, & soil_ssr_mWQ_lay_last, numsubstances);
    declstatvar ("Sd_mWQ_last", TDim :: NDEFN, "mass of solute Sd.", "(mg/l * mm*km^2/int)", & Sd_mWQ_last, & Sd_mWQ_lay_last, numsubstances);
    declstatvar ("gw_mWQ_last", TDim :: NDEFN, "mass of solute gw.", "(mg/l * mm*km^2/int)", & gw_mWQ_last, & gw_mWQ_lay_last, numsubstances);
    declstatvar ("soil_gw_mWQ_last", TDim :: NDEFN, "mass of solute soil_gw.", "(mg/l * mm*km^2/int)", & soil_gw_mWQ_last, & soil_gw_mWQ_lay_last, numsubstances);
    declstatvar ("gw_flow_mWQ_last", TDim :: NDEFN, "mass of solute gw_flow.", "(mg/l * mm*km^2/int)", & gw_flow_mWQ_last, & gw_flow_mWQ_lay_last, numsubstances);
    declstatvar ("infil_act_mWQ_last", TDim :: NDEFN, "mass of solute infil_act.", "(mg/l * mm*km^2/int)", & infil_act_mWQ_last, & infil_act_mWQ_lay_last, numsubstances);
    declstatvar ("redirected_residual_mWQ_last", TDim :: NDEFN, "mass of solute redirected_residual.", "(mg/l * mm*km^2/int)", & redirected_residual_mWQ_last, & redirected_residual_mWQ_lay_last, numsubstances);
    declstatvar ("soil_runoff_mWQ_last", TDim :: NDEFN, "mass of solute soil_runoff.", "(mg/l * mm*km^2/int)", & soil_runoff_mWQ_last, & soil_runoff_mWQ_lay_last, numsubstances);
    declgetvar ("*", "soil_rechr", "(mm)", & soil_rechr);
    declgetvar ("*", "soil_lower", "(mm)", & soil_lower);
    declgetvar ("*", "soil_moist", "(mm)", & soil_moist);
    declgetvar ("*", "soil_ssr", "(mm)", & soil_ssr);
    declgetvar ("*", "Sd", "(mm)", & Sd);
    declgetvar ("*", "gw", "(mm)", & gw);
    declgetvar ("*", "soil_gw", "(mm)", & soil_gw);
    declgetvar ("*", "gw_flow", "(mm)", & gw_flow);
    declgetvar ("*", "infil_act", "(mm)", & infil_act);
    declgetvar ("*", "redirected_residual", "(mm)", & redirected_residual);
    declgetvar ("*", "soil_runoff", "(mm)", & soil_runoff);
    declgetvar ("*", "conc_soil_rechr", "(mg/l)", & conc_soil_rechr, & conc_soil_rechr_lay);
    declgetvar ("*", "conc_soil_lower", "(mg/l)", & conc_soil_lower, & conc_soil_lower_lay);
    declgetvar ("*", "soil_ssr_conc", "(mg/l)", & soil_ssr_conc, & soil_ssr_conc_lay);
    declgetvar ("*", "Sd_conc", "(mg/l)", & Sd_conc, & Sd_conc_lay);
    declgetvar ("*", "gw_conc", "(mg/l)", & gw_conc, & gw_conc_lay);
    declgetvar ("*", "soil_gw_conc", "(mg/l)", & soil_gw_conc, & soil_gw_conc_lay);
    declgetvar ("*", "gw_flow_conc", "(mg/l)", & gw_flow_conc, & gw_flow_conc_lay);
    declgetvar ("*", "infil_act_conc", "(mg/l)", & infil_act_conc, & infil_act_conc_lay);
    declgetvar ("*", "redirected_residual_conc", "(mg/l)", & redirected_residual_conc, & redirected_residual_conc_lay);
    declgetvar ("*", "soil_runoff_cWQ", "(mg/l)", & soil_runoff_cWQ, & soil_runoff_cWQ_lay);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Soil::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Soil :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Soil::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    for (hh = 0; chkStruct (); ++ hh) {
        for (long Sub = 0; Sub < numsubstances; ++ Sub) {
            soil_top_change_mWQ_lay [Sub] [hh] = 0.0;
            soil_moist_change_mWQ_lay [Sub] [hh] = 0.0;
            soil_bottom_change_mWQ_lay [Sub] [hh] = 0.0;
            soil_ssr_change_mWQ_lay [Sub] [hh] = 0.0;
            Sd_change_mWQ_lay [Sub] [hh] = 0.0;
            gw_change_mWQ_lay [Sub] [hh] = 0.0;
            soil_gw_change_mWQ_lay [Sub] [hh] = 0.0;
            gw_flow_change_mWQ_lay [Sub] [hh] = 0.0;
            infil_act_change_mWQ_lay [Sub] [hh] = 0.0;
            redirected_residual_change_mWQ_lay [Sub] [hh] = 0.0;
            soil_runoff_change_mWQ_lay [Sub] [hh] = 0.0;
            soil_top_mWQ_lay_last [Sub] [hh] = soil_rechr [hh] * conc_soil_rechr_lay [Sub] [hh];
            soil_bottom_mWQ_lay_last [Sub] [hh] = soil_lower [hh] * conc_soil_lower_lay [Sub] [hh];
            soil_ssr_mWQ_lay_last [Sub] [hh] = soil_ssr [hh] * soil_ssr_conc_lay [Sub] [hh];
            Sd_mWQ_lay_last [Sub] [hh] = Sd [hh] * Sd_conc_lay [Sub] [hh];
            gw_mWQ_lay_last [Sub] [hh] = gw [hh] * gw_conc_lay [Sub] [hh];
            soil_gw_mWQ_lay_last [Sub] [hh] = soil_gw [hh] * soil_gw_conc_lay [Sub] [hh];
            gw_flow_mWQ_lay_last [Sub] [hh] = gw_flow [hh] * gw_flow_conc_lay [Sub] [hh];
            infil_act_mWQ_lay_last [Sub] [hh] = infil_act [hh] * infil_act_conc_lay [Sub] [hh];
            redirected_residual_mWQ_lay_last [Sub] [hh] = redirected_residual [hh] * redirected_residual_conc_lay [Sub] [hh];
            soil_runoff_mWQ_lay_last [Sub] [hh] = soil_runoff [hh] * soil_runoff_cWQ_lay [Sub] [hh];
        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Soil::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Soil :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Soil::run(void)@@@WQ_CRHM.cpp>");
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        double temp;
        for (hh = 0; chkStruct (); ++ hh) {
            temp = soil_rechr [hh] * conc_soil_rechr_lay [Sub] [hh];
            soil_top_change_mWQ_lay [Sub] [hh] = temp - soil_top_mWQ_lay_last [Sub] [hh];
            soil_top_mWQ_lay_last [Sub] [hh] = temp;
            temp = soil_lower [hh] * conc_soil_lower_lay [Sub] [hh];
            soil_bottom_change_mWQ_lay [Sub] [hh] = temp - soil_bottom_mWQ_lay_last [Sub] [hh];
            soil_bottom_mWQ_lay_last [Sub] [hh] = temp;
            soil_moist_change_mWQ_lay [Sub] [hh] = soil_top_change_mWQ_lay [Sub] [hh] + soil_bottom_change_mWQ_lay [Sub] [hh];
            temp = soil_ssr [hh] * soil_ssr_conc_lay [Sub] [hh];
            soil_ssr_change_mWQ_lay [Sub] [hh] = temp - soil_ssr_mWQ_lay_last [Sub] [hh];
            soil_ssr_mWQ_lay_last [Sub] [hh] = temp;
            temp = Sd [hh] * Sd_conc_lay [Sub] [hh];
            Sd_change_mWQ_lay [Sub] [hh] = temp - Sd_mWQ_lay_last [Sub] [hh];
            Sd_mWQ_lay_last [Sub] [hh] = temp;
            temp = gw [hh] * gw_conc_lay [Sub] [hh];
            gw_change_mWQ_lay [Sub] [hh] = temp - gw_mWQ_lay_last [Sub] [hh];
            gw_mWQ_lay_last [Sub] [hh] = temp;
            temp = soil_gw [hh] * soil_gw_conc_lay [Sub] [hh];
            soil_gw_change_mWQ_lay [Sub] [hh] = temp - soil_gw_mWQ_lay_last [Sub] [hh];
            soil_gw_mWQ_lay_last [Sub] [hh] = temp;
            temp = gw_flow [hh] * gw_flow_conc_lay [Sub] [hh];
            gw_flow_change_mWQ_lay [Sub] [hh] = temp - gw_flow_mWQ_lay_last [Sub] [hh];
            gw_flow_mWQ_lay_last [Sub] [hh] = temp;
            temp = infil_act [hh] * infil_act_conc_lay [Sub] [hh];
            infil_act_change_mWQ_lay [Sub] [hh] = temp - infil_act_mWQ_lay_last [Sub] [hh];
            infil_act_mWQ_lay_last [Sub] [hh] = temp;
            temp = redirected_residual [hh] * redirected_residual_conc_lay [Sub] [hh];
            redirected_residual_change_mWQ_lay [Sub] [hh] = temp - redirected_residual_mWQ_lay_last [Sub] [hh];
            redirected_residual_mWQ_lay_last [Sub] [hh] = temp;
            temp = soil_runoff [hh] * soil_runoff_cWQ_lay [Sub] [hh];
            soil_runoff_change_mWQ_lay [Sub] [hh] = temp - soil_runoff_mWQ_lay_last [Sub] [hh];
            soil_runoff_mWQ_lay_last [Sub] [hh] = temp;
        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Soil::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Soil :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Soil::finish(bool good)@@@WQ_CRHM.cpp>");
    LogDebug ("**WQ_Gen_Mass_Var_Soil**");
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Soil::finish(bool good)@@@WQ_CRHM.cpp>");
}
ClassGrow_crops_annually * ClassGrow_crops_annually :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_crops_annually::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassGrow_crops_annually (name);
InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassGrow_crops_annually :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_crops_annually::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Misc modules.' \
                 'nothing,' \
                 'Hype fertilizer.'\
                 'Grow_crops.'\
                 'Both.'";
    variation_set = VARIATION_1 + VARIATION_3;
    declvar ("Fert_N_amount_up", TDim :: NHRU, "current hru N fertilizer amount", "(kg/km^2)", & Fert_N_amount_up);
    declvar ("Fert_P_amount_up", TDim :: NHRU, "current hru P fertilizer amount", "(kg/km^2)", & Fert_P_amount_up);
    declvar ("Man_N_amount_up", TDim :: NHRU, "current hru N manure amount", "(kg/km^2)", & Man_N_amount_up);
    declvar ("Man_P_amount_up", TDim :: NHRU, "current hru P manure amount", "(kg/km^2)", & Man_P_amount_up);
    declvar ("Res_N_amount_up", TDim :: NHRU, "current hru N residue amount", "(kg/km^2)", & Res_N_amount_up);
    declvar ("Res_P_amount_up", TDim :: NHRU, "current hru P residue amount", "(kg/km^2)", & Res_P_amount_up);
    declvar ("Fert_N_amount_down", TDim :: NHRU, "current hru N fertilizer amount", "(kg/km^2)", & Fert_N_amount_down);
    declvar ("Fert_P_amount_down", TDim :: NHRU, "current hru P fertilizer amount", "(kg/km^2)", & Fert_P_amount_down);
    declvar ("Man_N_amount_down", TDim :: NHRU, "current hru N manure amount", "(kg/km^2)", & Man_N_amount_down);
    declvar ("Man_P_amount_down", TDim :: NHRU, "current hru P manure amount", "(kg/km^2)", & Man_P_amount_down);
    declvar ("Res_N_amount_down", TDim :: NHRU, "current hru N residue amount", "(kg/km^2)", & Res_N_amount_down);
    declvar ("Res_P_amount_down", TDim :: NHRU, "current hru P residue amount", "(kg/km^2)", & Res_P_amount_down);
    declvar ("Fertperiod", TDim :: NHRU, "current period for spreading fertilizer and manure amount", "()", & Fertperiod);
    declvar ("Litterperiod", TDim :: NHRU, "current period for residue amount", "()", & Litterperiod);
    declvar ("LockOut", TDim :: NHRU, "prevents changes of fertilizer and manure amounts on multi day periods", "()", & LockOut);
    ObsCnt_N_up = declreadobs ("Fert_N_up", TDim :: NHRU, "annual N fertilizer dates and amount", "(kg/km^2)", & Fert_N_up, HRU_OBS_Q, true);
    ObsCnt_P_up = declreadobs ("Fert_P_up", TDim :: NHRU, "annual P fertilizer dates and amount", "(kg/km^2)", & Fert_P_up, HRU_OBS_Q, true);
    ObsCntMan_N_up = declreadobs ("Man_N_up", TDim :: NHRU, "annual N Manure dates and amount", "(kg/km^2)", & Man_N_up, HRU_OBS_Q, true);
    ObsCntMan_P_up = declreadobs ("Man_P_up", TDim :: NHRU, "annual P Manure dates and amount", "(kg/km^2)", & Man_P_up, HRU_OBS_Q, true);
    ObsCntRes_N_up = declreadobs ("Res_N_up", TDim :: NHRU, "annual N Residues dates and amount", "(kg/km^2)", & Res_N_up, HRU_OBS_Q, true);
    ObsCntRes_P_up = declreadobs ("Res_P_up", TDim :: NHRU, "annual P Residues dates and amount", "(kg/km^2)", & Res_P_up, HRU_OBS_Q, true);
    ObsCnt_N_down = declreadobs ("Fert_N_down", TDim :: NHRU, "annual N fertilizer dates and amount", "(kg/km^2)", & Fert_N_down, HRU_OBS_Q, true);
    ObsCnt_P_down = declreadobs ("Fert_P_down", TDim :: NHRU, "annual P fertilizer dates and amount", "(kg/km^2)", & Fert_P_down, HRU_OBS_Q, true);
    ObsCntMan_N_down = declreadobs ("Man_N_down", TDim :: NHRU, "annual N Manure dates and amount", "(kg/km^2)", & Man_N_down, HRU_OBS_Q, true);
    ObsCntMan_P_down = declreadobs ("Man_P_down", TDim :: NHRU, "annual P Manure dates and amount", "(kg/km^2)", & Man_P_down, HRU_OBS_Q, true);
    ObsCntRes_N_down = declreadobs ("Res_N_down", TDim :: NHRU, "annual N Residues dates and amount", "(kg/km^2)", & Res_N_down, HRU_OBS_Q, true);
    ObsCntRes_P_down = declreadobs ("Res_P_down", TDim :: NHRU, "annual P Residues dates and amount", "(kg/km^2)", & Res_P_down, HRU_OBS_Q, true);
    ObsCnt_fertperiod = declreadobs ("Fert_period", TDim :: NHRU, "spreading period for feritilzer and manure", "(d)", & Fert_period, HRU_OBS_Q, true);
    declparam ("Ag_YearStart", TDim :: NHRU, "[0]", "0", "10", " suggestions for northern hemisphere - 0, southern hemisphere - 183", "()", & Ag_YearStart);
    declputparam ("*", "fertNamount_up", "(kg/km^2)", & fertNamount_up);
    declputparam ("*", "fertNamount_down", "(kg/km^2)", & fertNamount_down);
    declputparam ("*", "fertPamount_up", "(kg/km^2)", & fertPamount_up);
    declputparam ("*", "fertPamount_down", "(kg/km^2)", & fertPamount_down);
    declputparam ("*", "manNamount_up", "(kg/km^2)", & manNamount_up);
    declputparam ("*", "manNamount_down", "(kg/km^2)", & manNamount_down);
    declputparam ("*", "manPamount_up", "(kg/km^2)", & manPamount_up);
    declputparam ("*", "manPamount_down", "(kg/km^2)", & manPamount_down);
    declputparam ("*", "resNamount_up", "(kg/km^2)", & resNamount_up);
    declputparam ("*", "resNamount_down", "(kg/km^2)", & resNamount_down);
    declputparam ("*", "resPamount_up", "(kg/km^2)", & resPamount_up);
    declputparam ("*", "resPamount_down", "(kg/km^2)", & resPamount_down);
    declputparam ("*", "fertday_up", "()", & fertday_up);
    declputparam ("*", "fertday_down", "()", & fertday_down);
    declputparam ("*", "manday_up", "()", & manday_up);
    declputparam ("*", "manday_down", "()", & manday_down);
    declputparam ("*", "resday_up", "()", & resday_up);
    declputparam ("*", "resday_down", "()", & resday_down);
    declputparam ("*", "fertperiod", "()", & fertperiod);
    declputparam ("*", "litterperiod", "()", & litterperiod);
    variation_set = VARIATION_2 + VARIATION_3;
    declparam ("Htmax", TDim :: NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "maximum vegetation height", "(m)", & Htmax);
    declparam ("Init_Crop_Ht_1", TDim :: NHRU, "[0.1]", "0.001", "100.0", "initial crop height (1)", "(m)", & Init_Crop_Ht_1);
    declparam ("Crop_Grow_Rate_1", TDim :: NHRU, "[0.8]", "0.0", "1.0", "crop growth rate (1)", "(m/d)", & Crop_Grow_Rate_1);
    declparam ("JCrop_Start_1", TDim :: NHRU, "[250]", "0", "366", "start Julian day (1); JCrop_Start_1 = 0 if no crop", "()", & JCrop_Start_1);
    declparam ("JCrop_Harvest_1", TDim :: NHRU, "[228]", "0", "366", "harvest Julian day (1); JCrop_Harvest_1 = 0 if no crop", "()", & JCrop_Harvest_1);
    declparam ("Crop_Htmax_1", TDim :: NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "maximum vegetation height (1)", "(m)", & Crop_Htmax_1);
    declparam ("Init_Crop_Ht_2", TDim :: NHRU, "[0.1]", "0.001", "100.0", "initial crop height (2)", "(m)", & Init_Crop_Ht_2);
    declparam ("Crop_Grow_Rate_2", TDim :: NHRU, "[0.8]", "0.0", "1.0", "crop growth rate (2)", "(m/d)", & Crop_Grow_Rate_2);
    declparam ("JCrop_Start_2", TDim :: NHRU, "[250]", "0", "366", "start Julian day (2); JCrop_Start_2 = 0 if no crop", "()", & JCrop_Start_2);
    declparam ("JCrop_Harvest_2", TDim :: NHRU, "[228]", "0", "366", "harvest Julian day (2); JCrop_Harvest_2 = 0 if no crop", "()", & JCrop_Harvest_2);
    declparam ("Crop_Htmax_2", TDim :: NHRU, "[0.1, 0.25, 1.0]", "0.001", "100.0", "maximum vegetation height (2)", "(m)", & Crop_Htmax_2);
    declputparam ("*", "Ht", "(m)", & Ht);
    variation_set = VARIATION_ORG;
InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassGrow_crops_annually :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_crops_annually::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    if (variation == VARIATION_1 || variation == VARIATION_3)
    {
        if (ObsCnt_N_up > - 1)
        {
            CRHMException TExcept ("Handling N fertilizer from (Fert_N_up) observation.", TExcept :: WARNING);
            LogError (TExcept);
        }

        if (ObsCnt_P_up > - 1)
        {
            CRHMException TExcept ("Handling P fertilizer from (Fert_P_up) observation.", TExcept :: WARNING);
            LogError (TExcept);
        }

        if (ObsCntMan_N_up > - 1)
        {
            CRHMException TExcept ("Handling N manure from (Man_N_up) observation.", TExcept :: WARNING);
            LogError (TExcept);
        }

        if (ObsCntMan_P_up > - 1)
        {
            CRHMException TExcept ("Handling P manure from (Man_P_up) observation.", TExcept :: WARNING);
            LogError (TExcept);
        }

        if (ObsCntRes_N_up > - 1)
        {
            CRHMException TExcept ("Handling N residues from (Res_N_up) observation.", TExcept :: WARNING);
            LogError (TExcept);
        }

        if (ObsCntRes_P_up > - 1)
        {
            CRHMException TExcept ("Handling P residues from (Res_P_up) observation.", TExcept :: WARNING);
            LogError (TExcept);
        }

        if (ObsCnt_fertperiod > - 1)
        {
            CRHMException TExcept ("Handling fertilizer and manure period (Fert_period) observation.", TExcept :: WARNING);
            LogError (TExcept);
        }

        for (hh = 0; hh < nhru; ++ hh) {
            Fert_N_amount_up [hh] = 0.0;
            Fert_P_amount_up [hh] = 0.0;
            Man_N_amount_up [hh] = 0.0;
            Man_P_amount_up [hh] = 0.0;
            Fert_N_amount_down [hh] = 0.0;
            Fert_P_amount_down [hh] = 0.0;
            Man_N_amount_down [hh] = 0.0;
            Man_P_amount_down [hh] = 0.0;
            Res_N_amount_up [hh] = 0.0;
            Res_P_amount_up [hh] = 0.0;
            Fertperiod [hh] = 1;
            Litterperiod [hh] = 1;
            LockOut [hh] = 0;
        }
    }

    if (variation == VARIATION_2 || variation == VARIATION_3)
    {
        if (Good_Dates (JCrop_Start_1))
        {
            CRHMException TExcept ("JCrop_Start_1 dates out of range!", TExcept :: TERMINATE);
            LogError (TExcept);
        }

        if (Good_Dates (JCrop_Harvest_1))
        {
            CRHMException TExcept ("JCrop_Harvest_1 dates out of range!", TExcept :: TERMINATE);
            LogError (TExcept);
        }

        if (Good_Dates (JCrop_Start_2))
        {
            CRHMException TExcept ("JCrop_Start_2 dates out of range!", TExcept :: TERMINATE);
            LogError (TExcept);
        }

        if (Good_Dates (JCrop_Harvest_2))
        {
            CRHMException TExcept ("JCrop_Harvest_2 dates out of range!", TExcept :: TERMINATE);
            LogError (TExcept);
        }

        for (hh = 0; hh < nhru; ++ hh)
            const_cast < double * > (Ht) [hh] = Init_Crop_Ht_1 [hh];
    }

InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::init(void)@@@WQ_CRHM.cpp>");
}
void ClassGrow_crops_annually :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_crops_annually::run(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    long today = julian ("now");
    bool Good_N, Good_P;
    if (step == 1)
    {
        for (hh = 0; chkStruct (); ++ hh) {
            if (variation == VARIATION_1 || variation == VARIATION_3)
            {
                if (ObsCnt_N_up >= hh)
                {
                    declputparam ("*", "fertNamount_up", "(kg/km^2)", & fertNamount_up);
                    const_cast < double * > (fertNamount_up) [hh] = 0.0;
                }

                if (ObsCnt_P_up >= hh)
                {
                    declputparam ("*", "fertPamount_up", "(kg/km^2)", & fertPamount_up);
                    const_cast < double * > (fertPamount_up) [hh] = 0.0;
                }

                if (ObsCntMan_N_up >= hh)
                {
                    declputparam ("*", "manNamount_up", "(kg/km^2)", & manNamount_up);
                    const_cast < double * > (manNamount_up) [hh] = 0.0;
                }

                if (ObsCntMan_P_up >= hh)
                {
                    declputparam ("*", "manPamount_up", "(kg/km^2)", & manPamount_up);
                    const_cast < double * > (manPamount_up) [hh] = 0.0;
                }

                if (ObsCntRes_N_up >= hh)
                {
                    declputparam ("*", "resNamount_up", "(kg/km^2)", & resNamount_up);
                    const_cast < double * > (resNamount_up) [hh] = 0.0;
                }

                if (ObsCntRes_P_up >= hh)
                {
                    declputparam ("*", "resPamount_up", "(kg/km^2)", & resPamount_up);
                    const_cast < double * > (resPamount_up) [hh] = 0.0;
                }

                if (ObsCnt_N_up >= hh || ObsCnt_P_up >= hh)
                {
                    declputparam ("*", "fertday_up", "(d)", & fertday_up);
                    const_cast < double * > (fertday_up) [hh] = 0;
                }

                if (ObsCntMan_N_up >= hh || ObsCntMan_P_up >= hh)
                {
                    declputparam ("*", "manday_up", "(d)", & manday_up);
                    const_cast < double * > (manday_up) [hh] = 0;
                }

                if (ObsCnt_N_down >= hh)
                {
                    declputparam ("*", "fertNamount_down", "(kg/km^2)", & fertNamount_down);
                    const_cast < double * > (fertNamount_down) [hh] = 0.0;
                }

                if (ObsCnt_P_down >= hh)
                {
                    declputparam ("*", "fertPamount_down", "(kg/km^2)", & fertPamount_down);
                    const_cast < double * > (fertPamount_down) [hh] = 0.0;
                }

                if (ObsCntMan_N_down >= hh)
                {
                    declputparam ("*", "manNamount_down", "(kg/km^2)", & manNamount_down);
                    const_cast < double * > (manNamount_down) [hh] = 0.0;
                }

                if (ObsCntMan_P_down >= hh)
                {
                    declputparam ("*", "manPamount_down", "(kg/km^2)", & manPamount_down);
                    const_cast < double * > (manPamount_down) [hh] = 0.0;
                }

                if (ObsCntRes_N_down >= hh)
                {
                    declputparam ("*", "resNamount_down", "(kg/km^2)", & resNamount_down);
                    const_cast < double * > (resNamount_down) [hh] = 0.0;
                }

                if (ObsCntRes_P_down >= hh)
                {
                    declputparam ("*", "resPamount_down", "(kg/km^2)", & resPamount_down);
                    const_cast < double * > (resPamount_down) [hh] = 0.0;
                }

                if (ObsCnt_N_down >= hh || ObsCnt_P_down >= hh)
                {
                    declputparam ("*", "fertday_down", "(d)", & fertday_down);
                    const_cast < double * > (fertday_down) [hh] = 0;
                }

                if (ObsCntMan_N_down >= hh || ObsCntMan_P_down >= hh)
                {
                    declputparam ("*", "manday_down", "(d)", & manday_down);
                    const_cast < double * > (manday_down) [hh] = 0;
                }

                if (ObsCnt_fertperiod >= hh)
                {
                    declputparam ("*", "fertperiod", "(d)", & fertperiod);
                    const_cast < long * > (fertperiod) [hh] = 0;
                }

                declputparam ("*", "litterperiod", "(d)", & litterperiod);
            }

        }
    }

    if (nstep == 1)
    {
        for (hh = 0; chkStruct (); ++ hh) {
            if (variation == VARIATION_1 || variation == VARIATION_3)
            {
                if (Ag_YearStart [hh] == today)
                {
                    fertday_up [hh] = 0;
                    fertday_down [hh] = 0;
                    manday_up [hh] = 0;
                    manday_down [hh] = 0;
                    resday_up [hh] = 0;
                    resday_down [hh] = 0;
                }

                -- LockOut [hh];
                if (LockOut [hh] <= 0)
                {
                    if (ObsCnt_fertperiod > 0)
                    {
                        if (! (Fert_period [hh] >= fLimit || ! Fert_period [hh]))
                        {
                            fertperiod [hh] = 1;
                            if (Fert_period [hh] > 1 && Fert_period [hh] < 366)
                            {
                                fertperiod [hh] = Fert_period [hh];
                            }

                            Fertperiod [hh] = Fert_period [hh];
                        }

                    }

                    else
                    {
                        Fertperiod [hh] = fertperiod [hh];
                    }

                    Good_N = true, Good_P = true;
                    if (ObsCnt_N_up >= hh || ObsCnt_P_up >= hh)
                    {
                        if (ObsCnt_N_up < 0 || Fert_N_up [hh] >= fLimit || ! Fert_N_up [hh] || Fert_N_up [hh] <= 0.0)
                        {
                            Fert_N_amount_up [hh] = 0.0;
                            fertNamount_up [hh] = 0.0;
                            Good_N = false;
                        }

                        if (ObsCnt_P_up < 0 || Fert_P_up [hh] >= fLimit || ! Fert_P_up [hh] || Fert_P_up [hh] <= 0.0)
                        {
                            Fert_P_amount_up [hh] = 0.0;
                            fertPamount_up [hh] = 0.0;
                            Good_P = false;
                        }

                        if (Good_N || Good_P)
                        {
                            LockOut [hh] = fertperiod [hh];
                            if (Good_N && Fert_N_up [hh] > 0.0)
                            {
                                fertNamount_up [hh] = Fert_N_up [hh] / Global :: Freq;
                                Fert_N_amount_up [hh] = fertNamount_up [hh];
                            }

                            if (Good_P && Fert_P_up [hh] > 0.0)
                            {
                                fertPamount_up [hh] = Fert_P_up [hh] / Global :: Freq;
                                Fert_P_amount_up [hh] = fertPamount_up [hh];
                            }

                            fertday_up [hh] = today;
                        }

                        else
                        {
                            fertday_up [hh] = 0;
                        }

                    }

                    Good_N = true, Good_P = true;
                    if (ObsCnt_N_down >= hh || ObsCnt_P_down >= hh)
                    {
                        if (ObsCnt_N_down < 0 || Fert_N_down [hh] >= fLimit || ! Fert_N_down [hh] || Fert_N_down [hh] <= 0.0)
                        {
                            Fert_N_amount_down [hh] = 0.0;
                            fertNamount_down [hh] = 0.0;
                            Good_N = false;
                        }

                        if (ObsCnt_P_down < 0 || Fert_P_down [hh] >= fLimit || ! Fert_P_down [hh] || Fert_P_down [hh] <= 0.0)
                        {
                            Fert_P_amount_down [hh] = 0.0;
                            fertPamount_down [hh] = 0.0;
                            Good_P = false;
                        }

                        if (Good_N || Good_P)
                        {
                            LockOut [hh] = fertperiod [hh];
                            if (Good_N && Fert_N_down [hh] > 0.0)
                            {
                                fertNamount_down [hh] = Fert_N_down [hh] / Global :: Freq;
                                Fert_N_amount_down [hh] = fertNamount_down [hh];
                            }

                            if (Good_P && Fert_P_down [hh] > 0.0)
                            {
                                fertPamount_down [hh] = Fert_P_down [hh] / Global :: Freq;
                                Fert_P_amount_down [hh] = fertPamount_down [hh];
                            }

                            fertday_down [hh] = today;
                        }

                        else
                        {
                            fertday_down [hh] = 0;
                        }

                    }

                    Good_N = true, Good_P = true;
                    if (ObsCntMan_N_up >= hh || ObsCntMan_P_up >= hh)
                    {
                        if (ObsCntMan_N_up < 0 || Man_N_up [hh] >= fLimit || ! Man_N_up [hh] || Man_N_up [hh] <= 0.0)
                        {
                            Man_N_amount_up [hh] = 0.0;
                            manNamount_up [hh] = 0.0;
                            Good_N = false;
                        }

                        if (ObsCntMan_P_up < 0 || Man_P_up [hh] >= fLimit || ! Man_P_up [hh] || Man_P_up [hh] <= 0.0)
                        {
                            Man_P_amount_up [hh] = 0.0;
                            manPamount_up [hh] = 0.0;
                            Good_P = false;
                        }

                        if (Good_N || Good_P)
                        {
                            LockOut [hh] = fertperiod [hh];
                            if (Good_N && Man_N_up [hh] > 0.0)
                            {
                                manNamount_up [hh] = Man_N_up [hh] / Global :: Freq;
                                Man_N_amount_up [hh] = manNamount_up [hh];
                            }

                            if (Good_P && Man_P_up [hh] > 0.0)
                            {
                                manPamount_up [hh] = Man_P_up [hh] / Global :: Freq;
                                Man_P_amount_up [hh] = manPamount_up [hh];
                            }

                            manday_up [hh] = today;
                        }

                        else
                        {
                            manday_up [hh] = 0;
                        }

                    }

                    Good_N = true, Good_P = true;
                    if (ObsCntMan_N_down >= hh || ObsCntMan_P_down >= hh)
                    {
                        if (ObsCntMan_N_down < 0 || Man_N_down [hh] >= fLimit || ! Man_N_down [hh] || Man_N_down [hh] <= 0.0)
                        {
                            Man_N_amount_down [hh] = 0.0;
                            manNamount_down [hh] = 0.0;
                            Good_N = false;
                        }

                        if (ObsCntMan_P_down < 0 || Man_P_down [hh] >= fLimit || ! Man_P_down [hh] || Man_P_down [hh] <= 0.0)
                        {
                            Man_P_amount_down [hh] = 0.0;
                            manPamount_down [hh] = 0.0;
                            Good_P = false;
                        }

                        if (Good_N || Good_P)
                        {
                            LockOut [hh] = fertperiod [hh];
                            if (Good_N && Man_N_down [hh] > 0.0)
                            {
                                manNamount_down [hh] = Man_N_down [hh] / Global :: Freq;
                                Man_N_amount_down [hh] = manNamount_down [hh];
                            }

                            if (Good_P && Man_P_down [hh] > 0.0)
                            {
                                manPamount_down [hh] = Man_P_down [hh] / Global :: Freq;
                                Man_P_amount_down [hh] = manPamount_down [hh];
                            }

                            manday_down [hh] = today;
                        }

                        else
                        {
                            manday_down [hh] = 0;
                        }

                    }

                    Good_N = true, Good_P = true;
                    if (ObsCntRes_N_up >= hh && ObsCntRes_P_up >= hh)
                    {
                        if (ObsCntRes_N_up < 0 || Res_N_up [hh] >= fLimit || ! Res_N_up [hh] || Res_N_up [hh] <= 0.0)
                        {
                            Res_N_amount_up [hh] = 0.0;
                            resNamount_up [hh] = 0.0;
                            Good_N = false;
                        }

                        if (ObsCntRes_P_up < 0 || Res_P_up [hh] >= fLimit || ! Res_P_up [hh] || Res_P_up [hh] <= 0.0)
                        {
                            Res_P_amount_up [hh] = 0.0;
                            resPamount_up [hh] = 0.0;
                            Good_P = false;
                        }

                        if (Good_N || Good_P)
                        {
                            LockOut [hh] = fertperiod [hh];
                            if (Good_N && Man_N_up [hh])
                            {
                                resNamount_up [hh] = Res_N_up [hh] / Global :: Freq;
                                Res_N_amount_up [hh] = Res_N_up [hh];
                            }

                            if (Good_P && Man_P_up [hh])
                            {
                                resPamount_up [hh] = Res_P_up [hh] / Global :: Freq;
                                Res_P_amount_up [hh] = Res_P_up [hh];
                            }

                            resday_up [hh] = today;
                        }

                        else
                        {
                            resday_up [hh] = 0;
                        }

                    }

                    Good_N = true, Good_P = true;
                    if (ObsCntRes_N_down >= hh && ObsCntRes_P_down >= hh)
                    {
                        if (ObsCntRes_N_down < 0 || Res_N_down [hh] >= fLimit || ! Res_N_down [hh] || Res_N_down [hh] <= 0.0)
                        {
                            Res_N_amount_down [hh] = 0.0;
                            resNamount_down [hh] = 0.0;
                            Good_N = false;
                        }

                        if (ObsCntRes_P_down < 0 || Res_P_down [hh] >= fLimit || ! Res_P_down [hh] || Res_P_down [hh] <= 0.0)
                        {
                            Res_P_amount_down [hh] = 0.0;
                            resPamount_down [hh] = 0.0;
                            Good_P = false;
                        }

                        if (Good_N || Good_P)
                        {
                            LockOut [hh] = fertperiod [hh];
                            if (Good_N && Man_N_down [hh])
                            {
                                resNamount_down [hh] = Res_N_down [hh] / Global :: Freq;
                                Res_N_amount_down [hh] = Res_N_down [hh];
                            }

                            if (Good_P && Man_P_down [hh])
                            {
                                resPamount_down [hh] = Res_P_down [hh] / Global :: Freq;
                                Res_P_amount_down [hh] = Res_P_up [hh];
                            }

                            resday_down [hh] = today;
                        }

                        else
                        {
                            resday_down [hh] = 0;
                        }

                    }

                }

            }

            if (variation == VARIATION_2 || variation == VARIATION_3)
            {
                if (today == JCrop_Start_1 [hh])
                {
                    const_cast < double * > (Ht) [hh] = Init_Crop_Ht_1 [hh];
                }

                if (JCrop_Start_1 [hh] != 0 && today >= JCrop_Start_1 [hh] && today <= JCrop_Harvest_1 [hh])
                {
                    const_cast < double * > (Htmax) [hh] = Crop_Htmax_1 [hh];
                }

                if (Ht [hh] < Crop_Htmax_1 [hh])
                {
                    const_cast < double * > (Ht) [hh] = Ht [hh] + Crop_Grow_Rate_1 [hh];
                }

                else
                {
                    const_cast < double * > (Ht) [hh] = Htmax [hh];
                }

                if (today == JCrop_Harvest_1 [hh])
                {
                    const_cast < double * > (Ht) [hh] = Init_Crop_Ht_1 [hh];
                }

                if (today == JCrop_Start_2 [hh])
                {
                    const_cast < double * > (Ht) [hh] = Init_Crop_Ht_2 [hh];
                }

                if (Ht [hh] < Crop_Htmax_2 [hh])
                {
                    const_cast < double * > (Ht) [hh] = Ht [hh] + Crop_Grow_Rate_2 [hh];
                }

                else
                {
                    const_cast < double * > (Ht) [hh] = Htmax [hh];
                }

                if (today == JCrop_Harvest_2 [hh])
                {
                    const_cast < double * > (Ht) [hh] = Init_Crop_Ht_2 [hh];
                }

            }

        }
    }

InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::run(void)@@@WQ_CRHM.cpp>");
}
void ClassGrow_crops_annually :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_crops_annually::finish(bool good)@@@WQ_CRHM.cpp>");
    for (hh = 0; chkStruct (); ++ hh) {
        LogDebug (" ");
    }
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::finish(bool good)@@@WQ_CRHM.cpp>");
}
bool ClassGrow_crops_annually :: Good_Dates (const double * dates)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGrow_crops_annually::Good_Dates(const double * dates)@@@WQ_CRHM.cpp>");
    for (hh = 0; hh < nhru; ++ hh) {
        if (dates [hh] > 366 || dates [hh] < 0)
        {

InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::Good_Dates(const double * dates)@@@WQ_CRHM.cpp>");
            return false;
        }

        ;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::Good_Dates(const double * dates)@@@WQ_CRHM.cpp>");
    return true;
InstrumentLogger::instance()->log_instrument_log("</ClassGrow_crops_annually::Good_Dates(const double * dates)@@@WQ_CRHM.cpp>");
}
ClassWQ_Gen_Mass_Var_Netroute * ClassWQ_Gen_Mass_Var_Netroute :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Netroute::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Netroute::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassWQ_Gen_Mass_Var_Netroute (name);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Netroute::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Netroute :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Netroute::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Generates variable mWQ balances in Netroute*.'";
    declvar ("basinflow_change_mWQ", TDim :: NDEFN, "mass of solute basinflow change.", "(mg)", & basinflow_change_mWQ, & basinflow_change_mWQ_lay, numsubstances);
    declvar ("basingw_change_mWQ", TDim :: NDEFN, "mass of solute basingwflow change.", "(mg)", & basingw_change_mWQ, & basingw_change_mWQ_lay, numsubstances);
    declvar ("outflow_diverted_change_mWQ", TDim :: NDEFN, "mass of solute outflow_diverted change.", "(mg)", & outflow_diverted_change_mWQ, & outflow_diverted_change_mWQ_lay, numsubstances);
    declvar ("gwoutflow_diverted_change_mWQ", TDim :: NDEFN, "mass of solute outflow_diverted change.", "(mg)", & gwoutflow_diverted_change_mWQ, & gwoutflow_diverted_change_mWQ_lay, numsubstances);
    declvar ("basinflow_conc_last", TDim :: NDEFN, "last mass of solute basinflow.", "(mg)", & basinflow_mWQ_last, & basinflow_mWQ_lay_last, numsubstances);
    declvar ("basingw_conc_last", TDim :: NDEFN, "last mass of solute basin gw flow.", "(mg)", & basingw_mWQ_last, & basingw_mWQ_lay_last, numsubstances);
    declvar ("outflow_diverted_conc_last", TDim :: NDEFN, "last mass of solute outflow_diverted.", "(mg)", & outflow_diverted_mWQ_last, & outflow_diverted_mWQ_lay_last, numsubstances);
    declvar ("gwoutflow_diverted_conc_last", TDim :: NDEFN, "last mass of solute outflow_diverted.", "(mg)", & gwoutflow_diverted_mWQ_last, & gwoutflow_diverted_mWQ_lay_last, numsubstances);
    declputvar ("*", "basinflow", "(mm)", & basinflow);
    declputvar ("*", "basinflow_conc", "(mg/l)", & basinflow_conc, & basinflow_conc_lay);
    declputvar ("*", "basingw", "(mm)", & basingw);
    declputvar ("*", "basingw_conc", "(mg/l)", & basingw_conc, & basingw_conc_lay);
    declputvar ("*", "outflow_diverted", "(mm)", & outflow_diverted);
    declputvar ("*", "outflow_diverted_conc", "(mg/l)", & outflow_diverted_conc, & outflow_diverted_conc_lay);
    declputvar ("*", "gwoutflow_diverted", "(mm)", & gwoutflow_diverted);
    declputvar ("*", "gwoutflow_diverted_conc", "(mg/l)", & gwoutflow_diverted_conc, & gwoutflow_diverted_conc_lay);
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Netroute::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Netroute :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Netroute::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        basinflow_change_mWQ_lay [Sub] [0] = 0.0;
        basinflow_mWQ_lay_last [Sub] [0] = 0.0;
        basingw_change_mWQ_lay [Sub] [0] = 0.0;
        basingw_mWQ_lay_last [Sub] [0] = 0.0;
        for (hh = 0; chkStruct (); ++ hh) {
            outflow_diverted_change_mWQ_lay [Sub] [hh] = 0.0;
            outflow_diverted_mWQ_lay_last [Sub] [hh] = outflow_diverted [hh] * outflow_diverted_conc_lay [Sub] [hh];
            gwoutflow_diverted_change_mWQ_lay [Sub] [hh] = 0.0;
            gwoutflow_diverted_mWQ_lay_last [Sub] [hh] = gwoutflow_diverted [hh] * gwoutflow_diverted_conc_lay [Sub] [hh];
        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Netroute::init(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Netroute :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Netroute::run(void)@@@WQ_CRHM.cpp>");
    double temp;
    for (long Sub = 0; Sub < numsubstances; ++ Sub) {
        temp = basinflow [0] * basinflow_conc_lay [Sub] [0];
        basinflow_change_mWQ_lay [Sub] [0] = temp - basinflow_mWQ_lay_last [Sub] [0];
        basinflow_mWQ_lay_last [Sub] [0] = temp;
        temp = basingw [0] * basingw_conc_lay [Sub] [0];
        basingw_change_mWQ_lay [Sub] [0] = temp - basingw_mWQ_lay_last [Sub] [0];
        basingw_mWQ_lay_last [Sub] [0] = temp;
        for (hh = 0; chkStruct (); ++ hh) {
            temp = outflow_diverted [hh] * outflow_diverted_conc_lay [Sub] [hh];
            outflow_diverted_change_mWQ_lay [Sub] [hh] = temp - outflow_diverted_mWQ_lay_last [Sub] [hh];
            outflow_diverted_mWQ_lay_last [Sub] [hh] = temp;
            temp = gwoutflow_diverted_conc_lay [Sub] [hh];
            gwoutflow_diverted_change_mWQ_lay [Sub] [hh] = temp - gwoutflow_diverted_mWQ_lay_last [Sub] [hh];
            gwoutflow_diverted_mWQ_lay_last [Sub] [hh] = temp;
        }
    }
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Netroute::run(void)@@@WQ_CRHM.cpp>");
}
void ClassWQ_Gen_Mass_Var_Netroute :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassWQ_Gen_Mass_Var_Netroute::finish(bool good)@@@WQ_CRHM.cpp>");
    LogDebug ("**WQ_Gen_Mass_Var_Netroute**");
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassWQ_Gen_Mass_Var_Netroute::finish(bool good)@@@WQ_CRHM.cpp>");
}
Classlapse_rate_Monthly_Mod * Classlapse_rate_Monthly_Mod :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<Classlapse_rate_Monthly_Mod::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</Classlapse_rate_Monthly_Mod::klone(string name) const@@@WQ_CRHM.cpp>");
    return new Classlapse_rate_Monthly_Mod (name);
InstrumentLogger::instance()->log_instrument_log("</Classlapse_rate_Monthly_Mod::klone(string name) const@@@WQ_CRHM.cpp>");
}
void Classlapse_rate_Monthly_Mod :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<Classlapse_rate_Monthly_Mod::decl(void)@@@WQ_CRHM.cpp>");
    declvar ("lapse_rate_Mthly", TDim :: NHRU, "temperature lapse rate Mthly.", "(°C/100m)", & lapse_rate_Mthly);
    declvar ("lapse_rate", TDim :: NHRU, "temperature lapse rate.", "(°C/100m)", & lapse_rate);
InstrumentLogger::instance()->log_instrument_log("</Classlapse_rate_Monthly_Mod::decl(void)@@@WQ_CRHM.cpp>");
}
void Classlapse_rate_Monthly_Mod :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<Classlapse_rate_Monthly_Mod::init(void)@@@WQ_CRHM.cpp>");
    for (hh = 0; hh < nhru; ++ hh) {
        lapse_rate [hh] = 0.0;
        lapse_rate_Mthly [hh] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</Classlapse_rate_Monthly_Mod::init(void)@@@WQ_CRHM.cpp>");
}
void Classlapse_rate_Monthly_Mod :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<Classlapse_rate_Monthly_Mod::run(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    long JULIAN = julian ("now");
    if (nstep == 0)
    {
        for (hh = 0; hh < nhru; ++ hh) {
            if (JULIAN >= 274 && JULIAN < 305)
            {
                lapse_rate [hh] = 0.81;
            }

            else
            if (JULIAN >= 305 && JULIAN < 335)
            {
                lapse_rate [hh] = 0.83;
            }

            else
            if (JULIAN >= 335 && JULIAN < 365)
            {
                lapse_rate [hh] = 0.73;
            }

            else
            if (JULIAN >= 1 && JULIAN < 32)
            {
                lapse_rate [hh] = 0.66;
            }

            else
            if (JULIAN >= 32 && JULIAN < 60)
            {
                lapse_rate [hh] = 0.72;
            }

            else
            if (JULIAN >= 60 && JULIAN < 91)
            {
                lapse_rate [hh] = 0.8;
            }

            else
            if (JULIAN >= 91 && JULIAN < 121)
            {
                lapse_rate [hh] = 0.85;
            }

            else
            if (JULIAN >= 121 && JULIAN < 152)
            {
                lapse_rate [hh] = 0.8;
            }

            else
            if (JULIAN >= 152 && JULIAN < 182)
            {
                lapse_rate [hh] = 0.78;
            }

            else
            if (JULIAN >= 182 && JULIAN < 213)
            {
                lapse_rate [hh] = 0.6;
            }

            else
            if (JULIAN >= 213 && JULIAN < 244)
            {
                lapse_rate [hh] = 0.57;
            }

            else
            if (JULIAN >= 244 && JULIAN < 274)
            {
                lapse_rate [hh] = 0.76;
            }

            lapse_rate_Mthly [hh] = lapse_rate [hh];
        }
    }

InstrumentLogger::instance()->log_instrument_log("</Classlapse_rate_Monthly_Mod::run(void)@@@WQ_CRHM.cpp>");
}
ClassGlacier_melt_debris_cover_estimate_Mod * ClassGlacier_melt_debris_cover_estimate_Mod :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_melt_debris_cover_estimate_Mod::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_melt_debris_cover_estimate_Mod::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassGlacier_melt_debris_cover_estimate_Mod (name);
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_melt_debris_cover_estimate_Mod::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassGlacier_melt_debris_cover_estimate_Mod :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_melt_debris_cover_estimate_Mod::decl(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    declvar ("lagT", TDim :: NHRU, "lagged temperature.", "(°C)", & lagT);
    declvar ("lagSW", TDim :: NHRU, "lagged shortwave radiation.", "(W/m^2)", & lagSW);
    declvar ("Gmelt_energy", TDim :: NHRU, "Energy available for glacier melt.", "(W/m^2)", & Gmelt_energy);
    declvar ("den_air", TDim :: NHRU, "air density.", "(kg/m^-3)", & den_air);
    declvar ("K_kat", TDim :: NHRU, "katabatic bulk exchange coefficient.", "()", & K_kat);
    declvar ("Cp_water", TDim :: NHRU, "specific heat of water.", "(J/(kg*K))", & Cp_water);
    declvar ("Xmelt", TDim :: NHRU, "Experimental Firn and Ice melt at daily timestepe.", "(mm/d)", & Xmelt);
    declvar ("Xmelt_int", TDim :: NHRU, "Experimental Firn and Ice melt at interval timestep.", "(mm/int)", & Xmelt_int);
    declvar ("Xmelt_acc", TDim :: NHRU, "Experimental Firn and Ice melt daily accumulator.", "(mm)", & Xmelt_acc);
    declvar ("Xmelt_V5", TDim :: NHRU, "Version 5 Firn and Ice melt daily variable.", "(mm)", & Xmelt_V5);
    declparam ("debris_h", TDim :: NHRU, "[0.0]", "0", "10", "debris thickness.", "(m)", & debris_h);
    declparam ("T_threshold", TDim :: NHRU, "[1.0]", "-30", "30", "threshold temperature for melt to occur.", "(°C)", & T_threshold);
    declreadobs ("lagT1", TDim :: NHRU, "lagged temperature 1", "(°C)", & lagT1, HRU_OBS_Q, true);
    declreadobs ("lagT2", TDim :: NHRU, "lagged temperature 2", "(°C)", & lagT2, HRU_OBS_Q, true);
    declreadobs ("lagT3", TDim :: NHRU, "lagged temperature 3", "(°C)", & lagT3, HRU_OBS_Q, true);
    declreadobs ("lagT4", TDim :: NHRU, "lagged temperature 4", "(°C)", & lagT4, HRU_OBS_Q, true);
    declreadobs ("lagSW1", TDim :: NHRU, "lagged shortwave radiation 1", "(W/m^2)", & lagSW1, HRU_OBS_Q, true);
    declreadobs ("lagSW2", TDim :: NHRU, "lagged shortwave radiation 2", "(W/m^2)", & lagSW2, HRU_OBS_Q, true);
    declreadobs ("lagSW3", TDim :: NHRU, "lagged shortwave radiation 3", "(W/m^2)", & lagSW3, HRU_OBS_Q, true);
    declreadobs ("lagSW4", TDim :: NHRU, "lagged shortwave radiation 4", "(W/m^2)", & lagSW4, HRU_OBS_Q, true);
    declreadobs ("lagSW5", TDim :: NHRU, "lagged shortwave radiation 5", "(W/m^2)", & lagSW5, HRU_OBS_Q, true);
    declreadobs ("lagSW6", TDim :: NHRU, "lagged shortwave radiation 6", "(W/m^2)", & lagSW6, HRU_OBS_Q, true);
    declgetvar ("*", "Albedo", "()", & Albedo);
    declgetvar ("*", "T_rain", "(°C)", & T_rain);
    declgetvar ("*", "hru_t", "(°C)", & hru_t);
    declgetvar ("*", "hru_ea", "(kPa)", & hru_ea);
    declgetvar ("*", "net_rain", "(mm/int)", & net_rain);
    declgetvar ("*", "Pa", "(kPa)", & Pa);
    declgetvar ("*", "Qnsn_Var", "(W/m^2*int)", & Qnsn_Var);
    declputvar ("*", "Gn_ebsm", "(W/m^2)", & Gn_ebsm);
    declputvar ("*", "Gh_ebsm", "(W/m^2)", & Gh_ebsm);
    declputvar ("*", "Ge_ebsm", "(W/m^2)", & Ge_ebsm);
    declputvar ("*", "Gp_ebsm", "(W/m^2)", & Gp_ebsm);
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_melt_debris_cover_estimate_Mod::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassGlacier_melt_debris_cover_estimate_Mod :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_melt_debris_cover_estimate_Mod::init(void)@@@WQ_CRHM.cpp>");
    for (hh = 0; hh < nhru; ++ hh) {
        lagT [hh] = 0.0;
        lagSW [hh] = 0.0;
        Gmelt_energy [hh] = 0.0;
        den_air [hh] = 0.0;
        Cp_water [hh] = 0.0;
        Xmelt [hh] = 0.0;
        Xmelt_int [hh] = 0.0;
        Xmelt_acc [hh] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_melt_debris_cover_estimate_Mod::init(void)@@@WQ_CRHM.cpp>");
}
void ClassGlacier_melt_debris_cover_estimate_Mod :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_melt_debris_cover_estimate_Mod::run(void)@@@WQ_CRHM.cpp>");
    const double Cp_air = 1006;
    const double MOL_wt_ratio_h2o_to_air = 0.622;
    const double Lv = 2.5e6;
    const double Ts_glacier = 273.15;
    const double e_s = 0.6113;
    const double Pr = 5;
    const double Katabatic = 0.0004;
    const double g = 9.8;
    const double FREEZE = 273.15;
    const double Cp_W0 = 4217.7;
    const double rho_h2o = 1000.0;
    const double B = 0.95;
    const double lapse_rate = 0.005;
    const double LH_fusion = 333.5;
    double TF = 0.0;
    double SRF = 0.0;
    long nstep = getstep ();
    nstep = nstep % Global :: Freq;
    for (hh = 0; hh < nhru; ++ hh) {
        if (debris_h [hh] <= 0)
        {
            lagT [hh] = hru_t [hh];
            lagSW [hh] = Qnsn_Var [hh];
            TF = 0;
            SRF = 0;
            Gn_ebsm [hh] = Qnsn_Var [hh];
            den_air [hh] = (Pa [hh] - hru_ea [hh]) * 1000 / (287.05 * (hru_t [hh] + 273.15)) + hru_ea [hh] * 1000 / (461.495 * (hru_t [hh] + 273.15));
            K_kat [hh] = Katabatic * ((hru_t [hh] + 273.15) - Ts_glacier) * pow (g / Ts_glacier * lapse_rate * Pr, 0.5);
            Gh_ebsm [hh] = den_air [hh] * Cp_air * ((0.01 + K_kat [hh]) / 2) * ((hru_t [hh] + 273.15) - Ts_glacier);
            Ge_ebsm [hh] = (MOL_wt_ratio_h2o_to_air * den_air [hh] * Lv) / (Pa [hh] * 10) * ((0.01 + K_kat [hh]) / 2) * (hru_ea [hh] - e_s) * 0.1;
            Cp_water [hh] = Cp_W0 - 2.55 * ((T_rain [hh] + 273.15) - FREEZE);
            Gp_ebsm [hh] = Cp_water [hh] * rho_h2o * net_rain [hh] / 1000 * ((T_rain [hh] + 273.15) - Ts_glacier) / (24ll / (long long) Global :: Freq * 3600ll);
            Gmelt_energy [hh] = Gn_ebsm [hh] + Gh_ebsm [hh] + Ge_ebsm [hh] + Gp_ebsm [hh];
            Xmelt_int [hh] = Gmelt_energy [hh] / (996.0 * 334000.0) * 1000.0 * 3600.0;
            if (Xmelt_int [hh] < 0.0)
            {
                Xmelt_int [hh] = 0.0;
            }

        }

        else
        {
            Xmelt_int [hh] = 0.0;
            if (debris_h [hh] <= 0.05)
            {
                lagT [hh] = hru_t [hh];
                lagSW [hh] = Qnsn_Var [hh];
                TF = 0.0984;
                SRF = 0.0044;
            }

            else
            if (debris_h [hh] <= 0.1)
            {
                lagT [hh] = hru_t [hh];
                lagSW [hh] = lagSW1 [hh];
                TF = 0.066;
                SRF = 0.0023;
            }

            else
            if (debris_h [hh] <= 0.2)
            {
                lagT [hh] = lagT1 [hh];
                lagSW [hh] = lagSW2 [hh];
                TF = 0.0456;
                SRF = 0.0009;
            }

            else
            if (debris_h [hh] <= 0.23)
            {
                lagT [hh] = lagT1 [hh];
                lagSW [hh] = lagSW3 [hh];
                TF = 0.04338;
                SRF = 0.0006;
            }

            else
            if (debris_h [hh] <= 0.3)
            {
                lagT [hh] = lagT2 [hh];
                lagSW [hh] = lagSW4 [hh];
                TF = 0.0392;
                SRF = 0.0002;
            }

            else
            if (debris_h [hh] <= 0.4)
            {
                lagT [hh] = lagT3 [hh];
                lagSW [hh] = lagSW5 [hh];
                TF = 0.0334;
                SRF = 0.0001;
            }

            else
            {
                lagT [hh] = lagT4 [hh];
                lagSW [hh] = lagSW6 [hh];
                TF = 0.0265;
                SRF = 0;
            }

            if (hru_t [hh] > T_threshold [hh])
            {
                Xmelt_int [hh] = TF * lagT [hh] + SRF * (1 - Albedo [hh]) * lagSW [hh];
                if (Xmelt_int [hh] <= 0.0)
                {
                    Xmelt_int [hh] = 0.0;
                }

            }

        }

        if (nstep == 1)
        {
            Xmelt_acc [hh] = Xmelt_int [hh];
        }

        else
        {
            Xmelt_acc [hh] += Xmelt_int [hh];
        }

        if (nstep == 0 || Global :: Freq == 1)
        {
            Xmelt [hh] = Xmelt_acc [hh];
            Xmelt_acc [hh] = 0.0;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_melt_debris_cover_estimate_Mod::run(void)@@@WQ_CRHM.cpp>");
}
ClassSoilPrairie * ClassSoilPrairie :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassSoilPrairie (name);
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassSoilPrairie :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::decl(void)@@@WQ_CRHM.cpp>");
    Description = "'Handles soil moisture throughout the year.' \
                 'Parametric contibuting fraction model with Pond,'";
    if (Global :: nlay < 2)
    {
        Global :: nlay = 2;
        Global :: maxlay = 2;
    }

    declvar ("redirected_residual", TDim :: NHRU, "redirected residual after topping up Sd and soil_rechar in Netroute/_D/_M/_M_D.", "(mm*km^2/int)", & redirected_residual);
    declstatdiag ("cum_redirected_residual", TDim :: NHRU, "cumulative HRU redirected_residual to another HRU.", "(mm*km^2)", & cum_redirected_residual);
    declvar ("direct_rain", TDim :: NHRU, "direct rainfall through canopy", "(mm/int)", & direct_rain);
    declstatvar ("Sd", TDim :: NHRU, "Depression storage.", "(mm)", & Sd);
    declstatvar ("gw", TDim :: NHRU, "ground water storage.", "(mm)", & gw);
    declstatvar ("soil_rechr", TDim :: NHRU, "moisture content of soil recharge zone, ie, the"
        "portion of the soil profile from which evaporation can take place.", "(mm)", & soil_rechr);
    declstatvar ("soil_moist", TDim :: NHRU, "moisture content of soil profile to the depth"
        "of the rooting zone of the major vegetation type on the HRU.", "(mm)", & soil_moist);
    decllocal ("cum_hru_condense", TDim :: NHRU, "cumulative condensation over HRU.", "(mm)", & cum_hru_condense);
    declvar ("soil_gw", TDim :: NHRU, "Portion of excess soil water from a HRU that enters groundwater reservoirs.", "(mm/int)", & soil_gw);
    declvar ("soil_gw_D", TDim :: NHRU, "Portion of excess soil water from a HRU that enters groundwater reservoirs.", "(mm/d)", & soil_gw_D);
    declvar ("gw_flow", TDim :: NHRU, "Drainage from HRU ground water reservoir.", "(mm/int)", & gw_flow);
    declvar ("gw_flow_D", TDim :: NHRU, "Daily drainage from HRU ground water reservoir.", "(mm/d)", & gw_flow_D);
    declvar ("infil_act", TDim :: NHRU, "Actual amount of water infiltrating the soil on each HRU.", "(mm/int)", & infil_act);
    declvar ("cum_infil_act", TDim :: NHRU, "Accumulation of the actual amount of water infiltrating the soil on each HRU.", "(mm)", & cum_infil_act);
    declvar ("infil_act_D", TDim :: NHRU, "Daily actual amount of water infiltrating the soil on each HRU.", "(mm/d)", & infil_act_D);
    declstatdiag ("cum_gw_flow", TDim :: NHRU, "Accumulation of excess soil water from a HRU that enters groundwater reservoirs.", "(mm)", & cum_gw_flow);
    declvar ("soil_ssr", TDim :: NHRU, "Portion of soil moisture and recharge excess from a HRU that enters subsurface reservoirs.", "(mm/int)", & soil_ssr);
    declvar ("rechr_ssr", TDim :: NHRU, "Portion of excess soil water from a HRU that enters subsurface reservoirs.", "(mm/int)", & rechr_ssr);
    declstatdiag ("cum_soil_ssr", TDim :: NHRU, "Accumulation of soil moisture from a HRU to ssr.", "(mm)", & cum_soil_ssr);
    declstatdiag ("cum_rechr_ssr", TDim :: NHRU, "Accumulation of Portion of excess from a HRU to ssr.", "(mm)", & cum_rechr_ssr);
    declvar ("soil_ssr_D", TDim :: NHRU, "Portion of excess soil water from a HRU that enters subsurface reservoirs.", "(mm/d)", & soil_ssr_D);
    declvar ("soil_runoff", TDim :: NHRU, "Portion of excess soil water from a HRU to runoff.", "(mm/int)", & soil_runoff);
    declstatdiag ("cum_soil_runoff", TDim :: NHRU, "Accumulation of Portion of excess soil water from a HRU to runoff.", "(mm)", & cum_soil_runoff);
    declvar ("soil_runoff_D", TDim :: NHRU, "Portion of excess soil water from a HRU that enters groundwater reservoirs.", "(mm/d)", & soil_runoff_D);
    declvar ("Pond_water_frac", TDim :: NHRU, "fraction of the maximum possible Sd HRU fraction that is under water.", "()", & Pond_water_frac);
    declvar ("Small_Ponds_water_frac", TDim :: NHRU, "fraction of the maximum possible Small_Ponds HRU fraction that is under water.", "()", & Small_Ponds_water_frac);
    decldiag ("cum_runoff_to_Sd", TDim :: NHRU, "Cumulative portion of runoff to depression storage.", "(mm/int)", & cum_runoff_to_Sd);
    decldiag ("cum_soil_gw", TDim :: NHRU, "Accumulation of excess soil water from a HRU that enters groundwater reservoirs.", "(mm)", & cum_soil_gw);
    declvar ("Sd_water_frac", TDim :: NHRU, "fraction of the maximum possible Sd HRU fraction that is under water.", "()", & Sd_water_frac);
    declvar ("Pond_water_frac", TDim :: NHRU, "Fraction of the wetlands contributing water.", "()", & Pond_water_frac);
    declvar ("Small_Ponds_water_frac", TDim :: NHRU, "Small Ponds contrib fraction", "()", & Small_Ponds_water_frac);
    declvar ("wetlands_evap", TDim :: NHRU, "Depth of water evaporating from wetlands using Priestley-Taylor.", "(mm/int)", & wetlands_evap);
    declvar ("Pond_evap", TDim :: NHRU, "pond evaporation.", "(mm)", & Pond_evap);
    declvar ("Pond_area", TDim :: NHRU, "pond surface area.", "(mm^2)", & Pond_area);
    declvar ("Pond_volume", TDim :: NHRU, "pond volume.", "(mm^3)", & Pond_volume);
    declvar ("Pond_s", TDim :: NHRU, "link between the shape of the basin and A-h relation. p = 2 corresponds to a paraboloid basin and p = 10 a flat bottom.", "(mm^3)", & Pond_s);
    declvar ("Pond_h", TDim :: NHRU, "pond depth.", "(mm)", & Pond_h);
    declvar ("Pond_hmax", TDim :: NHRU, "pond maximum depth ", "(m)", & Pond_hmax);
    declvar ("runoff_to_Pond", TDim :: NHRU, "runoff to pond", "(mm)", & runoff_to_Pond);
    declvar ("Small_Ponds_runoff_to_Pond", TDim :: NHRU, "Small pond runoff to pond", "(mm)", & Small_Ponds_runoff_to_Pond);
    declvar ("Small_Ponds_evap", TDim :: NHRU, "Small pond evaporation.", "(mm)", & Small_Ponds_evap);
    declvar ("Small_Ponds_area", TDim :: NHRU, "pond surface area.", "(mm^2)", & Small_Ponds_area);
    declstatvar ("Small_Ponds_Sd", TDim :: NHRU, "Small Ponds depression storage.", "(mm)", & Small_Ponds_Sd);
    declvar ("runoff_to_Small_Ponds", TDim :: NHRU, "runoff to small ponds", "(mm)", & runoff_to_Small_Ponds);
    decllocal ("snowinfil_buf", TDim :: NHRU, "buffer snow infiltration.", "(mm/d)", & snowinfil_buf);
    decllocal ("runoff_buf", TDim :: NHRU, "buffer runoff.", "(mm/d)", & runoff_buf);
    decllocal ("meltrunoff_buf", TDim :: NHRU, "buffer melt runoff.", "(mm/d)", & meltrunoff_buf);
    decllocal ("hru_evap_buf", TDim :: NHRU, "buffer evaporation.", "(mm/d)", & hru_evap_buf);
    decllocal ("soil_moist_Init", TDim :: NHRU, "initial soil moisture.", "(mm)", & soil_moist_Init);
    decllocal ("soil_rechr_Init", TDim :: NHRU, "initial soil recharge.", "(mm)", & soil_rechr_Init);
    decllocal ("Sd_Init", TDim :: NHRU, "initial Depression storage.", "(mm)", & Sd_Init);
    decllocal ("gw_Init", TDim :: NHRU, "initial ground water storage.", "(mm)", & gw_Init);
    declparam ("basin_area", TDim :: BASIN, "3", "1e-6", "1e+09", "total basin area.", "(km^2)", & basin_area);
    declparam ("hru_area", TDim :: NHRU, "[1]", "1e-6", "1e+09", "hru area.", "(km^2)", & hru_area);
    declparam ("Sdmax", TDim :: NHRU, "[0]", "0.0", "5000.0", "Maximum depression storage.", "(mm)", & Sdmax);
    declparam ("Sdinit", TDim :: NHRU, "[0]", "0.0", "5000.0", "Initial depression storage.", "(mm)", & Sdinit);
    declparam ("Small_Ponds_Sdmax", TDim :: NHRU, "[0]", "0.0", "5000.0", "Maximum depression storage.", "(mm)", & Small_Ponds_Sdmax);
    declparam ("Small_Ponds_Sdinit", TDim :: NHRU, "[0]", "0.0", "5000.0", "Initial depression storage.", "(mm)", & Small_Ponds_Sdinit);
    declparam ("soil_rechr_max", TDim :: NHRU, "[60.0]", "0.0", "350.0", "Maximum value for soil recharge zone (upper portion of soil_moist where losses occur as both evaporation "
        "and transpiration).  Must be less than or equal to soil_moist.", "(mm)", & soil_rechr_max);
    declparam ("soil_rechr_init", TDim :: NHRU, "[30.0]", "0.0", "250.0", "Initial value for soil recharge zone (upper part of "
        "soil_moist).  Must be less than or equal to soil_moist_init.", "(mm)", & soil_rechr_init);
    declparam ("soil_moist_max", TDim :: NHRU, "[375.0]", "0.0", "5000.0", "Maximum available water holding capacity of soil profile."
        "Soil profile is surface to bottom of rooting zone.", "(mm)", & soil_moist_max);
    declparam ("soil_moist_init", TDim :: NHRU, "[187.0]", "0.0", "5000.0", "Initial value of available water in soil profile.", "(mm)", & soil_moist_init);
    declparam ("Small_Ponds_area_max", TDim :: NHRU, "3", "1e-6", "1e+09", "small pond maximum area.", "(km^2)", & Small_Ponds_area_max);
    declparam ("Pond_area_max", TDim :: NHRU, "3", "1e-6", "1e+09", "pond maximum area.", "(km^2)", & Pond_area_max);
    declparam ("soil_gw_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "The maximum amount of the soil water excess for an HRU "
        "that is routed directly to the associated groundwater reservoir each day.", "(mm/d)", & soil_gw_K);
    declparam ("gw_max", TDim :: NHRU, "[375.0]", "0.0", "5000.0", "Maximum available water holding capacity of ground water reservoir.", "(mm)", & gw_max);
    declparam ("gw_init", TDim :: NHRU, "[187.0]", "0.0", "5000.0", "Initial value of available water in ground water reservoir.", "(mm)", & gw_init);
    declparam ("gw_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily ground water drainage from gw reservoir.", "(mm/d)", & gw_K);
    declparam ("rechr_ssr_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily ssr drainage from recharge.", "(mm/d)", & rechr_ssr_K);
    declparam ("lower_ssr_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily ssr drainage from soil column.", "(mm/d)", & lower_ssr_K);
    declparam ("Sd_ssr_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily depression storage ssr drainage factor.", "(mm/d)", & Sd_ssr_K);
    declparam ("Sd_gw_K", TDim :: NHRU, "[0.0]", "0.", "100.0", "daily depression storage gw drainage.", "(mm/d)", & Sd_gw_K);
    declparam ("Sd_water_frac_max", TDim :: NHRU, "[0.24]", "0.0", "1.0", "Maximum fraction of the HRU area that can be water.", "()", & Sd_water_frac_max);
    declparam ("evap_threshold", TDim :: NHRU, "[10]", "0.0", "20.0", "Threshold evaporation necessary for setting the contribution fraction to zero.", "(mm)", & evap_threshold);
    declparam ("soil_withdrawal", TDim :: NDEFN, "[3]", "1", "4", "Select water withdrawal function for soil type: 1 = sand, 2 = loam, 3 = clay, 4 = organic. soil_withdrawal[1] - rechr layer, soil_withdrawal[2] - lower layer", "()", & soil_withdrawal, & soil_withdrawal_Tables, 2);
    declparam ("cov_type", TDim :: NHRU, "[1]", "0", "2", "Vegetation evaporation type designation for HRU:  "
        "0 = bare soil (no evaporation), 1 = crops (recharge layer), 2 = grasses & shrubs (all soil moisture).", "()", & cov_type);
    declparam ("transp_limited", TDim :: NHRU, "[0]", "0", "1", "limit transpiration to recharge layer only  on-1/off-0.", "()", & transp_limited);
    declparam ("soil_ssr_runoff", TDim :: NHRU, "[1]", "0", "1", "soil column excess to interflow(ssr)/runoff (and possibly Sd)  interflow-0/runoff-1.", "()", & soil_ssr_runoff);
    decldiagparam ("inhibit_evap", TDim :: NHRU, "[0]", "0", "1", "inhibit evaporation, 1 -> inhibit.", "()", & inhibit_evap);
    declparam ("Sd_normal", TDim :: NHRU, "[0]", "0", "1", "inhibit pond handling of depressional storage", "()", & Sd_normal);
    declparam ("Pond_contrib_frac", TDim :: NHRU, "[0.0]", "0", "1", "Pond contrib fraction of basin", "()", & Pond_contrib_frac);
    declparam ("Pond_p", TDim :: NHRU, "[2]", ".1", "10", "link between the shape of the basin and A-h relation. p = 2 corresponds to a paraboloid basin and p = 10 a flat bottom", "()", & Pond_p);
    declparam ("Pond_C1", TDim :: NHRU, "[-0.254337]", "-1.0", "10", "coefficient 1 to calculate pond area from Sd/Sdmax", "()", & Pond_C1);
    declparam ("Pond_C2", TDim :: NHRU, "[1.22233]", "0.1", "10", "coefficient 2 to calculate pond area from Sd/Sdmax", "()", & Pond_C2);
    declparam ("Pond_C3", TDim :: NHRU, "[0.0261854]", "0.1", "10", "coefficient 3 to calculate pond area from Sd/Sdmax", "()", & Pond_C3);
    declparam ("Small_Ponds_contrib_frac", TDim :: NHRU, "[0.0]", "0", "1", "Small_Ponds contrib fraction of basin", "()", & Small_Ponds_contrib_frac);
    declparam ("Small_Ponds_p", TDim :: NHRU, "[2]", ".1", "10", "link between the shape of the basin and A-h relation. p = 2 corresponds to a paraboloid basin and p = 10 a flat bottom", "()", & Small_Ponds_p);
    declparam ("Small_Ponds_C1", TDim :: NHRU, "[-0.254337]", "-1.0", "10", "coefficient 1 to calculate small pond area from Sd/Sdmax", "()", & Small_Ponds_C1);
    declparam ("Small_Ponds_C2", TDim :: NHRU, "[1.22233]", "0.1", "10", "coefficient 2 to calculate small area from Sd/Sdmax", "()", & Small_Ponds_C2);
    declparam ("Small_Ponds_C3", TDim :: NHRU, "[0.0261854]", "0.1", "10", "coefficient 3 to calculate small area from Sd/Sdmax", "()", & Small_Ponds_C3);
    declputvar ("*", "hru_actet", "(mm/int)", & hru_actet);
    declputvar ("*", "hru_cum_actet", "(mm)", & hru_cum_actet);
    evapDiv = declgetvar ("*", "hru_evap", "(mm/int)", & hru_evap);
    declgetvar ("*", "hru_evap_PT", "(mm/int)", & hru_evap_PT);
    declgetvar ("*", "infil", "(mm/int)", & infil);
    snowinfilDiv = declgetvar ("*", "snowinfil", "(mm/int)", & snowinfil);
    runoffDiv = declgetvar ("*", "runoff", "(mm/int)", & runoff);
    meltrunoffDiv = declgetvar ("*", "meltrunoff", "(mm/int)", & meltrunoff);
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassSoilPrairie :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::init(void)@@@WQ_CRHM.cpp>");
    nhru = getdim (TDim :: NHRU);
    if (snowinfilDiv > 1)
    {
        string S = "Soil:  \"snowinfil\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (evapDiv > 1)
    {
        string S = "Soil:  \"hru_evap\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (meltrunoffDiv > 1)
    {
        string S = "Netroute:  \"meltrunoff\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    if (runoffDiv > 1)
    {
        string S = "Netroute:  \"runoff\". Converting to mm/int";
        CRHMException TExcept (S.c_str (), TExcept :: WARNING);
        LogError (TExcept);
    }

    for (hh = 0; hh < nhru; ++ hh) {
        if (soil_moist [hh] > soil_moist_max [hh])
        {
            string S = string ("'") + Name + " (Soil)' soil_moist cannot be greater than soil_moist_max in hru = " + to_string (hh + 1).c_str ();
            CRHMException TExcept (S.c_str (), TExcept :: TERMINATE);
            LogError (TExcept);
            throw TExcept;
        }

        if (soil_rechr [hh] > soil_rechr_max [hh])
        {
            string S = string ("'") + Name + " (Soil)' soil_rechr cannot be greater than soil_rechr_max in hru = " + to_string (hh + 1).c_str ();
            CRHMException TExcept (S.c_str (), TExcept :: TERMINATE);
            LogError (TExcept);
            throw TExcept;
        }

        if (soil_rechr_max [hh] > soil_moist_max [hh])
        {
            string S = string ("'") + Name + " (Soil)' Soil_rechr_max cannot be greater than soil_moist_max in hru = " + to_string (hh + 1).c_str ();
            CRHMException TExcept (S.c_str (), TExcept :: WARNING);
            LogError (TExcept);
            throw TExcept;
        }

        if (Sdinit [hh] > Sdmax [hh])
        {
            string S = string ("'") + Name + " (Soil)' Initial value of depression storage is greater than the maximum value in hru = " + to_string (hh + 1).c_str ();
            CRHMException Except (S.c_str (), TExcept :: TERMINATE);
            LogError (Except);
            throw Except;
        }

        if (gw_init [hh] > gw_max [hh])
        {
            string S = string ("'") + Name + " (Soil)' Initial value of gw storage is greater than the maximum value in hru = " + to_string (hh + 1).c_str ();
            CRHMException Except (S.c_str (), TExcept :: TERMINATE);
            LogError (Except);
            throw Except;
        }

        soil_rechr [hh] = soil_rechr_init [hh];
        soil_moist [hh] = soil_moist_init [hh];
        redirected_residual [hh] = 0.0;
        Sd [hh] = Sdinit [hh];
        gw [hh] = gw_init [hh];
        hru_cum_actet [hh] = 0.0;
        cum_hru_condense [hh] = 0.0;
        cum_infil_act [hh] = 0.0;
        soil_runoff_D [hh] = 0.0;
        soil_ssr_D [hh] = 0.0;
        soil_gw_D [hh] = 0.0;
        gw_flow_D [hh] = 0.0;
        infil_act [hh] = 0.0;
        infil_act_D [hh] = 0.0;
        cum_soil_runoff [hh] = 0.0;
        cum_soil_ssr [hh] = 0.0;
        cum_rechr_ssr [hh] = 0.0;
        cum_soil_gw [hh] = 0.0;
        cum_gw_flow [hh] = 0.0;
        cum_runoff_to_Sd [hh] = 0.0;
        cum_redirected_residual [hh] = 0.0;
        Pond_area [hh] = 0.0;
        Pond_evap [hh] = 0.0;
        Pond_h [hh] = 0.0;
        Pond_s [hh] = 0.0;
        Small_Ponds_area [hh] = 0.0;
        Small_Ponds_evap [hh] = 0.0;
        Small_Ponds_runoff_to_Pond [hh] = 0.0;
        Small_Ponds_Sd [hh] = Small_Ponds_Sdinit [hh];
        if (Sdmax [hh] > 0.0 && soil_moist_max [hh] > 0.0)
        {
            calc_h_and_S (Sdmax [hh], Pond_area_max [hh], Pond_p [hh], Pond_h [hh], Pond_s [hh]);
            Pond_hmax [hh] = Pond_h [hh];
            Pond_calc_h_A (Pond_s [hh], Pond_p [hh], Sdmax [hh], Pond_h [hh], Pond_area [hh]);
        }

        if (Sdmax [hh] > 0.0 && soil_moist_max [hh] > 0.0)
        {
            if (Sdmax [hh] > 0.0)
            {
                Pond_water_frac [hh] = Sd [hh] / Sdmax [hh];
            }

            else
            {
                Pond_water_frac [hh] = 0.0;
            }

            if (Small_Ponds_Sdmax [hh] > 0.0)
            {
                Small_Ponds_water_frac [hh] = Small_Ponds_Sd [hh] / Small_Ponds_Sdmax [hh];
            }

            else
            {
                Small_Ponds_water_frac [hh] = 0.0;
            }

        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::init(void)@@@WQ_CRHM.cpp>");
}
void ClassSoilPrairie :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::run(void)@@@WQ_CRHM.cpp>");
    double soil_lower, excs, condense;
    double et;
    long nstep = getstep ();
    if (nstep == 1)
    {
        for (hh = 0; chkStruct (); ++ hh) {
            LogMessageA (hh, string ("'" + Name + " (Soil)' soil_rechr_init     (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (Soil)' soil_moist_init     (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (Soil)' Sd_init             (mm) (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh], hru_area [hh], basin_area [0]);
            LogMessageA (hh, string ("'" + Name + " (Soil)' gw_init             (mm) (mm*hru) (mm*hru/basin): ").c_str (), gw [hh], hru_area [hh], basin_area [0]);
            LogDebug (" ");
            soil_moist_Init [hh] = soil_moist [hh];
            soil_rechr_Init [hh] = soil_rechr [hh];
            Sd_Init [hh] = Sd [hh];
            gw_Init [hh] = gw [hh];
        }
    }

    nstep = nstep % Global :: Freq;
    for (hh = 0; chkStruct (); ++ hh) {
        if (snowinfilDiv == 1)
        {
            snowinfil_buf [hh] = snowinfil [hh];
        }

        if (runoffDiv == 1)
        {
            runoff_buf [hh] = runoff [hh];
        }

        if (meltrunoffDiv == 1)
        {
            meltrunoff_buf [hh] = meltrunoff [hh];
        }

        if (evapDiv == 1)
        {
            hru_evap_buf [hh] = hru_evap [hh];
        }

        if (nstep == 1)
        {
            soil_runoff_D [hh] = 0.0;
            soil_ssr_D [hh] = 0.0;
            soil_gw_D [hh] = 0.0;
            gw_flow_D [hh] = 0.0;
            infil_act_D [hh] = 0.0;
        }

        hru_actet [hh] = 0.0;
        infil_act [hh] = 0.0;
        soil_gw [hh] = 0.0;
        soil_ssr [hh] = 0.0;
        rechr_ssr [hh] = 0.0;
        wetlands_evap [hh] = 0.0;
        if (hru_evap_buf [hh] < 0.0)
        {
            condense = - hru_evap_buf [hh];
            cum_hru_condense [hh] += condense;
            hru_evap_buf [hh] = 0.0;
        }

        else
        {
            condense = 0.0;
        }

        if (soil_moist_max [hh] > 0.0)
        {
            soil_lower = soil_moist [hh] - soil_rechr [hh];
            double potential = infil [hh] + snowinfil_buf [hh] + condense;
            soil_rechr [hh] = soil_rechr [hh] + potential;
            if (soil_rechr [hh] > soil_rechr_max [hh])
            {
                excs = soil_rechr [hh] - soil_rechr_max [hh];
                soil_rechr [hh] = soil_rechr_max [hh];
                soil_lower = soil_lower + excs;
            }

            soil_moist [hh] = soil_lower + soil_rechr [hh];
            if (soil_moist [hh] > soil_moist_max [hh])
            {
                excs = soil_moist [hh] - soil_moist_max [hh];
                soil_moist [hh] = soil_moist_max [hh];
            }

            else
            {
                excs = 0.0;
            }

            infil_act [hh] = potential - excs;
            cum_infil_act [hh] += infil_act [hh];
            infil_act_D [hh] += infil_act [hh];
            if (! inhibit_evap [hh])
            {
                if (soil_rechr_max [hh] > 0.0)
                {
                    rechr_ssr [hh] = soil_rechr [hh] / soil_rechr_max [hh] * rechr_ssr_K [hh] / Global :: Freq;
                }

                if (rechr_ssr [hh] > soil_rechr [hh])
                {
                    rechr_ssr [hh] = soil_rechr [hh];
                }

                soil_rechr [hh] -= rechr_ssr [hh];
                if (soil_rechr [hh] < 0.0)
                {
                    soil_rechr [hh] = 0.0;
                }

                soil_moist [hh] -= rechr_ssr [hh];
                soil_ssr [hh] = rechr_ssr [hh];
                cum_rechr_ssr [hh] += rechr_ssr [hh];
            }

            double s2gw_k = soil_gw_K [hh] / Global :: Freq;
            if (excs >= s2gw_k)
            {
                soil_gw [hh] = s2gw_k;
                excs -= s2gw_k;
            }

            else
            {
                soil_gw [hh] = excs;
                excs = 0.0;
            }

            if (! soil_ssr_runoff [hh] && excs > 0.0)
            {
                soil_ssr [hh] += excs;
                excs = 0.0;
            }

        }

        else
        {
            excs = infil [hh] + snowinfil_buf [hh] + condense;
        }

        runoff_to_Pond [hh] = 0.0;
        runoff_to_Small_Ponds [hh] = 0.0;
        Small_Ponds_runoff_to_Pond [hh] = 0.0;
        Small_Ponds_evap [hh] = 0.0;
        Pond_evap [hh] = 0.0;
        soil_runoff [hh] = meltrunoff_buf [hh] + runoff_buf [hh] + excs;
        if (soil_runoff [hh] > 0.0 || redirected_residual [hh] > 0.0)
        {
            if (Sd_normal [hh] == 0 && soil_runoff [hh] > 0.0)
            {
                runoff_to_Small_Ponds [hh] = soil_runoff [hh] * Small_Ponds_contrib_frac [hh];
                runoff_to_Pond [hh] = soil_runoff [hh] * Pond_contrib_frac [hh];
                soil_runoff [hh] -= (runoff_to_Pond [hh] + runoff_to_Small_Ponds [hh]);
                if (Small_Ponds_Sd [hh] + runoff_to_Small_Ponds [hh] > Small_Ponds_Sdmax [hh])
                {
                    runoff_to_Pond [hh] += (Small_Ponds_Sd [hh] + runoff_to_Small_Ponds [hh] - Small_Ponds_Sdmax [hh]);
                }

                else
                {
                    Small_Ponds_Sd [hh] += runoff_to_Small_Ponds [hh];
                }

                Small_Ponds_Sd [hh] += runoff_to_Small_Ponds [hh];
                Small_Ponds_runoff_to_Pond [hh] = Small_Ponds_runoff_frac (Small_Ponds_Sd [hh] / Small_Ponds_Sdmax [hh], hh) * Small_Ponds_Sd [hh];
                runoff_to_Pond [hh] += Small_Ponds_runoff_to_Pond [hh];
                Small_Ponds_Sd [hh] -= Small_Ponds_runoff_to_Pond [hh];
                if (redirected_residual [hh] + runoff_to_Pond [hh] > 0.0)
                {
                    if (Sd [hh] + runoff_to_Pond [hh] + redirected_residual [hh] > Sdmax [hh])
                    {
                        soil_runoff [hh] += (Sd [hh] + runoff_to_Pond [hh] + redirected_residual [hh] - Sdmax [hh]);
                    }

                    else
                    {
                        Sd [hh] += runoff_to_Pond [hh] + redirected_residual [hh];
                    }

                    redirected_residual [hh] = 0.0;
                    Pond_calc_h_A (Sd [hh], Pond_s [hh], Pond_p [hh], Pond_h [hh], Pond_area [hh]);
                }

            }

            else
            {
                soil_runoff [hh] = meltrunoff_buf [hh] + runoff_buf [hh] + excs + (redirected_residual [hh] / hru_area [hh]);
                redirected_residual [hh] = 0.0;
            }

        }

        else
        {
            if (Small_Ponds_Sd [hh] > 0.0)
            {
                Small_Ponds_water_frac [hh] = Small_Ponds_area_frac (Small_Ponds_Sd [hh] / Small_Ponds_Sdmax [hh], hh);
                Small_Ponds_evap [hh] = Small_Ponds_water_frac [hh] * hru_evap_PT [hh];
                if (Small_Ponds_evap [hh] > Small_Ponds_Sd [hh])
                {
                    Small_Ponds_evap [hh] = Small_Ponds_Sd [hh];
                }

                wetlands_evap [hh] = Small_Ponds_evap [hh];
                Small_Ponds_water_frac [hh] = Small_Ponds_area_frac (Small_Ponds_Sd [hh] / Small_Ponds_Sdmax [hh], hh);
                Small_Ponds_Sd [hh] -= Small_Ponds_evap [hh];
            }

            if (Sd [hh] > 0.0)
            {
                Pond_water_frac [hh] = Pond_area_frac (Sd [hh] / Sdmax [hh], hh);
                Pond_evap [hh] = Pond_water_frac [hh] * hru_evap_PT [hh];
                if (Pond_evap [hh] > Sd [hh])
                {
                    Pond_evap [hh] = Sd [hh];
                }

                wetlands_evap [hh] += Pond_evap [hh];
                Sd [hh] -= Pond_evap [hh];
            }

        }

        cum_redirected_residual [hh] += (redirected_residual [hh] / hru_area [hh]);
        redirected_residual [hh] = 0;
        soil_runoff_D [hh] += soil_runoff [hh];
        cum_soil_runoff [hh] += soil_runoff [hh];
        cum_runoff_to_Sd [hh] += runoff_to_Pond [hh];
        if (Sd [hh] > 0.0 && Sd_gw_K [hh] > 0.0)
        {
            double Sd2gw_k = Sd_gw_K [hh] / Global :: Freq;
            if (Sd2gw_k > Sd [hh])
            {
                Sd2gw_k = Sd [hh];
            }

            soil_gw [hh] += Sd2gw_k;
            Sd [hh] -= Sd2gw_k;
            if (Sd [hh] < 0.0)
            {
                Sd [hh] = 0.0;
            }

        }

        soil_gw_D [hh] += soil_gw [hh];
        cum_soil_gw [hh] += soil_gw [hh];
        gw [hh] += soil_gw [hh];
        gw_flow [hh] = 0.0;
        if (gw [hh] > gw_max [hh])
        {
            gw_flow [hh] += (gw [hh] - gw_max [hh]);
            gw [hh] = gw_max [hh];
        }

        if (gw_max [hh] > 0.0)
        {
            double spill = gw [hh] / gw_max [hh] * gw_K [hh] / Global :: Freq;
            gw [hh] -= spill;
            gw_flow [hh] += spill;
        }

        gw_flow_D [hh] += gw_flow [hh];
        cum_gw_flow [hh] += gw_flow [hh];
        if (Sd [hh] > 0.0 && Sd_ssr_K [hh] > 0.0)
        {
            double Sd2ssr_k = Sd_ssr_K [hh] / Global :: Freq;
            if (Sd2ssr_k >= Sd [hh])
            {
                Sd2ssr_k = Sd [hh];
            }

            soil_ssr [hh] += Sd2ssr_k;
            Sd [hh] -= Sd2ssr_k;
            if (Sd [hh] < 0.0)
            {
                Sd [hh] = 0.0;
            }

        }

        double s2ssr_k = lower_ssr_K [hh] / Global :: Freq;
        if (s2ssr_k > 0.0)
        {
            double avail = soil_moist [hh] - soil_rechr [hh];
            if (s2ssr_k >= avail)
            {
                s2ssr_k = avail;
            }

            soil_moist [hh] -= s2ssr_k;
            soil_ssr [hh] += s2ssr_k;
        }

        cum_soil_ssr [hh] += soil_ssr [hh];
        soil_ssr_D [hh] += soil_ssr [hh];
        double avail_evap = hru_evap_buf [hh] * (1.0 - (Pond_water_frac [hh] + Small_Ponds_water_frac [hh]));
        if (avail_evap > 0.0 && soil_moist [hh] > 0.0 && cov_type [hh] > 0)
        {
            double pctl, pctr, etl, etr;
            if ((soil_moist_max [hh] - soil_rechr_max [hh]) > 0.0)
            {
                pctl = (soil_moist [hh] - soil_rechr [hh]) / (soil_moist_max [hh] - soil_rechr_max [hh]);
            }

            else
            {
                pctl = 0.0;
            }

            if (soil_rechr_max [hh] > 0.0)
            {
                pctr = soil_rechr [hh] / soil_rechr_max [hh];
            }

            else
            {
                pctr = 0.0;
            }

            etr = avail_evap;
            switch (soil_withdrawal_Tables [0] [hh]) {
            case 1:
                if (pctr < 0.25)
                {
                    etr = 0.5 * pctr * avail_evap;
                }

                break;
            case 2:
                if (pctr < 0.5)
                {
                    etr = pctr * avail_evap;
                }

                break;
            case 3:
                if (pctr <= 0.33)
                {
                    etr = 0.5 * pctr * avail_evap;
                }

                else
                if (pctr < 0.67)
                {
                    etr = pctr * avail_evap;
                }

                break;
            case 4:
                break;
            }
            if (etr > avail_evap)
            {
                etl = 0.0;
                etr = avail_evap;
            }

            else
            {
                etl = avail_evap - etr;
            }

            switch (soil_withdrawal_Tables [1] [hh]) {
            case 1:
                if (pctl < 0.25)
                {
                    etl = 0.5 * pctl * etl;
                }

                break;
            case 2:
                if (pctl < 0.5)
                {
                    etl = pctl * etl;
                }

                break;
            case 3:
                if (pctl <= 0.33)
                {
                    etl = 0.5 * pctl * etl;
                }

                else
                if (pctr < 0.67)
                {
                    etl = pctl * etl;
                }

                break;
            case 4:
                break;
            }
            long et_type = cov_type [hh];
            if (transp_limited [hh] == 1 && et_type == 2)
            {
                et_type = 1;
            }

            et = 0.0;
            switch (et_type) {
            case 0:
            case - 1:
                break;
            case 1:
                if (etr > soil_rechr [hh])
                {
                    soil_rechr [hh] = 0.0;
                    et = soil_rechr [hh];
                }

                else
                {
                    soil_rechr [hh] = soil_rechr [hh] - etr;
                    et = etr;
                }

                soil_moist [hh] = soil_moist [hh] - et;
                break;
            case 2:
                if (etr + etl >= soil_moist [hh])
                {
                    et = soil_moist [hh];
                    soil_moist [hh] = 0.0;
                    soil_rechr [hh] = 0.0;
                }

                else
                {
                    et = etr + etl;
                    soil_moist [hh] = soil_moist [hh] - et;
                    if (etr > soil_rechr [hh])
                    {
                        soil_rechr [hh] = 0.0;
                    }

                    else
                    {
                        soil_rechr [hh] = soil_rechr [hh] - etr;
                    }

                }

                break;
            }
            hru_actet [hh] += et;
        }

        if (wetlands_evap [hh] > 0.0)
        {
            hru_actet [hh] = wetlands_evap [hh];
        }

        hru_cum_actet [hh] += hru_actet [hh];
        if (nstep == 0)
        {
            if (snowinfilDiv > 1)
            {
                snowinfil_buf [hh] = snowinfil [hh] / snowinfilDiv;
            }

            if (runoffDiv > 1)
            {
                runoff_buf [hh] = runoff [hh] / runoffDiv;
            }

            if (meltrunoffDiv > 1)
            {
                meltrunoff_buf [hh] = meltrunoff [hh] / meltrunoffDiv;
            }

            if (evapDiv > 1)
            {
                hru_evap_buf [hh] = hru_evap [hh] / evapDiv;
            }

        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::run(void)@@@WQ_CRHM.cpp>");
}
void ClassSoilPrairie :: finish (bool good)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::finish(bool good)@@@WQ_CRHM.cpp>");
    double Allcum_soil_runoff = 0.0;
    double Allcum_soil_ssr = 0.0;
    double Allcum_rechr_ssr = 0.0;
    double Allcum_soil_gw = 0.0;
    double Allcum_gw_flow = 0.0;
    double Allcum_infil_act = 0.0;
    double Allcum_soil_moist_change = 0.0;
    double Allcum_Sd_change = 0.0;
    double Allcum_gw_change = 0.0;
    for (hh = 0; chkStruct (); ++ hh) {
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' soil_rechr         (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'soil_moist'.");
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' soil_rechr_change  (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_rechr [hh] - soil_rechr_Init [hh], hru_area [hh], basin_area [0], " *** information only - already included in 'soil_moist'.");
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' soil_moist         (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' soil_moist_change  (mm) (mm*hru) (mm*hru/basin): ").c_str (), soil_moist [hh] - soil_moist_Init [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' hru_cum_actet      (mm) (mm*hru) (mm*hru/basin): ").c_str (), hru_cum_actet [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_hru_condense   (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_hru_condense [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_infil_act(all) (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_infil_act [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_soil_gw        (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_gw [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_soil_runoff    (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_runoff [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_rechr_ssr      (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_rechr_ssr [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_soil_ssr       (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_soil_ssr [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' Sd                 (mm) (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' Sd_change          (mm) (mm*hru) (mm*hru/basin): ").c_str (), Sd [hh] - Sd_Init [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_runoff_to_Sd   (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_runoff_to_Sd [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' gw                 (mm) (mm*hru) (mm*hru/basin): ").c_str (), gw [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' gw_change          (mm) (mm*hru) (mm*hru/basin): ").c_str (), gw [hh] - gw_Init [hh], hru_area [hh], basin_area [0]);
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_gw_flow        (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_gw_flow [hh], hru_area [hh], basin_area [0]);
        LogDebug (" ");
        LogMessageA (hh, string ("'" + Name + " (SoilPrairie)' cum_redirected_residual (mm) (mm*hru) (mm*hru/basin): ").c_str (), cum_redirected_residual [hh] / hru_area [hh], hru_area [hh], basin_area [0], " *** Added to this HRU surface runoff");
        LogDebug (" ");
        Allcum_soil_runoff += cum_soil_runoff [hh] * hru_area [hh];
        Allcum_soil_ssr += cum_soil_ssr [hh] * hru_area [hh];
        Allcum_rechr_ssr += cum_rechr_ssr [hh] * hru_area [hh];
        Allcum_soil_gw += cum_soil_gw [hh] * hru_area [hh];
        Allcum_gw_flow += cum_gw_flow [hh] * hru_area [hh];
        Allcum_infil_act += cum_infil_act [hh] * hru_area [hh];
        Allcum_soil_moist_change += (soil_moist [hh] - soil_moist_Init [hh]) * hru_area [hh];
        Allcum_Sd_change += (Sd [hh] - Sd_Init [hh]) * hru_area [hh];
        Allcum_gw_change += (gw [hh] - gw_Init [hh]) * hru_area [hh];
    }
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_soil_runoff (mm*basin): ").c_str (), Allcum_soil_runoff);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_soil_ssr (mm*basin): ").c_str (), Allcum_soil_ssr);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_rechr_ssr (mm*basin): ").c_str (), Allcum_rechr_ssr);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_soil_gw (mm*basin): ").c_str (), Allcum_soil_gw);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_gw_flow (mm*basin): ").c_str (), Allcum_gw_flow);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_infil_act (mm*basin): ").c_str (), Allcum_infil_act);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_soil_moist_change (mm*basin): ").c_str (), Allcum_soil_moist_change);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_Sd_change (mm*basin): ").c_str (), Allcum_Sd_change);
    LogMessage (string ("'" + Name + " (SoilPrairie)' Allcum_gw_change (mm*basin): ").c_str (), Allcum_gw_change);
    LogDebug (" ");
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::finish(bool good)@@@WQ_CRHM.cpp>");
}
double ClassSoilPrairie :: Pond_area_frac (const double area_frac, const long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::Pond_area_frac(const double area_frac, const long hh)@@@WQ_CRHM.cpp>");
    double ZZ = Pond_C1 [hh] * sqr (area_frac) + Pond_C2 [hh] * area_frac + Pond_C3 [hh];
    if (ZZ < 0.0)
    {
        ZZ = 0.0;
    }

    else
    if (ZZ > 1.0)
    {
        ZZ = 1.0;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::Pond_area_frac(const double area_frac, const long hh)@@@WQ_CRHM.cpp>");
    return ZZ;
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::Pond_area_frac(const double area_frac, const long hh)@@@WQ_CRHM.cpp>");
}
double ClassSoilPrairie :: Small_Ponds_area_frac (const double area_frac, const long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::Small_Ponds_area_frac(const double area_frac, const long hh)@@@WQ_CRHM.cpp>");
    double ZZ = Small_Ponds_C1 [hh] * sqr (area_frac) + Small_Ponds_C2 [hh] * area_frac + Small_Ponds_C3 [hh];
    if (ZZ < 0.0)
    {
        ZZ = 0.0;
    }

    else
    if (ZZ > 1.0)
    {
        ZZ = 1.0;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::Small_Ponds_area_frac(const double area_frac, const long hh)@@@WQ_CRHM.cpp>");
    return ZZ;
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::Small_Ponds_area_frac(const double area_frac, const long hh)@@@WQ_CRHM.cpp>");
}
double ClassSoilPrairie :: Small_Ponds_runoff_frac (const double Sd_Sdmax, const long hh)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::Small_Ponds_runoff_frac(const double Sd_Sdmax, const long hh)@@@WQ_CRHM.cpp>");
    double ZZ = Sd_Sdmax;
    if (ZZ < 0.0)
    {
        ZZ = 0.0;
    }

    else
    if (ZZ > 1.0)
    {
        ZZ = 1.0;
    }

InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::Small_Ponds_runoff_frac(const double Sd_Sdmax, const long hh)@@@WQ_CRHM.cpp>");
    return ZZ;
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::Small_Ponds_runoff_frac(const double Sd_Sdmax, const long hh)@@@WQ_CRHM.cpp>");
}
void ClassSoilPrairie :: Pond_calc_h_A (const double volume, const double s, const double p, double & h, double & area)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::Pond_calc_h_A(const double volume, const double s, const double p, double & h, double & area)@@@WQ_CRHM.cpp>");
    h = pow ((volume * (1.0 + 2.0 / p) / s), (1.0 / (1.0 + 2.0 / p)));
    area = s * pow (h, 2.0 / p);
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::Pond_calc_h_A(const double volume, const double s, const double p, double & h, double & area)@@@WQ_CRHM.cpp>");
}
void ClassSoilPrairie :: calc_h_and_S (const double maxvolume, const double maxarea, const double pval, double & S, double & h)
{
InstrumentLogger::instance()->log_instrument_log("<ClassSoilPrairie::calc_h_and_S(const double maxvolume, const double maxarea, const double pval, double & S, double & h)@@@WQ_CRHM.cpp>");
    const double maxerror = 0.000001;
    const double maxiterations = 100;
    double est_area, area_error;
    long iterations;
    long done;
    h = 1.0;
    done = false;
    iterations = 0;
    while (! done) {
        S = maxarea / pow (h, (2.0 / pval));
        h = pow (maxvolume * (1.0 + 2.0 / pval) / S, 1.0 / (1.0 + 2.0 / pval));
        est_area = S * pow (h, (2.0 / pval));
        area_error = fabs (est_area - maxarea) / maxarea;
        if ((area_error < maxerror) || (iterations > maxiterations))
        {
            done = true;
        }

        else
        {
            iterations = iterations + 1;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassSoilPrairie::calc_h_and_S(const double maxvolume, const double maxarea, const double pval, double & S, double & h)@@@WQ_CRHM.cpp>");
}
ClassGlacier_debris_cover * ClassGlacier_debris_cover :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_debris_cover::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_debris_cover::klone(string name) const@@@WQ_CRHM.cpp>");
    return new ClassGlacier_debris_cover (name);
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_debris_cover::klone(string name) const@@@WQ_CRHM.cpp>");
}
void ClassGlacier_debris_cover :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_debris_cover::decl(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    declvar ("lagT", TDim :: NHRU, "lagged temperature.", "(°C)", & lagT);
    declvar ("lagSW", TDim :: NHRU, "lagged shortwave radiation.", "(W/m^2)", & lagSW);
    declvar ("Xmelt", TDim :: NHRU, "Experimental Firn and Ice melt at daily timestepe.", "(mm/d)", & Xmelt);
    declvar ("Xmelt_int", TDim :: NHRU, "Experimental Firn and Ice melt at interval timestep.", "(mm/int)", & Xmelt_int);
    declvar ("Xmelt_acc", TDim :: NHRU, "Experimental Firn and Ice melt daily accumulator.", "(mm)", & Xmelt_acc);
    declparam ("debris_h", TDim :: NHRU, "[0.0]", "0", "10", "debris thickness.", "(m)", & debris_h);
    declparam ("T_threshold", TDim :: NHRU, "[1.0]", "-30", "30", "threshold temperature for melt to occur.", "(°C)", & T_threshold);
    declreadobs ("lagT1", TDim :: NHRU, "lagged temperature 1", "(°C)", & lagT1, HRU_OBS_Q, true);
    declreadobs ("lagT2", TDim :: NHRU, "lagged temperature 2", "(°C)", & lagT2, HRU_OBS_Q, true);
    declreadobs ("lagT3", TDim :: NHRU, "lagged temperature 3", "(°C)", & lagT3, HRU_OBS_Q, true);
    declreadobs ("lagT4", TDim :: NHRU, "lagged temperature 4", "(°C)", & lagT4, HRU_OBS_Q, true);
    declreadobs ("lagSW1", TDim :: NHRU, "lagged shortwave radiation 1", "(W/m^2)", & lagSW1, HRU_OBS_Q, true);
    declreadobs ("lagSW2", TDim :: NHRU, "lagged shortwave radiation 2", "(W/m^2)", & lagSW2, HRU_OBS_Q, true);
    declreadobs ("lagSW3", TDim :: NHRU, "lagged shortwave radiation 3", "(W/m^2)", & lagSW3, HRU_OBS_Q, true);
    declreadobs ("lagSW4", TDim :: NHRU, "lagged shortwave radiation 4", "(W/m^2)", & lagSW4, HRU_OBS_Q, true);
    declreadobs ("lagSW5", TDim :: NHRU, "lagged shortwave radiation 5", "(W/m^2)", & lagSW5, HRU_OBS_Q, true);
    declreadobs ("lagSW6", TDim :: NHRU, "lagged shortwave radiation 6", "(W/m^2)", & lagSW6, HRU_OBS_Q, true);
    declgetvar ("*", "Albedo", "()", & Albedo);
    declgetvar ("*", "hru_t", "(°C)", & hru_t);
    declgetvar ("*", "Qnsn_Var", "(W/m^2*int)", & Qnsn_Var);
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_debris_cover::decl(void)@@@WQ_CRHM.cpp>");
}
void ClassGlacier_debris_cover :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_debris_cover::init(void)@@@WQ_CRHM.cpp>");
    for (hh = 0; hh < nhru; ++ hh) {
        lagT [hh] = 0.0;
        lagSW [hh] = 0.0;
        Xmelt [hh] = 0.0;
        Xmelt_int [hh] = 0.0;
        Xmelt_acc [hh] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_debris_cover::init(void)@@@WQ_CRHM.cpp>");
}
void ClassGlacier_debris_cover :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<ClassGlacier_debris_cover::run(void)@@@WQ_CRHM.cpp>");
    double TF = 0.0;
    double SRF = 0.0;
    long nstep = getstep ();
    nstep = nstep % Global :: Freq;
    for (hh = 0; hh < nhru; ++ hh) {
        if (debris_h [hh] > 0.0)
        {
            Xmelt_int [hh] = 0.0;
            if (debris_h [hh] <= 0.05)
            {
                lagT [hh] = hru_t [hh];
                lagSW [hh] = Qnsn_Var [hh];
                TF = 0.0984;
                SRF = 0.0044;
            }

            else
            if (debris_h [hh] <= 0.1)
            {
                lagT [hh] = hru_t [hh];
                lagSW [hh] = lagSW1 [hh];
                TF = 0.066;
                SRF = 0.0023;
            }

            else
            if (debris_h [hh] <= 0.2)
            {
                lagT [hh] = lagT1 [hh];
                lagSW [hh] = lagSW2 [hh];
                TF = 0.0456;
                SRF = 0.0009;
            }

            else
            if (debris_h [hh] <= 0.23)
            {
                lagT [hh] = lagT1 [hh];
                lagSW [hh] = lagSW3 [hh];
                TF = 0.04338;
                SRF = 0.0006;
            }

            else
            if (debris_h [hh] <= 0.3)
            {
                lagT [hh] = lagT2 [hh];
                lagSW [hh] = lagSW4 [hh];
                TF = 0.0392;
                SRF = 0.0002;
            }

            else
            if (debris_h [hh] <= 0.4)
            {
                lagT [hh] = lagT3 [hh];
                lagSW [hh] = lagSW5 [hh];
                TF = 0.0334;
                SRF = 0.0001;
            }

            else
            if (debris_h [hh] > 0.4)
            {
                lagT [hh] = lagT4 [hh];
                lagSW [hh] = lagSW6 [hh];
                TF = 0.0265;
                SRF = 0;
            }

            if (hru_t [hh] > T_threshold [hh])
            {
                Xmelt_int [hh] = TF * lagT [hh] + SRF * (1 - Albedo [hh]) * lagSW [hh];
                if (Xmelt_int [hh] <= 0.0)
                {
                    Xmelt_int [hh] = 0.0;
                }

            }

        }

        if (nstep == 1)
        {
            Xmelt_acc [hh] = Xmelt_int [hh];
        }

        else
        {
            Xmelt_acc [hh] += Xmelt_int [hh];
        }

        if (nstep == 0 || Global :: Freq == 1)
        {
            Xmelt [hh] = Xmelt_acc [hh];
            Xmelt_acc [hh] = 0.0;
        }

    }
InstrumentLogger::instance()->log_instrument_log("</ClassGlacier_debris_cover::run(void)@@@WQ_CRHM.cpp>");
}
Class_lapse_rate_Monthly * Class_lapse_rate_Monthly :: klone (string name) const
{
InstrumentLogger::instance()->log_instrument_log("<Class_lapse_rate_Monthly::klone(string name) const@@@WQ_CRHM.cpp>");

InstrumentLogger::instance()->log_instrument_log("</Class_lapse_rate_Monthly::klone(string name) const@@@WQ_CRHM.cpp>");
    return new Class_lapse_rate_Monthly (name);
InstrumentLogger::instance()->log_instrument_log("</Class_lapse_rate_Monthly::klone(string name) const@@@WQ_CRHM.cpp>");
}
void Class_lapse_rate_Monthly :: decl (void)
{
InstrumentLogger::instance()->log_instrument_log("<Class_lapse_rate_Monthly::decl(void)@@@WQ_CRHM.cpp>");
    declvar ("lapse_rate_Mthly", TDim :: NHRU, "temperature lapse rate Mthly.", "(°C/100m)", & lapse_rate_Mthly);
    declvar ("lapse_rate", TDim :: NHRU, "temperature lapse rate.", "(°C/100m)", & lapse_rate);
InstrumentLogger::instance()->log_instrument_log("</Class_lapse_rate_Monthly::decl(void)@@@WQ_CRHM.cpp>");
}
void Class_lapse_rate_Monthly :: init (void)
{
InstrumentLogger::instance()->log_instrument_log("<Class_lapse_rate_Monthly::init(void)@@@WQ_CRHM.cpp>");
    for (hh = 0; hh < nhru; ++ hh) {
        lapse_rate [hh] = 0.0;
        lapse_rate_Mthly [hh] = 0.0;
    }
InstrumentLogger::instance()->log_instrument_log("</Class_lapse_rate_Monthly::init(void)@@@WQ_CRHM.cpp>");
}
void Class_lapse_rate_Monthly :: run (void)
{
InstrumentLogger::instance()->log_instrument_log("<Class_lapse_rate_Monthly::run(void)@@@WQ_CRHM.cpp>");
    long step = getstep ();
    long nstep = step % Global :: Freq;
    long JULIAN = julian ("now");
    if (nstep == 0)
    {
        for (hh = 0; hh < nhru; ++ hh) {
            if (JULIAN >= 274 && JULIAN < 305)
            {
                lapse_rate [hh] = 0.81;
            }

            else
            if (JULIAN >= 305 && JULIAN < 335)
            {
                lapse_rate [hh] = 0.83;
            }

            else
            if (JULIAN >= 335 && JULIAN < 365)
            {
                lapse_rate [hh] = 0.73;
            }

            else
            if (JULIAN >= 1 && JULIAN < 32)
            {
                lapse_rate [hh] = 0.66;
            }

            else
            if (JULIAN >= 32 && JULIAN < 60)
            {
                lapse_rate [hh] = 0.72;
            }

            else
            if (JULIAN >= 60 && JULIAN < 91)
            {
                lapse_rate [hh] = 0.8;
            }

            else
            if (JULIAN >= 91 && JULIAN < 121)
            {
                lapse_rate [hh] = 0.85;
            }

            else
            if (JULIAN >= 121 && JULIAN < 152)
            {
                lapse_rate [hh] = 0.8;
            }

            else
            if (JULIAN >= 152 && JULIAN < 182)
            {
                lapse_rate [hh] = 0.78;
            }

            else
            if (JULIAN >= 182 && JULIAN < 213)
            {
                lapse_rate [hh] = 0.6;
            }

            else
            if (JULIAN >= 213 && JULIAN < 244)
            {
                lapse_rate [hh] = 0.57;
            }

            else
            if (JULIAN >= 244 && JULIAN < 274)
            {
                lapse_rate [hh] = 0.76;
            }

            lapse_rate_Mthly [hh] = lapse_rate [hh];
        }
    }

InstrumentLogger::instance()->log_instrument_log("</Class_lapse_rate_Monthly::run(void)@@@WQ_CRHM.cpp>");
}