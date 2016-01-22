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

#include <dos.h>
#include <stdlib.h>
#include <string.h>

// Local header files.

#include "objstr.h"
#include "objfrmt.h"
#include "dgscan.h"
#include "filedets.h"

// Prototype functions in this source.

void   DisplayHelp( void );
int    FileLoop( char * );
int    ScriptLoop( char * );
long   DGroupDetect( STRING, STRING );
void   ScanTheAdr( FILE *, FILEDETAILS * );
void   ScanLNames( FILE *, FILEDETAILS * );
void   ScanSegDef( FILE *, FILEDETAILS * );
void   ScanGrpDef( FILE *, FILEDETAILS * );
void   ScanLeData( FILE *, FILEDETAILS * );
UINT   ClipperStatics( FILE *, UINT );
void   Skip( FILE * );
UINT   GetRecordLen( FILE * );
int    GetIndex( FILE *, UINTP );
STRING FitStringInto( STRING, int );
void   ScanArgs( int, char ** );
void   ScanEnv( void );
void   SwitchScan( char * );
UINT   GetUINT( FILE * );

// Switch values.

int iTotalsOnly   = FALSE;
int iNoClpStatics = FALSE;
int iShowHelp     = FALSE;
int iQuiet        = FALSE;
int iFileName     = 0;

// Constants.

const char *csDGroupID  = "DGROUP";     // Name of DGroup
const char *cs_SympID   = "_SYMP";      // Name of _SYMP segment.
const char *csStaticsID = "STATICS$";   // Segment name if Clipper Static is
// used in a .PRG file.
const BYTE pbStatStamp[] = { 0x83,      // Bytes to look for when finding
                             0xC4,      // the number of statics in a
                             0x08,      // STATICS$ segment.
                             0xCB,
                             0x3B };
const int  iStaticSize   = 0xE;         // Size of a clipper static.

/*****************************************************************************
* Function..: main()                                                         *
* Syntax....: int main( int ArgC, char **ArgV )                              *
* Notes.....: Guess!                                                         *
*****************************************************************************/

int main( int ArgC, char **ArgV )
{
    int iReturn = 0;

    ScanEnv();
    ScanArgs( ArgC, ArgV );

    if ( !iQuiet || iShowHelp )
    {
        fprintf( stderr, "\nDgScan v" SCAN_VER
                 " - DGROUP Usage Detector\nBy Dave Pearson\n" );
    }

    if ( iFileName && !iShowHelp )
    {
        printf( "\nFile-------- Module--------------------------- "
                "Segment----------- Bytes-----\n" );

        if ( *ArgV[ iFileName ] == '@' )
        {
            iReturn = ScriptLoop( &ArgV[ iFileName ][ 1 ] );
        }
        else
        {
            iReturn = FileLoop( ArgV[ iFileName ] );
        }
    }
    else
    {
        DisplayHelp();
    }

    return( iReturn );
}

/*****************************************************************************
* Function..: DisplayHelp()                                                  *
* Syntax....: void DisplayHelp( void )                                       *
* Notes.....: For those who need it, here is some help.                      *
*****************************************************************************/

void DisplayHelp( void )
{
    fprintf( stderr, "\nSyntax: DGScan [<Switches>] <FileSpec> | "
             "@<ScriptName> [<Switches>]\n"
             "\n where   <FileSpec> is the name of an OBJ or LIB file. "
             "Wildcards are ok."
             "\n       <ScriptName> is the name of a script file. This "
             "file should contain"
             "\n                    the names of the files you want to "
             "scan, one per line.\n"
             "\n Switches:"
             "\n    /q      Quiet. Don't display the title."
             "\n    /s      Exclude Clipper static variables from the scan."
             "\n    /t      Display totals only.\n"
             "\nThis utility will scan object and library files that you "
             "intend to link into"
             "\nyour Clipper 5.x application and detect their possible "
             "usage of Clipper's"
             "\nDGROUP. The main cause of such usage is usualy hard coded "
             "strings in C and"
             "\nassembler code, but this utility will also calculate the "
             "impact of Clipper"
             "\nstatic variables as well.\n" );
}

