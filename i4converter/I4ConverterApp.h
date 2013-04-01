#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/dnd.h>
#include <wx/dir.h>
#include <wx/button.h>
#include <wx/progdlg.h>
#include <wx/artprov.h>
#include <vector>
using namespace std;

struct I4ExportInfo
{
	I4ExportInfo(const wxString& name)
		: fileName(name)
		, isMesh(true)
		, isMaterial(true)
		, isBone(true)
		, isAnimation(true)
	{
	}

	~I4ExportInfo()
	{
	}
	wxString	fileName;
	bool		isMesh;
	bool		isMaterial;
	bool		isBone;
	bool		isAnimation;
};


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

	void OnListSelected(wxListEvent& e);
	void OnListInsert(wxListEvent& e);
	void OnListDelete(wxListEvent& e);
	void OnListKey(wxListEvent& e);

	void OnBtnConvertClicked(wxCommandEvent& e);

	void OnPropertyChanged(wxPropertyGridEvent& e);

	void OnUpdateWorker(wxUpdateUIEvent& e);
	void OnWorkerEvent(wxThreadEvent& e);
	bool Cancelled();

	void BeginFbx(const wxString& srcFile);
	void WriteFbxMeshes();
	void WriteFbxMaterials();
	void WriteFbxBones();
	void WriteFbxAnimations();
	void EndFbx();

private:
	wxListCtrl*				listCtrl;
	wxPropertyGrid*			propGrid;
	wxButton*				btnConvert;
	wxTextCtrl*				logWindow;
	wxLog*					logOld;
	I4FbxConverter*			fbxConverter;

	// the progress dialog which we show while worker thread is running
    wxProgressDialog *m_dlgProgress;

    // was the worker thread cancelled by user?
    bool m_cancelled;
    wxCriticalSection m_csCancelled;        // protects m_cancelled

	wxString savePath;
};

class I4ExportWorkerThread : public wxThread
{
public:
    I4ExportWorkerThread(I4ConverterFrame *frame);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

	void setExportInfo(const vector<I4ExportInfo*>& info)
	{
		vecExportInfo = info;
	}
private:
	void UpdateProgress();

public:
    I4ConverterFrame*		frame;
	vector<I4ExportInfo*>	vecExportInfo;
	int						totalStep;
	int						curStep;
};
