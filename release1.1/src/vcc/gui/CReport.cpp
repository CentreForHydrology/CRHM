// CReport.cpp : implementation file
//

#include "stdafx.h"
#include "CRHM_GUI.h"
#include "CReport.h"
#include "afxdialogex.h"
#include "MacroEntryDlg.h"
#include "../../core/CRHMmain/CRHMmain.h"


// CReport dialog

IMPLEMENT_DYNAMIC(CReport, CDialog)

CReport::CReport(CWnd* pParent /*=nullptr*/)
	: CDialog(REPORT_DLG, pParent)
{

}

CReport::~CReport()
{
}

void CReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, reportlabel, reportlabel);
	DDX_Control(pDX, IDC_LIST1, listbox1);
	DDX_Control(pDX, IDC_EDIT1, editbox1);
	DDX_Control(pDX, IDC_STATIC2, static2);
}


BEGIN_MESSAGE_MAP(CReport, CDialog)
	ON_COMMAND(ID_EXTRACTGROUP, &CReport::OnExtractgroup)
	ON_LBN_DBLCLK(IDC_LIST1, &CReport::OnLbnDblclkList1)
	ON_COMMAND(ID_FILE_SAVE32791, &CReport::OnFileSave32791)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CReport message handlers


void CReport::OnExtractgroup()
{
	// TODO: Add your command handler code here



	ClassModule *thisMod;

	listbox1.ResetContent();
	editbox1.SetWindowText(_T(""));
	//ListBox1->Clear();
	//Memo1->Lines->Clear();

	static2.SetWindowTextW(_T("Macros Used"));
	//SetWindowText(reportlabel, _T("ExtractGroup"));	
	//Label1->Tag = 4;

	for (int ii = 0; ii < Global::OurModulesList->Count; ++ii) {
		string S = Global::OurModulesList->Strings[ii];
		long Indx = Global::AllModulesList->IndexOf(S);
		thisMod = (ClassModule *)Global::AllModulesList->Objects[Indx];
		if (thisMod->isGroup)
		{
			CString cstr(S.c_str());
			listbox1.AddString(cstr);
			//ListBox1->Add(S, (TObject*)thisMod);
		}
	} // for
}


void CReport::OnLbnDblclkList1()
{
	// TODO: Add your control notification handler code here

	//getting the group name.
	CString cstr;
	int index = listbox1.GetCurSel();
	listbox1.GetText(index, cstr);
	CT2A str(cstr);
	string groupname = str;

	//getting the group object and class module.
	ClassModule *thisMod;
	long Indx = Global::AllModulesList->IndexOf(groupname);
	thisMod = (ClassModule *)Global::AllModulesList->Objects[Indx];

	ExtractGroupFile(thisMod);

}

