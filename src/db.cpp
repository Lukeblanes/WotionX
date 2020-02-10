#include "db.h"
#include <algorithm>

#define DB_DEBUG 1
#define DEBUG(TO_PRINT) \
    if(DB_DEBUG) \
{ \
        TO_PRINT \
}

#include <iostream>

std::vector<std::string> DB::queryResults;
std::vector<std::string> DB::queryErrors;

int DB::workSpaceID = 0;

int DB::callback(void *notUsed, int argc, char** argv, char** errorMessage)
{
    for (int i = 0; i < argc; ++i) {
        DB::queryResults.push_back(argv[i]);
        DB::queryErrors.push_back(errorMessage[i]);
    }
    return 0;
}

void DB::verbose_query_results()
{
DEBUG(
    std::cout << "Verbose query results: \n";
    for (int i = 0; i < DB::queryResults.size(); ++i )
        std::cout << "QueryError: " << DB::queryErrors[i] << "\nQueryResults: " << DB::queryResults[i] << '\n';
)
}

void DB::query_clear()
{
    DB::queryErrors.clear();
    DB::queryResults.clear();
}

void DB::open_connection()
{
   int rc;

   std::string fullDBName = m_databaseName + ".db";
   rc = sqlite3_open(fullDBName.c_str(), &db);

   if( rc ) {
      std::cout <<  "Can't open database: " << sqlite3_errmsg(db) << "\n";
   } else {
      std::cout <<  "Opened database successfully\n";
   }
}

void DB::execute_sql(const char* sql_command, const std::string& err_message = "")
{
    char* zErrMsg = nullptr;
    /* Execute SQL statement */
    int rc = sqlite3_exec(db, sql_command, callback, nullptr, &zErrMsg);

    if( rc != SQLITE_OK ) {
        std::cout << "SQL error: " << zErrMsg << '\n';
        std::cout << err_message;
        sqlite3_free(zErrMsg);
        exit(-1);
    } else {
        DEBUG(std::cout << "SQL Command(s):\n" << sql_command << "\nexecuted successfully\n";)
    }
}

DB::~DB()
{
    std::cout <<  "Closing Database";
    sqlite3_close(db);
}

DB &DB::GetInstance()
{
    static DB instance;

    if (instance.db == nullptr)
        instance.open_connection();
    return instance;
}

void DB::Init()
{
    std::string query =
    "CREATE TABLE IF NOT EXISTS WORKSPACES(" \
    "ID	INTEGER PRIMARY KEY NOT NULL," \
    "NAME TEXT NOT NULL," \
    "SELECTED INT NOT NULL);";
    execute_sql(query.data());
    query_clear();

    query = "SELECT count(*) FROM WORKSPACES;";
    execute_sql(query.data());
    // Select query Only returns one element
    if (queryResults[0] == "0") {
        query = "INSERT INTO WORKSPACES (ID, NAME, SELECTED)" \
                "VALUES (" + std::to_string(++DB::workSpaceID)+ ", " \
                "\"Default_W\", " \
                "1);";

        execute_sql(query.data(), "Couldn't create default workspace. Terminating...\n");
        verbose_query_results();
    }
    // Workspace is created so we need to obtain:
    // Selected workspace, highest workspaceID and workspaceMenuId
    else {
        DB::workSpaceID += std::stoi(queryResults[0]);
    }
    query_clear();

    query =
    "CREATE TABLE IF NOT EXISTS PAGES(" \
    "ID	INTEGER PRIMARY KEY AUTOINCREMENT," \
    "NAME TEXT NOT NULL," \
    "PARENT_ID INT NOT NULL," \
    "WORKSPACE_ID INT NOT NULL," \
    "POSITION INT NOT NULL," \
    "TYPE INT NOT NULL," \
    "FOREIGN KEY(WORKSPACE_ID) REFERENCES WORKSPACES(Id))";
    execute_sql(query.data());
    query_clear();
}

workspace_DB DB::Get_SelectedWorkspace()
{
    execute_sql("SELECT * FROM WORKSPACES WHERE \"SELECTED\" = 1");
    verbose_query_results();

    // selected should always return true
    workspace_DB ret = {std::stoi(queryResults[0]), queryResults[1], true};
    query_clear();
    return ret;
}

