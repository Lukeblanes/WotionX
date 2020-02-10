#include "cApp.h"
#include "db.h"

wxIMPLEMENT_APP(cApp);

bool cApp::OnInit()
{
    DB::GetInstance().Init();

    frame1 = new cFrame();
    frame1->Show(true);

    return true;
}
