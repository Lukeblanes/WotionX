#include "cFrame.h"
#include <algorithm>

wxBEGIN_EVENT_TABLE(cFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, cFrame::OnLearnMore_Help)
    EVT_MENU(wxID_EXIT, cFrame::OnExit_File)
// UNIMPLEMENTED MENU BUTTONS
    EVT_MENU(wxID_UNDO, cFrame::OnUNIMPLEMENTED)
    EVT_MENU(wxID_REDO, cFrame::OnUNIMPLEMENTED)
    EVT_MENU(wxID_CUT, cFrame::OnUNIMPLEMENTED)
    EVT_MENU(wxID_COPY, cFrame::OnUNIMPLEMENTED)
    EVT_MENU(wxID_PASTE, cFrame::OnUNIMPLEMENTED)
    EVT_MENU(wxID_SELECTALL, cFrame::OnUNIMPLEMENTED)
// Workspace
    EVT_MENU(ID_NewWorkspace, cFrame::OnNewWorkspace_File)
    EVT_BUTTON(ID_ButtonWorkspace, cFrame::OnSelectWorkspaceSettings)
    EVT_TEXT_ENTER(ID_TextCtrlModifyWorkspace, cFrame::OnModifyWorkspace)
    EVT_BUTTON(ID_ButtonModifyWorkspace, cFrame::OnModifyWorkspace)
    EVT_BUTTON(ID_ButtonDeleteWorkspace, cFrame::OnDeleteWorkspace)
// Page Creation Buttons
    EVT_BUTTON(ID_ButtonNewRootPage, cFrame::OnCreateNewPage)
    EVT_BUTTON(ID_ButtonNewChildPage, cFrame::OnCreateNewPage)
    EVT_BUTTON(ID_ButtonCreatePage, cFrame::OnCreateNewPageWithType)
// Page Elements
    EVT_RADIOBUTTON(ID_RadioButtonSelectedType1, cFrame::OnSelectTypeButton)
    EVT_RADIOBUTTON(ID_RadioButtonSelectedType2, cFrame::OnSelectTypeButton)
    EVT_RADIOBUTTON(ID_RadioButtonSelectedType3, cFrame::OnSelectTypeButton)
    EVT_CHAR_HOOK(cFrame::OnKeyDown)
// Page Settings
    EVT_BUTTON(ID_ButtonSettingsPage, cFrame::OnCreateSettingsPage)
    EVT_TEXT_ENTER(ID_TextCtrlModifyPageName , cFrame::OnModifyPage)
    EVT_BUTTON(ID_ButtonSettingsPageModify, cFrame::OnModifyPage)
    EVT_BUTTON(ID_ButtonSettingsPageDelete, cFrame::OnDeletePage)
wxEND_EVENT_TABLE()

// This is our main widget with no Parent
cFrame::cFrame()
    : wxFrame(NULL, wxID_ANY, "WotionX", wxPoint(480, 270), wxSize(960, 540))
{
    // ---- Setup Menu ----
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);

    // Might have to move this to its own function since this will be dynamic and menuWorkspace will have to be a member of the class
    wxMenu *menuWorkspace = CreateWorkspaceMenu();

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append(wxID_UNDO);
    menuEdit->Append(wxID_REDO);
    menuEdit->AppendSeparator();
    menuEdit->Append(wxID_CUT);
    menuEdit->Append(wxID_COPY);
    menuEdit->Append(wxID_PASTE);
    menuEdit->Append(wxID_SELECTALL);

    menuBar = new wxMenuBar;
    menuBar->Append(menuFile,      "&File");
    menuBar->Append(menuEdit,      "&Edit");
    menuBar->Append(menuWorkspace, "&Workspace");
    menuBar->Append(menuHelp,      "&Help");
    SetMenuBar( menuBar );

    CreateStatusBar();
    SetStatusText("WotionX!");

    // ---- WindowSplitter Setup ----
    m_splitterWindow = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize , wxSP_BORDER);
    m_splitterWindow->SetMinimumPaneSize(200);

    DB &dbInstance = DB::GetInstance();
    workspace_DB currentWorkspace = dbInstance.Get_SelectedWorkspace();

    // Create Workspace Panel (left)
    m_workspaceInfoPanel = new cPanel(m_splitterWindow);
    m_workspaceName = new wxStaticText(m_workspaceInfoPanel, wxID_ANY, currentWorkspace.name, wxDefaultPosition );
    m_workspaceName->SetFont(m_H3Font);
    m_workspaceSizer = new wxBoxSizer(wxVERTICAL);
    m_workspaceSettingsButton = new wxButton(m_workspaceInfoPanel, ID_ButtonWorkspace, "Settings");
    m_workspaceNewPageButton = new wxButton(m_workspaceInfoPanel, ID_ButtonNewRootPage, "New Page");
    m_workspaceSizer->Add(m_workspaceName, 0, wxCENTER, 0);
    // Set some sort of separator here
    m_workspaceSizer->Add(m_workspaceSettingsButton, 0, wxEXPAND, 0);
    m_workspaceSizer->Add(m_workspaceNewPageButton, 0, wxEXPAND, 0);

    m_pagesList = new wxTreeCtrl(m_workspaceInfoPanel, PAGES_ID_START - 1,
                                 wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS);
    m_workspaceSizer->Add(m_pagesList, 0, wxEXPAND, 0);

    RefreshWorkspacePanel();

    m_workspaceInfoPanel->SetSizer(m_workspaceSizer);
    // Add Tree like structure here

    // Create the Page panel (right)
    m_pagePanel = new cPanel(m_splitterWindow);
    m_pageName = new wxStaticText(m_pagePanel, wxID_ANY, "Welcome to WotionX", wxDefaultPosition, wxDefaultSize);
    m_pageName->SetFont(m_pageNameFont);
    m_pageSizer = new wxBoxSizer(wxVERTICAL);
    m_pageSizer->Add(m_pageName, 1, wxCENTER);
    m_pagePanel->SetSizer(m_pageSizer);

    // Split the window vertically and set the left and right panes
    m_splitterWindow->SplitVertically(m_workspaceInfoPanel, m_pagePanel, 1);

    // Set up the sizer for the frame and resize the frame according to its contents
    frameSizer = new wxBoxSizer(wxHORIZONTAL);
    frameSizer->Add(m_splitterWindow, 1, wxEXPAND);
    SetSizerAndFit(frameSizer);
}

