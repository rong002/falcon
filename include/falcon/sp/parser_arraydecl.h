/*
   FALCON - The Falcon Programming Language.
   FILE: parser_arraydecl.h

   Parser for Falcon source files -- handler for array and dict decl
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: Sun, 03 Jul 2011 18:13:22 +0200

   -------------------------------------------------------------------
   (C) Copyright 2011: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

#ifndef _FALCON_SP_PARSER_ARRAYDECL_H_
#define _FALCON_SP_PARSER_ARRAYDECL_H_

#include <falcon/setup.h>

namespace Falcon {

class BinaryExpression;

namespace Parsing
{
class Rule;
class Parser;
}

using namespace Parsing;

void apply_expr_array_decl( const Rule&, Parser& p );
void apply_expr_array_decl2( const Rule&, Parser& p );

bool ArrayEntry_errHand( const NonTerminal& nt, Parser& p );

void apply_array_entry_expr( const Rule&, Parser& p );
void apply_array_entry_comma( const Rule&, Parser& p );
void apply_array_entry_eol( const Rule&, Parser& p );
void apply_array_entry_arrow( const Rule&, Parser& p );
void apply_array_entry_close( const Rule&, Parser& p );

void apply_array_entry_range3( const Rule&, Parser& p );
void apply_array_entry_range3bis( const Rule&, Parser& p );
void apply_array_entry_range2( const Rule&, Parser& p );
void apply_array_entry_range1( const Rule&, Parser& p );

void apply_array_entry_runaway( const Rule&, Parser& p );

}

#endif

/* end of parser_arraydecl.h */