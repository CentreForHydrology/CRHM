#pragma once

#include <afxwin.h>
#include <string>

#include "../resource.h"
#include "../../src/core/ClassPar.h"


class ParamDlgCard : public CDialog
{
private:
	/*
	* Defines the resource assoicated with this class.
	*/
	enum { IDD = PARAMETERS_CARD };

	/*
	* CRect object representing the original size of the inserted pane.
	*/
	CRect original_rectangle;

	/**
	* Tracks the scroll position of the pane.
	*/
	int	scroll_position;

	/**
	* Tracks the actual height of the pane
	*/
	int	pane_height;

	/**
	* Holds the parameter associated with this card 
	*/
	ClassPar * parameter;

	/**
	* Holders for fonts used on the card
	*/
	CFont * pointFont120;
	CFont * pointFont100;
	CFont * pointFont80;
	CFont * pointFont60;

	/**
	* Vector that holds the CEdit references for the row label items
	*/
	std::vector<CEdit*> rowLabels;

	/**
	* Vector that holds the CEdit references for the column header items
	*/
	std::vector<CEdit*> colHearders;

	std::vector<std::vector<CEdit*>> valueGrid;


public:
	/*
	* Standard constructor
	*/
	ParamDlgCard(ClassPar * param, CWnd* pParent = NULL );

	/*
	* Deconstructor
	*/
	~ParamDlgCard();

	/**
	* Initalizes the help information for the parameter card
	*/
	void InitalizeValues();

	/**
	* Renders the value grid for the parameter
	*/
	void RenderGrid();

protected:

	/**
	* Connect data layer to the view.
	*/
	virtual void DoDataExchange(CDataExchange* pDX);

	/**
	* Initalize the dialog
	*/
	virtual BOOL OnInitDialog();

	/**
	* Handles vertical scrolling by use of the scroll bar.
	*/
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	/**
	* Handles vertical scrolling by use of the scroll wheel.
	*/
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	/**
	* Handles resizing of the pane.
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	/**
	* Overrides OnOK to prevent Enter Key press from closing the card
	*/
	void OnOK();

	/**
	* Overrides OnCancel to prevent esc key press from closing the card
	*/
	void OnCancel();
	
	DECLARE_MESSAGE_MAP()

};