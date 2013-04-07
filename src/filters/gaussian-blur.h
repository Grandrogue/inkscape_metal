/** @file
 * @brief SVG Gaussian blur filter effect
 *//*
 * Authors:
 *   Hugo Rodrigues <haa.rodrigues@gmail.com>
 *
 * Copyright (C) 2006 Hugo Rodrigues
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifndef SP_GAUSSIANBLUR_H_SEEN
#define SP_GAUSSIANBLUR_H_SEEN

#include "sp-filter-primitive.h"
#include "number-opt-number.h"

#define SP_GAUSSIANBLUR(obj) ((SPGaussianBlur*)obj)
#define SP_IS_GAUSSIANBLUR(obj) (dynamic_cast<const SPGaussianBlur*>((SPObject*)obj))

class SPGaussianBlur : public SPFilterPrimitive {
public:
	SPGaussianBlur();
	virtual ~SPGaussianBlur();

    /** stdDeviation attribute */
    NumberOptNumber stdDeviation;

	virtual void build(SPDocument* doc, Inkscape::XML::Node* repr);
	virtual void release();

	virtual void set(unsigned int key, const gchar* value);

	virtual void update(SPCtx* ctx, unsigned int flags);

	virtual Inkscape::XML::Node* write(Inkscape::XML::Document* doc, Inkscape::XML::Node* repr, guint flags);

	virtual void build_renderer(Inkscape::Filters::Filter* filter);
};

void  sp_gaussianBlur_setDeviation(SPGaussianBlur *blur, float num);
void  sp_gaussianBlur_setDeviation(SPGaussianBlur *blur, float num, float optnum);

#endif /* !SP_GAUSSIANBLUR_H_SEEN */

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