void cFrame::OnExit_File(wxCommandEvent &event)
{
    Close(true);
    event.Skip();
}

void cFrame::OnLearnMore_Help(wxCommandEvent& event)
{
    wxMessageBox("Developed by Luke Blanes \ngithub.com/lukeblanes",
                 "About", wxOK | wxICON_INFORMATION);
    event.Skip();
}

void cFrame::OnNewWorkspace_File(wxCommandEvent& event)
{
    if ( DB::GetInstance().Create_Workspace() == DB_CODE::TOO_MANY_WORKSPACES) {
        wxLogMessage("Maximum number of workspaces reached!");
    } else {
        ResetGUI();
        RefreshWorkspacePanel();
        RefreshMenu();
    }
    event.Skip();
}

void cFrame::OnChangeWorkspace_Workspace(wxCommandEvent &event)
{
    auto newWorkspaceId = event.GetId() % WORKSPACE_MENU_ID_START;
    DB::GetInstance().Update_CurrentWorkspace(newWorkspaceId);
    ResetGUI();
    RefreshWorkspacePanel();
    event.Skip();
}

void cFrame::OnSelectWorkspaceSettings(wxCommandEvent &event)
{
    m_pageSizer->Clear(true);

    // Create sizer and textCtrl
    auto currentWorkspace = DB::GetInstance().Get_SelectedWorkspace();

    // Adding Settings Elements
    auto *settingsTitle = new wxStaticText(m_pagePanel, wxID_ANY, "Settings");
    settingsTitle->SetFont(m_pageNameFont);
    m_pageSizer->Add(settingsTitle, 0, wxALIGN_CENTER | wxALL, 10);

    m_modifyWorkspaceTextCtrl = new wxTextCtrl(m_pagePanel, ID_TextCtrlModifyWorkspace, currentWorkspace.name, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_modifyWorkspaceTextCtrl->SetFont(m_H3Font);

    auto *modifyButton =new wxButton(m_pagePanel, ID_ButtonModifyWorkspace, "Modify Workspace");
    modifyButton->SetFont(m_H3Font);

    auto *deleteButton =new wxButton(m_pagePanel, ID_ButtonDeleteWorkspace, "Delete Workspace");
    deleteButton->SetFont(m_H3Font);

    wxBoxSizer *settingsSizer = new wxBoxSizer(wxHORIZONTAL);
    settingsSizer->Add(m_modifyWorkspaceTextCtrl, 3);
    settingsSizer->Add(modifyButton, 1);
    settingsSizer->Add(deleteButton, 1);

    m_pageSizer->Add(settingsSizer, 0, wxEXPAND | wxALL, 10);
    m_pageSizer->SetSizeHints(this);

    event.Skip();
}

void cFrame::OnModifyWorkspace(wxCommandEvent &event)
{
    // Change Current
    auto newWorkspaceName = m_modifyWorkspaceTextCtrl->GetValue();
    DB::GetInstance().Update_CurrentWorkspaceName(newWorkspaceName.ToStdString());
    RefreshMenu();
    RefreshWorkspacePanel();
    event.Skip();
}

void cFrame::OnDeleteWorkspace(wxCommandEvent &event)
{
    int answer = wxMessageBox("Are you sure you want to delete current workspace?\n"
                              "Deleting your workspace will delete all pages in the workspace",
                              "Confirm",
                               wxYES_NO, this);
    if(answer == wxYES) {
        if(DB::GetInstance().Delete_CurrentWorkspace() == DB_CODE::DELETING_LAST_WORKSPACE) {
            wxLogMessage("Can't delete last Workspace!");
        } else {
            ResetGUI();
            RefreshMenu();
            RefreshWorkspacePanel();

            wxLogMessage("Deleted Correctly!");
        }
    }

    event.Skip();
}

void cFrame::OnCreateNewPage(wxCommandEvent &event)
{
    // Determine if the button pressed is for child node or Parent node
    m_isParentPage = (event.GetId() == ID_ButtonNewRootPage) ? true : false;

    // Creates the page to select the type
    m_pageSizer->Clear(true);

    m_pageName = new wxStaticText(m_pagePanel, wxID_ANY, "Select page type:");
    m_pageName->SetFont(m_pageNameFont);
    m_pageSizer->Add(m_pageName);

    auto *radio1 = new wxRadioButton(m_pagePanel, ID_RadioButtonSelectedType1, "Markdown");
    radio1->SetValue(true);
    radio1->SetFont(m_H3Font);
    m_pageSizer->Add(radio1);

    auto *radio2 = new wxRadioButton(m_pagePanel, ID_RadioButtonSelectedType2, "Table");
    radio2->SetFont(m_H3Font);
    m_pageSizer->Add(radio2);

    auto *auxBut = new wxButton(m_pagePanel, ID_ButtonCreatePage, "Create");
    auxBut->SetFont(m_H3Font);
    m_pageSizer->Add(auxBut);

    m_pageSizer->SetSizeHints(this);

    event.Skip();
}

void cFrame::OnCreateNewPageWithType(wxCommandEvent &event)
{
    auto currentWorkspace = DB::GetInstance().Get_SelectedWorkspace();

    // if it's a child page, then we need to indicate the current pages father id.
    int parent_id = m_isParentPage ? -1 : m_currentSelectedPage;

    int position = DB::GetInstance().Get_HighestPageId(parent_id) + 1;
    auto newId = DB::GetInstance().Create_WorkspacePage(parent_id, currentWorkspace.id, position, m_typeSelector);

    if (m_typeSelector == 1) // Markdown
        DB::GetInstance().Create_MarkdownPage(newId);
    else if(m_typeSelector == 2) // Table
        DB::GetInstance().Create_TablePage(newId);

    // Create corresponding page element of current Workspace if it isn't a root page
    if (parent_id != -1)
    {
        int highestPos = 0;
        // there might be a better way of getting the highest position of elements
        for (auto& elem: m_pageElementsAux)
        {
            if(highestPos < elem.position)
            {
                highestPos = elem.position;
            }
        }

        // get max position of m_page
        // Get last element page
        int newChildPosition = highestPos+1; // new child node will be the highest current occupied position + 1
        auto childNodeLink = "[#"+ std::to_string(newId) +"]";

        // Todo: by default, we shouldn't have to introduce '^ ' in InsertWorkspacePageElement
        DB::GetInstance().Insert_MarkdownElement(m_currentSelectedPage, childNodeLink, newChildPosition);
        DB::GetInstance().Update_MarkdownElement(m_currentSelectedPage, childNodeLink, newChildPosition);
    }

    // After using m_currentSelectedPage for child node operations,
    // we change the currently selected page to the created page so
    // we switch to it on refresh
    m_currentSelectedPage = newId;

    // Reset to default selected type after creating new page
    m_typeSelector = 1;

    RefreshWorkspacePanel();
    RefreshPagePanel();

    wxLogMessage("Created new page");
    event.Skip();
}

void cFrame::OnSelectTreeElement(wxTreeEvent &event)
{
    auto selectedTreeElemId = event.GetItem();
    bool foundCurrentElem = false;
    int currentElemPosition = 0;
    while(currentElemPosition < m_pagesAux.size() && !foundCurrentElem) {
        if(m_pagesIdAux[currentElemPosition] == selectedTreeElemId)
            foundCurrentElem = true;

        currentElemPosition++;
    }

    if(!foundCurrentElem) {
        std::cout << "Error: Selected Tree Element was not found in loaded pages!\n";
    } else {
        currentElemPosition--; // Normalize from last decrement
        m_currentSelectedPage = m_pagesAux[currentElemPosition].id;
    }
    RefreshPagePanel();
    m_CollapseTree = false;
}

void cFrame::OnCollapseTreeElement(wxTreeEvent &event)
{
    if(m_CollapseTree == false)
        event.Veto();
    m_CollapseTree = true;
    event.Skip();
}

void cFrame::OnCreateNewPageElement(wxCommandEvent &event)
{
    int currentElementPosition = ((event.GetId() % PAGE_ELEMENTS_ID_START) / NUMBER_PAGE_ELEMENTS) + 1;
    DB::GetInstance().Insert_MarkdownElement(m_currentSelectedPage, "New_Cell", currentElementPosition +1);
    RefreshPagePanel();
}

void cFrame::OnCreateSettingsPage(wxCommandEvent &event)
{
    m_pageSizer->Clear(true);

    auto currentPageName = GetCurrentPageName(m_currentSelectedPage);

    auto *settingsTitle = new wxStaticText(m_pagePanel, wxID_ANY, "Page Settings");
    settingsTitle->SetFont(m_pageNameFont);
    m_pageSizer->Add(settingsTitle, 0, wxALIGN_CENTER | wxALL, 10);

    m_modifyWorkspaceTextCtrl = new wxTextCtrl(m_pagePanel, ID_TextCtrlModifyPageName, currentPageName, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    m_modifyWorkspaceTextCtrl->SetFont(m_H3Font);

    auto *modifyButton =new wxButton(m_pagePanel, ID_ButtonSettingsPageModify, "Modify Page Name");
    modifyButton->SetFont(m_H3Font);

    auto *deleteButton =new wxButton(m_pagePanel, ID_ButtonSettingsPageDelete, "Delete Page");
    deleteButton->SetFont(m_H3Font);

    wxBoxSizer *settingsSizer = new wxBoxSizer(wxHORIZONTAL);
    settingsSizer->Add(m_modifyWorkspaceTextCtrl, 3);
    settingsSizer->Add(modifyButton, 1);
    settingsSizer->Add(deleteButton, 1);

    m_pageSizer->Add(settingsSizer, 0, wxEXPAND | wxALL, 10);
    m_pageSizer->SetSizeHints(this);

    event.Skip();
}

void cFrame::OnModifyPageElement(wxCommandEvent &event)
{
    int currentElementPosition = ((event.GetId() % PAGE_ELEMENTS_ID_START) / NUMBER_PAGE_ELEMENTS) + 1;
    std::string cleanInput = std::string(event.GetString().mb_str());
    CleanDBInput(cleanInput); // Returns cleanedInput
    DB::GetInstance().Update_MarkdownElement(m_currentSelectedPage, cleanInput, currentElementPosition);
    OnCreateNewPageElement(event);
}

void cFrame::OnEditPageElement(wxCommandEvent &event)
{
    int currentElementPosition = ((event.GetId() % PAGE_ELEMENTS_ID_START) / NUMBER_PAGE_ELEMENTS) + 1;

    // Note: We can do this because m_pageElementsAux is always ordered by position not id
    if ( m_pageElementsAux[currentElementPosition-1].content[0] == '^' &&
         m_pageElementsAux[currentElementPosition-1].content[1] == ' ') {
        wxLogMessage("Already Editable");
    } else {
        DB::GetInstance().Make_MarkdownElement_Editable(m_currentSelectedPage, currentElementPosition);
        RefreshPagePanel();
    }
}

void cFrame::OnEditPageElementNonEditable(wxCommandEvent &event)
{
    wxLogMessage("This element can't be edited");
}

// We can try and detect when the user switched to a different textBox using this
void cFrame::OnModifyTextCtrl(wxCommandEvent &event)
{
    int currentElementPosition = ((event.GetId() % PAGE_ELEMENTS_ID_START) / NUMBER_PAGE_ELEMENTS) + 1;
    std::string aux = std::string(event.GetString().mb_str());
    auto&[position, textBoxContent] = textBoxAux;
    position = currentElementPosition;
    textBoxContent = aux;
    event.Skip();
}

void cFrame::OnKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode() )
    {
        case WXK_BACK: {
            auto[position, textBoxContent] = textBoxAux;
            if(textBoxContent == "")
            {
                if( m_pageElementsAux.size() == 1) {
                    wxLogMessage("You can't delete the last page element.");
                } else{
                    DB::GetInstance().Delete_MarkdownElement(m_currentSelectedPage, position);
                    RefreshPagePanel(); // After deleting we update
                }
            }
            else {
                event.Skip();
            }
            break;
        }
        default: {
            event.Skip();
            break;
        }
    }
}

