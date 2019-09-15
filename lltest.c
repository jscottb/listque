/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
   LLTEST.c

   Test out the listque functions.

   Copyright (c) 1990, 1997, 2019 Scott Beasley.
   Released into the Public Domain

   Scott Beasley.
   scottbeasley@gmail.com

*/

/*
   Simple (Very), spread sheet like program to test 
   various linked list functions.  We basicly build a 2d 
   array with a linked list.  There's a simple interface
   to add/change data in the elements and move around the
   list's.  This program does not go over all functions,
   but should give you and idea, of how to use the library.

   The commands are:
       M  - Start display, from Col, Row.
       W  - Change data in Col, Row. And start display from there.
       D  - Delete Col, Row.
       F  - Forward sort.
       R  - Reverse sort.
       Q  - Quit.
*/

/* Un-comment if using curses library. */
#define CURSES_APP

//#define WIN32CON

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "listque.h"

#ifdef __TURBOC__
#include <bios.h>
#include <dos.h>
#include <conio.h>
#endif

#ifdef __DJGPP__
#include <conio.h>
#endif

#define LINE_LEN  79
#define MAX_LINES 24

#define MAX_COLS  26
#define MAX_ROWS  100
#define COLWID    6
#define XSTART    4
#define YSTART    4
#define XEND      70
#define YEND      23

void main (void);
void sprdsht (void);
void displaysht (PLLHND llhnd, int iCol, int iRow);
void clrline (int y);
int cmpint (const LLELEMENT, const LLELEMENT);

#ifndef __DJGPP__
void clrscr (void);
void gotoxy (int x, int y);
#endif

#ifdef WIN32CON
#include <windows.h>
#include <conio.h>
HANDLE hstdout, hstdin;
void InitTTY (void);
#endif

/* CBOX.C functions. */
char *GetStrFld (char *strbuf, int fldno, int ofset, char *sep, char *retstr);
char *TrimLeft (char *strbuf);
char *TrimRight (char *strbuf);

void main (void)
{
   #ifdef WIN32CON
   InitTTY ();
   #endif

   sprdsht ();
}

