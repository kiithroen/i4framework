#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/dnd.h>
#include <wx/dir.h>
#include <wx/button.h>
#include <wx/progdlg.h>

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

enum
{
	WORKER_EVENT = wxID_HIGHEST+1,
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
	void OnUpdateWorker(wxUpdateUIEvent& e);
	void OnWorkerEvent(wxThreadEvent& e);
	bool Cancelled();

	void ConvertFbx(const wxString& srcFile);

private:
	wxListCtrl*				listCtrl;
	wxPropertyGrid*			propGrid;
	wxTextCtrl*				logWindow;
	wxLog*					logOld;
	I4FbxConverter*			fbxConverter;
	wxCriticalSection		m_csFbxConverter;

	// the progress dialog which we show while worker thread is running
    wxProgressDialog *m_dlgProgress;

    // was the worker thread cancelled by user?
    bool m_cancelled;
    wxCriticalSection m_csCancelled;        // protects m_cancelled

	wxCriticalSection		m_csDlgProgress;
	wxString savePath;
};

class MyWorkerThread : public wxThread
{
public:
    MyWorkerThread(I4ConverterFrame *frame, const wxArrayString& fileNames);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

private:
	void UpdateProgress();

public:
    I4ConverterFrame*	m_frame;
	wxArrayString		fileNames;
	int					totalStep;
	int					curStep;
};
