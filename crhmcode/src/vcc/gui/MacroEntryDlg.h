#pragma once

//Visual studio includes
#include "stdafx.h"
#include "afxdialogex.h"
//Standard library includes
#include <vector>
#include <string>
//Source includes
#include "CRHM_GUI.h"
#include "GlobalDll.h"
#include "Module.h"
#include "FileDialog.h"


class MacroEntryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MacroEntryDlg)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	MacroEntryDlg(CWnd* pParent = nullptr);   // standard constructor
	
	virtual ~MacroEntryDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = MacroEntry_DLG };
#endif

private:
	CEdit macroEditBox;

	std::vector<std::string>* ProjectFile{ NULL }; // hold project file
	std::vector<std::string>* ModuleList{ NULL };  // list of modules in group
	std::vector<std::pair<std::string, Module*>>* MacroList{ NULL };   // macros in project file
	std::vector<std::pair<std::string, Module*>>* DefinedList{ NULL }; // macros already defined
	std::vector<std::string>* ParaList{ NULL };    // defined parameters
	std::vector<std::string>* AKAList{ NULL };     // AKA defines

	Module GlobalModule;

	FileDialog* openfile{ NULL };
	FileDialog* savefile{ NULL };
	FileDialog* macrofile{ NULL };
	
	virtual BOOL OnInitDialog();

	//Buttons
	afx_msg void OnMacroSaveChanges();

	afx_msg void OnMacroCancelChanges();

	//Menu Items
	afx_msg void OnMacroFileOpen();

	afx_msg void OnMacroFileSave();

	afx_msg void OnMacroFileSaveAs();

	afx_msg void OnMacroFileCreateGroup();

	afx_msg void OnMacroFileClear();

	afx_msg void OnMacroFileExit();
	
	void OpenClick();
	afx_msg void OnBnClickedButton3();
	void CreateGroupClick();
	void AddGroup(int ii);
	void AddSupport(Module* ThisModule);
	void AddStringsToEditBox(std::vector<std::string>* tstr);
	void AddStringsToEditBox(std::string tstr);
	std::vector<std::string> * GetLinesFromEditBox();
	std::vector<std::string> * GetSelectedLinesFromEditBox();
	void SaveChangesClick();
	void DoSave();
	void SaveClick();
	void SaveAsClick();
	void DoLoad(std::string filepath);
	void OnBnClickedButton2(); //Old Cancel Code. Looks to actualy clear macros. 
};
