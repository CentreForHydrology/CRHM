#include "Defdeclvar.h"

Defdeclvar::Defdeclvar(ClassMacro* Macro_) : DefCRHM(Macro_) 
{
	name = DefStringList->at(1);
	Dim = getTDim(DefStringList->at(2));
	Description = DefStringList->at(3);
	Units = DefStringList->at(4);

	if (DefCRHM::DefStringList->size() > 5 && DefStringList->at(5) == "Int")
	{
		Int = true;
	}
	else
	{
		Int = false;
	}

	if (DefCRHM::DefStringList->size() > 6)
	{
		nlay = Strtolong(DefStringList->at(6));
	}	
}


void Defdeclvar::CallDecl() {

	if (Int) {
		// TODO: extend this call to support multiple dimensions (fix_long2, nlay)
		Macro->declvar(name, Dim, Description, Units, &fix_long);
	} else {
		Macro->declvar(name, Dim, Description, Units, &fix, &fix2, nlay);
	}

	FP = Macro->vars.find(name);
	if (FP == Macro->vars.end()) {
		Macro->vars.insert(make_pair(name, VarCHRM()));
		FP = Macro->vars.find(name);
	}

	FP->second.name = name;
	FP->second.Ihh = 0;
	FP->second.Ill = 0;
	FP->second.IndexMax = Global::nhru - 1;
	FP->second.IndexMaxLay = Macro->nhru - 1; // FIX *******
	FP->second.Me = Macro;

	if (Int) {
		FP->second.kind = TV::CRHMint;
		FP->second.ivalue = const_cast<long*> (fix_long);
	}
	else if (Dim == TDim::NDEF || Dim == TDim::NDEFN) {
		FP->second.value2 = fix2;
		FP->second.kind = TV::CRHM2;
	}
	else {
		FP->second.kind = TV::CRHM;
		FP->second.value = fix;
	}
}

void Defdeclvar::CallInit(long nhru, long nlay) {
	FP->second.IndexMax = nhru - 1;
	for (int hh = 0; hh <= FP->second.IndexMax; ++hh) {
		FP->second.Ihh = hh;
		FP->second.put(0.0);
	}
}