/*****************************************************************************
* Function..: FileLoop()                                                     *
* Syntax....: int FileLoop( char *pszFileSpec )                              *
* Notes.....: Main file loop. This function allows the use of wildcards on   *
* ..........: the command line.                                              *
*****************************************************************************/

int FileLoop( char *pszFileSpec )
{
    char szWork[ _MAX_PATH ];
    char szDrive[ _MAX_DRIVE ];
    char szDir[ _MAX_DIR ];
    char szName[ _MAX_FNAME ];
    char szExt[ _MAX_EXT ];

    strcpy( szWork, pszFileSpec );
    strupr( szWork );

    _splitpath( szWork, szDrive, szDir, szName, szExt );

    struct find_t ffbFiles;
    int    iDone = _dos_findfirst( szWork, _A_NORMAL, &ffbFiles );

    if ( *szExt == 0 )
    {
        for ( int iTry = 0; iTry < 2 && iDone; iTry++ )
        {
            _makepath( szWork, szDrive, szDir, szName, iTry == 0 ? "OBJ" :
                       "LIB" );
            iDone = _dos_findfirst( szWork, _A_NORMAL, &ffbFiles );
        }
    }

    if ( iDone )
    {
        printf( "'%s' not found!\n", szWork );
    }

    long lGrandTotal = 0L;
    int  iFiles      = 0;

    while ( !iDone )
    {
        sprintf( szWork, "%s%s%s", szDrive, szDir, ffbFiles.name );

        lGrandTotal += DGroupDetect( (STRING) szWork, (STRING) ffbFiles.name );
        iDone = _dos_findnext( &ffbFiles );
        ++iFiles;
    }

    if ( iFiles > 1 )
    {
        printf( "***** Total ***** %58ld\n\n", lGrandTotal );
    }

    return( lGrandTotal > 0L );
}

/*****************************************************************************
* Function..: ScriptLoop()                                                   *
* Syntax....: int ScriptLoop( char *szScript )                               *
* Notes.....: Script file loop. This function allows the use of a script file*
* ..........: name on the command line.                                      *
*****************************************************************************/

int ScriptLoop( char *szScript )
{
    long lGrandTotal = 0L;
    FILE *fScript    = fopen( szScript, "r+" );

    if ( fScript != NULL )
    {
        int  iFiles = 0;
        char Buffer[ 132 ];

        while ( fgets( Buffer, sizeof( Buffer ), fScript ) != 0 )
        {
            if ( *Buffer != ' ' && *Buffer != ';' && *Buffer != 0xD &&
                 *Buffer != 0xA )
            {
                char szName[ _MAX_FNAME ];
                char szExt[ _MAX_EXT ];
                char szFile[ _MAX_FNAME + _MAX_EXT ];

                Buffer[ strlen( Buffer ) - 1 ] = 0;
                _splitpath( Buffer, NULL, NULL, szName, szExt );
                sprintf( szFile, "%s%s", szName, szExt );
                strupr( szFile );

                lGrandTotal += DGroupDetect( (STRING) Buffer,
                                             (STRING) szFile );
                ++iFiles;
            }
        }

        fclose( fScript );

        if ( iFiles > 1 )
        {
            printf( "***** Total ***** %58ld\n\n", lGrandTotal );
        }
    }
    else
    {
        printf( "Can't open script file '%s'\n", szScript );
    }

    return( lGrandTotal > 0L );
}

/*****************************************************************************
* Function..: DGroupDetect()                                                 *
* Syntax....: long DGroupDetect( STRING sPath, STRING sName )                *
* Notes.....: File processing function. This function takes apart the file   *
* ..........: and hands control to the correct record scanner.               *
*****************************************************************************/

