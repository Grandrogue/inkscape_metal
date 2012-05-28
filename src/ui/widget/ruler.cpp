/** \file
 * Gtkmm facade/wrapper around SPRuler code that formerly lived
 * in desktop-events.cpp
 */
/*
 * Authors:
 *   Ralf Stephan <ralf@ark.in-berlin.de>
 *   Lauris Kaplinski
 *   Jon A. Cruz <jon@joncruz.org>
 *   Abhishek Sharma
 *
 * Copyright (C) 2005 Ralf Stephan
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glibmm/i18n.h>

#include "ui/widget/ruler.h"
#include "widgets/ruler.h"
#include "helper/units.h"

#include "xml/repr.h"
#include "display/sp-canvas.h"
#include "display/guideline.h"
#include "desktop.h"
#include "desktop-handles.h"
#include "document.h"
#include "document-undo.h"
#include "sp-namedview.h"
#include "verbs.h"

namespace Gtk {
namespace Deprecated {
void Ruler::set_range(double lower, double upper, double position, double max_size)
{
sp_ruler_set_range(gobj(), lower, upper, position, max_size); 
}

void Ruler::get_range(double& lower, double& upper, double& position, double& max_size)
{
sp_ruler_get_range(gobj(), &(lower), &(upper), &(position), &(max_size)); 
}
}// namespace Deprecated
}// namespace Gtk


namespace Inkscape {
namespace UI {
namespace Widget {

void
Ruler::init(SPDesktop *dt, Gtk::Widget &w)
{
    _dt = dt;
    _canvas_widget = &w;
    _dragging = false;
    _guide = 0;
    sp_ruler_set_metric(SP_RULER(_r->gobj()), SP_PT);
    _r->set_range(-500, 500, 0, 1000);
}

void
Ruler::get_range(double &lo, double &up, double &pos, double &max)
{
    _r->get_range(lo, up, pos, max);
}

void
Ruler::set_range(double lo, double up, double pos, double max)
{
    _r->set_range(lo, up, pos, max);
}

/// Set metric from namedview
void
Ruler::update_metric()
{
    if (!_dt) return;
    sp_ruler_set_metric(SP_RULER(_r->gobj()), _dt->namedview->getDefaultMetric());
}

/// Returns text to be used for tooltip for ruler.
/// \todo incorrect
Glib::ustring
Ruler::get_tip()
{
    return gettext(sp_unit_get_plural( _dt
                                       ? _dt->namedview->doc_units
                                       : &sp_unit_get_by_id(SP_UNIT_PT) ));
}

/// Helper that gets mouse coordinates relative to canvas widget.
void
Ruler::canvas_get_pointer(int &x, int &y)
{
    Gdk::ModifierType mask;
    (void) _canvas_widget->get_window()->get_pointer(x, y, mask);
}

Geom::Point
Ruler::get_event_dt()
{
    int wx, wy;
    canvas_get_pointer(wx, wy);
    Geom::Point const event_win(wx, wy);
    Geom::Point const event_w(sp_canvas_window_to_world(_dt->canvas, event_win));
    return _dt->w2d(event_w);
}

bool
Ruler::on_button_press_event(GdkEventButton *evb)
{
    g_assert(_dt);
    Geom::Point const &event_dt = get_event_dt();
    Inkscape::XML::Node *repr = _dt->namedview->getRepr();

    if (evb->button == 1) {
        _dragging = true;
        sp_repr_set_boolean(repr, "showguides", TRUE);
        sp_repr_set_boolean(repr, "inkscape:guide-bbox", TRUE);
        _guide = sp_guideline_new(_dt->guides, NULL, event_dt, _horiz_f ? Geom::Point(0.,1.) : Geom::Point(1.,0.));
        sp_guideline_set_color(SP_GUIDELINE(_guide), _dt->namedview->guidehicolor);

#if WITH_GTKMM_3_0
	Glib::RefPtr<Gdk::Device> device = Glib::wrap(gdk_event_get_device((GdkEvent*)evb));
	device->grab(get_window(),
                     Gdk::OWNERSHIP_NONE, // TODO: Check that this is correct
		     false,
                     Gdk::BUTTON_RELEASE_MASK |
                     Gdk::POINTER_MOTION_MASK |
                     Gdk::POINTER_MOTION_HINT_MASK,
		     evb->time);
#else
        (void) get_window()->pointer_grab(false,
                        Gdk::BUTTON_RELEASE_MASK |
                        Gdk::POINTER_MOTION_MASK |
                        Gdk::POINTER_MOTION_HINT_MASK,
                        evb->time);
#endif
    }
    return false;
}

bool
Ruler::on_motion_notify_event(GdkEventMotion *)
{
    g_assert(_dt);
    Geom::Point const &event_dt = get_event_dt();

    if (_dragging) {
        sp_guideline_set_position(SP_GUIDELINE(_guide), event_dt);
        _dt->set_coordinate_status(event_dt);
        _dt->setPosition(event_dt);
    }
    return false;
}

bool
Ruler::on_button_release_event(GdkEventButton *evb)
{
    g_assert(_dt);
    int wx, wy;
    canvas_get_pointer(wx, wy);
    Geom::Point const &event_dt = get_event_dt();

    if (_dragging && evb->button == 1) {

#if WITH_GTKMM_3_0
	Glib::RefPtr<Gdk::Device> device = Glib::wrap(gdk_event_get_device((GdkEvent*)evb));
	device->ungrab(evb->time);
#else
        Gdk::Window::pointer_ungrab(evb->time);
#endif

        gtk_object_destroy(GTK_OBJECT(_guide));
        _guide = 0;
        _dragging = false;

        if ( (_horiz_f ? wy : wx ) >= 0 ) {
            Inkscape::XML::Document *xml_doc = _dt->doc()->getReprDoc();
            Inkscape::XML::Node *repr = xml_doc->createElement("sodipodi:guide");
            repr->setAttribute("orientation", _horiz_f ? "horizontal" : "vertical");
            double const guide_pos_dt = event_dt[ _horiz_f ? Geom::Y : Geom::X ];
            sp_repr_set_svg_double(repr, "position", guide_pos_dt);
            _dt->namedview->getRepr()->appendChild(repr);
            Inkscape::GC::release(repr);
            DocumentUndo::done(sp_desktop_document(_dt), SP_VERB_NONE, 
                               /* TODO: annotate */ "ruler.cpp:157");
        }
        _dt->set_coordinate_status(event_dt);
    }
    return false;
}

//------------------------------------------------------------
HRuler::HRuler()
{
    _dt = 0;
    _r = static_cast<Gtk::Deprecated::HRuler*>(Glib::wrap(static_cast<GtkWidget*> (sp_ruler_new(GTK_ORIENTATION_HORIZONTAL))));
    add(*_r);
    _horiz_f = true;
}

HRuler::~HRuler()
{
}


VRuler::VRuler()
{
    _dt = 0;
    _r = static_cast<Gtk::Deprecated::VRuler*>(Glib::wrap(static_cast<GtkWidget*> (sp_ruler_new(GTK_ORIENTATION_VERTICAL))));
    add(*_r);
    _horiz_f = false;
}

VRuler::~VRuler()
{
}

}}}

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
