/**
 *  \file gtk_Layout.cpp
 */

#include "gtk_Layout.hpp"

#include "gtk_Adjustment.hpp"

#include <gtk/gtk.h>

/*#
   @beginmodule gtk
*/

namespace Falcon {
namespace Gtk {

/**
 *  \brief module init
 */
void Layout::modInit( Falcon::Module* mod )
{
    Falcon::Symbol* c_Layout = mod->addClass( "GtkLayout", &Layout::init );

    Falcon::InheritDef* in = new Falcon::InheritDef( mod->findGlobalSymbol( "GtkContainer" ) );
    c_Layout->getClassDef()->addInheritance( in );

    c_Layout->getClassDef()->factory( &Layout::factory );

    Gtk::MethodTab methods[] =
    {
    { "put",            Layout::put },
    { "move",           Layout::move },
    { "set_size",       Layout::set_size },
    { "get_size",       Layout::get_size },
    { "get_hadjustment",Layout::get_hadjustment },
    { "get_vadjustment",Layout::get_vadjustment },
    { "set_hadjustment",Layout::set_hadjustment },
    { "set_vadjustment",Layout::set_vadjustment },
    //{ "get_bin_window", Layout::get_bin_window },
    { NULL, NULL }
    };

    for ( Gtk::MethodTab* meth = methods; meth->name; ++meth )
        mod->addClassMethod( c_Layout, meth->name, meth->cb );
}


Layout::Layout( const Falcon::CoreClass* gen, const GtkLayout* lyt )
    :
    Gtk::CoreGObject( gen, (GObject*) lyt )
{}


Falcon::CoreObject* Layout::factory( const Falcon::CoreClass* gen, void* lyt, bool )
{
    return new Layout( gen, (GtkLayout*) lyt );
}


/*#
    @class GtkLayout
    @brief Infinite scrollable area containing child widgets and/or custom drawing
    @optparam hadjustment (GtkAdjustment) horizontal scroll adjustment
    @optparam vadjustment (GtkAdjustment) vertical scroll adjustment

    GtkLayout is similar to GtkDrawingArea in that it's a "blank slate" and doesn't
    do anything but paint a blank background by default. It's different in that it
    supports scrolling natively (you can add it to a GtkScrolledWindow), and it can
    contain child widgets, since it's a GtkContainer. However if you're just going
    to draw, a GtkDrawingArea is a better choice since it has lower overhead.

    When handling expose events on a GtkLayout, you must draw to
    GTK_LAYOUT (layout)->bin_window, rather than to GTK_WIDGET (layout)->window,
    as you would for a drawing area.
 */
FALCON_FUNC Layout::init( VMARG )
{
    Gtk::ArgCheck0 args( vm, "[GtkAdjustment,GtkAdjustment]" );

    CoreGObject* o_hadj = args.getCoreGObject( 0, false );
#ifndef NO_PARAMETER_CHECK
    if ( o_hadj && !CoreObject_IS_DERIVED( o_hadj, GtkAdjustment ) )
            throw_inv_params( "[GtkAdjustment,GtkAdjustment]" );
#endif
    GtkAdjustment* hadj = (GtkAdjustment*) o_hadj->getObject();

    CoreGObject* o_vadj = args.getCoreGObject( 1, false );
#ifndef NO_PARAMETER_CHECK
    if ( o_vadj && !CoreObject_IS_DERIVED( o_vadj, GtkAdjustment ) )
            throw_inv_params( "[GtkAdjustment,GtkAdjustment]" );
#endif
    GtkAdjustment* vadj = (GtkAdjustment*) o_vadj->getObject();

    MYSELF;
    GtkWidget* wdt = gtk_layout_new( hadj, vadj );
    self->setObject( (GObject*) wdt );
}


/*#
    @method put GtkLayout
    @brief Adds child_widget to layout, at position (x,y). layout becomes the new parent container of child_widget.
    @param child (GtkWidget)
    @param x X position of child widget
    @param y Y position of child widget
 */
FALCON_FUNC Layout::put( VMARG )
{
    Gtk::ArgCheck0 args( vm, "GtkWidget,I,I" );

    CoreGObject* o_wdt = args.getCoreGObject( 0 );
#ifndef NO_PARAMETER_CHECK
    if ( o_wdt && !CoreObject_IS_DERIVED( o_wdt, GtkWidget ) )
        throw_inv_params( "GtkWidget,I,I" );
#endif
    GtkWidget* wdt = (GtkWidget*) o_wdt->getObject();

    gint x = args.getInteger( 1 );
    gint y = args.getInteger( 2 );

    MYSELF;
    GET_OBJ( self );
    gtk_layout_put( (GtkLayout*)_obj, wdt, x, y );
}


/*#
    @method move GtkLayout
    @brief Moves a current child of layout to a new position.
    @param child_widget a current child of layout
    @param x X position to move to
    @param y Y position to move to
 */
FALCON_FUNC Layout::move( VMARG )
{
    Gtk::ArgCheck0 args( vm, "GtkWidget,I,I" );

    CoreGObject* o_wdt = args.getCoreGObject( 0 );
#ifndef NO_PARAMETER_CHECK
    if ( o_wdt && !CoreObject_IS_DERIVED( o_wdt, GtkWidget ) )
        throw_inv_params( "GtkWidget,I,I" );
#endif
    GtkWidget* wdt = (GtkWidget*) o_wdt->getObject();

    gint x = args.getInteger( 1 );
    gint y = args.getInteger( 2 );

    MYSELF;
    GET_OBJ( self );
    gtk_layout_move( (GtkLayout*)_obj, wdt, x, y );
}


/*#
    @method set_size GtkLayout
    @brief Sets the size of the scrollable area of the layout.
    @param width width of entire scrollable area
    @param height height of entire scrollable area
 */
FALCON_FUNC Layout::set_size( VMARG )
{
    Gtk::ArgCheck0 args( vm, "I,I" );

    guint width = args.getInteger( 0 );
    guint height = args.getInteger( 1 );

    MYSELF;
    GET_OBJ( self );
    gtk_layout_set_size( (GtkLayout*)_obj, width, height );
}


/*#
    @method get_size GtkLayout
    @brief Gets the size that has been set on the layout, and that determines the total extents of the layout's scrollbar area.
    @return [ width, height ]
 */
FALCON_FUNC Layout::get_size( VMARG )
{
    NO_ARGS
    guint width, height;
    MYSELF;
    GET_OBJ( self );
    gtk_layout_get_size( (GtkLayout*)_obj, &width, &height );
    CoreArray* arr = new CoreArray( 2 );
    arr->append( width );
    arr->append( height );
    vm->retval( arr );
}


/*#
    @method get_hadjustment GtkLayout
    @brief Returns the GtkAdjustment used for communication between the horizontal scrollbar and layout.
    @return horizontal scroll adjustment

    This function should only be called after the layout has been placed in a
    GtkScrolledWindow or otherwise configured for scrolling. It returns the
    GtkAdjustment used for communication between the horizontal scrollbar and layout.

    See GtkScrolledWindow, GtkScrollbar, GtkAdjustment for details.
 */
FALCON_FUNC Layout::get_hadjustment( VMARG )
{
    NO_ARGS
    MYSELF;
    GET_OBJ( self );
    GtkAdjustment* adj = gtk_layout_get_hadjustment( (GtkLayout*)_obj );
    vm->retval( new Gtk::Adjustment( vm->findWKI( "GtkAdjustment" )->asClass(), adj ) );
}


/*#
    @method get_vadjustment GtkLayout
    @brief Returns the GtkAdjustment used for communication between the vertical scrollbar and layout.
    @return vertical scroll adjustment

    This function should only be called after the layout has been placed in a
    GtkScrolledWindow or otherwise configured for scrolling. It returns the
    GtkAdjustment used for communication between the vertical scrollbar and layout.

    See GtkScrolledWindow, GtkScrollbar, GtkAdjustment for details.
 */
FALCON_FUNC Layout::get_vadjustment( VMARG )
{
    NO_ARGS
    MYSELF;
    GET_OBJ( self );
    GtkAdjustment* adj = gtk_layout_get_vadjustment( (GtkLayout*)_obj );
    vm->retval( new Gtk::Adjustment( vm->findWKI( "GtkAdjustment" )->asClass(), adj ) );
}


/*#
    @method set_hadjustment GtkLayout
    @brief Sets the horizontal scroll adjustment for the layout.
    @param adjustment new scroll adjustment (or nil)

    See GtkScrolledWindow, GtkScrollbar, GtkAdjustment for details.
 */
FALCON_FUNC Layout::set_hadjustment( VMARG )
{
    Item* i_adj = vm->param( 0 );
    // this method accepts nil
#ifndef NO_PARAMETER_CHECK
    if ( i_adj && ( i_adj->isNil() || !i_adj->isObject()
        || !IS_DERIVED( i_adj, GtkAdjustment ) ) )
        throw_inv_params( "GtkAdjustment" );
#endif
    GtkAdjustment* adj = NULL;
    if ( i_adj )
        adj = (GtkAdjustment*) COREGOBJECT( i_adj )->getObject();
    MYSELF;
    GET_OBJ( self );
    gtk_layout_set_hadjustment( (GtkLayout*)_obj, adj );
}


/*#
    @method set_vadjustment GtkLayout
    @brief Sets the vertical scroll adjustment for the layout.
    @param adjustment new scroll adjustment (or nil)
    See GtkScrolledWindow, GtkScrollbar, GtkAdjustment for details.
 */
FALCON_FUNC Layout::set_vadjustment( VMARG )
{
    Item* i_adj = vm->param( 0 );
    // this method accepts nil
#ifndef NO_PARAMETER_CHECK
    if ( i_adj && ( i_adj->isNil() || !i_adj->isObject()
        || !IS_DERIVED( i_adj, GtkAdjustment ) ) )
        throw_inv_params( "GtkAdjustment" );
#endif
    GtkAdjustment* adj = NULL;
    if ( i_adj )
        adj = (GtkAdjustment*) COREGOBJECT( i_adj )->getObject();
    MYSELF;
    GET_OBJ( self );
    gtk_layout_set_vadjustment( (GtkLayout*)_obj, adj );
}


//FALCON_FUNC Layout::get_bin_window( VMARG );


} // Gtk
} // Falcon

// vi: set ai et sw=4:
// kate: replace-tabs on; shift-width 4;