void cFrame::OnToggleTodo(wxCommandEvent &event)
{
    int currentElementPosition = ((event.GetId() % PAGE_ELEMENTS_ID_START) / NUMBER_PAGE_ELEMENTS) + 1;

    auto content = m_pageElementsAux[currentElementPosition-1].content;
    if (content.find("- [ ]") != std::string::npos)
        content = "- [x]" + content.substr(5);
    else
        content = "- [ ]" + content.substr(5);
    auto s = "Switching selected in position " + std::to_string(currentElementPosition) + "\nContent: "+ content;
    DB::GetInstance().Update_MarkdownElement(m_currentSelectedPage, content, currentElementPosition);

    // In this case we don't need to refresh but we do need to update the cache of m_pageElements
    DB::GetInstance().Get_MarkdownElements(m_pageElementsAux, m_currentSelectedPage);
}

void cFrame::OnClickLink(wxHyperlinkEvent &event)
{
    auto url = event.GetURL();

    if (url.Contains(wxString("http"))) {
        wxLaunchDefaultBrowser(url);
    } else {
        m_currentSelectedPage = std::stoi(url.ToStdString());
        RefreshPagePanel();
        RefreshWorkspacePanel();
    }
}

void cFrame::OnSelectTypeButton(wxCommandEvent &event)
{
    auto eventId = event.GetId();

    if (eventId == ID_RadioButtonSelectedType1)
        m_typeSelector = 1;
    else if (eventId == ID_RadioButtonSelectedType2)
        m_typeSelector = 2;
    else if (eventId == ID_RadioButtonSelectedType3)
        m_typeSelector = 3;

    event.Skip();
}

