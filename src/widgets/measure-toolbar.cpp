/**
 * @file
 * Measure aux toolbar
 */
/* Authors:
 *   MenTaLguY <mental@rydia.net>
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   bulia byak <buliabyak@users.sf.net>
 *   Frank Felfe <innerspace@iname.com>
 *   John Cliff <simarilius@yahoo.com>
 *   David Turner <novalis@gnu.org>
 *   Josh Andler <scislac@scislac.com>
 *   Jon A. Cruz <jon@joncruz.org>
 *   Maximilian Albert <maximilian.albert@gmail.com>
 *   Tavmjong Bah <tavmjong@free.fr>
 *   Abhishek Sharma
 *   Kris De Gussem <Kris.DeGussem@gmail.com>
 *
 * Copyright (C) 2004 David Turner
 * Copyright (C) 2003 MenTaLguY
 * Copyright (C) 1999-2011 authors
 * Copyright (C) 2001-2002 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <glibmm/i18n.h>

#include "measure-toolbar.h"
#include "desktop-handles.h"
#include "desktop.h"
#include "document-undo.h"
#include "ege-adjustment-action.h"
#include "ege-output-action.h"
#include "ege-select-one-action.h"
#include "ink-action.h"
#include "selection.h"
#include "ui/icon-names.h"
#include "preferences.h"
#include "toolbox.h"
#include "ui/widget/unit-tracker.h"

using Inkscape::UI::Widget::UnitTracker;
using Inkscape::Util::Unit;
using Inkscape::DocumentUndo;
using Inkscape::UI::ToolboxFactory;
using Inkscape::UI::PrefPusher;

//########################
//##  Measure Toolbox   ##
//########################

static void sp_stb_measure_mode_state_changed( EgeSelectOneAction *act, GObject *dataKludge )
{
    SPDesktop *desktop = static_cast<SPDesktop *>(g_object_get_data( dataKludge, "desktop" ));
    bool isUnFixed = ege_select_one_action_get_active( act ) == 0;

    if (DocumentUndo::getUndoSensitive(sp_desktop_document(desktop))) {
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();
        prefs->setBool( "/tools/measure/isunfixed", isUnFixed);
    }

    //// quit if run by the attr_changed listener
    if (g_object_get_data( dataKludge, "freeze" )) {
        return;
    }

    //// in turn, prevent listener from responding
    g_object_set_data( dataKludge, "freeze", GINT_TO_POINTER(TRUE) );

    GtkAction* mode_action = GTK_ACTION( g_object_get_data( dataKludge, "measure_coord_action_x" ) );

    if ( mode_action ) {
        gtk_action_set_sensitive( mode_action, !isUnFixed );
    }

    mode_action = GTK_ACTION( g_object_get_data( dataKludge, "measure_coord_action_y" ) );

	if ( mode_action ) {
        gtk_action_set_sensitive( mode_action, !isUnFixed );
    }

    g_object_set_data( dataKludge, "freeze", GINT_TO_POINTER(FALSE) );
}

static void sp_measure_measure_coord_action_x_value_changed(GtkAdjustment *adj, GObject *tbl)
{
    SPDesktop *desktop = static_cast<SPDesktop *>(g_object_get_data( tbl, "desktop" ));

    if (DocumentUndo::getUndoSensitive(sp_desktop_document(desktop))) {
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();
        prefs->setDouble(Glib::ustring("/tools/measure/measure_coord_action_x"),
            gtk_adjustment_get_value(adj));
    }
}

static void sp_measure_measure_coord_action_y_value_changed(GtkAdjustment *adj, GObject *tbl)
{
    SPDesktop *desktop = static_cast<SPDesktop *>(g_object_get_data( tbl, "desktop" ));

    if (DocumentUndo::getUndoSensitive(sp_desktop_document(desktop))) {
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();
        prefs->setDouble(Glib::ustring("/tools/measure/measure_coord_action_y"),
            gtk_adjustment_get_value(adj));
    }
}

static void sp_measure_fontsize_value_changed(GtkAdjustment *adj, GObject *tbl)
{
    SPDesktop *desktop = static_cast<SPDesktop *>(g_object_get_data( tbl, "desktop" ));

    if (DocumentUndo::getUndoSensitive(sp_desktop_document(desktop))) {
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();
        prefs->setInt(Glib::ustring("/tools/measure/fontsize"),
            gtk_adjustment_get_value(adj));
    }
}

static void measure_unit_changed(GtkAction* /*act*/, GObject* tbl)
{
    UnitTracker* tracker = reinterpret_cast<UnitTracker*>(g_object_get_data(tbl, "tracker"));
    Glib::ustring const unit = tracker->getActiveUnit()->abbr;
    Inkscape::Preferences *prefs = Inkscape::Preferences::get();
    prefs->setString("/tools/measure/unit", unit);
}

