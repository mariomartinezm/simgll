#include "glhelper.h"

#include <chrono>
#include <wx/event.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>


class GLFrame : public wxFrame
{
public:
    GLFrame(wxWindow*);
    ~GLFrame();

private:
    void OnCanvasSize(wxSizeEvent&);
    void OnCanvasPaint(wxPaintEvent&);
    void OnIdle(wxIdleEvent& evt);

    void initGL();

    wxGLCanvas*   mCanvas;
    wxGLContext*  mContext;
    wxStaticText* mStaticTextFPS;
    GLHelper      mHelper;
    bool mIsInitialized = { false };
    int counter = { 0 };

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> end;
    GLfloat elapsed = { 0 };
};
