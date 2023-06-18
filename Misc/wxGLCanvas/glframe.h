#include "glhelper.h"

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

    void initGL();

    wxGLCanvas*  mCanvas;
    wxGLContext* mContext;
    GLHelper     mHelper;
};
