/*
   FALCON - The Falcon Programming Language.
   FILE: statement.cpp

   Syntactic tree item definitions -- statements.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: Sun, 02 Jan 2011 20:37:39 +0100

   -------------------------------------------------------------------
   (C) Copyright 2011: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

#include <falcon/statement.h>
#include <falcon/expression.h>
#include <falcon/syntree.h>
#include <falcon/vm.h>

#include <falcon/codeerror.h>
#include <falcon/trace.h>

namespace Falcon
{

Breakpoint::Breakpoint( int32 line, int32 chr ):
   Statement(breakpoint_t, line, chr )
{
   apply = apply_;
}

Breakpoint::~Breakpoint()
{
}

void Breakpoint::describe( String& tgt ) const
{
   tgt = "(*)";
}

void Breakpoint::apply_( const PStep*, VMContext* ctx )
{
   ctx->vm()->breakpoint();
}


//====================================================================
//

StmtWhile::StmtWhile( Expression* check, SynTree* stmts, int32 line, int32 chr ):
   Statement( while_t, line, chr ),
   m_check(check),
   m_stmts( stmts )
{
   apply = apply_;

   check->precompile(&m_pcCheck);

   // push ourselves and the expression in the steps
   m_step0 = this;
   m_step1 = &m_pcCheck;
}

StmtWhile::~StmtWhile()
{
   delete m_check;
   delete m_stmts;
}

void StmtWhile::oneLiner( String& tgt ) const
{
   tgt = "while " + m_check->describe();
}


void StmtWhile::describe( String& tgt ) const
{
   for( int32 i = 1; i < chr(); i++ ) {
      tgt.append(' ');
   }
   
   tgt += "while " + m_check->describe() + "\n" +
           m_stmts->describe() +
         "end\n";
}

void StmtWhile::apply_( const PStep* s1, VMContext* ctx )
{
   const StmtWhile* self = static_cast<const StmtWhile*>(s1);
   
   if ( ctx->topData().isTrue() )
   {
      TRACE1( "Apply 'while' at line %d -- redo ", self->line() );
      // redo.
      ctx->pushCode( &self->m_pcCheck );
      ctx->pushCode( self->m_stmts );
   }
   else {
      TRACE1( "Apply 'while' at line %d -- leave ", self->line() );
      
      //we're done
      ctx->popCode();
   }
   
   // in both cases, the data is used.
   ctx->popData();
}



//====================================================================
//

StmtIf::StmtIf( Expression* check, SynTree* ifTrue, SynTree* ifFalse, int32 line, int32 chr ):
   Statement( if_t, line, chr )
{
   apply = apply_;

   m_ifFalse = ifFalse;
   addElif( check, ifTrue );
   // push ourselves and the expression in the steps
   m_step0 = this;
   m_step1 = &m_elifs[0]->m_pcCheck;
}

StmtIf::~StmtIf()
{
   delete m_ifFalse;
   for( unsigned i = 0; i < m_elifs.size(); ++i )
      delete m_elifs[i];
}

StmtIf& StmtIf::addElif( Expression *check, SynTree* ifTrue, int32 line, int32 chr  )
{
   m_elifs.push_back( new ElifBranch(check, ifTrue, line, chr ) );
   return *this;
}

StmtIf& StmtIf::setElse( SynTree* ifFalse )
{
   delete m_ifFalse; // in case it was there.
   m_ifFalse = ifFalse;
   return *this;
}


void StmtIf::oneLiner( String& tgt ) const
{
   tgt = "if "+ m_elifs[0]->m_check->describe();
}

void StmtIf::describe( String& tgt ) const
{
   tgt += "if "+ m_elifs[0]->m_check->describe() + "\n"
              + m_elifs[0]->m_ifTrue->describe();

   for ( size_t i = 1; i < m_elifs.size(); ++i )
   {      
      tgt += "elif " + m_elifs[i]->m_check->describe() + "\n"
                     + m_elifs[i]->m_ifTrue->describe();
   }

   if( m_ifFalse != 0  ) {
      tgt += "else\n" + m_ifFalse->describe();
   }

   tgt += "end\n";
}


void StmtIf::apply_( const PStep* s1, VMContext* ctx )
{
   const StmtIf* self = static_cast<const StmtIf*>(s1);

   TRACE1( "Apply 'if' at line %d ", self->line() );

   int sid = ctx->currentCode().m_seqId;
   if ( ctx->topData().isTrue() )
   {
      ctx->popData(); // anyhow, we have consumed the data

      TRACE1( "--Entering elif %d", sid );
      // we're gone -- but we may use our frame.
      ctx->resetCode( self->m_elifs[sid]->m_ifTrue );
   }
   else
   {
      ctx->popData(); // anyhow, we have consumed the data

      // try next else-if
      if( ++sid < (int) self->m_elifs.size() )
      {
         TRACE2( "--Trying branch %d", sid );
         ctx->currentCode().m_seqId = sid;
         ctx->pushCode( &self->m_elifs[sid]->m_pcCheck );
      }
      else
      {
         // we're out of elifs.
         if( self->m_ifFalse != 0 )
         {
            MESSAGE1( "--Entering else" );
            ctx->resetCode(self->m_ifFalse);
         }
         else
         {
            // just pop
            MESSAGE1( "--Failed" );
            ctx->popCode();
         }
      }
   }
}


StmtIf::ElifBranch::~ElifBranch()
{
   delete m_check;
   delete m_ifTrue;
}

void StmtIf::ElifBranch::compile()
{
   m_check->precompile( &m_pcCheck );
}

//============================================================
// Return
//
StmtReturn::StmtReturn( Expression* expr, int32 line, int32 chr ):
      Statement(return_t, line, chr ),
      m_expr( expr )
{
   apply = apply_;

   m_step0 = this;

   if ( expr )
   {
      m_expr = expr;
      expr->precompile( &m_pcExpr );
      m_step1 = &m_pcExpr;
   }
}

StmtReturn::~StmtReturn()
{
   delete m_expr;
}

void StmtReturn::expression( Expression* expr )
{
   delete m_expr;
   m_expr = expr;
}

void StmtReturn::describe( String& tgt ) const
{
   for( int32 i = 1; i < chr(); i++ ) {
      tgt.append(' ');
   }

   if( m_expr != 0 )
   {
      tgt = "return " + m_expr->describe() +"\n";
   }
   else
   {
      tgt = "return\n";
   }
}


void StmtReturn::apply_( const PStep*ps, VMContext* ctx )
{
   const StmtReturn* stmt = static_cast<const StmtReturn*>(ps);
   TRACE1( "Apply 'return' at line %d ", stmt->line() );

   Item retval;
   // clear A if there wasn't any expression
   if ( stmt->m_expr != 0 )
   {
      ctx->returnFrame( ctx->topData() );
   }
   else
   {
      ctx->returnFrame();
   }
}


}

/* end of statement.cpp */