void sprdsht (void)
{
   int i = 0, rows, iRow, iCol;
   PLLHND llcols, llcollist;
   char strUserRep[81], strRow[26], strCol[26], cCmd;
   char strTmp[11];

   clrscr ();

   /* Create a list to store pointers in. */   
   llcols = LLcreate (sizeof (PLLHND), 0, LLNORMAL, NULL);

   while (i < MAX_COLS) {
      /* Create a list to store dynamic string data in. */   
      llcollist = LLcreate (STRSIZE, 1, LLDESSORT, cmpint);
      LLwrite (llcols, (LLELEMENT) llcollist, LLAPPEND, 0);

      /* Init the row's and write them to the list. */
      rows = 0;
      while (rows < MAX_ROWS) {
         sprintf (strTmp, "%d", rows);
         LLwrite (llcollist, (LLELEMENT) strTmp, LLAPPEND, 0);
         rows++;
      }

      LLhomecursor (llcollist);

      i++;
   }

   iCol = 0;
   iRow = 0;
   LLhomecursor (llcols);
   while (1) {
      clrscr ();
      displaysht (llcols, iCol, iRow);

      LLsetcursor (llcols, iCol); 
      llcollist = (PLLHND) LLreadcursor (llcols);
      LLsetcursor (llcollist, iRow);

      gotoxy (1, 2);
      #ifdef __DJGPP__
      cprintf ("%c, %d [%s]\n", (char) 'A' + iCol, iRow,
                (char *) LLreadcursor (llcollist));
      #endif
      #ifndef __DJGPP__
      printf ("%c, %d [%s]\n", (char) 'A' + iCol, iRow,
               (char *) LLreadcursor (llcollist));
      #endif

      gotoxy (1, 1);
      clrline (1);
      gotoxy (1, 1);
      cCmd = *strUserRep = '\0';
      fputs ("Cmd:", stdout);
      fgets (strUserRep, 80, stdin);
      TrimLeft (strUserRep);
      TrimRight (strUserRep);
      cCmd = toupper (*strUserRep);

      if (!strchr ("QMWDFR", (int) cCmd))
         continue;

      if (cCmd == 'Q') {
         break;         
      }

      gotoxy (1, 1);
      clrline (1);
      gotoxy (1, 1);
      fputs ("Col, Row: ", stdout);
      fgets (strUserRep, 80, stdin);

      TrimLeft (strUserRep);
      TrimRight (strUserRep);
      GetStrFld (strUserRep, 0, 0, ", ", strCol);
      GetStrFld (strUserRep, 1, 0, ", ", strRow);
      TrimLeft (strCol);
      TrimRight (strCol);

      iCol = (int) toupper (*strCol) - (int) 'A';
      iRow = atoi (strRow);

      if (iCol < 0 || iRow < 0) {
         iCol = 0;
         iRow = 0;
      }

      LLsetcursor (llcols, iCol); 
      llcollist = (PLLHND) LLreadcursor (llcols);
      LLsetcursor (llcollist, iRow); 

      if (cCmd == 'W') {
         gotoxy (1, 1);
         clrline (1);
         gotoxy (1, 1);
         fputs ("Enter new value:", stdout);
         fgets (strUserRep, 80, stdin);            
         TrimLeft (strUserRep);
         TrimRight (strUserRep);
         LLreplace (llcollist, iRow, (LLELEMENT) strUserRep);
      }

      if (cCmd == 'D') {
         LLdelete (llcollist, iRow);
      }

      if (cCmd == 'F') {
         LLshellsort (llcollist, LLASNSORT);
      }

      if (cCmd == 'R') {
         LLshellsort (llcollist, LLDESSORT);
      }
   } 

   clrscr ();
   /* Clean up and leave. */
   LLhomecursor (llcols);
   while (1) {
      LLdestroy ((PLLHND) LLreadcursor (llcols));
      if (LLnext (llcols) == EOL) 
         break;
   }

   /*
      NOTE:
      This is not usally done to a writable list, but
      this list contained, ptr's to other linked list 
      handles. So when they were destroyed, the only 
      thing left to do was to free the list handle
      itself.
   */
   LLclose (llcols);
}

void displaysht (PLLHND llhnd, int iCol, int iRow)
{
   int x = XSTART, y = YSTART, ircnt = iRow, bTop = 1, iccnt = iCol;
   PLLHND llcollist;

   llcollist = (PLLHND) LLreadcursor (llhnd);
   LLsetcursor (llcollist, iRow); 
   while (1) {
      if (x == XSTART) {
         gotoxy (1, y);
         #ifdef __DJGPP__
         cprintf ("%02.2d", ircnt++);
         #endif

         #ifndef __DJGPP__
         printf ("%2.2d", ircnt++);
         #endif
      }

      if (bTop) {
         bTop = 0;
         gotoxy (x, YSTART - 1);
         #ifdef __DJGPP__
         cprintf ("%c", (char) 'A' + iccnt++);
         #endif

         #ifndef __DJGPP__
         printf ("%c", (char) 'A' + iccnt++);
         #endif
      }
     
      gotoxy (x, y++);
      #ifdef __DJGPP__
      cprintf ("%-5.5s", (char *) LLreadcursor (llcollist));
      #endif

      #ifndef __DJGPP__
      printf ("%-5.5s", (char *) LLreadcursor (llcollist));
      #endif

      if (LLnext (llcollist) == EOL || y > YEND) {
         if (LLnext (llhnd) == EOL || x > XEND)
            break;
     
         llcollist = (PLLHND) LLreadcursor (llhnd);
         LLsetcursor (llcollist, iRow); 
         x += COLWID;
         y = YSTART;
         bTop = 1;
      }
   }
}

