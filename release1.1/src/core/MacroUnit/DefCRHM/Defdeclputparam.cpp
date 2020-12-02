#include "Defdeclputparam.h"

Defdeclputparam::Defdeclputparam(ClassMacro* Macro_) : DefCRHM(Macro_) {
	module = DefStringList->Strings[1];
	name = DefStringList->Strings[2];
	Units = DefStringList->Strings[3];
}


void Defdeclputparam::CallDecl() {
	MapPar::iterator itPar;
	ClassPar* thisPar = NULL;
	int GetUnit;

	GetUnit = Macro->FindWildParFloat(name, thisPar, false, false); // name

	if (thisPar != NULL && thisPar->varType == TVar::Int)
		Int = true;
	else
		Int = false;

	if (Int)
		Macro->declputparam(module, name, Units, &fix_long);
	else
		Macro->declputparam(module, name, Units, &fix, &fix2);

	if (Global::BuildFlag == TBuild::INIT) { // moved down 06/20/11

		FP = Macro->vars.find(name);
		if (FP == Macro->vars.end()) {
			Macro->vars.insert(make_pair(name, VarCHRM()));
			FP = Macro->vars.find(name);
		}

		FP->second.name = name;
		FP->second.Ihh = 0;
		FP->second.Ill = 0;
		FP->second.IndexMax = Macro->nhru - 1;
		FP->second.IndexMaxLay = Macro->nhru - 1; // FIX *******
		FP->second.Me = Macro;

		if (Int) {
			FP->second.kind = TV::CRHMint;
			FP->second.ivalue = const_cast<long*> (fix_long);
		}
		else if (thisPar != NULL && (thisPar->dimen == TDim::NDEF || thisPar->dimen == TDim::NDEFN)) {
			FP->second.kind = TV::CRHM2;
			FP->second.value2 = const_cast<double**> (fix2);
		}
		else {
			FP->second.kind = TV::CRHM;
			FP->second.value = const_cast<double*> (fix);
		}
	}
}


void Defdeclputparam::CallInit(long nhru, long nlay) { // check
	FP->second.IndexMax = nhru - 1;
}