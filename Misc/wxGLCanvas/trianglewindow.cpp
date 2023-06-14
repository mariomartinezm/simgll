#include "trianglewindow.h"
#include "trianglecanvas.h"

// Triangle canvas widget ID
const int triCanvasId = 2000;

TriangleWindow::TriangleWindow(wxWindow* parent, const wxString& title,
                               const wxPoint& pos, const wxSize& size)
    : wxFrame(parent, wxID_ANY, title, pos, size, wxMINIMIZE_BOX | wxCLOSE_BOX |
              wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN)
{
    TriangleCanvas* canvas = new TriangleCanvas(this, triCanvasId, nullptr,
                                                { 0, 0 }, { 800, 800 });

    Fit();
    Centre();
}

TriangleWindow::~TriangleWindow()
{
}
