/*
   FALCON - The Falcon Programming Language.
   FILE: conio_srv_win.h

   Basic Console I/O support
   Interface extension functions
   -------------------------------------------------------------------
   Author: Unknown author
   Begin: Thu, 05 Sep 2008 20:12:14 +0200

   -------------------------------------------------------------------
   (C) Copyright 2008: The above AUTHOR

      Licensed under the Falcon Programming Language License,
   Version 1.1 (the "License"); you may not use this file
   except in compliance with the License. You may obtain
   a copy of the License at

      http://www.falconpl.org/?page_id=license_1_1

   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on
   an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied. See the License for the
   specific language governing permissions and limitations
   under the License.

*/


#ifndef conio_SRV_WIN_H
#define conio_SRV_WIN_H

#include <conio_srv.h>
#include <windows.h>

namespace Falcon {
namespace Srv {

class ConioSrvSys
{
public:
   HANDLE hConsoleIn;
   HANDLE hConsoleOut;
   CONSOLE_SCREEN_BUFFER_INFO csbi; 
   
   ConioSrvSys( HANDLE hIn, HANDLE hOut ):
      hConsoleIn( hIn ),
      hConsoleOut( hOut )
      {}
};

}
}


#endif
/* end of conio_srv.h */
