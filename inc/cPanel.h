#pragma once

#include <wx/wx.h>
#include <wx/vscroll.h>

class cPanel : public wxVScrolledWindow
{
public:
    cPanel(wxWindow *parent);
private:
    int workSpaceID;
    virtual wxCoord OnGetRowHeight (size_t row)	const override { return 100; } ;

    wxDECLARE_EVENT_TABLE();
};
