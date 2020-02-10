#include "cPanel.h"

wxBEGIN_EVENT_TABLE(cPanel, wxVScrolledWindow)
wxEND_EVENT_TABLE()

#include "wx/dcbuffer.h"

cPanel::cPanel(wxWindow* parent) :
    wxVScrolledWindow(parent, wxID_ANY, wxPoint(0, 0),wxSize(100, 11000))
{
    SetRowCount(0);
}