void DB::Update_CurrentWorkspace(int newWorkspaceId)
{
    execute_sql("UPDATE WORKSPACES SET \"Selected\" = 0 WHERE \"Selected\" = 1;",
                "ERROR: No Workspaces created\n");

    DEBUG(std::cout << "SwitchWorkspace Query 1:\n";)
    verbose_query_results();
    query_clear();

    std::string query = "UPDATE WORKSPACES SET \"Selected\" = 1 WHERE \"Id\" = " + std::to_string(newWorkspaceId) + ";";
    execute_sql(query.data(), "ERROR: No Workspaces created\n");
    DEBUG(std::cout << "SwitchWorkspace Query 2:\n";)
    verbose_query_results();
    query_clear();
}

void DB::Update_CurrentWorkspaceName(const std::string& newName)
{
    std::string query = "UPDATE WORKSPACES SET \"name\" = \""+ newName + "\" WHERE \"Selected\" = 1;";
    execute_sql(query.data(), "ERROR: Current Workspace couldn't be modified\n");

    DEBUG(std::cout << "update current workspace query :\n";)
    verbose_query_results();
    query_clear();
}

DB_CODE DB::Delete_CurrentWorkspace()
{
    DEBUG(std::cout << "Delete_CurrentWorkspace\n";)
    if (DB::workSpaceID == 1)
        return DB_CODE::DELETING_LAST_WORKSPACE;

    std::string query = "";
    DEBUG(std::cout << "Get_SelectedWorkspace id call\n";)
    auto deletedId = std::to_string(Get_SelectedWorkspace().id);

    DEBUG(std::cout << "Get_WorkspacePages call\n";)
    std::vector<pages_DB> aux_pages;
    Get_WorkspacePages(aux_pages);

    DEBUG(std::cout << "aux_pages size is " << aux_pages.size() << "\n";)
    if(aux_pages.size() > 0)
    {
        // Delete Page_Elems for each page in workspace
        for (auto& page : aux_pages) {
            query = "DROP TABLE PAGE_ELEMS_" + std::to_string(page.id) + ";";
            execute_sql(query.data(), "Error: couldn't drop page_elements table");
        }

         // Delete Pages and update pages to new workspace ids from offset
        query = "DELETE FROM PAGES WHERE WORKSPACE_ID = " + deletedId + ";";
        execute_sql(query.data());
        query_clear();
    }

    DEBUG(std::cout << "\n";)
    query = "DELETE FROM WORKSPACES WHERE \"Selected\" = 1;"
            "UPDATE PAGES SET \"Workspace_Id\" = \"Workspace_Id\" - 1 "
                    "WHERE \"Workspace_Id\" > " + deletedId + ";\n" +
            "UPDATE WORKSPACES SET \"Id\" = \"Id\" - 1 WHERE \"Id\" > " + deletedId + ";" +
            "UPDATE WORKSPACES SET \"Selected\" = 1 WHERE \"Id\" = 1;";
    execute_sql(query.data());
    query_clear();

    // update workSpaceId for next Created Workspace
    DB::workSpaceID--;
    return DB_CODE::OK;
}


// TODO: Function not needed
int DB::Get_HighestPageId(int parentNode)
{
    auto currentWorkspace = std::to_string(Get_SelectedWorkspace().id);

    int return_value = 0;
    std::string query = "SELECT count(*) FROM PAGES WHERE PARENT_ID = "+ std::to_string(parentNode) +
            " AND WORKSPACE_ID = " + currentWorkspace + ";";
    execute_sql(query.data());
    auto elements = queryResults[0];
    query_clear();

    if (std::stoi(elements) > 0 )
    {
        std::string query = "SELECT max(position) FROM PAGES WHERE PARENT_ID ="+ std::to_string(parentNode)+
                            " AND WORKSPACE_ID = " + currentWorkspace + ";";
        execute_sql(query.data());
        return_value = std::stoi(queryResults[0]);
        query_clear();
    }

    return return_value;
}

void DB::Get_AllPages(std::vector<pages_DB>& ret_vec)
{
    ret_vec.clear();
    execute_sql("SELECT * FROM PAGES;");
    DEBUG(std::cout << "GetPages:\n";)
    verbose_query_results();

    for(int i = 0; i < queryResults.size(); i+= PAGES_COL_SIZE )
    {
        ret_vec.push_back(
        {std::stoi(queryResults[i + 0]), queryResults[i + 1], std::stoi(queryResults[i + 2]),
         std::stoi(queryResults[i+3]), std::stoi(queryResults[i+4]), static_cast<PAGE_TYPE>(std::stoi(queryResults[i+5]))} );
    }

    query_clear();
}

