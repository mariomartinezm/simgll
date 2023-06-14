#include "trianglewindow.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

bool MyApp::OnInit()
{
    TriangleWindow* window = new TriangleWindow(nullptr, "Hello OpenGL");
    window->Show();

    return true;
}

// This defines the equivalent of main() for the current platform
wxIMPLEMENT_APP(MyApp);