void cFrame::OnAddRowTable(wxCommandEvent &event)
{
    std::string columns = "(";
    std::string content = "(";
    for(auto& col : m_pagesElementsColumns)
    {
        if (col != "ID")
        {
            columns += col + ",";
            content += "' ',";
        }
    }
    columns = (columns.substr(0, columns.size()-1)) + ")"; // remove last , and add )
    content = (content.substr(0, content.size()-1)) + ")"; // remove last , and add )

    DB::GetInstance().Insert_TableElement(m_currentSelectedPage, content, columns);
    RefreshPagePanel();
}

void cFrame::OnEditRowTable(wxCommandEvent &event)
{
    auto selected = m_listView->GetFirstSelected();

    if (selected == -1)
        wxLogMessage("Select an item on the table to edit.");
    else
        Create_TableEditPage();
}

void cFrame::OnDeleteRowTable(wxCommandEvent &event)
{
    auto selected = m_listView->GetFirstSelected();

    if (selected == -1) {
        wxLogMessage("Select an item on the table to delete");
    } else {
        if (m_tableElements.size() > 1) {
            auto row = m_tableElements[selected];
            DB::GetInstance().Delete_TableElement(m_currentSelectedPage, row[0]);
            wxLogMessage("Row succesfully deleted!");
            RefreshPagePanel();
        } else {
            wxLogMessage("Can't delete last element from table!");
        }
    }
}

