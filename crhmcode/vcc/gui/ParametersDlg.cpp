#include "../stdafx.h"
#include "afxdialogex.h"

#include "CRHM_GUI.h"
#include "ParametersDlg.h"

IMPLEMENT_DYNAMIC(ParametersDlg, CDialog)


ParametersDlg::ParametersDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(PARAMETERS_DLG, pParent)
{

}


ParametersDlg::~ParametersDlg()
{
}


void ParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_PARAM_DLG_MODULES_LIST_BOX, this->modules_list_box);
	DDX_Control(pDX, ID_PARAM_DLG_PARAM_LIST_BOX, this->parameters_list_box);
}


BEGIN_MESSAGE_MAP(ParametersDlg, CDialog)
	ON_LBN_SELCHANGE(ID_PARAM_DLG_MODULES_LIST_BOX, &ParametersDlg::OnSelectModule)
END_MESSAGE_MAP()


BOOL ParametersDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRHMmain * main = CRHMmain::getInstance();
	this->initalizeModulesListBox(main);
	
	/**
	* Sets the width of the list boxes for scroling purposes.
	*/
	this->modules_list_box.SetHorizontalExtent(1000);
	this->parameters_list_box.SetHorizontalExtent(1000);

	return TRUE;
}

void ParametersDlg::OnSelectModule()
{
	// Reset the parameters list box
	this->parameters_list_box.ResetContent();

	// Get the selected CString
	CString selectedText;
	int selectedIndex = this->modules_list_box.GetCurSel();
	this->modules_list_box.GetText(selectedIndex, selectedText);

	// Convert the CString to std::string
	CT2CA pszConvertedAnsiString(selectedText);
	std::string selectedString(pszConvertedAnsiString);

	
	if (selectedString == "Shared")
	{
		/*
		* If selected module is the shared module iterate
		* through all shared parameters and add to list box.
		*/
		for (
			std::map<std::string, ClassPar*, Classless<std::string>>::iterator sharedIt = Global::SharedMapPars.begin();
			sharedIt != Global::SharedMapPars.end(); 
			sharedIt++
			)
		{
			std::string trimedString = sharedIt->first.substr(7, std::string::npos);
			CString paramName(trimedString.c_str());
			this->parameters_list_box.AddString(paramName);
		}

	}
	else
	{
		// Remove module variation suffix from selected string
		int suffPos;
		if (suffPos = selectedString.find("#"), suffPos > -1)
		{
			selectedString = selectedString.substr(0, selectedString.length() - 2);
		}

		// Retrive the module from the model
		CRHMmain* model = CRHMmain::getInstance();
		std::map<std::string, ClassModule*>* modulesMap = model->getAllmodules();
		std::map<std::string, ClassModule*>::iterator selectedModuleIt = modulesMap->find(selectedString);

		if (!selectedModuleIt->second->isGroup)
		{
			// Module is not a group macro

			//Copy the parameters list and sort
			std::list<std::pair<std::string, ClassPar*>> parametersList = *selectedModuleIt->second->getParametersList();
			parametersList.sort(&ParametersDlg::compareParametersAlphabeticalyNoCase);

			// Add '*' to the begining of the shared parameters.
			for (
				std::list<std::pair<std::string, ClassPar*>>::iterator it = parametersList.begin();
				it != parametersList.end();
				it++
				)
			{
				if (Global::SharedMapPars.find("Shared "+it->first) != Global::SharedMapPars.end())
				{
					it->first = "*" + it->first;
				}
			}

			// Add the parameter names to the list box
			for (
				std::list<std::pair<std::string, ClassPar*>>::iterator it = parametersList.begin();
				it != parametersList.end();
				it++
				)
			{
				CString paramName(it->first.c_str());
				this->parameters_list_box.AddString(paramName);
			}

		}
		else
		{
			// Module is a group macro
			ClassMacro * groupModule = (ClassMacro *) selectedModuleIt->second;

			// Retrive list of modules that make up the group macro 
			std::vector<std::pair<std::string, ClassModule*>> * modulesVector = groupModule->GrpStringList;
			// Declare a list to hold the parameters
			std::list<std::pair<std::string, ClassPar*>> allParametersList;

			// For each module add the assoicated parameters to the list. 
			for (
				std::vector<std::pair<std::string, ClassModule*>>::iterator modulesIt = modulesVector->begin();
				modulesIt != modulesVector->end();
				modulesIt++
				)
			{

				std::list<std::pair<std::string, ClassPar*>> parametersList = *modulesIt->second->getParametersList();
				
				for (
					std::list<std::pair<std::string, ClassPar*>>::iterator it = parametersList.begin();
					it != parametersList.end();
					it++
					)
				{
					allParametersList.push_back(std::pair<std::string, ClassPar*>(it->first, it->second));
				}

			}

			// Sort the list of parameters
			allParametersList.sort(&ParametersDlg::compareParametersAlphabeticalyNoCase);

			// Add '*' to the begining of the shared parameters.
			for (
				std::list<std::pair<std::string, ClassPar*>>::iterator it = allParametersList.begin();
				it != allParametersList.end();
				it++
				)
			{
				if (Global::SharedMapPars.find("Shared " + it->first) != Global::SharedMapPars.end())
				{
					it->first = "*" + it->first;
				}
			}

			// Add the parameter names to the list box
			for (
				std::list<std::pair<std::string, ClassPar*>>::iterator it = allParametersList.begin();
				it != allParametersList.end();
				it++
				)
			{
				CString paramName(it->first.c_str());
				this->parameters_list_box.AddString(paramName);
			}

		}

	}

}