void DB::Get_WorkspacePages(std::vector<pages_DB>& ret_vec)
{
    ret_vec.clear();

    DEBUG(std::cout << "Get_WorkspacePages:\n";)
    auto currentWorkspace = std::to_string(Get_SelectedWorkspace().id);
    std::string query = "SELECT count(*) FROM PAGES WHERE WORKSPACE_ID =" + currentWorkspace + ";";
    execute_sql(query.data(), "Error: couldn't do count(*) on pages with current workspace\n");
    auto pagesCount = std::stoi(queryResults[0]);
    query_clear();

    DEBUG(std::cout << "if pagescount > 0 we enter if. pagescount is " << pagesCount << "\n";)
    if(pagesCount > 0)
    {
        query = "SELECT * FROM PAGES WHERE WORKSPACE_ID ="+currentWorkspace+";";
        execute_sql(query.data(), "Error: couldn't select");
        DEBUG(std::cout << "GetWorkspacePages:\n";)
        verbose_query_results();

        // fix this for type
        for(int i = 0; i < queryResults.size(); i+= PAGES_COL_SIZE )
        ret_vec.push_back({std::stoi(queryResults[i + 0]), queryResults[i + 1], std::stoi(queryResults[i + 2]),
                           std::stoi(queryResults[i+3]), std::stoi(queryResults[i+4]), static_cast<PAGE_TYPE>(std::stoi(queryResults[i+5])) });
        query_clear();
    }
}

// Returns the id of the newly created page
int DB::Create_WorkspacePage(int parent, int workspace, int position, int type)
{
     // Create new Page and set it as selected
    std::string query = "INSERT INTO PAGES (NAME, PARENT_ID, WORKSPACE_ID, POSITION, TYPE)" \
                        "VALUES (\"Untitled\", " \
                        + std::to_string(parent)+ ", " \
                        + std::to_string(workspace)+ ", " \
                        + std::to_string(position) + ", " \
                        + std::to_string(type) + ");";

    execute_sql(query.data(), "ERROR: PAGE COULD NOT BE CREATED\n" );
    verbose_query_results();
    query_clear();

    query = "SELECT max(ID) FROM PAGES;";
    execute_sql(query.data(), "ERROR: Could not obtain max id after creating pages\n" );
    verbose_query_results();
    auto newId = std::stoi(queryResults[0]);
    query_clear();
    return newId;
}

int DB::Get_ParentId(int currentPageId)
{
    // Create new Page and set it as selected
    std::string query = "SELECT parent_id FROM PAGES WHERE ID = " + std::to_string(currentPageId) + ";";
    execute_sql(query.data(), "ERROR: parent_id could not be selected\n" );
    auto parent_id = queryResults[0];
    DEBUG(std::cout << "Obtained parent_id = " << parent_id << "\n";)
    verbose_query_results();
    query_clear();

    return std::stoi(parent_id);
}

int DB::Delete_PageAndChildren(int currentPageId)
{
    DEBUG(std::cout << "before executing Delete_PageAndChildren, currentPageId is "+  std::to_string(currentPageId)  + " \n";)

    // we first get all children from current parent
    std::string query = "SELECT * FROM PAGES WHERE PARENT_ID = " + std::to_string(currentPageId) + ";";
    execute_sql(query.data(), "ERROR: Could not get current page's children.\n" );

    std::vector<uint32_t> children; // We save it locally so that recursive calls can freely use queryResults
    uint32_t cycler = 0; // POSITION for id is 0 and we will offset by the number of columns in PAGES
    while(queryResults.size() > cycler)
    {
        children.push_back(std::stoi( queryResults[cycler]));
        cycler += PAGES_COL_SIZE;
    }
    query_clear();

    for(auto& child : children)
        DB::GetInstance().Delete_PageAndChildren(child);

    query = "DROP TABLE PAGE_ELEMS_" + std::to_string(currentPageId) + ";\n" +
            "DELETE FROM PAGES WHERE ID = " + std::to_string(currentPageId) + ";";
    execute_sql(query.data(), "ERROR: Could not currentPageId PAGE_ELEMS and/or delete from PAGES\n" );
    query_clear();
    return 0;
}

int DB::Update_PageName(int currentPageId, const std::string& newName)
{
    std::string query = "UPDATE PAGES SET \"NAME\" = \""+ newName + "\" WHERE \"ID\" = "+ std::to_string(currentPageId) + ";";
    execute_sql(query.data(), "ERROR: Current Page name couldn't be modified\n");

    DEBUG(std::cout << "update current page name query :\n";)
    verbose_query_results();
    query_clear();
}

