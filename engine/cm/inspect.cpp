/*
   FALCON - The Falcon Programming Language.
   FILE: inspect.cpp

   Falcon core module -- inspect function
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: Wed, 28 Dec 2011 11:12:52 +0100

   -------------------------------------------------------------------
   (C) Copyright 2011: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

#undef SRC
#define SRC "falcon/cm/inspect.cpp"

#include <falcon/cm/inspect.h>
#include <falcon/vm.h>
#include <falcon/vmcontext.h>
#include <falcon/error.h>

namespace Falcon {
namespace Ext {

Inspect::Inspect():
   Function( "inspect" )
{
   signature("X,[N],[N]");
   addParam("item");
   addParam("maxdepth");
   addParam("maxsize");
}

Inspect::~Inspect()
{
}

void Inspect::invoke( VMContext* , int32  )
{
   throw paramError();
   // TODO
}


}
}

/* end of inspect.cpp */