int cmpint (const LLELEMENT elm1, const LLELEMENT elm2)
{
   return atoi ((char *) elm1) - atoi ((char *) elm2);
}

/*---------------------------------------------------------------------------*/

/* Clear entire line given its Y coordinate. */
void clrline (int y)
{ 
  register int i = 0;

  gotoxy (1, y);
  for (i = LINE_LEN; i; i--)
     #ifdef __DJGPP__
     putch (' ');
     #endif
     #ifndef __DJGPP__
     putc (' ', stdout);
     #endif
}

#ifdef __TURBOC__
/* Clear the screen. */
void clrscr (void)
{  
   union REGS r;

   r.h.ah = 6;
   r.h.al = 0;
   r.h.ch = 0;
   r.h.cl = 0;
   r.h.dh = MAX_LINES;
   r.h.dl = LINE_LEN;
   r.h.bh = 7;
   int86 (0x10, &r, &r);
}

/* Send cursor to specified X,Y (0,0 is upper
   left corner). */
void gotoxy (int x, int y)
{
   union REGS i;

   i.h.dh = --y;
   i.h.dl = --x;
   i.h.ah = 2;
   i.h.bh = 0;
   int86 (16, &i, &i);
}
#endif

#ifdef CURSES_APP
void clrscr (void)
{  
   printf ("%c[2J", 27);
}

void gotoxy (int x, int y)
{
   printf ("%c[%d;%df", 27, y, x);
}
#endif

#ifdef WIN32CON
void clrscr (void)
{  
   COORD crds;
   DWORD lsize = 80 * 25;
   DWORD cWritten;

   crds.X = 0;
   crds.Y = 0;

   FillConsoleOutputCharacter (hstdout,
                                (TCHAR) ' ',
                                lsize,
                                crds,
                                &cWritten);
}

void gotoxy (int x, int y)
{
   COORD crds;
    
   crds.X = --x;
   crds.Y = --y;
   SetConsoleCursorPosition (hstdout, crds);
}

void InitTTY (void)
{
   hstdout = GetStdHandle (STD_OUTPUT_HANDLE);
   hstdin = GetStdHandle (STD_INPUT_HANDLE);
}
#endif

/*---------------------------------------------------------------------------*/

/*
   The following functions come from CBOX.C.
   Which can be downloaded from the same site
   you got this file from.

   Copyright (c) 1988, 1997, Scott Beasley.
   Released into the Public Domain

   Scott Beasley. (scottb@lowcountry.com)
   704 Otis RD. Walterboro, SC 29488.
*/

/* 
   Gets a sub-string from a formated string. 
*/
char *GetStrFld (char *strbuf, int fldno, int ofset, char *sep, char *retstr)
{
   char *offset, *strptr;
   int curfld;

   offset = strptr = (char *) NULL;
   curfld = 0;

   strbuf += ofset;

   while (*strbuf) {
      strptr = !offset ? strbuf : offset;
      offset = strpbrk ((!offset ? strbuf : offset), sep);

      if (offset)
         offset++;
      else if (curfld != fldno) {
         *retstr = '\0';
         break;
      }

      if (curfld == fldno) {
         strncpy (retstr, strptr,
         (int)(!offset ? strlen (strptr) + 1 : (int) (offset - strptr)));
         if (offset)
            retstr[offset - strptr - 1] = 0;
         break;
      }
      curfld++;
   }   
   return retstr;
}

char *TrimLeft (char *strbuf)
{
   while (isspace ((int) *strbuf)) {
      strcpy (strbuf, (strbuf + 1));
   }

   return strbuf;
}

char *TrimRight (char *strbuf)
{
   while (isspace ((int) *(strbuf + strlen (strbuf) - 1))) {
      *(strbuf + strlen (strbuf) - 1) = 0;
   }

   return strbuf;
}
