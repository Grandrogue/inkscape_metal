/** @file
 * @brief SVG flood filter effect
 *//*
 * Authors:
 *   Hugo Rodrigues <haa.rodrigues@gmail.com>
 *
 * Copyright (C) 2006 Hugo Rodrigues
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifndef SP_FEFLOOD_H_SEEN
#define SP_FEFLOOD_H_SEEN

#include "sp-filter-primitive.h"
#include "svg/svg-icc-color.h"

G_BEGIN_DECLS

#define SP_TYPE_FEFLOOD            (sp_feFlood_get_type())
#define SP_FEFLOOD(obj) ((SPFeFlood*)obj)
#define SP_IS_FEFLOOD(obj) (obj != NULL && static_cast<const SPObject*>(obj)->typeHierarchy.count(typeid(SPFeFlood)))

class CFeFlood;

class SPFeFlood : public SPFilterPrimitive {
public:
	CFeFlood* cfeflood;

    guint32 color;
    SVGICCColor *icc;
    double opacity;
};

struct SPFeFloodClass {
    SPFilterPrimitiveClass parent_class;
};

GType sp_feFlood_get_type() G_GNUC_CONST;

G_END_DECLS
class CFeFlood : public CFilterPrimitive {
public:
	CFeFlood(SPFeFlood* flood);
	virtual ~CFeFlood();

	virtual void build(SPDocument* doc, Inkscape::XML::Node* repr);
	virtual void release();

	virtual void set(unsigned int key, const gchar* value);

	virtual void update(SPCtx* ctx, unsigned int flags);

	virtual Inkscape::XML::Node* write(Inkscape::XML::Document* doc, Inkscape::XML::Node* repr, guint flags);

	virtual void build_renderer(Inkscape::Filters::Filter* filter);

private:
	SPFeFlood* spfeflood;
};

GType sp_feFlood_get_type();

#endif /* !SP_FEFLOOD_H_SEEN */

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
