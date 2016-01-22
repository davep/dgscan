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

// Local header files.

#include "objstr.h"
#include "dgscan.h"

/*****************************************************************************
* Function..: OBJSTR()                                                       *
* Syntax....: OBJSTR( FILE *f )                                              *
* Class.....: OBJSTR                                                         *
* Notes.....: Class constructor.                                             *
*****************************************************************************/

OBJSTR::OBJSTR( FILE *f )
{
    BYTE bLen = (BYTE) getc( f );

    psz = new BYTE[ bLen + 1 ];
    fread( psz, bLen, 1, f );
    psz[ bLen ] = 0;
}

/*****************************************************************************
* Function..: ~OBJSTR()                                                      *
* Syntax....: ~OBJSTR( void )                                                *
* Class.....: OBJSTR                                                         *
* Notes.....: Class destructor.                                              *
*****************************************************************************/

OBJSTR::~OBJSTR( void )
{
    delete[] psz;
}
