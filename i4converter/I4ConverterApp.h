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

struct ExportInfo
{
	ExportInfo(const wxString& name)
		: fileName(name)
		, isMesh(true)
		, isMaterial(true)
		, isBone(true)
		, isAnimation(true)
	{
	}

	~ExportInfo()
	{
	}
	wxString	fileName;
	bool		isMesh;
	bool		isMaterial;
	bool		isBone;
	bool		isAnimation;
};


class FbxConverter;

class ConverterApp : public wxApp
{
public:
	ConverterApp(void);
	virtual ~ConverterApp(void);

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

class ConverterFrame : public wxFrame
{
public:
	ConverterFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	virtual ~ConverterFrame();

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
	FbxConverter*			fbxConverter;

	// the progress dialog which we show while worker thread is running
    wxProgressDialog *m_dlgProgress;

    // was the worker thread cancelled by user?
    bool m_cancelled;
    wxCriticalSection m_csCancelled;        // protects m_cancelled

	wxString savePath;
};

class ExportWorkerThread : public wxThread
{
public:
    ExportWorkerThread(ConverterFrame *frame);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

	void setExportInfo(const vector<ExportInfo*>& info)
	{
		vecExportInfo = info;
	}
private:
	void UpdateProgress();

public:
    ConverterFrame*		frame;
	vector<ExportInfo*>	vecExportInfo;
	int						totalStep;
	int						curStep;
};
