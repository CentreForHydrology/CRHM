//#include "stdafx.h"
#include <stdio.h>
#include "CRHMmain.h"


int main(int argc, char *argv[])
{
    CRHMmain * m = new CRHMmain();
    m->FormCreate();
    m->DoPrjOpen(argv[1], "");
    m->RunClick ();
}