void cFrame::OnReturnFromEditTable(wxCommandEvent &event)
{
    RefreshPagePanel();
}

void cFrame::OnModifyRowTable(wxCommandEvent &event)
{
    auto selected = event.GetId() % ID_ButtonModifyRowTable;
    std::string modifyId =m_tableElements[selected][0];

    wxTextCtrl *testing = nullptr;

    std::vector<std::string> columns;
    std::vector<std::string> content;

    int columnCounter = 0;
    for(auto& col : m_pagesElementsColumns)
    {
        if (col != "ID")
        {
            // in the following line, we get the wxWindow pointer through the id and cast the pointer to wxTextCtrl to get the content in the textCtrl box
            testing = static_cast<wxTextCtrl*>(m_pagePanel->FindWindow(TABLE_ELEMENTS_ID_START + columnCounter++));
            columns.push_back(col);
            content.push_back("'" + testing->GetLineText(0).ToStdString() + "'");
        }
    }

    std::string test = "Selected is " + std::to_string(selected) + "\n";
    DB::GetInstance().Update_TableElement(m_currentSelectedPage, modifyId, content, columns);

    RefreshPagePanel();
}

void cFrame::OnModifyPage(wxCommandEvent &event)
{
    // Change Current
    auto newWorkspaceName = m_modifyWorkspaceTextCtrl->GetValue();
    DB::GetInstance().Update_PageName(m_currentSelectedPage, newWorkspaceName.ToStdString());
    RefreshMenu();
    RefreshWorkspacePanel();
    event.Skip();
}

void cFrame::OnDeletePage(wxCommandEvent &event)
{
    int answer = wxMessageBox("Are you sure you want to delete current workspace?\n"
                              "If current page has children, it will also delete them.\n",
                              "Confirm",
                               wxYES_NO, this);
    if(answer == wxYES) {
        DB::GetInstance().Delete_PageAndChildren(m_currentSelectedPage);
        // DB call to delete current page and child pages and all corresponding page Elems

        // After DB Modification, we perform a refresh
        RefreshMenu();
        RefreshWorkspacePanel();

        wxLogMessage("Deleted Correctly!");
    }

    m_pageSizer->Clear(true);
    ResetGUI();
}

wxMenu* cFrame::CreateWorkspaceMenu()
{
    wxMenu *menuWorkspace= new wxMenu;
    DB &dbInstance = DB::GetInstance();
    dbInstance.Get_Workspaces(m_workspacesAux);
    int i = 0;
    for (auto& workspace : m_workspacesAux) {
        menuWorkspace->Append(WORKSPACE_MENU_ID_START + workspace.id, "&"+ workspace.name +"\t\tCtrl-" + std::to_string(i++), "Select Workspace");

        // BIND FUNCTIONS
        Bind(wxEVT_MENU, &cFrame::OnChangeWorkspace_Workspace, this, WORKSPACE_MENU_ID_START + workspace.id);
    }
    menuWorkspace->AppendSeparator();
    menuWorkspace->Append(ID_NewWorkspace, "&New Workspace\tCtrl-N", "Creates a new Workspace");

    return menuWorkspace;
}

void cFrame::RefreshMenu()
{
    auto menuWorkspace = CreateWorkspaceMenu();

    // this returns previous menu so we delete it after creating the new one
    auto oldMenu = menuBar->Replace(WORKSPACE_MENU_POS, menuWorkspace, "&Workspace");
    delete oldMenu;
}