void sp_measure_toolbox_prep(SPDesktop * desktop, GtkActionGroup* mainActions, GObject* holder)
{
    UnitTracker* tracker = new UnitTracker(Inkscape::Util::UNIT_TYPE_LINEAR);
    Inkscape::Preferences *prefs = Inkscape::Preferences::get();
    tracker->setActiveUnitByAbbr(prefs->getString("/tools/measure/unit").c_str());
    
    g_object_set_data( holder, "tracker", tracker );

    EgeAdjustmentAction *eact = 0;

    Inkscape::IconSize secondarySize = ToolboxFactory::prefToSize("/toolbox/secondary", 1);
    bool isUnFixed = prefs->getBool("/tools/measure/isunfixed", true);

    /* Measure mode checkbox */
    {
        GtkListStore* model = gtk_list_store_new( 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING );

        GtkTreeIter iter;
        gtk_list_store_append( model, &iter );
        gtk_list_store_set( model, &iter,
                            0, _("Unfixed measure tool"),
                            1, _("Mode with unfixed start point"),
                            2, INKSCAPE_ICON("object-unlocked"),
                            -1 );

        gtk_list_store_append( model, &iter );
        gtk_list_store_set( model, &iter,
                            0, _("Fixed measure tool"),
                            1, _("Mode with fixed start point"),
                            2, INKSCAPE_ICON("object-locked"),
                            -1 );

        EgeSelectOneAction* act = ege_select_one_action_new( "MeasureModeAction", (""), (""), NULL, GTK_TREE_MODEL(model) );
        gtk_action_group_add_action( mainActions, GTK_ACTION(act) );
        g_object_set_data( holder, "measure_mode_action", act );

        ege_select_one_action_set_appearance( act, "full" );
        ege_select_one_action_set_radio_action_type( act, INK_RADIO_ACTION_TYPE );
        g_object_set( G_OBJECT(act), "icon-property", "iconId", NULL );
        ege_select_one_action_set_icon_column( act, 2 );
        ege_select_one_action_set_icon_size( act, secondarySize );
        ege_select_one_action_set_tooltip_column( act, 1  );

        ege_select_one_action_set_active( act, isUnFixed ? 0 : 1 );
        g_signal_connect_after( G_OBJECT(act), "changed", G_CALLBACK(sp_stb_measure_mode_state_changed), holder);
    }

    /* Coord X*/
    {
        eact = create_adjustment_action( "MeasureCoordActionX",
                                         _("X position"), _("X position:"),
                                         _("X position of the measurement tool"),
                                         "/tools/measure/measure_coord_action_x", 0.0,
                                         GTK_WIDGET(desktop->canvas), holder, FALSE, NULL,
                                         -1e6, 1e6, 1.0, 4.0,
                                         0, 0, 0,
                                         sp_measure_measure_coord_action_x_value_changed);
        gtk_action_group_add_action( mainActions, GTK_ACTION(eact) );
        g_object_set_data( holder, "measure_coord_action_x", eact );
    }

    if ( isUnFixed ) {
        gtk_action_set_sensitive( GTK_ACTION(eact), FALSE );
    } else {
        gtk_action_set_sensitive( GTK_ACTION(eact), TRUE );
    }

    /* Coord Y*/
    {
        eact = create_adjustment_action( "MeasureCoordActionY",
                                         _("Y position"), _("Y position:"),
                                         _("Y position of the measurement tool"),
                                         "/tools/measure/measure_coord_action_y", 0.0,
                                         GTK_WIDGET(desktop->canvas), holder, FALSE, NULL,
                                         -1e6, 1e6, 1.0, 4.0,
                                         0, 0, 0,
                                         sp_measure_measure_coord_action_y_value_changed);
        gtk_action_group_add_action( mainActions, GTK_ACTION(eact) );
        g_object_set_data( holder, "measure_coord_action_y", eact );
    }

    if ( isUnFixed ) {
        gtk_action_set_sensitive( GTK_ACTION(eact), FALSE );
    } else {
        gtk_action_set_sensitive( GTK_ACTION(eact), TRUE );
    }

    /* Font Size */
    {
        eact = create_adjustment_action( "MeasureFontSizeAction",
                                         _("Font Size"), _("Font Size:"),
                                         _("The font size to be used in the measurement labels"),
                                         "/tools/measure/fontsize", 0.0,
                                         GTK_WIDGET(desktop->canvas), holder, FALSE, NULL,
                                         10, 36, 1.0, 4.0,
                                         0, 0, 0,
                                         sp_measure_fontsize_value_changed);
        gtk_action_group_add_action( mainActions, GTK_ACTION(eact) );
    }

    // units label
    {
        EgeOutputAction* act = ege_output_action_new( "measure_units_label", _("Units:"), _("The units to be used for the measurements"), 0 );
        ege_output_action_set_use_markup( act, TRUE );
        g_object_set( act, "visible-overflown", FALSE, NULL );
        gtk_action_group_add_action( mainActions, GTK_ACTION( act ) );
    }

    // units menu
    {
        GtkAction* act = tracker->createAction( "MeasureUnitsAction", _("Units:"), _("The units to be used for the measurements") );
        g_signal_connect_after( G_OBJECT(act), "changed", G_CALLBACK(measure_unit_changed), holder );
        gtk_action_group_add_action( mainActions, act );
    }
} // end of sp_measure_toolbox_prep()


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
