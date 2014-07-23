#ifndef SEEN_SP_LAYOUT_CONTEXT_H
#define SEEN_SP_LAYOUT_CONTEXT_H

/*
 * Our fine layout tool
 *
 * Authors:
 *   Felipe Correa da Silva Sanches <juca@members.fsf.org>
 *
 * Copyright (C) 2011 Authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include "tool-base.h"
#include <2geom/point.h>
#include <boost/optional.hpp>

#define SP_LAYOUT_CONTEXT(obj) (dynamic_cast<Inkscape::UI::Tools::LayoutTool*>((Inkscape::UI::Tools::ToolBase*)obj))
#define SP_IS_LAYOUT_CONTEXT(obj) (dynamic_cast<const Inkscape::UI::Tools::LayoutTool*>((const Inkscape::UI::Tools::ToolBase*)obj) != NULL)

namespace Inkscape {
namespace UI {
namespace Tools {

class LayoutTool : public ToolBase {
public:
	LayoutTool();
	virtual ~LayoutTool();

	static const std::string prefsPath;

	virtual void finish();
	virtual bool root_handler(GdkEvent* event);

	virtual const std::string& getPrefsPath();

private:
	SPCanvasItem* grabbed;

    Geom::Point start_point;
    boost::optional<Geom::Point> explicitBase;
    boost::optional<Geom::Point> lastEnd;
};

}
}
}

#endif // SEEN_SP_LayoutTool_CONTEXT_H
