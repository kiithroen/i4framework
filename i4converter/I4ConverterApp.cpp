#include "stdafx.h"
#include "I4ConverterApp.h"
#include "I4FbxConverter.h"

class DnDFile : public wxFileDropTarget
{
public:
	DnDFile(wxListCtrl *pOwner) { listCtrl = pOwner; }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);

private:
	wxListCtrl*		listCtrl;
};

bool DnDFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
{
    size_t nFiles = filenames.GetCount();

    if (listCtrl != NULL)
    {
		wxArrayString files;

        for ( size_t n = 0; n < nFiles; n++ )
		{
			if (wxFileExists(filenames[n]))
			{
				files.push_back(filenames[n]);
			}
            else if (wxDirExists(filenames[n]))
			{
                wxDir::GetAllFiles(filenames[n], &files);
			}
		}

		for ( size_t n = 0; n < files.size(); n++ )
		{
			wxString ext;
			wxFileName::SplitPath(files[n], nullptr, nullptr, &ext);
			ext.LowerCase();
			if (ext == "fbx" && listCtrl->FindItem(-1, files[n]) == -1)
			{
				listCtrl->InsertItem(listCtrl->GetItemCount(), files[n]);
			}
		}
    }

    return true;
}

IMPLEMENT_APP(I4ConverterApp)

I4ConverterApp::I4ConverterApp(void)
{
}


I4ConverterApp::~I4ConverterApp(void)
{
}

bool I4ConverterApp::OnInit()
{
	I4ConverterFrame* frame = new I4ConverterFrame(_("I4Converter"), wxDefaultPosition, wxSize(800, 700));
	frame->Connect(ID_Quit, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&I4ConverterFrame::OnQuit);
	frame->Connect(ID_About, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&I4ConverterFrame::OnAbout);

	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

I4ConverterFrame::I4ConverterFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame( NULL, -1, title, pos, size )
{
	//------------- MENU -----------------
    wxMenuBar *menuBar = new wxMenuBar;

    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_About, _("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("E&xit") );

    menuBar->Append(menuFile, _("&File") );

    SetMenuBar(menuBar);

	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

	wxPanel* topPanel = new wxPanel(panel, wxID_ANY);
	vbox->Add(topPanel, 1, wxEXPAND|wxALL, 5);
	
	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

	//------------- LIST -----------------
	listCtrl = new wxListCtrl(topPanel, ID_LIST_CTRL,
                                    wxDefaultPosition, wxSize(400, 400),
                                    wxLC_REPORT | wxBORDER_THEME);

	listCtrl->SetDropTarget(new DnDFile(listCtrl));

	wxListItem itemCol;
    itemCol.SetText(wxT("FileName"));
    itemCol.SetImage(-1);
	itemCol.SetWidth(400);
    listCtrl->InsertColumn(0, itemCol);

	Connect(ID_LIST_CTRL, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(I4ConverterFrame::OnSelected));

	hbox->Add(listCtrl, 1, wxEXPAND|wxALL, 5);

	//------------- PROPERTY -----------------
	propGrid = new wxPropertyGrid(topPanel, ID_PROPERTY_GRID,wxDefaultPosition, wxSize(400, 400),
                        wxPG_SPLITTER_AUTO_CENTER |
                        wxPG_BOLD_MODIFIED );

	hbox->Add(propGrid, 1, wxEXPAND|wxALL, 5);
	
	topPanel->SetSizer(hbox);
	
	//------------ LOG -----------------------
	logWindow = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxSize(800, 100),
                                 wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);

    logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(logWindow));

	vbox->Add(logWindow, 0, wxEXPAND|wxALL, 10);

	//------------- BUTTON -----------------
	wxButton* btnConvert = new wxButton(panel, ID_CONVERT_BUTTON, wxT("Convert!"), wxDefaultPosition, wxSize(400, 50));
	vbox->Add(btnConvert, 0, wxEXPAND|wxALL, 10);

	Connect(ID_CONVERT_BUTTON, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(I4ConverterFrame::OnBtnConvertClicked));
	panel->SetSizer(vbox);

	//------------- PROGRESS -----------------
	m_dlgProgress = NULL;

	Connect(WORKER_EVENT, wxEVT_THREAD, wxThreadEventHandler(I4ConverterFrame::OnWorkerEvent));

	//------------- STATUS BAR -----------------
    CreateStatusBar();
    
	//----------- CONVERTER ------------------
	fbxConverter = new I4FbxConverter;
	fbxConverter->Initialize();

    SetStatusText( _("Ready!") );
}

I4ConverterFrame::~I4ConverterFrame()
{
	delete wxLog::SetActiveTarget(logOld);

	fbxConverter->Finalize();
	delete fbxConverter;
}

void I4ConverterFrame::OnQuit(wxCommandEvent& WXUNUSED(e))
{
    Close(true);
}

void I4ConverterFrame::OnAbout(wxCommandEvent& WXUNUSED(e))
{
    wxMessageBox( _("wxWidgets Hello World example."), 
                  _("About Hello World"),
                  wxOK|wxICON_INFORMATION, this );
}

