#ifndef  __JS_CANVASLITE_H
#define __JS_CANVASLITE_H

#include <js_object.h>
#include <pm_container.h>

#ifdef PKG_USING_VGL

namespace Persimmon
{

class JsCanvasLite : public Container
{
public:
    JsCanvasLite();
    virtual ~JsCanvasLite();

    void createContext();
    struct vgl_t* canvasContext()
    {
        return m_vgl;
    }
    void draw();
    void setCanvasBuffer(bool enabled);

    virtual void render(struct rtgui_dc* dc, const Point &dcPoint = Point(),
                        const Rect &srcRect = Rect(), RenderFlag flags = DrawNormal);

    virtual void setData(void *data);

private:
    void drawOnBuffer();
    void updateCanvasBuffer();

private:
    struct vgl_t *m_vgl, *m_drawVgl;
    struct rtgui_dc *m_canvasBuffer;
    struct vgl_surface_t *m_bufferSurface;
    bool m_useCanvasBuffer, m_isdirty;
};

};

#endif // ! __JS_CANVASLITE_H

#endif