long DGroupDetect( STRING sPath, STRING sName )
{
    UINT uFileTotal = 0;
    FILE *f         = fopen( (char *) sPath, "rb" );

    if ( f != NULL )
    {
        FILEDETAILS fd;
        int iEOF = FALSE;

        fd.SetFileName( sName );

        while ( !iEOF )
        {
            int i = getc( f );

            switch ( i )
            {
            case 0x0    : break;
            case MSLEND :
            case LIBEND :
            case EOF    : iEOF = TRUE; break;
            case THEADR :
            {
                uFileTotal += fd.uDGroupUsage;
                ScanTheAdr( f, &fd );
                break;
            }
            case LNAMES : ScanLNames( f, &fd ); break;
            case SEGDEF : ScanSegDef( f, &fd ); break;
            case GRPDEF : ScanGrpDef( f, &fd ); break;
            case LEDATA : ScanLeData( f, &fd ); break;
            default     : Skip( f );  break;
            }
        }

        uFileTotal += fd.uDGroupUsage;
        fclose( f );

        printf( "%-12s %-33s %-18s %10u\n%s", sName, "*** Total ***",
                "", uFileTotal, iTotalsOnly ? "" : "\n" );
    }
    else
    {
        printf( "%-12s Can't open file.\n", sName );
    }

    return( (long) uFileTotal );
}

/*****************************************************************************
* Function..: ScanTheAdr()                                                   *
* Syntax....: void ScanTheAdr( FILE *f, FILEDETAILS *fd )                    *
* Notes.....: Scans the THEADR record.                                       *
*****************************************************************************/

void ScanTheAdr( FILE *f, FILEDETAILS *fd )
{
    fseek( f, 2, SEEK_CUR );  // Skip the record len.

    OBJSTR Module( f );

    fd->SetModuleName( (STRING) Module );
    fd->Reset();

    (void) getc( f ); // Eat the checksum.
}

/*****************************************************************************
* Function..: ScanLNames()                                                   *
* Syntax....: void ScanLNames( FILE *f, FILEDETAILS *fd )                    *
* Notes.....: Read in the LNames listed in the LNAMES section and load them  *
* ..........: into the FILEDETAILS object.                                   *
*****************************************************************************/

void ScanLNames( FILE *f, FILEDETAILS *fd )
{
    UINT uRecLen = GetRecordLen( f );
    UINT uMax    = uRecLen - 1;
    UINT uRead   = 0;

    while ( uRead < uMax )
    {
        OBJSTR LName( f );

        fd->AddLName( (STRING) LName );

        uRead += strlen( (char *) ( (STRING) LName ) ) + 1;
    }

    (void) getc( f ); // Eat the checksum.
}

/*****************************************************************************
* Function..: ScanSegDef()                                                   *
* Syntax....: void ScanSegDef( FILE *f, FILEDETAILS *fd )                    *
* Notes.....: Read in the segment definitions and load them into the         *
* ..........: FILEDETAILS object.                                            *
*****************************************************************************/

void ScanSegDef( FILE *f, FILEDETAILS *fd )
{
    fseek( f, 2, SEEK_CUR );  // Skip the record len.

    BYTE bFlags = (BYTE) getc( f );
    BYTE bAlign = ( bFlags & 0x70 ) >> 5;

    if ( bAlign == 0 )
    {
        fseek( f, 4, SEEK_CUR );  // Skip four bytes.
    }

    UINT uSize = GetUINT( f );

    UINT uSegIndex;
    UINT uClassIndex;
    UINT uOvlIndex;

    GetIndex( f, &uSegIndex );
    GetIndex( f, &uClassIndex );

    if ( bAlign != 0 )
    {
        GetIndex( f, &uOvlIndex );
    }

    fd->AddSegSeg( uSegIndex );
    fd->AddSegSize( uSize );

    (void) getc( f ); // Eat the checksum.
}

/*****************************************************************************
* Function..: ScanGrpDef()                                                   *
* Syntax....: void ScanGrpDef( FILE *f, FILEDETAILS *fd )                    *
* Notes.....: Read in the group definitions. This is the first important     *
* ..........: function for DGroup detection. Here, we look for a GRPDEF for  *
* ..........: DGROUP. If this is DGROUP then we start writing out the details*
* ..........: of all the segments that use it.                               *
*****************************************************************************/