void DB::Get_DBMetaInfo(std::vector<std::string>& elements, int currentPageId)
{
    elements.clear();

    std::string query = "SELECT * FROM PAGE_ELEMS_" + std::to_string(currentPageId) + ";";
    execute_sql(query.data(), "Error: Page Elements couldn't be accessed");
    DEBUG(std::cout << "Get_DBMetaInfo:\n";)
    verbose_query_results();

    // queryErrors for some reason stores the columns twice
    // we have to store only unique columns
    for (auto& elem: queryErrors)
    {
        if( find(elements.begin(), elements.end(), elem) == elements.end() ) // If not in ignore list we enter
            elements.push_back(elem);
     }

    query_clear();
}

void DB::Create_MarkdownPage(int page)
{
    auto currentWorkspace = std::to_string(Get_SelectedWorkspace().id);

    std::string newPageElementsTable = "PAGE_ELEMS_" + std::to_string(page);

    std::string query =
    "CREATE TABLE IF NOT EXISTS "+ newPageElementsTable +  " (" \
    "ID	INTEGER PRIMARY KEY AUTOINCREMENT," \
    "CONTENT TEXT NOT NULL," \
    "POSITION INT NOT NULL);" \

    // Populate with default textboxCTRL
    "INSERT INTO " + newPageElementsTable + " (CONTENT, POSITION)" \
                        "VALUES (\"^ Modify Me\", 1);";

    execute_sql(query.data());
    verbose_query_results();
    query_clear();

}

void DB::Get_MarkdownElements(std::vector<markdown_elements_DB> &elements, int currentPage)
{
    elements.clear();

    std::string query = "SELECT * FROM PAGE_ELEMS_" + std::to_string(currentPage) +  " ORDER BY POSITION;";
    execute_sql(query.data(), "Error: Page Elements couldn't be accessed");
    DEBUG(std::cout << "GetCurrentPageElements:\n";)
    verbose_query_results();

    for(int i = 0; i < queryResults.size(); i+= MARKDOWN_ELEMENTS_COL_SIZE )
        elements.push_back({std::stoi(queryResults[i + 0]), queryResults[i + 1], std::stoi(queryResults[i + 2])});

    query_clear();
}

void DB::Insert_MarkdownElement(int currentPage, const std::string &content, int newPosition)
{
    // Before inserting check if newPosition is already in table
    // if it is, we update doing +1 to the positions, if it's not, just add.
    std::string query = "SELECT max(POSITION) from PAGE_ELEMS_" + std::to_string(currentPage)+ ";";
    execute_sql(query.data(), "ERROR: We couldn't insert into page_elems table.\n" );
    auto highestPos = std::stoi(queryResults[0]);

    // newPosition already occupied
    if (newPosition <= highestPos)
    {
        query = "UPDATE PAGE_ELEMS_" + std::to_string(currentPage) +" SET POSITION = POSITION + 1 " +
                "WHERE POSITION >= " + std::to_string(newPosition) + ";";

        execute_sql(query.data(), "ERROR: Couldn't update positions\n");
    }

    Insert_TableElement(currentPage, "('^ " + content + "', " + std::to_string(newPosition) + " )", "(CONTENT, POSITION)");
}

void DB::Create_TablePage(int page)
{
    auto currentWorkspace = std::to_string(Get_SelectedWorkspace().id);

    std::string newPageElementsTable = "PAGE_ELEMS_" + std::to_string(page);

    std::string query =
    "CREATE TABLE IF NOT EXISTS "+ newPageElementsTable +  " (" \
    "ID	INTEGER PRIMARY KEY AUTOINCREMENT," \
    "COLUMN TEXT NOT NULL);\n" \

    // Populate with default textboxCTRL
    "INSERT INTO " + newPageElementsTable + " (COLUMN) VALUES (\"Modify Me\");";

    execute_sql(query.data());
    verbose_query_results();
    query_clear();
}

void DB::Get_TableElements(std::vector<std::vector<std::string> > &elements, int currentPage)
{
    elements.clear();
    std::vector<std::string> aux; // Fist used to get columns and then to add to vector
    Get_DBMetaInfo(aux, currentPage); // This clears query_results so we have to do it here
    auto columns = aux.size();
    aux.clear();

    std::string query = "SELECT * FROM PAGE_ELEMS_" + std::to_string(currentPage) +  ";";
    execute_sql(query.data(), "Error: Page Elements couldn't be accessed");
    DEBUG(std::cout << "Get_TableElements:\n";)
    verbose_query_results();

    for(int i = 0; i < queryResults.size(); i+= columns)
    {
        for(int j = 0; j < columns; j++)
            aux.push_back(queryResults[i+j]);

        elements.push_back(aux);
        aux.clear();
    }

    query_clear();
}

