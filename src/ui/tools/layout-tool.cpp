/*
 * Our nice measuring tool
 *
 * Authors:
 *   Felipe Correa da Silva Sanches <juca@members.fsf.org>
 *   Jon A. Cruz <jon@joncruz.org>
 *
 * Copyright (C) 2011 Authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */


#include <gdk/gdkkeysyms.h>
#include <boost/none_t.hpp>
#include "util/units.h"
#include "macros.h"
#include "display/curve.h"
#include "sp-shape.h"
#include "sp-text.h"
#include "sp-flowtext.h"
#include "text-editing.h"
#include "display/sp-ctrlline.h"
#include "display/sodipodi-ctrl.h"
#include "display/sp-canvas-item.h"
#include "display/sp-canvas-util.h"
#include "desktop.h"
#include "document.h"
#include "preferences.h"
#include "inkscape.h"
#include "desktop-handles.h"
#include "layout-tool.h"
#include "ui/tools/freehand-base.h"
#include "display/canvas-text.h"
#include "path-chemistry.h"
#include "2geom/line.h"
#include <2geom/path-intersection.h>
#include <2geom/pathvector.h>
#include <2geom/crossing.h>
#include <2geom/angle.h>
#include "snap.h"
#include "sp-namedview.h"
#include "enums.h"
#include "ui/control-manager.h"
#include "knot-enums.h"

using Inkscape::ControlManager;
using Inkscape::CTLINE_SECONDARY;
using Inkscape::Util::unit_table;

#include "tool-factory.h"

namespace Inkscape {
namespace UI {
namespace Tools {

std::vector<Inkscape::Display::TemporaryItem*> layout_tmp_items;

namespace {
	ToolBase* createLayoutContext() {
		return new LayoutTool();
	}

	bool layoutContextRegistered = ToolFactory::instance().registerObject("/tools/layout", createLayoutContext);
}

const std::string& LayoutTool::getPrefsPath() {
	return LayoutTool::prefsPath;
}

const std::string LayoutTool::prefsPath = "/tools/layout";


LayoutTool::LayoutTool()
    : ToolBase(NULL, 4, 4)
    , grabbed(NULL)
{
}

LayoutTool::~LayoutTool() {
}

void LayoutTool::finish() {
    this->enableGrDrag(false);

    if (this->grabbed) {
        sp_canvas_item_ungrab(this->grabbed, GDK_CURRENT_TIME);
        this->grabbed = NULL;
    }

    ToolBase::finish();
}



bool LayoutTool::root_handler(GdkEvent* event) {
    Inkscape::Preferences *prefs = Inkscape::Preferences::get();
    tolerance = prefs->getIntLimited("/options/dragtolerance/value", 0, 0, 100);
//
    gint ret = FALSE;

    switch (event->type) {
        case GDK_BUTTON_PRESS: {
                ret = TRUE;
               
               //  draw main control line
               // {
					Geom::Point start_point1(0.0,525.0);
					Geom::Point end_point1(750.0,525.0);
                    SPCtrlLine *control_line = ControlManager::getManager().createControlLine(sp_desktop_tempgroup(desktop),
                                                                                              start_point1,
                                                                                              end_point1);
                    layout_tmp_items.push_back(desktop->add_temporary_canvasitem(control_line, 0));
					Geom::Point start_point2(375.0, 0.0);
					Geom::Point end_point2(375.0,1050.0);
					SPCtrlLine *control_line1 = ControlManager::getManager().createControlLine(sp_desktop_tempgroup(desktop),
                                                                                              start_point2,
                                                                                              end_point2);
                    layout_tmp_items.push_back(desktop->add_temporary_canvasitem(control_line1, 0));
           

                gobble_motion_events(GDK_BUTTON1_MASK);
           // }
            break;
        }
        case GDK_BUTTON_RELEASE: {
            sp_event_context_discard_delayed_snap_event(this);
            explicitBase = boost::none;
            lastEnd = boost::none;

            //clear all temporary canvas items related to the layoutment tool.
            for (size_t idx = 0; idx < layout_tmp_items.size(); ++idx) {
                desktop->remove_temporary_canvasitem(layout_tmp_items[idx]);
            }

            layout_tmp_items.clear();

            if (this->grabbed) {
                sp_canvas_item_ungrab(this->grabbed, event->button.time);
                this->grabbed = NULL;
            }

            xp = 0;
            yp = 0;
            break;
        }
        default:
            break;
    }

    if (!ret) {
    	ret = ToolBase::root_handler(event);
    }

    return ret;
}

}
}
}
/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :