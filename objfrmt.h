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

#ifndef __OBJFRMT_H
#define __OBJFRMT_H

#define BLKDEF  0x7A
#define BLKEND  0x7C
#define COMDEF  0xB0
#define COMENT  0x88
#define DEBSYM  0x7E
#define ENDREC  0x78
#define EXTDEF  0x8C
#define LEXTDEF 0xB6
#define FIXUPP  0x9C
#define GRPDEF  0x9A
#define LEDATA  0xA0
#define LHEADR  0x82
#define LIBDIC  0xAA
#define LIBHED  0xA4
#define LIBLOC  0xA8
#define LIBNAM  0xA6
#define LIDATA  0xA2
#define LINNUM  0x94
#define LNAMES  0x96
#define LOCSYM  0x92
#define MODEND  0x8A
#define OVLDEF  0x76
#define PEDATA  0x84
#define PIDATA  0x86
#define PUBDEF  0x90
#define REDATA  0x72
#define REGINT  0x70
#define RHEADR  0x6E
#define RIDATA  0x74
#define SEGDEF  0x98
#define THEADR  0x80
#define TYPDEF  0x8E
#define MSLEND  0x6D

// And a couple that I'm not too sure about.
#define LIBBEG  0xF0    // The start of a library?
#define LIBEND  0xF1    // The end of a library?
#define UNKOWN  0xB4

#endif
