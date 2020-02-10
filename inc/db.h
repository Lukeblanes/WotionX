#pragma once

#include <sqlite3.h>
#include <vector>
#include <string>

// PAGES
// ID | NAME | PARENT_ID | WORKSPACE_ID | POSITION | TYPE |
// if it doesn't have a parent, parent_id is -1
// each page is owned by a unique workspace which must exist
// Position determines where in the treeTextCtrl it is situated
// Type determines if the gui will render as Markdown, a table or a trello board

// PAGE_ELEMENTS_(int ID)
// ID | POS | CONTENT
// In this case each page will have its own page elements table

enum class DB_CODE { TOO_MANY_WORKSPACES, DELETING_LAST_WORKSPACE, OK };
enum PAGE_TYPE { MARKDOWN = 1, TABLE = 2};

#define WORKSPACES_COL_SIZE 3
struct workspace_DB
{
    int id;
    std::string name;
    bool selected;
};

#define PAGES_COL_SIZE 6
struct pages_DB
{
    int id;
    std::string name;
    int parent_id;
    int workspace_id;
    int position;
    PAGE_TYPE type;
};

#define MARKDOWN_ELEMENTS_COL_SIZE 3
struct markdown_elements_DB
{
    int id;
    std::string content;
    int position;
};

class DB
{
private:
    DB() { }
    void open_connection();
    void execute_sql(const char* sql_command, const std::string& err_message);
    static int callback(void *notUsed, int argc, char** argv, char** errorMessage);
    sqlite3 *db = nullptr;

    static std::vector<std::string> queryResults;
    static std::vector<std::string> queryErrors;

    void verbose_query_results();
    void query_clear();

    std::string m_databaseName = "test";

    static int workSpaceID;

public:
    ~DB();
    DB(DB const& db) = delete;
    void operator=(DB const& db) = delete;

    static DB& GetInstance();
    void Init();

    // Workspace
    DB_CODE Create_Workspace();
    workspace_DB Get_SelectedWorkspace();
    void Update_CurrentWorkspace(int newWorkspaceId);
    void Get_Workspaces(std::vector<workspace_DB>& workspaces);
    void Update_CurrentWorkspaceName(const std::string& newName);
    DB_CODE Delete_CurrentWorkspace();

    // Page
    int Get_HighestPageId(int parentNode);
    void Get_AllPages(std::vector<pages_DB>& ret_vec);
    void Get_WorkspacePages(std::vector<pages_DB>& ret_vec);
    int Create_WorkspacePage(int parent, int workspace, int position, int type);
    int Get_ParentId(int currentPageId);
    int Delete_PageAndChildren(int currentPageId);
    int Update_PageName(int currentPageId, const std::string& newName);

    // Page Elements
    void Get_DBMetaInfo(std::vector<std::string>& elements, int currentPageId);
    // - Markdown
    void Create_MarkdownPage(int page);
    void Get_MarkdownElements(std::vector<markdown_elements_DB>& elements, int currentPage);
    void Insert_MarkdownElement(int currentPage, const std::string& content, int newPosition);
    void Update_MarkdownElement(int currentPage, const std::string& newContent, int currentPosition);
    void Make_MarkdownElement_Editable(int currentPage, int currentPosition);
    void Delete_MarkdownElement(int currentPage, int currentPosition);
    // - Table
    void Create_TablePage(int page);
    void Get_TableElements(std::vector<std::vector<std::string>>& elements, int currentPage);
    void Insert_TableElement(int currentPage, const std::string& content, const std::string& columns);
    void Update_TableElement(int currentPage, const std::string& row, const std::vector<std::string>& content, const std::vector<std::string>& columns);
    void Delete_TableElement(int currentPage, const std::string& elementId);
};
