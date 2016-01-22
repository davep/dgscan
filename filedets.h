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

#ifndef __FILEDETS_H
#define __FILEDETS_H

// Include local header files.

#include "dgscan.h"

/*****************************************************************************
* Class.....: FILEDETAILS                                                    *
* Inherits..: <None>                                                         *
* Notes.....: Class to hold the details of an OBJ file (or it's entry in a   *
* ..........: LIB file).                                                     *
*****************************************************************************/

class FILEDETAILS
{
public:
    UINT uDGroupUsage;

    FILEDETAILS( void );
    ~FILEDETAILS( void );

    void   AddLName( STRING );
    STRING GetLName( UINT );
    int    IsLName( UINT u )    { return( u > 0 && u < uNextLName ); }

    void   AddSegSeg( UINT );
    STRING GetSegSegName( UINT );
    int    IsSegIndex( UINT u ) { return( u > 0 && u < uNextSegSeg ); }

    void   AddSegSize( UINT );
    UINT   GetSegSize( UINT );

    void   SetFileName( STRING );
    STRING GetFileName( void )      { return( sFileName ); }
    STRING GetModuleName( void )    { return( sModuleName ); }
    void   SetModuleName( STRING );

    void   Reset( void );

private:
    STRING sFileName;
    STRING sModuleName;

    STRING *LNames;
    UINT uNextLName;
    UINT uMaxLName;

    UINT *SegSegs;
    UINT uNextSegSeg;
    UINT uMaxSegSeg;

    UINT *SegSize;
    UINT uNextSegSize;
    UINT uMaxSegSize;

};

#endif
