/*
   FALCON - The Falcon Programming Language.
   FILE: fal_testsuite.cpp

   Simple Falcon module for test suite.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: lun feb 13 2006

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Simple Falcon module for test suite.
*/

#include <falcon/config.h>
#include <falcon/module.h>
#include <falcon/vm.h>
#include <falcon/item.h>
#include <falcon/string.h>
#include <falcon/testsuite.h>
#include <falcon/fstream.h>
#include <falcon/vmevent.h>

/************************************************
   Success and failure falcon functions
*************************************************/

static Falcon::String s_lastFailure;
static bool s_testStatus;
static Falcon::String s_testPrefix;
static Falcon::numeric s_totalTime;
static Falcon::numeric s_totalOps;
static Falcon::int64 s_timeFactor;

namespace Falcon
{
namespace TestSuite
{

bool getSuccess()
{
   return ::s_testStatus;
}

void setSuccess( bool status )
{
   ::s_testStatus = status;
}

const ::Falcon::String &getFailureReason()
{
   return ::s_lastFailure;
}

void setTestName( const ::Falcon::String &prefix )
{
   ::s_testPrefix = prefix;
   ::s_testPrefix.reserve(0);
}

void getTimings( ::Falcon::numeric &tot_t, ::Falcon::numeric &tot_ops )
{
   tot_t = ::s_totalTime;
   tot_ops = ::s_totalOps;
   ::s_totalTime = 0.0;
   ::s_totalOps = 0.0;
}

::Falcon::int64 getTimeFactor()
{
   return ::s_timeFactor;
}

void setTimeFactor( ::Falcon::int64 ts)
{
   ::s_timeFactor = ts;
}

}
}

FALCON_FUNC  flc_testrelect ( ::Falcon::VMachine *vm )
{
   if ( vm->paramCount() >= 1 )
      vm->retval( *vm->param( 0 ) );
   else
      vm->retnil();
}

FALCON_FUNC  flc_success ( ::Falcon::VMachine *vm )
{
   s_testStatus = true;
   throw Falcon::VMEventQuit();
}

FALCON_FUNC  flc_failure ( ::Falcon::VMachine *vm )
{
   s_testStatus = false;
   if ( vm->paramCount() > 0 && vm->param( 0 )->isString() )
   {
      s_lastFailure = *vm->param(0)->asString();
   }
   else
      s_lastFailure = "";
   throw Falcon::VMEventQuit();
}

FALCON_FUNC  flc_alive ( ::Falcon::VMachine *vm )
{
   static int pos = 0;
   static const char *frullo = { "-\\|/" };
   ::Falcon::Stream &out = *vm->stdOut();

   ::Falcon::int64 percent = 0;
   if ( vm->paramCount() > 0 && vm->param( 0 )->isOrdinal() )
   {
      percent = vm->param(0)->forceInteger();
      if( percent > 100 ) {
         percent = 100;
      }
      else if ( percent < 0 ) {
         percent = 0;
      }

      out.writeString( "\r" + ::s_testPrefix + ": " );
      Falcon::String temp = "[";
      temp.append( frullo[pos++] );
      temp += "]";
      temp.writeNumber( percent );
      temp += " ";
      out.writeString( temp );
   }
   else {
      out.writeString( "\r" );
      Falcon::String temp = "[";
      temp.append(  frullo[pos++] );
      temp += "]";
      temp.writeNumber( percent );
      temp += " ";
      out.writeString( temp );
   }
   out.writeString( "\r" + ::s_testPrefix + ": " );
   out.flush();

   if ( pos == 4 ) pos = 0;
}

FALCON_FUNC  flc_timings ( ::Falcon::VMachine *vm )
{
   ::Falcon::Item *i_totalTime = vm->param(0);
   ::Falcon::Item *i_totalOps = vm->param(1);
   ::s_totalOps = i_totalOps->forceNumeric();
   ::s_totalTime = i_totalTime->forceNumeric();
}

FALCON_FUNC  flc_timeFactor( ::Falcon::VMachine *vm )
{
   vm->retval( ::s_timeFactor );
}

#ifdef FALCON_EMBED_MODULES
Falcon::Module *init_testsuite_module()
{
#else
FALCON_MODULE_DECL 
{
#endif

   s_timeFactor = 1;
   Falcon::Module *tsuite = new Falcon::Module();
   tsuite->name( "falcon.testsuite" );
   tsuite->engineVersion( FALCON_VERSION_NUM );
   tsuite->version( FALCON_VERSION_NUM );

   tsuite->addExtFunc( "failure", flc_failure );
   tsuite->addExtFunc( "success", flc_success );
   tsuite->addExtFunc( "testReflect", flc_testrelect );
   tsuite->addExtFunc( "alive", flc_alive );
   tsuite->addExtFunc( "timings", flc_timings );
   tsuite->addExtFunc( "timeFactor", flc_timeFactor );

   return tsuite;

}

/* end of fal_testsuite.cpp */