void CReport::ExtractGroupFile(ClassModule *thisModule) {

	CString newline("\r\n");
	CString cstr("");
	//editbox.SetWindowText(cstr);

	ClassMacro *thisMacro;
	string S, SS;
	//long Variation; varible is unreferenced commenting out for now - jhs507
	MapPar::iterator itPar;
	ClassPar *thisPar;

	if (thisModule->isGroup)
		thisMacro = dynamic_cast<ClassMacro *> (thisModule);
	else
		return;

	editbox1.SetWindowTextW(_T(""));
	
	TStringList * stringlist;
	stringlist = new TStringList();
	
	
	CRHMmain * test = CRHMmain::getInstance();
	
	S = thisMacro->Name.c_str();
	

	//S = "*** extracted group: '" + S + "' from '" + Main->OpenDialogPrj->FileName + "' ***";
	//S = "*** extracted group: '" + S + "' from '" + "' ***";
	stringlist->Add("*** extracted group: '" + S + "' from '" + test->OpenProjectPath + "' ***");
	
	stringlist->Add("######");
	stringlist->Add("Dimensions:");
	stringlist->Add("######");
	stringlist->Add(string("nhru " + Common::longtoStr(Add_NHRU(S, Global::MacroModulesList))));
	stringlist->Add(string("nlay " + Common::longtoStr(Global::nlay)));
	stringlist->Add(string("nobs " + Common::longtoStr(1)));
	stringlist->Add("######");

	stringlist->Add("Macros:");
	stringlist->Add("######");

	long MacroPos = stringlist->Count;
	stringlist->Add("######");

	stringlist->Add("Modules:");
	stringlist->Add("######");

	list<ModulePtr> ::iterator iterM;
	iterM = ((ClassMacro *)thisModule)->Modules.begin();
	while (iterM != ((ClassMacro *)thisModule)->Modules.end()) {
		std::weak_ptr<ClassModule> MP((*iterM));
		S = (*iterM)->NameRoot.c_str();

		if ((*iterM)->DLLName == "Macro")
			AddMacroCode(S, Global::MacroModulesList, stringlist, MacroPos);

		if ((*iterM)->variation > 0) {
			string AA("#0 ");
			AA[1] += (char) (log((*iterM)->variation) / log(2) + 1);
			S = S + AA.c_str();
		}
		else
			S = S + " ";

		S = S + (*iterM)->DLLName + " ";
		S = S + (*iterM)->Version;
		stringlist->Add(S);
		++iterM;
	} // while

	stringlist->Add("######");

	//Main->RemoveSharedParams();

	stringlist->Add("Parameters:");
	stringlist->Add("######");

	S = thisMacro->Name.c_str();

	for (itPar = Global::MapPars.begin(); itPar != Global::MapPars.end(); itPar++) {
		thisPar = (*itPar).second;
		if (S == thisPar->module.c_str()) {
			if (thisPar->param == "HRU_group")
				continue;

			string S = string("Shared " + string(thisPar->param.c_str()));

			if (thisPar->varType != TVar::Txt)
				S += "\t<\t" + FloatToStrF(thisPar->minVal, TFloatFormat::ffGeneral, 4, 0) + "\t" + FloatToStrF(thisPar->maxVal, TFloatFormat::ffGeneral, 4, 0) + "\t>";

			stringlist->Add(S);

			for (int jj = 0; jj<thisPar->lay; jj++) {
				S = "";
				for (int ii = 0; ii < thisPar->dim; ii++) {
					if (thisPar->varType == TVar::Float)
						S = S + FloatToStrF(thisPar->layvalues[jj][ii], TFloatFormat::ffGeneral, 4, 0) + "\t";
					else if (thisPar->varType == TVar::Int)
						S = S + FloatToStrF(thisPar->ilayvalues[jj][ii], TFloatFormat::ffFixed, 8, 0) + "\t";
					else if (thisPar->varType == TVar::Txt)
						if (thisPar->Strings->Count > ii)
							S = S + "'" + thisPar->Strings->Strings[ii] + "'\t";
						else
							S = "''\t"; // handle null string

					if (ii % 16 == 15) {
						stringlist->Add(S);
						S = "";
					}
				} // for dim
				if (S.length() > 0) { stringlist->Add(S); S = ""; }
			} // for layers
			if (S.length() > 0) stringlist->Add(S);
		}
	} // for MapPars

	stringlist->Add("######");

	CString cstrtemp, text;
	for (int i = 0; i < stringlist->Count; i++)
	{
		string strtemp = stringlist->Strings[i];
		cstrtemp = strtemp.c_str();
		
		text += newline + cstrtemp;
	}

	editbox1.SetWindowTextW(text);

	//Main->SqueezeParams(Sender);  // converts identical parameters to shared parameters
}

long CReport::Add_NHRU(string S, TStringList *MacroModulesList) {

	for (int ii = 0; ii < MacroModulesList->Count; ++ii) {
		int found = MacroModulesList->Strings[ii].find(S);
		if (found >= 0) {
			string Trim = Common::trimleft(MacroModulesList->Strings[++ii]);
			long len = Trim.length();
			if (len < 11)
				return Global::nhru;
			else {
				string temp = Trim.substr(10, len - 10);
				CString ctemp(temp.c_str());
				long n = StrToInt(ctemp);
				if (n <= 0)
					return Global::nhru;
				else
					return n;
			}
		}
	}
	return 0; //added fall through case if the module is not found - jhs507
}

void CReport::AddMacroCode(string S, TStringList * MacroModulesList, TStringList* Memo1, long &MacroPos) {

	int endpos=0;

	for (int ii = 0; ii < MacroModulesList->Count; ++ii) {
		int found = MacroModulesList->Strings[ii].find(S);
		if (found >= 0) {
			Memo1->Add("'" + MacroModulesList->Strings[ii] + "'");
			MacroPos++;
			do {
				++ii;
				Memo1->Add("'" + MacroModulesList->Strings[ii] + "'");
				MacroPos++;

				endpos = MacroModulesList->Strings[ii].find("end");
			} while (!(endpos >= 0 && MacroModulesList->Strings[ii].length() == 3) && ii < MacroModulesList->Count - 1);
			break;
		}
	}
}



void CReport::OnFileSave32791()
{
	// TODO: Add your command handler code here

	//if (!SaveDialog1->FileName.IsEmpty())
	//	Memo1->Lines->SaveToFile(SaveDialog1->FileName);
	//else SaveAs1Click(Sender);

	CString cstr;
	editbox1.GetWindowText(cstr);
	CT2A str(cstr);
	TStringList * strings = new TStringList();
	string temp = str;	

	int l = temp.length();
	for (int i = 0; i < l; i++)
	{
		if (i < l - 1)
		{
			if (temp[i] == '\r') { temp[i] = ' '; }
		}
	}

	strings->Add(temp);


	FileDialog * filedialog;
	filedialog = new FileDialog();
	if (filedialog->Save("rpt") == true)
	{
		strings->SaveToFile(filedialog->filepath);
		MessageBox(_T("Group Saved"));
	}
}


void CReport::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: Add your message handler code here
	OnExtractgroup();
}