void cFrame::RefreshWorkspacePanel()
{
    workspace_DB currentWorkspace = DB::GetInstance().Get_SelectedWorkspace();
    m_workspaceName->SetLabel(currentWorkspace.name);

    DB::GetInstance().Get_WorkspacePages(m_pagesAux);
    m_pagesList->DeleteAllItems();

    if(m_pagesAux.size() > 0)
    {
        wxTreeItemId rootId = m_pagesList->AddRoot("Hidden_Root");
        m_pagesIdAux.clear();
        for(int i = 0; i < m_pagesAux.size(); ++i)
        {
            if ( m_pagesAux[i].parent_id == -1) {
                m_pagesIdAux.push_back(m_pagesList->AppendItem(rootId, m_pagesAux[i].name));
            } else {
                int parentTreeId = -1;
                for (int j = 0; j < m_pagesIdAux.size() && parentTreeId == -1; ++j) {
                    if (m_pagesAux[j].id == m_pagesAux[i].parent_id)
                        parentTreeId = j;
                }
                m_pagesIdAux.push_back(m_pagesList->AppendItem(m_pagesIdAux[parentTreeId], m_pagesAux[i].name));
            }
            Bind(wxEVT_TREE_ITEM_ACTIVATED, &cFrame::OnSelectTreeElement, this);
            Bind(wxEVT_TREE_SEL_CHANGED, &cFrame::OnSelectTreeElement, this);
            Bind(wxEVT_TREE_ITEM_COLLAPSING, &cFrame::OnCollapseTreeElement, this);
        }
        m_pagesList->Show();
        m_pagesList->ExpandAll();
    } else {
        m_pagesList->Hide();
    }
    m_workspaceSizer->SetSizeHints(this);
}

// This is not general, only when a page on TreeCtrl is selected
void cFrame::RefreshPagePanel()
{
    // In common behaviour for each type of page
    m_pageSizer->Clear(true);

    auto currentPageName = GetCurrentPageName(m_currentSelectedPage);

    m_pageName = new wxStaticText(m_pagePanel, wxID_ANY, currentPageName, wxDefaultPosition, wxDefaultSize);
    m_pageName->SetFont(m_pageNameFont);
    m_pageSizer->Add(m_pageName, 0, wxALIGN_CENTER);
    m_pageSizer->Add(new wxButton(m_pagePanel, ID_ButtonSettingsPage, "Page Settings"), 0, wxALIGN_CENTER);

    // Figure out way to not have to cycle through all the pages
    int type = 0;
    for ( auto &page: m_pagesAux)
    {
        if (page.id == m_currentSelectedPage)
        {
            type = page.type;
            break;
        }
    }

    // Specific behaviour for each type
    if (type == PAGE_TYPE::MARKDOWN)
        MarkdownPageGeneration();
    else if (type == PAGE_TYPE::TABLE)
        TablePageGeneration();
    else
        std::cout << "ERROR: current page type is not valid\n";

    m_pageSizer->SetSizeHints(this);
}

void cFrame::Create_TableEditPage()
{
    // I encode selected into the ID of ID_ButtonModifyRowTable
    // There's a cleaner way to do this
    auto selected = m_listView->GetFirstSelected();
    m_pageSizer->Clear(true);

    auto currentPageName = GetCurrentPageName(m_currentSelectedPage);

    m_pageName = new wxStaticText(m_pagePanel, wxID_ANY, "Modify " + currentPageName + " table", wxDefaultPosition, wxDefaultSize);
    m_pageName->SetFont(m_pageNameFont);
    m_pageSizer->Add(m_pageName);

    DB::GetInstance().Get_DBMetaInfo(m_pagesElementsColumns, m_currentSelectedPage);
    DB::GetInstance().Get_TableElements(m_tableElements, m_currentSelectedPage);

    wxBoxSizer *horizontalElement = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *column = nullptr;

    int columnCounter = 0;
    uint32_t rowCycler =  1;

    for (auto& elem : m_pagesElementsColumns)
    {
        if ( elem != "ID")
        {
            column = new wxBoxSizer(wxVERTICAL);
            column->Add(new wxStaticText(m_pagePanel, wxID_ANY, elem,  wxDefaultPosition, wxDefaultSize));
            // we use the info
            auto previousColumnValue = m_tableElements[selected][rowCycler++];
            column->Add(new wxTextCtrl(m_pagePanel, TABLE_ELEMENTS_ID_START + columnCounter, previousColumnValue));

            horizontalElement->Add(column, 1);
            columnCounter++;
        }
    }
    m_pageSizer->Add(horizontalElement, 0, wxEXPAND | wxALL, 5);

    horizontalElement = new wxBoxSizer(wxHORIZONTAL);
    horizontalElement->Add(new wxButton(m_pagePanel, ID_ButtonReturnFromModifyTable, "Return"), 0, wxALL, 5);
    Bind(wxEVT_BUTTON, &cFrame::OnReturnFromEditTable, this, ID_ButtonReturnFromModifyTable );
    horizontalElement->Add(new wxButton(m_pagePanel, ID_ButtonModifyRowTable + selected, "Modify Row"), 0, wxALL, 5 );
    Bind(wxEVT_BUTTON, &cFrame::OnModifyRowTable, this ,ID_ButtonModifyRowTable + selected);

    m_pageSizer->Add(horizontalElement);
    m_pageSizer->SetSizeHints(this);
}

