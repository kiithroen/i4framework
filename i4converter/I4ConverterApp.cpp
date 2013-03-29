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
			listCtrl->InsertItem(listCtrl->GetItemCount(), files[n]);
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

	wxString savePath;
	wxDirDialog dlg(this);
	if (dlg.ShowModal() != wxID_OK)
		return;

	savePath = dlg.GetPath();

	wxListItem info;
    info.m_itemId = 0;
    info.m_col = 0;
    info.m_mask = wxLIST_MASK_TEXT;
    if (listCtrl->GetItem(info))
	{
		wxString originFile = info.m_text;

		wxString filePath;
		wxString fileName;
		wxString fileExt;
		wxFileName::SplitPath(info.m_text, &filePath, &fileName, &fileExt);
		
		wxFileName destFileName(savePath, fileName);
		fbxConverter->Convert(originFile.c_str(), destFileName.GetFullPath().c_str());
	}
}