void DB::Insert_TableElement(int currentPage, const std::string& content, const std::string& columns)
{
    std::string query = "INSERT INTO PAGE_ELEMS_" + std::to_string(currentPage) + " " + columns + " " \
                        "VALUES " + content + ";" ;

    execute_sql(query.data(), "ERROR: Couldn't insert into page_elems table.\n" );
    verbose_query_results();
    query_clear();
}

void DB::Update_TableElement(int currentPage, const std::string& row, const std::vector<std::string> &content, const std::vector<std::string> &columns)
{
    std::string formatted_values = "";
    if(content.size() == columns.size()) {
        for(int i = 0; i < content.size(); ++i)
            formatted_values += columns[i] + " = " + content[i] + ", ";

        formatted_values = (formatted_values.substr(0, formatted_values.size()-2)); // remove last ', '
    } else {
        DEBUG(std::cout << "Column and Content mismatch;";)
        exit(-1);
    }

     std::string query = "UPDATE PAGE_ELEMS_" + std::to_string(currentPage) +
                        " SET " + formatted_values + " WHERE ID = " + row +  ";";

    execute_sql(query.data(), "ERROR: We couldn't insert into page_elems table.\n" );
    verbose_query_results();
    query_clear();

}

void DB::Delete_TableElement(int currentPage, const std::string& elementId)
{
    std::string query = "DELETE FROM PAGE_ELEMS_" + std::to_string(currentPage) +
                        " WHERE ID = " + elementId + ";";

    execute_sql(query.data(), "ERROR - Delete_TableElement: Couldn't delete table element.\n" );
    verbose_query_results();
    query_clear();

}

void DB::Update_MarkdownElement(int currentPage, const std::string &newContent, int currentPosition)
{
    std::string query = "UPDATE PAGE_ELEMS_" + std::to_string(currentPage) +
                        " SET CONTENT = \"" + newContent + "\" WHERE POSITION = " + std::to_string(currentPosition)+";";

    execute_sql(query.data(), "ERROR: We couldn't insert into page_elems table.\n" );
    verbose_query_results();
    query_clear();

}

void DB::Make_MarkdownElement_Editable(int currentPage, int currentPosition)
{
    std::string query = "UPDATE PAGE_ELEMS_" + std::to_string(currentPage) +
                        " SET CONTENT = '^ ' || CONTENT WHERE POSITION = " + std::to_string(currentPosition)+";";

    execute_sql(query.data(), "ERROR: We couldn't add '^ ' into page_elems content.\n" );
    verbose_query_results();
    query_clear();
}

void DB::Delete_MarkdownElement(int currentPage, int currentPosition)
{
    std::string query = "DELETE FROM PAGE_ELEMS_" + std::to_string(currentPage) +
                        " WHERE POSITION = " + std::to_string(currentPosition)+ ";" +
                        "UPDATE PAGE_ELEMS_" + std::to_string(currentPage) +" SET POSITION = POSITION - 1 " +
                        "WHERE POSITION > " + std::to_string(currentPosition) + ";";

    execute_sql(query.data(), "ERROR: We couldn't delete page element .\n" );
    verbose_query_results();
    query_clear();
}

void DB::Get_Workspaces(std::vector<workspace_DB>& ret_vec)
{
    ret_vec.clear();
    execute_sql("SELECT * FROM WORKSPACES;");
    DEBUG(std::cout << "GetWorkspaces:\n";)
    verbose_query_results();

    for(int i = 0; i < queryResults.size(); i+= WORKSPACES_COL_SIZE )
        ret_vec.push_back({std::stoi(queryResults[i + 0]), queryResults[i + 1], queryResults[i + 2] == "1" ? true : false});

    query_clear();
}

DB_CODE DB::Create_Workspace()
{
    if (DB::workSpaceID == 10)
        return DB_CODE::TOO_MANY_WORKSPACES;

    // Make Current Selected = 0
    execute_sql("UPDATE WORKSPACES SET SELECTED = 0 WHERE SELECTED = 1;", "ERROR: WORKSPACES COULD NOT BE UPDATED\n");
    verbose_query_results();
    query_clear();

    // Create new Workspace and set it as selected
    std::string query = "INSERT INTO WORKSPACES (ID, NAME, SELECTED)" \
                        "VALUES (" + std::to_string(++DB::workSpaceID)+ ", " \
                        "\"Workspace "+ std::to_string(DB::workSpaceID)+"\", " \
                        "1);";

    execute_sql(query.data(), "ERROR: WORKSPACES COULD NOT BE UPDATED\n" );
    verbose_query_results();
    query_clear();

    return DB_CODE::OK;
}