void I4ConverterFrame::OnSelected(wxListEvent& e)
{
	wxListItem info;
    info.m_itemId = e.m_itemIndex;
    info.m_col = 0;
    info.m_mask = wxLIST_MASK_TEXT;
    if ( listCtrl->GetItem(info) )
    {
        wxLogMessage(wxT("Value of the 2nd field of the selected item: %s"),
                        info.m_text.c_str());
    }
    else
    {
        wxFAIL_MSG(wxT("wxListCtrl::GetItem() failed"));
    }
}

void I4ConverterFrame::OnBtnConvertClicked(wxCommandEvent& WXUNUSED(e))
{
	if (listCtrl->GetItemCount() <= 0)
		return;

	wxDirDialog dlg(this);
	if (dlg.ShowModal() != wxID_OK)
		return;

	savePath = dlg.GetPath();


	wxArrayString fileNames;

	for (int i = 0; i < listCtrl->GetItemCount(); ++i)
	{		
		wxListItem info;
		info.m_itemId = i;
		info.m_col = 0;
		info.m_mask = wxLIST_MASK_TEXT;
		if (listCtrl->GetItem(info))
		{
			fileNames.push_back(info.m_text);		
		}
	}

	MyWorkerThread *thread = new MyWorkerThread(this, fileNames);

    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
        return;
    }

    m_dlgProgress = new wxProgressDialog
                        (
                         wxT("Progress dialog"),
                         wxT("Wait until the thread terminates or press [Cancel]"),
						 fileNames.size()*6 + 1,
                         this,
                         wxPD_CAN_ABORT |
                         wxPD_APP_MODAL |
                         wxPD_ELAPSED_TIME |
                         wxPD_ESTIMATED_TIME |
                         wxPD_REMAINING_TIME
                        );

    // thread is not running yet, no need for crit sect
    m_cancelled = false;

    thread->Run();
}

void I4ConverterFrame::BeginFbx(const wxString& srcFile)
{
	wxString filePath;
	wxString fileName;
	wxString fileExt;
	wxFileName::SplitPath(srcFile, &filePath, &fileName, &fileExt);
		
	wxFileName destFileName(savePath, fileName);
	fbxConverter->Begin(srcFile.c_str(), destFileName.GetFullPath().c_str());
}

void I4ConverterFrame::WriteFbxMeshes()
{
	fbxConverter->WriteMeshes();
}

void I4ConverterFrame::WriteFbxMaterials()
{
	fbxConverter->WriteMaterials();
}

void I4ConverterFrame::WriteFbxBones()
{
	fbxConverter->WriteBones();
}

void I4ConverterFrame::WriteFbxAnimations()
{
	fbxConverter->WriteAnimations();
}

void I4ConverterFrame::EndFbx()
{
	fbxConverter->End();
}

void I4ConverterFrame::OnUpdateWorker(wxUpdateUIEvent& e)
{
    e.Enable( m_dlgProgress == NULL );
}

void I4ConverterFrame::OnWorkerEvent(wxThreadEvent& e)
{
    int n = e.GetInt();
    if ( n == -1 )
    {
        m_dlgProgress->Destroy();
        m_dlgProgress = (wxProgressDialog *)NULL;

        // the dialog is aborted because the event came from another thread, so
        // we may need to wake up the main event loop for the dialog to be
        // really closed
        wxWakeUpIdle();
    }
    else
    {
        if ( m_dlgProgress != NULL && !m_dlgProgress->Update(n) )
        {
            wxCriticalSectionLocker lock(m_csCancelled);

            m_cancelled = true;
        }
    }
}

bool I4ConverterFrame::Cancelled()
{
    wxCriticalSectionLocker lock(m_csCancelled);

    return m_cancelled;
}

MyWorkerThread::MyWorkerThread(I4ConverterFrame *frame, const wxArrayString& _fileNames)
        : wxThread()
		, m_frame(frame)
		, fileNames(_fileNames)
		, totalStep(0)
		, curStep(0)
{
}

void MyWorkerThread::OnExit()
{
}

wxThread::ExitCode MyWorkerThread::Entry()
{
	totalStep = fileNames.size()*6;
	curStep = 0;

	for (unsigned int i = 0; i < fileNames.size(); ++i)
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;

		m_frame->BeginFbx(fileNames[i]);
		UpdateProgress();
		if (m_frame->Cancelled())
			break;

		m_frame->WriteFbxMeshes();
		UpdateProgress();
		if (m_frame->Cancelled())
			break;

		m_frame->WriteFbxMaterials();
		UpdateProgress();
		if (m_frame->Cancelled())
			break;

		m_frame->WriteFbxBones();
		UpdateProgress();
		if (m_frame->Cancelled())
			break;

		m_frame->WriteFbxAnimations();
		UpdateProgress();
		if (m_frame->Cancelled())
			break;

		m_frame->EndFbx();
		UpdateProgress();		
		if (m_frame->Cancelled())
			break;
    }

	if (m_frame->Cancelled())
	{
		m_frame->EndFbx();
	}

    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT );
    event.SetInt(-1); // that's all
    wxQueueEvent( m_frame, event.Clone() );

    return NULL;
}

void MyWorkerThread::UpdateProgress()
{
	++curStep;
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT );
	
    event.SetInt(curStep);
	
	// send in a thread-safe way
    wxQueueEvent( m_frame, event.Clone() );
}