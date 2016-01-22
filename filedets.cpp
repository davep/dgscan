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

// Standard C++ header files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Local header files.

#include "filedets.h"

/*****************************************************************************
* Function..: FILEDETAILS()                                                  *
* Syntax....: FILEDETAILS( void )                                            *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Class constructor.                                             *
*****************************************************************************/

FILEDETAILS::FILEDETAILS( void )
{
    sFileName   = 0;
    sModuleName = 0;

    uMaxLName  = 50;
    uNextLName = 0;
    LNames     = new STRING[ uMaxLName ];

    uMaxSegSeg  = 20;
    uNextSegSeg = 0;
    SegSegs     = new UINT[ uMaxSegSeg ];

    uMaxSegSize  = 20;
    uNextSegSize = 0;
    SegSize      = new UINT[ uMaxSegSize ];

    uDGroupUsage = 0;
}

/*****************************************************************************
* Function..: ~FILEDETAILS()                                                 *
* Syntax....: ~FILEDETAILS( void )                                           *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Class destructor.                                              *
*****************************************************************************/

FILEDETAILS::~FILEDETAILS( void )
{
    for ( UINT u = 0; u < uNextLName; u++ )
    {
        delete LNames[ u ];
    }

    delete[] LNames;
    delete[] SegSegs;
    delete[] SegSize;

    if ( sFileName )
    {
        delete[] sFileName;
    }

    if ( sModuleName )
    {
        delete[] sModuleName;
    }
}

/*****************************************************************************
* Function..: Reset()                                                        *
* Syntax....: void Reset( void )                                             *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Reset the array pointers for the LName, Segment and DGroup     *
* ..........: users arrays. This is used when reading a whole library.       *
*****************************************************************************/

void FILEDETAILS::Reset( void )
{
    for ( UINT u = 0; u < uNextLName; u++ )
    {
        delete LNames[ u ];
    }

    uMaxLName  = 20;
    uNextLName = 0;

    uMaxSegSeg  = 20;
    uNextSegSeg = 0;

    uMaxSegSize  = 20;
    uNextSegSize = 0;

    uDGroupUsage = 0;
}

/*****************************************************************************
* Function..: SetFileName()                                                  *
* Syntax....: void SetFileName( STRING sName )                               *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Set the name of the file we are working on.                    *
*****************************************************************************/

void FILEDETAILS::SetFileName( STRING sName )
{
    if ( sFileName )
    {
        delete[] sFileName;
    }

    sFileName = new BYTE[ strlen( (char *) sName ) + 1 ];
    strcpy( (char *) sFileName, (char *) sName );
}

/*****************************************************************************
* Function..: SetModuleName()                                                *
* Syntax....: void SetModuleName( STRING sName )                             *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Set the name of the module we are working on.                  *
*****************************************************************************/

void FILEDETAILS::SetModuleName( STRING sName )
{
    if ( sModuleName )
    {
        delete[] sModuleName;
    }

    sModuleName = new BYTE[ strlen( (char *) sName ) + 1 ];
    strcpy( (char *) sModuleName, (char *) sName );
}

/*****************************************************************************
* Function..: AddLName()                                                     *
* Syntax....: void AddLName( STRING LName )                                  *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Add an LName entry to FILEDETAILS.                             *
*****************************************************************************/

void FILEDETAILS::AddLName( STRING LName )
{
    if ( uNextLName == uMaxLName )
    {
        uMaxLName += 20;

        STRING *NewLNames = new STRING[ uMaxLName ];
        STRING *Temp;

        _fmemcpy( NewLNames, LNames, sizeof( STRING ) * uNextLName );

        Temp   = LNames;
        LNames = NewLNames;

        delete[] Temp;
    }

    LNames[ uNextLName ] = new BYTE[ strlen( (char *) LName ) + 1 ];
    strcpy( (char *) LNames[ uNextLName ], (char *) LName );
    ++uNextLName;
}

/*****************************************************************************
* Function..: GetLName()                                                     *
* Syntax....: STRING GetLName( UINT uLName )                                 *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Return an LName given the index uLName.                        *
*****************************************************************************/

STRING FILEDETAILS::GetLName( UINT uLName )
{
    if ( uLName > uNextLName )
    {
        printf( "\nERROR: Invalid LName index, %d requested but max is %d\n",
                uLName, uNextLName );
        exit( 1 );
    }
    return( LNames[ uLName - 1 ] );
}

/*****************************************************************************
* Function..: AddSegSeg()                                                    *
* Syntax....: void AddSegSeg( UINT uIndex )                                  *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Add a segment's segment name to FILEDETAILS.                   *
*****************************************************************************/

void FILEDETAILS::AddSegSeg( UINT uIndex )
{
    if ( uNextSegSeg == uMaxSegSeg )
    {
        uMaxSegSeg += 20;

        UINT *NewSegSegs = new UINT[ uMaxSegSeg ];
        UINT *Temp;

        _fmemcpy( NewSegSegs, SegSegs, sizeof( UINT ) * uNextSegSeg );

        Temp    = SegSegs;
        SegSegs = NewSegSegs;

        delete[] Temp;
    }

    SegSegs[ uNextSegSeg ] = uIndex;
    ++uNextSegSeg;
}

/*****************************************************************************
* Function..: GetSegSegName()                                                *
* Syntax....: STRING GetSegSegName( UINT uSeg )                              *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Return a segment's segment name given the index uSeg.          *
*****************************************************************************/

STRING FILEDETAILS::GetSegSegName( UINT uSeg )
{
    if ( uSeg > uNextSegSeg )
    {
        printf( "\nERROR: Invalid Segment index, %d requested but max is %d\n",
                uSeg, uNextSegSeg );
        exit( 1 );
    }
    return( GetLName( SegSegs[ uSeg - 1 ] ) );
}

/*****************************************************************************
* Function..: AddSegSize()                                                   *
* Syntax....: void AddSegSize( UINT uSize )                                  *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Add the size of a segment to the array of segment sizes.       *
*****************************************************************************/

void FILEDETAILS::AddSegSize( UINT uSize )
{
    if ( uNextSegSize == uMaxSegSize )
    {
        uMaxSegSize += 20;

        UINT *NewSegSize = new UINT[ uMaxSegSize ];
        UINT *Temp;

        _fmemcpy( NewSegSize, SegSize, sizeof( UINT ) * uNextSegSize );

        Temp    = SegSize;
        SegSize = NewSegSize;

        delete[] Temp;
    }

    SegSize[ uNextSegSize ] = uSize;
    ++uNextSegSize;
}

/*****************************************************************************
* Function..: GetSegSize()                                                   *
* Syntax....: UINT GetSegSize( UINT uIndex )                                 *
* Class.....: FILEDETAILS                                                    *
* Notes.....: Get the size of a segment given the index uIndex.              *
*****************************************************************************/

UINT FILEDETAILS::GetSegSize( UINT uIndex )
{
    return( SegSize[ uIndex - 1 ] );
}