void ScanGrpDef( FILE *f, FILEDETAILS *fd )
{
    UINT uRecLen = GetRecordLen( f );
    UINT uMax    = uRecLen - 1;
    UINT uRead   = 0;
    int  iDGroup = FALSE;
    UINT uIndex;

    uRead += GetIndex( f, &uIndex );

    iDGroup = ( strcmp( (char *) fd->GetLName( uIndex ), csDGroupID ) == 0 );

    while ( uRead < uMax )
    {
        (void) getc( f ); // Skip a byte.
        ++uRead;

        uRead += GetIndex( f, &uIndex );

        if ( iDGroup && fd->IsLName( uIndex ) )
        {
            if ( ( strcmp( (char *) fd->GetSegSegName( uIndex ),
                           cs_SympID ) != 0 ) &&
                 ( fd->GetSegSize( uIndex ) != 0 ) )
            {
                if ( !iTotalsOnly )
                {
                    printf( "%-12s %-33s %-18s %10u\n", fd->GetFileName(),
                            FitStringInto( fd->GetModuleName(), 33 ),
                            FitStringInto( fd->GetSegSegName( uIndex ), 18 ),
                            fd->GetSegSize( uIndex ) );
                }
                fd->uDGroupUsage += fd->GetSegSize( uIndex );
            }
        }
    }

    (void) getc( f ); // Eat the checksum.
}

/*****************************************************************************
* Function..: ScanLeData()                                                   *
* Syntax....: void ScanLeData( FILE *f, FILEDETAILS *fd )                    *
* Notes.....: The second important function for DGroup detection. This       *
* ..........: function looks at the segment of the LeData and if the         *
* ..........: segment name is STATICS$ we calculate the number of Clipper    *
* ..........: static variables in use.                                       *
*****************************************************************************/

void ScanLeData( FILE *f, FILEDETAILS *fd )
{
    UINT uRecLen  = GetRecordLen( f );
    UINT uRead    = 0;
    UINT uLen     = uRecLen - 1;
    UINT uSize;
    UINT uSegment;

    uRead += GetIndex( f, &uSegment );

    fseek( f, 2, SEEK_CUR );  // Skip two bytes.

    uRead += 2;

    uSize = ( uLen - uRead );

    if ( fd->IsLName( uSegment ) )
    {
        if ( strcmp( (char *) fd->GetSegSegName( uSegment ),
                     csStaticsID ) == 0 )
        {
            if ( !iNoClpStatics )
            {
                UINT uStatics = ClipperStatics( f, uSize );

                if ( !iTotalsOnly )
                {
                    printf( "%-12s %-33s %-18s %10u\n", fd->GetFileName(),
                            FitStringInto( fd->GetModuleName(), 33 ),
                            csStaticsID, uStatics * iStaticSize );
                }
                fd->uDGroupUsage += ( uStatics * iStaticSize );
            }
        }
    }

    fseek( f, uSize, SEEK_CUR );

    (void) getc( f ); // Eat the checksum.
}

/*****************************************************************************
* Function..: ClipperStatics()                                               *
* Syntax....: UINT ClipperStatics( FILE *f, UINT uSize )                     *
* Notes.....: Handle Clipper static variables. They are a special case and   *
* ..........: don't show up in the OBJ file as DGroup users even though they *
* ..........: do.                                                            *
*****************************************************************************/

UINT ClipperStatics( FILE *f, UINT uSize )
{
    BYTE *pbData = new BYTE[ uSize ];
    BYTE *p      = pbData;
    UINT uCount  = 0;
    long lLoc    = ftell( f );

    fread( pbData, uSize, 1, f );

    for ( int i = 0; i < ( uSize - sizeof( pbStatStamp ) ) && !uCount;
          i++, p++ )
    {
        if ( memcmp( p, pbStatStamp, sizeof( pbStatStamp ) ) == 0 )
        {
            p += sizeof( pbStatStamp );
            uCount = ( ( ( (BYTE *) p )[ 1 ] ) << 8 ) +
                ( ( (BYTE *) p )[ 0 ] );
        }
    }

    if ( !uCount )
    {
        uCount = 1;
    }

    delete[] pbData;

    fseek( f, lLoc, SEEK_SET );

    return( uCount );
}

/*****************************************************************************
* Function..: Skip()                                                         *
* Syntax....: void Skip( FILE *f )                                           *
* Notes.....: Generic record skipping function. Used to skip records in the  *
* ..........: object or library file that we don't need to scan.             *
*****************************************************************************/

