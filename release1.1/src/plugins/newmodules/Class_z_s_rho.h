//created by Manishankar Mondal

#include "ClassModule.h"

class Class_z_s_rho : public ClassModule {
public:

Class_z_s_rho(string Name, string Version = "undefined", CRHM::LMODULE Lvl = CRHM::PROTO) : ClassModule(Name, Version, Lvl) {};

// declared variables
  float*  z_s;            // total snowcover thickness (m)
  float*  rho;            // average snowcover density (kg/m^3)

// variable inputs
const float *SWE;

void decl(void);
void init(void);
void run(void);

Class_z_s_rho* klone(string name) const;
};