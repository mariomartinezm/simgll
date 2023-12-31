#include <wx/wx.h>
#include "glframe.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

bool MyApp::OnInit()
{
    GLFrame* frame = new GLFrame(nullptr);
    frame->Show();

    return true;
}

// This defines the equivalent of main() for the current platform
wxIMPLEMENT_APP(MyApp);