void Skip( FILE *f )
{
    UINT uRecLen = GetRecordLen( f );

    fseek( f, uRecLen, SEEK_CUR );
}

/*****************************************************************************
* Function..: GetRecordLen()                                                 *
* Syntax....: UINT GetRecordLen( FILE *f )                                   *
* Notes.....: Get the length of a record.                                    *
*****************************************************************************/

UINT GetRecordLen( FILE *f )
{
    return( GetUINT( f ) );
}

/*****************************************************************************
* Function..: GetIndex()                                                     *
* Syntax....: int GetIndex( FILE *f, UINTP uIndex )                          *
* Notes.....: Read an index from the file.                                   *
*****************************************************************************/

int GetIndex( FILE *f, UINTP uIndex )
{
    int  iSize;
    BYTE b1 = (BYTE) getc( f );

    if ( b1 & 0x80 )
    {
        BYTE b2 = (BYTE) getc( f );

        b1      = ( b1 & ~0x80 );
        *uIndex = (UINT) ( b1 << 8 ) + b2;
        iSize   = sizeof( UINT );
    }
    else
    {
        *uIndex = b1;
        iSize   = sizeof( BYTE );
    }

    return( iSize );
}

/*****************************************************************************
* Function..: FitStringInto()                                                *
* Syntax....: STRING FitStringInto( STRING pszString, int iLen )             *
* Notes.....: If a string is too long for the display trim off the correct   *
* ..........: number of leading characters.                                  *
*****************************************************************************/

STRING FitStringInto( STRING pszString, int iLen )
{
    if ( strlen( (char *) pszString ) > iLen )
    {
        pszString += ( strlen( (char *) pszString ) - iLen );
    }

    return( pszString );
}

/*****************************************************************************
* Function..: ScanArgs()                                                     *
* Syntax....: void ScanArgs( int ArgC, char **ArgV )                         *
* Notes.....: Scan the command line arguments and set any switches.          *
*****************************************************************************/

void ScanArgs( int ArgC, char **ArgV )
{
    for ( int i = 1; i < ArgC; i++ )
    {
        if ( *ArgV[ i ] != '/' && *ArgV[ i ] != '-' )
        {
            iFileName = i;
        }
        else
        {
            SwitchScan( ArgV[ i ] );
        }
    }

    if ( !iFileName )
    {
        iShowHelp = TRUE;
    }
}

/*****************************************************************************
* Function..: ScanEnv()                                                      *
* Syntax....: void ScanEnv( void )                                           *
* Notes.....: Scan an environment variable for any switches.                 *
*****************************************************************************/

void ScanEnv( void )
{
    char *pszEnv = getenv( "DGSCAN" );

    if ( pszEnv )
    {
        SwitchScan( pszEnv );
        iShowHelp = FALSE;
    }
}

/*****************************************************************************
* Function..: SwitchScan()                                                   *
* Syntax....: void SwitchScan( char *psz )                                   *
* Notes.....: Scan a string for switches and set the correct variables.      *
*****************************************************************************/

void SwitchScan( char *psz )
{
    for ( char *p = psz; *p; p++ )
    {
        if ( *p == '/' || *p == '-' )
        {
            switch ( *( p + 1 ) )
            {
            case 't' :
            case 'T' :
            {
                iTotalsOnly = TRUE;
                break;
            }
            case 'q' :
            case 'Q' :
            {
                iQuiet = TRUE;
                break;
            }
            case 's' :
            case 'S' :
            {
                iNoClpStatics = TRUE;
                break;
            }
            case '?' :
            case 'h' :
            case 'H' :
            {
                iShowHelp = TRUE;
                break;
            }
            }
        }
    }
}

/*****************************************************************************
* Function..: GetUINT()                                                      *
* Syntax....: UINT GetUINT( FILE *f )                                        *
* Notes.....: To ensure that the two byte word value is read correctly when  *
* ..........: compiled as a 32bit program we shall use a brute force method  *
* ..........: of reading the word from file.                                 *
*****************************************************************************/

UINT GetUINT( FILE *f )
{
    BYTE b1 = (BYTE) getc( f );
    BYTE b2 = (BYTE) getc( f );

    return( (UINT) ( b2 << 8 ) + b1 );
}
