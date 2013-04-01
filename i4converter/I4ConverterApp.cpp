#include "stdafx.h"
#include "I4ConverterApp.h"
#include "I4FbxConverter.h"
class I4DnDFileToListCtrl : public wxFileDropTarget
{
public:
	I4DnDFileToListCtrl(wxListCtrl *pOwner) { listCtrl = pOwner; }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);

private:
	wxListCtrl*		listCtrl;
};

bool I4DnDFileToListCtrl::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
{
    size_t nFiles = filenames.GetCount();

    if (listCtrl != NULL)
    {
		vector<I4ExportInfo*> vecExportInfo;

        for ( size_t n = 0; n < nFiles; n++ )
		{
			if (wxFileExists(filenames[n]))
			{
				vecExportInfo.push_back(new I4ExportInfo(filenames[n]));
			}
            else if (wxDirExists(filenames[n]))
			{
				wxArrayString files;
                wxDir::GetAllFiles(filenames[n], &files);
				
				for (unsigned int i = 0; i < files.GetCount(); ++i)
				{
					vecExportInfo.push_back(new I4ExportInfo(files[i]));
				}
			}
		}

		for ( size_t n = 0; n < vecExportInfo.size(); n++ )
		{
			wxString ext;
			wxFileName::SplitPath(vecExportInfo[n]->fileName, nullptr, nullptr, &ext);
			ext.LowerCase();
			if (ext == "fbx" && listCtrl->FindItem(-1, vecExportInfo[n]->fileName) == -1)
			{
				int id = listCtrl->InsertItem(listCtrl->GetItemCount(), vecExportInfo[n]->fileName);
				listCtrl->SetItemPtrData(id, (wxUIntPtr)vecExportInfo[n]);
				listCtrl->SetItemState(id, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
			}
			else
			{
				delete vecExportInfo[n];
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

	listCtrl->SetDropTarget(new I4DnDFileToListCtrl(listCtrl));

	wxListItem itemCol;
    itemCol.SetText(wxT("FileName"));
    itemCol.SetImage(-1);
	itemCol.SetWidth(400);
    listCtrl->InsertColumn(0, itemCol);

	Connect(ID_LIST_CTRL, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(I4ConverterFrame::OnListSelected));
	Connect(ID_LIST_CTRL, wxEVT_COMMAND_LIST_INSERT_ITEM, wxListEventHandler(I4ConverterFrame::OnListInsert));
	Connect(ID_LIST_CTRL, wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler(I4ConverterFrame::OnListDelete));
	Connect(ID_LIST_CTRL, wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(I4ConverterFrame::OnListKey));

	hbox->Add(listCtrl, 1, wxEXPAND|wxALL, 5);

	//------------- PROPERTY -----------------
	propGrid = new wxPropertyGrid(topPanel, ID_PROPERTY_GRID,wxDefaultPosition, wxSize(400, 400),
                        wxPG_SPLITTER_AUTO_CENTER |
                        wxPG_BOLD_MODIFIED );

	Connect(ID_PROPERTY_GRID, wxEVT_PG_CHANGED, wxPropertyGridEventHandler(I4ConverterFrame::OnPropertyChanged));

	hbox->Add(propGrid, 1, wxEXPAND|wxALL, 5);
	
	topPanel->SetSizer(hbox);
	
	//------------ LOG -----------------------
	logWindow = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxSize(800, 100),
                                 wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);

    logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(logWindow));

	vbox->Add(logWindow, 0, wxEXPAND|wxALL, 10);

	//------------- BUTTON -----------------
	btnConvert = new wxButton(panel, ID_CONVERT_BUTTON, wxT("Convert!"), wxDefaultPosition, wxSize(400, 50));
	btnConvert->Disable();

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

    SetStatusText( _("Ready! Drag and Drop Files or Directory.") );
}

I4ConverterFrame::~I4ConverterFrame()
{
	for (int i = 0; i < listCtrl->GetItemCount(); ++i)
    {
		I4ExportInfo* info = (I4ExportInfo*)listCtrl->GetItemData(i);
		if (info)
		{
			delete info;
		}
    }

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

void I4ConverterFrame::OnListSelected(wxListEvent& e)
{
	I4ExportInfo* info = (I4ExportInfo*)listCtrl->GetItemData(e.m_itemIndex);
	if (info)
	{
		propGrid->Clear();
		wxBitmap bmp = wxArtProvider::GetBitmap(wxART_REPORT_VIEW);
		wxPGProperty* pid = propGrid->Append( new wxPropertyCategory( wxT("wxWidgets Library Configuration") ) );

		propGrid->Append(new wxBoolProperty("Mesh", wxPG_LABEL,info->isMesh));
		propGrid->Append(new wxBoolProperty("Material", wxPG_LABEL, info->isMaterial));
		propGrid->Append(new wxBoolProperty("Bone", wxPG_LABEL, info->isBone));
		propGrid->Append(new wxBoolProperty("Animation", wxPG_LABEL,info->isAnimation));
		propGrid->SetPropertyAttribute(pid,wxPG_BOOL_USE_CHECKBOX,true,wxPG_RECURSE);
	}
}

void I4ConverterFrame::OnListInsert(wxListEvent& e)
{
	btnConvert->Enable();
}

void I4ConverterFrame::OnListDelete(wxListEvent& e)
{
	if (listCtrl->GetItemCount() == 1)
	{
		propGrid->Clear();
		btnConvert->Disable();
	}
}


void I4ConverterFrame::OnListKey(wxListEvent& e)
{
	if (e.GetKeyCode() == WXK_DELETE)
	{
		int item = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while ( item != -1 )
        {
			I4ExportInfo* info = (I4ExportInfo*)listCtrl->GetItemData(item);
			if (info)
			{
				delete info;
			}
            listCtrl->DeleteItem(item);

            // -1 because the indices were shifted by DeleteItem()
            item = listCtrl->GetNextItem(item - 1,
                                wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }
	}
}

void I4ConverterFrame::OnPropertyChanged(wxPropertyGridEvent& e)
{
	 wxPGProperty *property = e.GetProperty();
    if ( !property )
        return;

	int item = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    while ( item != -1 )
    {
		I4ExportInfo* info = (I4ExportInfo*)listCtrl->GetItemData(item);
		if (info)
		{
			wxVariant value = property->GetValue();
			const wxString& name = property->GetName();
			if (name == "Mesh")
			{
				info->isMesh = value.GetBool();
			}
			else if (name == "Material")
			{
				info->isMaterial = value.GetBool();
			}
			else if (name == "Bone")
			{
				info->isBone = value.GetBool();
			}
			else if (name == "Animation")
			{
				info->isAnimation = value.GetBool();
			}
		}

        item = listCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }
}

void I4ConverterFrame::OnBtnConvertClicked(wxCommandEvent& WXUNUSED(e))
{
	if (listCtrl->GetItemCount() <= 0)
	{
		wxMessageBox("No item");
		return;
	}

	wxDirDialog dlg(this);
	if (dlg.ShowModal() != wxID_OK)
		return;

	savePath = dlg.GetPath();


	vector<I4ExportInfo*> vecExportInfo;

	for (int i = 0; i < listCtrl->GetItemCount(); ++i)
	{
		I4ExportInfo* info = (I4ExportInfo*)listCtrl->GetItemData(i);
		if (info)
		{
			vecExportInfo.push_back(info);		
		}
	}

	I4ExportWorkerThread *thread = new I4ExportWorkerThread(this);
    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
        return;
    }

	thread->setExportInfo(vecExportInfo);

    m_dlgProgress = new wxProgressDialog
                        (
                         wxT("Progress dialog"),
                         wxT("Wait until the thread terminates or press [Cancel]"),
						 vecExportInfo.size()*6 + 1,
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

I4ExportWorkerThread::I4ExportWorkerThread(I4ConverterFrame *frame)
        : wxThread()
		, frame(frame)
		, totalStep(0)
		, curStep(0)
{
}

void I4ExportWorkerThread::OnExit()
{
}

wxThread::ExitCode I4ExportWorkerThread::Entry()
{
	totalStep = vecExportInfo.size()*6;
	curStep = 0;

	for (unsigned int i = 0; i < vecExportInfo.size(); ++i)
    {
        // check if we were asked to exit
        if ( TestDestroy() )
            break;

		I4ExportInfo* info = vecExportInfo[i];
		frame->BeginFbx(info->fileName);
		UpdateProgress();
		if (frame->Cancelled())
			break;

		if (info->isMesh)
		{
			frame->WriteFbxMeshes();
		}
		UpdateProgress();
		if (frame->Cancelled())
			break;

		if (info->isMaterial)
		{
			frame->WriteFbxMaterials();
		}
		UpdateProgress();
		if (frame->Cancelled())
			break;

		if (info->isBone)
		{
			frame->WriteFbxBones();
		}
		UpdateProgress();
		if (frame->Cancelled())
			break;

		if (info->isAnimation)
		{
			frame->WriteFbxAnimations();
		}
		UpdateProgress();
		if (frame->Cancelled())
			break;

		frame->EndFbx();
		UpdateProgress();		
		if (frame->Cancelled())
			break;
    }

	if (frame->Cancelled())
	{
		frame->EndFbx();
	}

    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT );
    event.SetInt(-1); // that's all
    wxQueueEvent( frame, event.Clone() );

    return NULL;
}

void I4ExportWorkerThread::UpdateProgress()
{
	++curStep;
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT );
	
    event.SetInt(curStep);
	
	// send in a thread-safe way
    wxQueueEvent( frame, event.Clone() );
}