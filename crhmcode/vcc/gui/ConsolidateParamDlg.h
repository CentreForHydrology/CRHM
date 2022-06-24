#pragma once

#include "../resource.h"
#include "../../src/core/ClassPar.h"
#include "CardScrollPane.h"

/**
* Dialog for consolidating parameters that have the same name but different values across modules
*/
class ConsolidateParamDlg : public CDialog
{

	DECLARE_DYNAMIC(ConsolidateParamDlg)

	/*
	* Defines the dialog resource ID 
	*/
	enum { IDD = CONSOLIDATE_PARAM_DLG };

public:

	/**
	* Constructor
	* 
	* @param pParent - CWnd* pointer to parent window. Optional.
	*/
	ConsolidateParamDlg(std::map<std::string, std::list<ClassPar*>*>* candidates, CWnd* pParent = nullptr);
	
	/**
	* Deconstructor 
	*/
	~ConsolidateParamDlg();

private:

	/**
	* Map of parameter names to a list of parameter objects that have differing values.
	*/
	std::map<std::string, std::list<ClassPar*>*> candidates;

	/**
	* List box containing the candidate parameters for consolidation
	*/
	CListBox candidates_list_box;

	/**
	* Scroll pane that holds the parameter cards
	*/
	CardScrollPane * scrollPane;

	/**
	* Performs data exchange to properly connect graphical elements to the model.
	*/
	virtual void DoDataExchange(CDataExchange* pDX);

	/**
	* Initalizes the dialog elements after creating the dialog.
	*/
	virtual BOOL OnInitDialog();

	/**
	* Sets the inital contents of the candidates list box based on the passed in map.
	*/
	void InitalizeCandidatesListBox();

	/*
	* Handler when selecting a candidate parameter to consolidate.
	* 
	* When a parameter is selected the parameter version cards are created in the scroll pane.
	*/
	afx_msg void OnSelectCandidate();

	/**
	* Handles the receiving of a UWM_USE_FOR_ALL message
	*
	* Processes the passed parameter and sets all parameters with that name to have the same values.
	* 
	* @param wParam - WPARAM pointer to the shared parameter to be used for all parameters with that name
	* @param lParam - LPARAM pointer to the shared parameter to be used for all parameters with that name
	*/
	afx_msg LRESULT OnUseForAllMsg(WPARAM wParam, LPARAM lParam);

	/**
	* Handles when the Return button is clicked. 
	* 
	* Closes the dialog while returning false.
	*/
	afx_msg void OnReturnClicked();

	/**
	* Handles when the Finish button is clicked. 
	* 
	* Closes the dialog while returning true.
	*/
	afx_msg void OnFinishClicked();

	/**
	* Method that triggers when the window is resized
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	/**
	* Declares handlers for messages dispached by the GUI
	*/
	DECLARE_MESSAGE_MAP()
};

