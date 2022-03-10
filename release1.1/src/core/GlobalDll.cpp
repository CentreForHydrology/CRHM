#if defined (_WIN32)
    #include "stdafx.h"
#endif

#include <stdio.h>
#include "GlobalDll.h"
#include "InstrumentLogger.h"

using namespace std; bool Global :: NaNcheck;
bool Global :: LOGVARLOAD;
long Global :: GroupCntTrk;
long Global :: StructCntTrk;
long Global :: Freq;
long Global :: DTindx;
long Global :: DTmin;
long Global :: DTmax;
long Global :: IndxMin;
long Global :: IndxMax;
long Global :: maxobs;
long Global :: maxlay;
long Global :: maxhru;
long Global :: nobs;
long Global :: nlay;
long Global :: nhru;
long Global :: CRHMStatus;
long Global :: CurrentModuleRun;
long Global :: CRHMControlSaveCnt;
BitSet Global :: RunUpBitSet;
BitSet Global :: ModuleBitSet;
double Global :: Interval;
double Global :: DTstart;
double Global :: DTend;
TDateTime Global :: DTnow;
TDateTime Global :: RapidAdvanceTo;
TDateTime Global :: LoopTo;
long Global :: LoopCnt;
long Global :: LoopCntDown;
TBuild Global :: BuildFlag;
TIMEFORMAT Global :: TimeFormat;
MapVar Global :: MapVars;
MapVar Global :: MapVarsGet;
MapVar Global :: MapVarsPut;
MapVar Global :: MapObsGet;
MapPar Global :: MapPars;
MapPar Global :: SharedMapPars;
Mapstr Global :: Mapgetvar;
Mapstr Global :: Mapputvar;
Mapstr Global :: Mapreadvar;
Mapstr Global :: Mapdeclvar;
Mapstr Global :: Mapdeclpar;
Mapstr Global :: Mapdeclobs;
Mapstr Global :: Mapdeclstat;
Mapstr2 Global :: MapAKA;
TStringList * (Global :: AllModulesList);
TStringList * (Global :: OurModulesList);
TStringList * (Global :: MacroModulesList);
TStringList * (Global :: AllModelsList);
TStringList * (Global :: ModelModulesList);
TStringList * (Global :: OurHelpList);
TStringList * (Global :: PendingDLLModuleList);
TStringList * (Global :: ReportList);
TStringList * (Global :: DeclRootList);
TStringList * (Global :: OldModuleName);
TStringList * (Global :: NewModuleName);
double * Global :: lapse_rate;
double * Global :: ppt_adj;
double * Global :: hru_elev;
long ** Global :: HRU_OBS;
double * Global :: Warming_t;
double * Global :: Warming_p;
long * Global :: RH_VP_flag;
long * Global :: RH_VP_flag2;
long Global :: RH_EA_obs;
long Global :: OBS_AS_IS;
long Global :: WQ_prj;
double ** Global :: OBS_ELEV;
long ** Global :: HRU_OBS_DIRECT;
double ** Global :: obs_t;
double ** Global :: obs_ea;
double ** Global :: obs_rh;
double ** Global :: obs_t_obs;
TObject * Global :: thisVar;
double ** Global :: t_layvalues;
double ** Global :: rh_layvalues;
