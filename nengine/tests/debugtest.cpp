/*
 * main.cpp
 *
 *  Created on: 12/gen/2011
 *      Author: gian
 */

#include <cstdio>
#include <iostream>
#include <string>

#include <falcon/vm.h>
#include <falcon/syntree.h>
#include <falcon/localsymbol.h>
#include <falcon/expression.h>
#include <falcon/exprvalue.h>
#include <falcon/exprcompare.h>
#include <falcon/exprmath.h>
#include <falcon/statement.h>
#include <falcon/stmtautoexpr.h>


#include <falcon/synfunc.h>
#include <falcon/module.h>
#include <falcon/application.h>


class DebugTestApp: public Falcon::Application
{

public:
void go()
{
   Falcon::Module module("debugtest", "./debugtest.cpp");
   Falcon::SynFunc fmain( "__main__" );
   fmain.module(&module);

   // create a program:
   // count = 0
   // while count < 5
   //   count = count + 1
   // end

   Falcon::Symbol* count = new Falcon::LocalSymbol("count",0);
   Falcon::SynTree* assign = new Falcon::SynTree;
   assign->append(
         new Falcon::StmtAutoexpr(
               new Falcon::ExprAssign( count->makeExpression(),
                     new Falcon::ExprPlus( count->makeExpression(), new Falcon::ExprValue(1) )
         ), 3,3));


   Falcon::SynTree* program = &fmain.syntree();
   (*program)
      .append( new Falcon::StmtAutoexpr(
               new Falcon::ExprAssign( count->makeExpression(), new Falcon::ExprValue(0) ), 1,1 ) )
      .append( new Falcon::StmtWhile(
                     new Falcon::ExprLT( count->makeExpression(), new Falcon::ExprValue(5) ),
                     assign, 2,1 ) );

   std::cout << program->describe().c_ize() << std::endl;

   // And now, run the code.
   Falcon::VMachine vm;
   vm.currentContext()->call(&fmain,0);
   vm.currentContext()->pushData(Falcon::Item());  // create an item -- local 0

   while( vm.nextStep() != 0 )
   {
      std::string choice;

      Falcon::String status = vm.location();
      std::cout << status.c_ize() << " - " << vm.nextStep()->oneLiner().c_ize()
            << "..."<< std::endl;
      std::cout << vm.report().c_ize() << std::endl;

      std::cin >> choice;

      if ( choice == "quit" )
      {
         break;
      }
      else if( choice == "step" || choice == "s" )
      {
         vm.step();
      }
      else if( choice == "run" || choice == "r" )
      {
         vm.run();
      }
   }
   
   std::cout << "Top: " << vm.regA().describe().c_ize() << std::endl;
   }
};

// This is just a test.
int main( int, char*[] )
{
   std::cout << "Debug test" << std::endl;

   DebugTestApp gotest;
   gotest.go();
   return 0;
}