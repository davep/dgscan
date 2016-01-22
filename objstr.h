/*

     DGSCAN - DGROUP USAGE SCANNER FOR CLIPPER
     Copyright (C) 1996 David A Pearson
   
     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the license, or 
     (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __OBJSTR_H
#define __OBJSTR_H

// Include standard C++ header files.

#include <stdio.h>

// Include local header files.

#include "dgscan.h"

/*****************************************************************************
* Class.....: OBJSTR                                                         *
* Inherits..: <None>                                                         *
* Notes.....: Class to read a string from an object file.                    *
*****************************************************************************/

class OBJSTR
{
public:
    OBJSTR( FILE * );
    ~OBJSTR( void );
    operator STRING( void ) { return( psz ); }

private:
    STRING  psz;
};

#endif