void cFrame::MarkdownPageGeneration()
{
    DB::GetInstance().Get_MarkdownElements(m_pageElementsAux, m_currentSelectedPage);
    // Position of this should probably change
    m_pageSizer->Add(new wxButton(m_pagePanel, ID_ButtonNewChildPage, "New child page"));

    // Dynamically create current Page
    wxBoxSizer *horizontalAux = nullptr;

    int elemCounter = 0;

    for (auto& elem : m_pageElementsAux) {

        auto element = parseElement(elem.content); // parse element, should probably return the type and the string
        auto createdHorElem = ProcessElement(element, elemCounter);

        horizontalAux= new wxBoxSizer(wxHORIZONTAL);
        horizontalAux->Add(new wxButton(m_pagePanel, PAGE_ELEMENTS_ID_START + elemCounter * NUMBER_PAGE_ELEMENTS + 0, "+", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALIGN_LEFT | wxLEFT | wxTOP | wxBOTTOM , 5);
        Bind(wxEVT_BUTTON, &cFrame::OnCreateNewPageElement, this , PAGE_ELEMENTS_ID_START + elemCounter * NUMBER_PAGE_ELEMENTS + 0);

        horizontalAux->Add(new wxButton(m_pagePanel, PAGE_ELEMENTS_ID_START + elemCounter * NUMBER_PAGE_ELEMENTS + 1, "^", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALIGN_LEFT | wxRIGHT | wxTOP | wxBOTTOM, 5 );
        if(element.type == elemType::CHILD_NODE)
            Bind(wxEVT_BUTTON, &cFrame::OnEditPageElementNonEditable, this , PAGE_ELEMENTS_ID_START + elemCounter * NUMBER_PAGE_ELEMENTS + 1);
        else
            Bind(wxEVT_BUTTON, &cFrame::OnEditPageElement, this , PAGE_ELEMENTS_ID_START + elemCounter * NUMBER_PAGE_ELEMENTS + 1);

        horizontalAux->Add(createdHorElem, 1, wxALIGN_CENTER | wxLEFT, 20);

        m_pageSizer->Add(horizontalAux, 0, wxEXPAND | wxRIGHT, 10);
        elemCounter++;
    }
}

void cFrame::TablePageGeneration()
{
    DB::GetInstance().Get_DBMetaInfo(m_pagesElementsColumns, m_currentSelectedPage);
    DB::GetInstance().Get_TableElements(m_tableElements, m_currentSelectedPage);

    m_listView = new wxListView(m_pagePanel, ID_TableListView ,wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL);

    wxBoxSizer *horizontalElement = new wxBoxSizer(wxHORIZONTAL);
    horizontalElement->Add(new wxButton(m_pagePanel, ID_ButtonAddRowTable, "Add Row") );
    Bind(wxEVT_BUTTON, &cFrame::OnAddRowTable, this , ID_ButtonAddRowTable);

    horizontalElement->Add(new wxButton(m_pagePanel, ID_ButtonEditRowTable, "Edit Row") );
    Bind(wxEVT_BUTTON, &cFrame::OnEditRowTable, this , ID_ButtonEditRowTable);

    horizontalElement->Add(new wxButton(m_pagePanel, ID_ButtonDeleteRowTable, "Delete Row") );
    Bind(wxEVT_BUTTON, &cFrame::OnDeleteRowTable, this , ID_ButtonDeleteRowTable);

    m_pageSizer->Add(horizontalElement);

    // Remove hidden columns ( id, timestamp, etc )
    for(auto& elem: m_pagesElementsColumns)
    {
        if (elem != "ID")
            m_listView->AppendColumn(elem);
    }
    auto columns = m_pagesElementsColumns.size();

    // Might have to change this to make it safer
    std::vector<uint16_t> ignore;
    ignore.push_back(0); // In this array we can add the position from db info to ignore when displaying

    auto counter = 0;
    auto columnIndex = 0;

    for(auto& row: m_tableElements)
    {
        for(int i = 0; i < columns; i++)
        {
            if( find(ignore.begin(), ignore.end(), i % columns) == ignore.end() ) // If not in ignore list we enter
            {
                if (columnIndex == 0)
                    m_listView->InsertItem(counter, row[i]);
                else
                    m_listView->SetItem(counter, ++columnIndex, row[i]);

                columnIndex++;
            }
        }
        columnIndex = 0;
        counter++;
    }

    for(int i = 0; i < m_listView->GetColumnCount(); ++i)
    {
        // Hacky solution to get correct width for columns
        int headerLength = m_listView->GetColumnWidth(i); // initial length is the length of the header
        m_listView->SetColumnWidth(i, wxLIST_AUTOSIZE );
        int longestLength = m_listView->GetColumnWidth(i);
        if(headerLength > longestLength)
            m_listView->SetColumnWidth(i, headerLength);
    }

    m_pageSizer->Add(m_listView, 1, wxEXPAND | wxALL , 5);
}

wxControl* cFrame::ProcessElement(pageElement &element, int id)
{
    auto type = element.type;
    wxControl* aux = nullptr;
    if(type == elemType::RAW_TEXT) {
        aux = new wxTextCtrl(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, element.cleanContent, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
        Bind(wxEVT_TEXT_ENTER, &cFrame::OnModifyPageElement, this , PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2);
        Bind(wxEVT_TEXT, &cFrame::OnModifyTextCtrl, this , PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2);
    } else if (type == elemType::HEADER1 ){
        aux = new wxStaticText(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, element.cleanContent);
        aux->SetFont(m_H1Font);
    } else if (type == elemType::HEADER2 ){
        aux = new wxStaticText(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, element.cleanContent);
        aux->SetFont(m_H2Font);
    } else if (type == elemType::HEADER3 ){
        aux = new wxStaticText(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, element.cleanContent);
        aux->SetFont(m_H3Font);
    } else if (type == elemType::TASK_LIST_ELEMENT_MARKED){
        auto *UncheckedBox = new wxCheckBox(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, element.cleanContent);
        UncheckedBox->SetValue(1);
        Bind(wxEVT_CHECKBOX, &cFrame::OnToggleTodo, this, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2);
        aux = UncheckedBox;
    } else if (type == elemType::TASK_LIST_ELEMENT_NONMARKED){
        auto *UncheckedBox = new wxCheckBox(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, element.cleanContent);
        Bind(wxEVT_CHECKBOX, &cFrame::OnToggleTodo, this, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2);
        aux = UncheckedBox;
    } else if (type == elemType::LINK_MD){
        wxString label = wxString(GetSlice('[', ']', element.cleanContent));
        wxString url = wxString(GetSlice('(', ')', element.cleanContent));
        aux = new wxHyperlinkCtrl(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, label, url, wxDefaultPosition, wxDefaultSize, wxHL_ALIGN_LEFT);
        Bind(wxEVT_HYPERLINK, &cFrame::OnClickLink, this, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2);
    } else if (type == elemType::CHILD_NODE){
        wxString label = wxString("Name_not_found");
        wxString url = wxString("Name_not_found");
        DB::GetInstance().Get_WorkspacePages(m_pagesAux);
        for (auto& elem: m_pagesAux)
        {
            if(std::to_string(elem.id) == element.cleanContent)
            {
                label = wxString(elem.name);
                url = wxString(element.cleanContent);
                break;
            }
        }
        aux = new wxHyperlinkCtrl(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, label, url, wxDefaultPosition, wxDefaultSize, wxHL_ALIGN_LEFT);
        Bind(wxEVT_HYPERLINK, &cFrame::OnClickLink, this, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2);
    } else {
        aux = new wxStaticText(m_pagePanel, PAGE_ELEMENTS_ID_START + id * NUMBER_PAGE_ELEMENTS + 2, element.cleanContent);
        aux->SetFont(m_defaultFont);
    }

    return aux;
}

std::string cFrame::GetSlice(const char front, const char back, const std::string& original)
{
    auto firstCharPos = original.find_first_of(front);
    auto lastCharPos = original.find_last_of(back);
    auto len = lastCharPos - firstCharPos - 1;
    return original.substr(firstCharPos + 1, len);
}

void cFrame::ResetGUI()
{
    m_pageSizer->Clear(true);
    workspace_DB current_workspace = DB::GetInstance().Get_SelectedWorkspace();

    m_pageName = new wxStaticText(m_pagePanel, wxID_ANY, current_workspace.name);
    m_pageName->SetFont(m_pageNameFont);
    m_pageSizer->Add(m_pageName, 1, wxCENTER);

    m_pageName->SetFont(m_pageNameFont);
}

std::string cFrame::GetCurrentPageName(int pageId)
{
    DB::GetInstance().Get_WorkspacePages(m_pagesAux);
    std::string ret = "ERROR";
    for(auto& elem: m_pagesAux)
    {
        if(elem.id == pageId)
        {
            ret = elem.name;
            break;
        }
    }
    return ret;
}

void cFrame::CleanDBInput(std::string& input)
{
    // Check if input is hyperlink
    if(input.substr(0, 7) == "http://" || input.substr(0, 8) == "https://")
        input = "[" + input + "](" + input + ")";

    // Remove leading and trailing whitespace
    const auto strBegin = input.find_first_not_of(" \t");
    input = input.substr(strBegin, std::string::npos);

    // stuff we should remove before introducing to DB
    auto issues = "\"";

    auto issueFinder = input.find_first_of(issues);
    if ( issueFinder != std::string::npos)
        wxLogMessage("Found issue with string. Replaces problematic characters with a space.");

    while (issueFinder != std::string::npos)
    {
        input.replace(issueFinder, 1, " ");
        issueFinder = input.find_first_of(issues, issueFinder + 1);
    }
}
