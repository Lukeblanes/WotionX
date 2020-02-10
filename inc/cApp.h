#pragma once

#include <wx/wx.h>
#include "cFrame.h"

class cApp : public wxApp
{
public:
    virtual bool OnInit();
private:
    cFrame *frame1;
};
