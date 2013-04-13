#ifndef SEEN_PENCIL_CONTEXT_H
#define SEEN_PENCIL_CONTEXT_H

/** \file
 * SPPencilContext: a context for pencil tool events
 */

#include "draw-context.h"


#define SP_TYPE_PENCIL_CONTEXT (sp_pencil_context_get_type())
//#define SP_PENCIL_CONTEXT(o) (G_TYPE_CHECK_INSTANCE_CAST((o), SP_TYPE_PENCIL_CONTEXT, SPPencilContext))
#define SP_PENCIL_CONTEXT_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k), SP_TYPE_PENCIL_CONTEXT, SPPencilContextClass))
//#define SP_IS_PENCIL_CONTEXT(o) (G_TYPE_CHECK_INSTANCE_TYPE((o), SP_TYPE_PENCIL_CONTEXT))
#define SP_IS_PENCIL_CONTEXT_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE((k), SP_TYPE_PENCIL_CONTEXT))
#define SP_PENCIL_CONTEXT(obj) ((SPPencilContext*)obj)
#define SP_IS_PENCIL_CONTEXT(obj) (((SPEventContext*)obj)->types.count(typeid(SPPencilContext)))

enum PencilState {
    SP_PENCIL_CONTEXT_IDLE,
    SP_PENCIL_CONTEXT_ADDLINE,
    SP_PENCIL_CONTEXT_FREEHAND,
    SP_PENCIL_CONTEXT_SKETCH
};

class CPencilContext;

/**
 * SPPencilContext: a context for pencil tool events
 */
class SPPencilContext : public SPDrawContext {
public:
	SPPencilContext();
	CPencilContext* cpencilcontext;

    Geom::Point p[16];
    gint npoints;
    PencilState state;
    Geom::Point req_tangent;

    bool is_drawing;

    std::vector<Geom::Point> ps;

    Geom::Piecewise<Geom::D2<Geom::SBasis> > sketch_interpolation; // the current proposal from the sketched paths
    unsigned sketch_n; // number of sketches done

	static const std::string prefsPath;
};

/// The SPPencilContext vtable (empty).
struct SPPencilContextClass : public SPEventContextClass { };

class CPencilContext : public CDrawContext {
public:
	CPencilContext(SPPencilContext* pencilcontext);

	virtual void setup();
	virtual gint root_handler(GdkEvent* event);

	virtual const std::string& getPrefsPath();

private:
	SPPencilContext* sppencilcontext;
};

GType sp_pencil_context_get_type();


#endif /* !SEEN_PENCIL_CONTEXT_H */

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fileencoding=utf-8:textwidth=99 :
