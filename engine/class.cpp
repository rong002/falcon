/*
   FALCON - The Falcon Programming Language.
   FILE: class.cpp

   Class definition of a Falcon Class
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: Thu, 06 Jan 2011 15:01:08 +0100

   -------------------------------------------------------------------
   (C) Copyright 2011: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

#undef SRC
#define SRC "engine/class.cpp"

#include <falcon/trace.h>
#include <falcon/module.h>
#include <falcon/class.h>
#include <falcon/itemid.h>
#include <falcon/vmcontext.h>
#include <falcon/bom.h>
#include <falcon/error.h>
#include <falcon/errors/operanderror.h>
#include <falcon/errors/unserializableerror.h>
#include <falcon/errors/accesserror.h>
#include <falcon/errors/accesstypeerror.h>
#include <falcon/stdhandlers.h>
#include <falcon/extfunc.h>

#include <falcon/function.h>
#include <falcon/extfunc.h>

#include <map>

namespace Falcon {

static void setReadOnly( const Class*, const String& name, void *, const Item& )
{
   throw new AccessError( ErrorParam( e_prop_ro, __LINE__, SRC ).extra(name) );
}

static void getWriteOnly( const Class*, const String& name, void *, Item& )
{
   throw new AccessError( ErrorParam( e_prop_wo, __LINE__, SRC ).extra(name) );
}

class Property
{
public:
   Class::setter setFunc;
   Class::getter getFunc;

   Function* method;
   Item value;
   bool bHidden;
   bool bStatic;
   bool bConst;
   Property() {}

   Property( Class::getter getf, Class::setter setf, bool s, bool h, bool c, Function* meth, Item& v )
   {
      set( getf, setf, s, h, c, meth, v );
   }
   ~Property() {}

   void set( Class::getter getf, Class::setter setf, bool s, bool h, bool c, Function* meth, const Item& v )
   {
      if ( getf == 0 )
      {
         h = true;
         getf = &getWriteOnly;
      }

      getFunc = getf;
      setFunc = setf == 0 ? setReadOnly : setf;

      bStatic = s;
      bHidden = h;
      bConst = c;
      method = meth;
      value = v;
   }
};


class Class::Private
{
public:
   typedef std::map<String, Property> PropertyMap;
   PropertyMap m_props;

   Function* m_constructor;

   Private():
      m_constructor(0)
   {}

   ~Private() {
   }
};


Class::Class( const String& name ):
   Mantra( name, 0, 0, 0 ),
   m_bIsfalconClass( false ),
   m_bIsErrorClass( false ),
   m_bIsFlatInstance(false),
   m_userFlags(0),
   m_typeID( FLC_ITEM_USER ),
   m_clearPriority( 0 )
{
   m_category = e_c_class;
   _p = new Private;
   m_parent = 0;
}


Class::Class( const String& name, int64 tid ):
   Mantra( name, 0, 0, 0 ),
   m_bIsfalconClass( false ),
   m_bIsErrorClass( false ),
   m_bIsFlatInstance(false),
   m_userFlags(0),
   m_typeID( tid ),
   m_clearPriority( 0 )
{
   m_category = e_c_class;
   _p = new Private;
   m_parent = 0;
}


Class::Class( const String& name, Module* module, int line, int chr ):
   Mantra( name, module, line, chr ),
   m_bIsfalconClass( false ),
   m_bIsErrorClass( false ),
   m_bIsFlatInstance(false),
   m_userFlags(0),
   m_typeID( FLC_ITEM_USER ),
   m_clearPriority( 0 )
{
   m_category = e_c_class;
   _p = new Private;
   m_parent = 0;
}


Class::Class( const String& name, int64 tid, Module* module, int line, int chr ):
   Mantra( name, module, line, chr ),
   m_bIsfalconClass( false ),
   m_bIsErrorClass( false ),
   m_bIsFlatInstance(false),
   m_userFlags(0),
   m_typeID( tid ),
   m_clearPriority( 0 )
{
   m_category = e_c_class;
   _p = new Private;
   m_parent = 0;
}


Class::~Class()
{
   TRACE1( "Destroying class %s.%s",
      m_module != 0 ? m_module->name().c_ize() : "<internal>",
      m_name.c_ize() );

   Private::PropertyMap::iterator iter = _p->m_props.begin();
   while( _p->m_props.end() != iter )
   {
      Property& prop = iter->second;
      if( prop.method != 0 && prop.method->methodOf() == this )
      {
         delete prop.method;
      }
      ++iter;
   }
   delete _p;
}


Class* Class::handler() const
{
   static Class* meta = Engine::handlers()->metaClass();
   return meta;
}

Class* Class::getParent( const String& name ) const
{
   if( m_parent == 0 || m_parent->name() != name ) { 
      return 0;
   }
   return m_parent;
}


bool Class::isDerivedFrom( const Class* cls ) const
{
   return this == cls || (m_parent != 0 && cls->isDerivedFrom(m_parent));
}


void Class::enumerateParents( Class::ClassEnumerator& pe ) const
{
   if( m_parent != 0 ) { pe(m_parent); }
}

void* Class::getParentData( const Class* parent, void* data ) const
{
   if( parent == this ) return data;
   if( m_parent != 0 ) return m_parent->getParentData( parent, data );
   return 0;
}

int64 Class::occupiedMemory( void* ) const
{
   return 0;
}
 
void Class::store( VMContext*, DataWriter*, void* ) const
{
      throw new UnserializableError(ErrorParam( e_unserializable, __LINE__, __FILE__ )
      .origin( ErrorParam::e_orig_vm )
      .extra(name() + " unsupported store"));
}


void Class::restore( VMContext*, DataReader*) const
{
   throw new UnserializableError(ErrorParam( e_unserializable, __LINE__, __FILE__ )
      .origin( ErrorParam::e_orig_vm )
      .extra(name() + " unsupported restore"));
}


void Class::flatten( VMContext*, ItemArray&, void* ) const
{
   // normally does nothing
}


void Class::unflatten( VMContext*, ItemArray&, void* ) const
{
   // normally does nothing
}


void Class::gcMarkInstance( void*, uint32 ) const
{
   // normally does nothing
}


bool Class::gcCheckInstance( void*, uint32 ) const
{
   return true;
}


void Class::describe( void* instance, String& target, int depth, int maxlen) const
{
   String temp;
   
   target.reserve(128);
   target.size(0);
   
   target.append("Class " );
   target.append(name());

   if( depth == 0 )
   {
       target += "{...}";
   }
   else
   {
      Private::PropertyMap::const_iterator iter = _p->m_props.begin();
      bool bFirst = true;
      
      target += '{';
      
      while( iter != _p->m_props.end() )
      {
         const Property* prop = &iter->second;
         if( ! prop->bHidden )
         {
            Item value;
            prop->getFunc( this, iter->first, instance, value );

            if( ! (value.isFunction() || value.isMethod()) )
            {
               if( bFirst )
               {
                  bFirst = false;
               }
               else
               {
                  target += ','; target += ' ';
               }

               value.describe( temp, depth-1, maxlen );
               target.append( iter->first );
               target.append('=');
               target.append(temp);
               temp.size(0);
            }
         }
         
         ++iter;
      }
            
      target += '}';
   }
}


void Class::enumerateProperties( void*, Class::PropertyEnumerator& pe ) const
{
   Private::PropertyMap::iterator iter = _p->m_props.begin();
   while( iter != _p->m_props.end() )
   {
      pe( iter->first );
      ++iter;
   }
}


void Class::enumeratePV( void* inst, Class::PVEnumerator& pve) const
{
   Private::PropertyMap::iterator iter = _p->m_props.begin();
   while( iter != _p->m_props.end() )
   {
      Property& prop = iter->second;
      if( prop.bConst )
      {
         pve( iter->first, prop.value );
      }
      else if( prop.getFunc != 0 ) 
      {
         Item temp;
         prop.getFunc(this, iter->first, inst, temp );
         pve( iter->first, temp );
      }
      ++iter;
   }
}


bool Class::hasProperty( void*, const String& prop ) const
{
   Private::PropertyMap::iterator iter = _p->m_props.find( prop );
   return iter != _p->m_props.end();
}


//==========================================================
// Property management
//==========================================================

void Class::addProperty( const String& name, getter get, setter set, bool isStatic, bool isHidden )
{
   _p->m_props[name].set(get, set, isStatic, isHidden, false, 0, Item() );
}


void Class::addMethod( Function* func, bool isStatic )
{
   Private::PropertyMap::iterator pos = _p->m_props.find( func->name() ); 
   
   if( pos != _p->m_props.end() )
   {
      Property& prop = pos->second;
      if( prop.method != 0 && prop.method->methodOf() == this )
      {
         delete prop.method;
      }

      prop.set( 0, 0, isStatic, true, true, func, Item() );
   }
   else {
      _p->m_props[func->name()].set(0, 0, isStatic, true, true, func, Item() );
   }

   if( func->methodOf() == 0 )
   {
      func->methodOf(this);
   }
}


void Class::addMethod( const String& name, ext_func_t func, const String& prototype, bool isStatic )
{
   Function* f = new ExtFunc(name, func, 0, 0);
   f->parseDescription( prototype );
   addMethod( f, isStatic );
}

void Class::setConstuctor( Function* func )
{
   delete _p->m_constructor;
   _p->m_constructor = func;
   func->methodOf( this );   
}

 
void Class::setConstuctor( const String& name, ext_func_t func, const String& prototype )
{
   Function* f = new ExtFunc(name, func, 0, 0);
   f->parseDescription( prototype );
   setConstuctor( f );
}


void Class::addConstant( const String& name, const Item& value )
{
   _p->m_props[name].set(0, 0, true, true, true, 0, value );
}


void Class::setParent( Class* parent )
{
   m_parent = parent;
   // copy all the properties here.
   Private::PropertyMap& props = parent->_p->m_props;
   Private::PropertyMap::iterator iter = props.begin();
   while( iter != props.end() )
   {
      Property& prop = iter->second;
      _p->m_props[iter->first] = prop; // use default copy
      ++iter;
   }
}

void Class::gcMark( uint32 mark )
{
   if( m_mark != mark )
   {
      Mantra::gcMark(mark);

      // copy all the properties here.
      Private::PropertyMap& props = _p->m_props;
      Private::PropertyMap::iterator iter = props.begin();
      while( iter != props.end() )
      {
         Property& prop = iter->second;
         prop.value.gcMark(mark); // this will include our mehtods.
         ++iter;
      }

      if( _p->m_constructor != 0 )
      {
         _p->m_constructor->gcMark(mark);
      }
   }
}


void Class::op_compare( VMContext* ctx, void* self ) const
{
   void* inst;
   Item *op1, *op2;
   
   ctx->operands( op1, op2 );
   
   if( op2->isUser() )
   {
      if( (inst = op2->asInst()) == self )
      {
         ctx->stackResult(2, 0 );
         return;
      }

      byte* bself = static_cast<byte*>(self);
      byte* bop2 = static_cast<byte*>(op2->asInst());

      ctx->stackResult(2, (int64)  (bself - bop2) );
      return;
   }

   // we have no information about what an item might be here, but we can
   // order the items by type
   ctx->stackResult(2, (int64) op1->type() - op2->type() );
}


void Class::onInheritanceResolved( ExprInherit* )
{
   // do nothing
}

//=====================================================================
// VM Operator override.
//

bool Class::op_init( VMContext* ctx, void* inst, int32 pCount ) const
{
   if( _p->m_constructor != 0 )
   {
      ctx->callInternal( _p->m_constructor, pCount, Item(this,inst) );
      return true;
   }

   throw FALCON_SIGN_XERROR( OperandError, e_invop, .extra("init") );
}


void Class::op_neg( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("neg") );
}

void Class::op_add( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("add") );
}

void Class::op_sub( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("sub") );
}


void Class::op_mul( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("mul") );
}


void Class::op_div( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("div") );
}


void Class::op_mod( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("mod") );
}


void Class::op_pow( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("pow") );
}

void Class::op_shr( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("shift right (>>)") );
}

void Class::op_shl( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
            .extra("shift left (<<)") );
}


void Class::op_aadd( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto add (+=)") );
}


void Class::op_asub( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto sub (-=)") );
}


void Class::op_amul( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto amul (-=)") );;
}


void Class::op_adiv( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto div (/=)") );
}


void Class::op_amod( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto mod (%=)") );
}


void Class::op_apow( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto pow (**=)") );
}

void Class::op_ashr( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto shr (>>=)") );
}

void Class::op_ashl( VMContext* ctx, void*  ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Auto shl (<<=)") );
}


void Class::op_inc( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Pre-inc (++x)") );
}


void Class::op_dec( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Pre-dec (--x)") );
}


void Class::op_incpost( VMContext* ctx, void*) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Post-inc (x++)") );
}


void Class::op_decpost( VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Post-dec (x--)") );
}


void Class::op_call( VMContext* ctx, int32 count , void* ) const
{
   ctx->popData(count);
}


void Class::op_getIndex(VMContext* ctx, void* ) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
               .extra("Get index []") );
}


void Class::op_setIndex(VMContext* ctx, void* ) const
{
   // TODO: IS it worth to add more infos about self in the error?
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
                .extra("Set index []=") );
}


void Class::op_getProperty( VMContext* ctx, void* data, const String& prop ) const
{
   static BOM* bom = Engine::instance()->getBom();

   Private::PropertyMap::iterator iter = _p->m_props.find( prop );
   if( iter != _p->m_props.end() )
   {
      Property& prop = iter->second;
      if( prop.method != 0 ) 
      {
         ctx->topData().setUser(this, data);
         ctx->topData().methodize(prop.method);
      }
      else if (prop.bConst )
      {
         ctx->topData() = prop.value;
      }
      else
      {
         prop.getFunc( this, iter->first, data, ctx->topData() );
      }

      return;
   }

   // try to find a valid BOM propery.
   BOM::handler handler = bom->get( prop );
   if ( handler != 0  )
   {
      handler( ctx, this, data );
   }
   else
   {
      FALCON_RESIGN_XERROR( AccessError, e_prop_acc, ctx,
                   .extra(prop) );
   }
}


void Class::op_setProperty( VMContext* ctx, void* data, const String& prop ) const
{
   Private::PropertyMap::iterator iter = _p->m_props.find( prop );
   if( iter != _p->m_props.end() )
   {
      Property& prop = iter->second;
      if( !prop.bConst )
      {
         ctx->popData();
         prop.setFunc( this, iter->first, data, ctx->topData() );
      }
      return;
   }
   FALCON_RESIGN_XERROR( AccessError, e_prop_acc, ctx,
                   .extra(prop) );
}


void Class::op_getClassProperty( VMContext* ctx, const String& prop) const
{
   static BOM* bom = Engine::instance()->getBom();

   Private::PropertyMap::iterator iter = _p->m_props.find( prop );
   if( iter != _p->m_props.end() && iter->second.bStatic )
   {
      Property& prop = iter->second;
      if( prop.method != 0 ) {
         ctx->topData() = prop.method;
      }
      else if ( prop.bConst )
      {
         ctx->topData() = prop.value;
      }
      else {
         prop.getFunc( this, iter->first, 0, ctx->topData() );
      }
      return;
   }

   // try to find a valid BOM propery.
   BOM::handler handler = bom->get( prop );
   if ( handler != 0  )
   {
      // all bom methods do not modify their object
      // we can safely use a const cast
      handler( ctx, this, const_cast<Class*>(this) );
   }
   else
   {
      FALCON_RESIGN_XERROR( AccessTypeError, e_prop_acc, ctx,
                   .extra(prop) );
   }
}


void Class::op_setClassProperty( VMContext* ctx, const String& prop ) const
{
   FALCON_RESIGN_XERROR( AccessError, e_prop_acc, ctx,
                .extra(prop) );
}

void Class::op_isTrue( VMContext* ctx, void* ) const
{
   ctx->topData().setBoolean(true);
}


void Class::op_in( VMContext* ctx, void*) const
{
   FALCON_RESIGN_XERROR( OperandError, e_invop, ctx,
                .extra("in") );
}

void Class::op_provides( VMContext* ctx, void* instance, const String& propName ) const
{
   ctx->topData().setBoolean( hasProperty(instance, propName ) );
}


void Class::op_toString( VMContext* ctx, void *self ) const
{
   String *descr = new String();
   describe( self, *descr );
   ctx->stackResult(1, FALCON_GC_HANDLE(descr));
}


void Class::op_iter( VMContext* ctx, void* ) const
{
   Item item;
   item.setBreak();
   ctx->pushData(item);
}

void Class::op_next( VMContext* ctx, void* ) const
{
   ctx->topData().setBreak();
}

}

/* end of class.cpp */