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
   Lsttst.c

   Test out the listque functions.

   Copyright (c) 1990, 1997, 2019 Scott Beasley.
   Released into the Public Domain

   Scott Beasley.
   scottbeasley@gmail.com

*/
#include <stdlib.h>
#include <stdio.h>
#include "listque.h"

int main (void);

int main (void)
{
   PLLHND llhnd;
   int icnt = 0;

   llhnd = LLcreate (0, 1, 1, (COMPFUNC) NULL);

   LLwrite (llhnd, (LLELEMENT) "Nine", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Two", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Five", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Seven", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Three", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Eight", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "Four", LLAPPEND, 0);
   LLwrite (llhnd, (LLELEMENT) "One", LLINSERT, 3);

   icnt = LLentrycount (llhnd);
   char entry[20];
   LLhomecursor(llhnd);
   while (!EOLL(llhnd)) {
      LLread (llhnd, (LLELEMENT)entry, LLNEXT, 0);
      printf ("%s\n", entry);
   }

   LLdestroy (llhnd);
}