void ParametersDlg::initalizeModulesListBox(CRHMmain* main)
{
	//Add the "Shared" module to the list box
	this->modules_list_box.AddString(L"Shared");

	//Get the list of used modules and copy into a seccond list for sorting.
	std::list<std::pair<std::string, ClassModule*>>* modules_list = main->getOurModules();
	std::list<std::pair<std::string, ClassModule*>> sorted_modules_list = *modules_list;

	// Sort the list of modules alphabeticaly ignoring case.
	sorted_modules_list.sort(&ParametersDlg::compareModulesAlphabeticalyNoCase);

	// Place each of the modules into the list box
	for (
		std::list<std::pair<std::string, ClassModule*>>::iterator moduleIteratior = sorted_modules_list.begin();
		moduleIteratior != sorted_modules_list.end();
		moduleIteratior++
		)
	{
		// If the module has a varaition append the apropreate suffix
		if (moduleIteratior->second->variation != 0)
		{
			std::string varSuffix("#0");
			varSuffix[1] += (char)(log(moduleIteratior->second->variation) / log(2) + 1);
			std::string moduleStringWithVariation = moduleIteratior->first + varSuffix;
			
			// Add the module to the list box
			CString moduleName(moduleStringWithVariation.c_str());
			this->modules_list_box.AddString(moduleName);
		}
		else
		{
			// Add the module to the list box
			CString moduleName(moduleIteratior->first.c_str());
			this->modules_list_box.AddString(moduleName);
		}
		
	}
}


BOOL ParametersDlg::compareModulesAlphabeticalyNoCase(std::pair<std::string, ClassModule*> item_a, std::pair<std::string, ClassModule*> item_b)
{
	std::string lower_a;
	for (std::string::iterator c = item_a.first.begin(); c != item_a.first.end(); c++)
	{
		lower_a.push_back(std::tolower(*c));
	}

	std::string lower_b;
	for (std::string::iterator c = item_b.first.begin(); c != item_b.first.end(); c++)
	{
		lower_b.push_back(std::tolower(*c));
	}

	return lower_a < lower_b;

}


BOOL ParametersDlg::compareParametersAlphabeticalyNoCase(std::pair<std::string, ClassPar*> item_a, std::pair<std::string, ClassPar*> item_b)
{
	std::string lower_a;
	for (std::string::iterator c = item_a.first.begin(); c != item_a.first.end(); c++)
	{
		lower_a.push_back(std::tolower(*c));
	}

	std::string lower_b;
	for (std::string::iterator c = item_b.first.begin(); c != item_b.first.end(); c++)
	{
		lower_b.push_back(std::tolower(*c));
	}

	return lower_a < lower_b;

}