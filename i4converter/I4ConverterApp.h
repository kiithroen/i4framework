#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/dnd.h>
#include <wx/dir.h>
#include <wx/button.h>

class I4FbxConverter;

class I4ConverterApp : public wxApp
{
public:
	I4ConverterApp(void);
	virtual ~I4ConverterApp(void);

	virtual bool OnInit();
};

enum
{
	ID_Quit = 1,
	ID_About,
};

enum
{
	ID_LIST_CTRL = 1000,
	ID_PROPERTY_GRID,
	ID_CONVERT_BUTTON,
};

class I4ConverterFrame : public wxFrame
{
public:
	I4ConverterFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	virtual ~I4ConverterFrame();

	void OnQuit(wxCommandEvent& e);
	void OnAbout(wxCommandEvent& e);

	void OnSelected(wxListEvent& e);

	void OnBtnConvertClicked(wxCommandEvent& e);
private:
	wxListCtrl*				listCtrl;
	wxPropertyGrid*			propGrid;
	wxTextCtrl*				logWindow;
	wxLog*					logOld;
	I4FbxConverter*			fbxConverter;
};