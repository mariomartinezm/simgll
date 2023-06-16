#include "GL/glew.h"
#include "wx/wx.h"
#include "wx/glcanvas.h"

#include "glhelper.h"

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
