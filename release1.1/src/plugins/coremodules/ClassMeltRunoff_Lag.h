//created by Manishankar Mondal

#include "ClassModule.h"

class ClassMeltRunoff_Lag : public ClassModule {
public:

ClassMeltRunoff_Lag(string Name, string Version = "undefined", CRHM::LMODULE Lvl = CRHM::PROTO) : ClassModule(Name, Version, Lvl) {};

// index to module to be called
ClassModule* Netroute_module; // used to call preser for simple projects

// declared variables
float *In_Lagstorage;
long *Tf;

// get variables:
const float *snowmelt_int;

// put variables
float *runLag;

// declared parameters
const float *hru_area;
const long  *inhibit_MeltRunoff_Lag;

void decl(void);
void init(void);
void run(void);

ClassMeltRunoff_Lag* klone(string name) const;
};