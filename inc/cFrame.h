#pragma once

#include <wx/wx.h>

// range 10001 - 10010 Maximum of 10 workspaces
#define WORKSPACE_MENU_ID_START 10000
#define WORKSPACE_MENU_POS 2

#define NUMBER_PAGE_ELEMENTS 3

#define PAGES_ID_START 11000
#define PAGE_ELEMENTS_ID_START 20000
#define TABLE_ELEMENTS_ID_START 30000

#include "cPanel.h"
#include "db.h"
#include "elemParser.h"
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/hyperlink.h>
#include <wx/listctrl.h>

#include <vector>
#include <tuple>

class cFrame : public wxFrame
{
public:
    cFrame();

private:
    // Callback Functions
    // 	Command Event
    void OnUNIMPLEMENTED(wxCommandEvent& event) { wxLogMessage("NOT IMPLEMENTED!"); event.Skip(); };
    void OnExit_File(wxCommandEvent& event);
    void OnLearnMore_Help(wxCommandEvent& event);
    void OnNewWorkspace_File(wxCommandEvent& event);
    void OnChangeWorkspace_Workspace(wxCommandEvent &event);
    void OnSelectWorkspaceSettings(wxCommandEvent &event);
    void OnModifyWorkspace(wxCommandEvent &event);
    void OnDeleteWorkspace(wxCommandEvent &event);
    void OnCreateNewPage(wxCommandEvent &event);
    void OnCreateNewPageWithType(wxCommandEvent &event);
    void OnCreateNewPageElement(wxCommandEvent &event);
    void OnCreateSettingsPage(wxCommandEvent &event);
    void OnModifyPageElement(wxCommandEvent &event);
    void OnEditPageElement(wxCommandEvent &event);
    void OnEditPageElementNonEditable(wxCommandEvent &event);
    void OnModifyTextCtrl(wxCommandEvent &event);
    void OnToggleTodo(wxCommandEvent &event);
    void OnSelectTypeButton(wxCommandEvent& event);
    void OnSelectTableElement(wxCommandEvent& event);
    void OnAddRowTable(wxCommandEvent& event);
    void OnEditRowTable(wxCommandEvent& event);
    void OnDeleteRowTable(wxCommandEvent& event);
    void OnCreateViewTable(wxCommandEvent& event);
    void OnReturnFromEditTable(wxCommandEvent& event);
    void OnModifyRowTable(wxCommandEvent& event);
    void OnModifyPage(wxCommandEvent &event);
    void OnDeletePage(wxCommandEvent &event);


    // Tree Event
    bool m_CollapseTree = true;
    void OnSelectTreeElement(wxTreeEvent &event);
    void OnCollapseTreeElement(wxTreeEvent &event);

    // Key Event
    void OnKeyDown(wxKeyEvent &event);

    // Hyperlink Event
    void OnClickLink(wxHyperlinkEvent& event);
    wxDECLARE_EVENT_TABLE();

    // Menu
    wxMenuBar *menuBar;
    wxMenu* CreateWorkspaceMenu();
    void RefreshMenu();
    void RefreshWorkspacePanel();
    void RefreshPagePanel();
    void Create_TableEditPage();
    void MarkdownPageGeneration();
    void TablePageGeneration();
    wxControl* ProcessElement(pageElement &element, int id);
    void CleanDBInput(std::string& input);
    std::string GetSlice(const char front, const char back, const std::string& original);
    std::string& DeCleanDBInput(const std::string& input);
    void ResetGUI();
    std::string GetCurrentPageName(int pageId);

    // Splitter
    wxBoxSizer *frameSizer = nullptr;
    wxSplitterWindow *m_splitterWindow = nullptr;

    // Panels
    // Workspace Panel
    std::vector<workspace_DB> m_workspacesAux;
    // Pages Tree
    wxTreeCtrl* m_pagesList = nullptr;

    // optimizations
    // used so we don't have to create and return from the DB function
    std::vector<pages_DB> m_pagesAux;

    // To store the current pageElements of the current Page
    std::vector<markdown_elements_DB> m_pageElementsAux;

    // for getting colum names
    std::vector<std::string> m_pagesElementsColumns;

    std::vector<std::vector<std::string>> m_tableElements;


    // used to store the different wxTreeItemIds of the different pages
    // we store it concurrently with m_pagesAux so that later we can display the
    // corresponding info of each page
    std::vector<wxTreeItemId> m_pagesIdAux;

    cPanel* m_workspaceInfoPanel = nullptr;
    wxBoxSizer *m_workspaceSizer = nullptr;
    wxStaticText *m_workspaceName = nullptr;
    wxButton *m_workspaceSettingsButton = nullptr;
    wxButton *m_workspaceNewPageButton= nullptr;

    // Root pages will have a value of -1
    // child pages will have their corresponding parent
    int m_currentSelectedPage = -1;

    wxListView* m_listView = nullptr;

    // 1 for markdown page
    // 2 for table
    int m_typeSelector = 1;

    bool m_isParentPage;

    // Page Panel
    cPanel* m_pagePanel = nullptr;
    wxBoxSizer *m_pageSizer = nullptr;
    wxStaticText *m_pageName = nullptr;
    wxTextCtrl *m_modifyWorkspaceTextCtrl = nullptr;

    // This will be used to check when a textBox is empty to delete it when pressing BACKSPACE
    std::tuple<int, std::string> textBoxAux = {9999, "AUX"};

    // Fonts
    wxFont m_pageNameFont = wxFont(33, wxMODERN, wxSLANT, wxBOLD);
    wxFont m_defaultFont = wxFont(10, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont m_H1Font = wxFont(25, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont m_H2Font = wxFont(20, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxFont m_H3Font = wxFont(15, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
};

enum
{
    ID_NewWorkspace                 = 10012,
    ID_ButtonWorkspace              = 10013,
    ID_TextCtrlModifyWorkspace      = 10014,
    ID_ButtonModifyWorkspace        = 10015,
    ID_ButtonDeleteWorkspace        = 10016,
    ID_ButtonNewRootPage            = 10017,
    ID_ButtonNewChildPage           = 10018,
    ID_TableListView                = 10019,
    ID_RadioButtonSelectedType1     = 10020,
    ID_RadioButtonSelectedType2     = 10021,
    ID_RadioButtonSelectedType3     = 10022,
    ID_ButtonCreatePage             = 10023,
    ID_ButtonAddRowTable            = 10024,
    ID_ButtonEditRowTable           = 10025,
    ID_ButtonDeleteRowTable         = 10026,
    ID_ButtonCreateViewTable        = 10027,
    ID_ButtonReturnFromModifyTable  = 10028,
    ID_ButtonModifyRowTable         = 10029, // This is inside the generated edit page. Different to editRow button
    ID_ButtonSettingsPage  			= 10030,
    ID_ButtonSettingsPageModify		= 10031,
    ID_ButtonSettingsPageDelete		= 10032,
    ID_TextCtrlModifyPageName       = 10033
};
